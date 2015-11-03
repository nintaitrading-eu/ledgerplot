/* See LICENSE.txt for license and copyright information. */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ledgerplot.h"
#include "docopt.c"

#define CMD_GNUPLOT "gnuplot -persist"
#define FILE_TEMP "data.tmp"
#define FILE_IVE_LAYOUT "income_vs_expenses_layout.gnu"
#define FILE_BARCHART "barchart.gnu"
#define INPUT_LINE_MAX 1024 // MAX line length to copy
#define OUTPUT_ARRAY_MAX 8000 // MAX command length for the dat info
#define NUM_COMMANDS 2

char *f_cmd_gnuplot =
    "plot for [COL=STARTCOL:ENDCOL] 'data.tmp' u COL:xtic(1) w histogram title columnheader(COL) lc rgb word(COLORS, COL), \\"
        "for [COL=STARTCOL:ENDCOL] 'data.tmp' u (column(0)+BOXWIDTH*(COL-STARTCOL+GAPSIZE/2+1)-0.5):COL:COL notitle w labels";
char *f_cmd_income_vs_expenses =
    "ledger -f %s bal --real -X EUR -s -p %d -d \"T&l<=1\" expenses income | grep -Eo '[0-9\\.]{1,100}'";

static int prepare_temp_file(
    char *a_input_file,
    FILE *a_output_file,
    int a_start_year,
    int a_end_year
);
static int write_to_gnuplot();
char *trim_whitespace(char *str);
int get_lines_from_file(char *a_file, char a_gnu_command[OUTPUT_ARRAY_MAX]);

// TODO: write function that loads info from barchart.gnu and combines it with
// info from income_vs_expenses.gnu => user settings for a specific graph
// barchart.gnu => barchart code
// add the plotting code at the end, through a define.
int main(int argc, char *argv[])
{
    FILE *l_output_file; // Temp dat file, where the final script is written to.
    int l_start_year;
    int l_end_year;
    char l_gnu_command[OUTPUT_ARRAY_MAX];

    DocoptArgs args = docopt(
        argc,
        argv,
        1, /* help */
        "ledgerplot version 0.1" /* version */
    );

    if(argc == 1)
    {
        printf("%s\n", args.help_message);
        return 1;
    }
    
    l_start_year = atoi(args.startyear);
    l_end_year = atoi(args.endyear);
    
    l_output_file = fopen(FILE_TEMP, "w");
    if (l_output_file == NULL)
    {
        printf("Error: could not open output file %s.\n", FILE_TEMP);
        exit(1);
    }
    
    /* 
     * Note: To update in realtime: use fflush(gp)
     * Otherwise, the application will wait until the processing is finished.
     */
    if(prepare_temp_file(args.file, l_output_file, l_start_year, l_end_year) != 0)
    {
        fprintf(stderr, "Could not prepare temporary data-file %s.", FILE_TEMP);
        exit(1);
    }
    
     /* 1. Load layout commands */ 
    if (get_lines_from_file(FILE_IVE_LAYOUT, l_gnu_command) != 0)
    {
        fprintf(stderr, "Could not read %s.\n");
        return 1;
    }
    /* 2. Load barchart commands */ 
    if (get_lines_from_file(FILE_BARCHART, l_gnu_command) != 0)
    {
        fprintf(stderr, "Could not read %s.\n");
        return 1;
    }
    /* 3. Load barchart plot command */
    // TODO: doesn't this give the max OUTPUT_ARRAY_MAX + 1?
    // Perhaps return l_count from get_lines_from_file?
    l_gnu_command[sizeof(l_gnu_command) + 1]  = f_cmd_gnuplot;
    /*sprintf(
        l_gnu_command[sizeof(l_gnu_command) + 1],
        f_cmd_gnuplot,
        a_input_file
    );*/
    return write_to_gnuplot();
}

/*
 * prepare_temp_file:
 * This function prepares the data to be plotted,
 * in a temporary file that can be read by gnuplot.
 */
static int prepare_temp_file(
    char *a_input_file,
    FILE *a_output_file,
    int a_start_year,
    int a_end_year
)
{
    int l_records;
    int i;
    FILE *l_fp;
    char l_cmd[INPUT_LINE_MAX];
    char l_line[INPUT_LINE_MAX];
    char l_dat_info[OUTPUT_ARRAY_MAX];
    double l_d1;
    double l_d2;
    double l_d3;
    char *l_tmp;

    l_records = (a_end_year - a_start_year);    
    for (i = 0; i < l_records; i++)
    {   
        sprintf(l_cmd, f_cmd_income_vs_expenses, a_input_file, 2015);
        l_fp = popen(l_cmd, "r");
        if (l_fp == NULL)
        {
            fprintf(stderr, "Could not execute ledger command.\n");
            exit(1);
        }

        while (fgets(l_line, INPUT_LINE_MAX, l_fp) != NULL)
        {
            if (strlen(l_dat_info) <= 0)
                sprintf(l_dat_info, "%s", trim_whitespace(l_line));
            else
                sprintf(l_dat_info, "%s %s", l_dat_info, trim_whitespace(l_line));
        }
        l_d1 = strtod(l_dat_info, &l_tmp);
        l_d2 = strtod(l_tmp, &l_tmp);
        l_d3 = strtod(l_tmp, NULL);
        
        if (pclose(l_fp) == -1)
            fprintf(stderr, "Error reported by pclose().\n");
            
        // TODO: check if we need to perform operations on these values?
        fprintf(a_output_file, "%.2lf %.2lf %.2lf\n", l_d1, l_d2, l_d3); //Write the data to a temporary file
    }
    return 0;
}

/*
 * Writes the generated script lines to a
 * gnuplot pipe.
 */
static int write_to_gnuplot(char l_gcommands[])
{
    FILE *l_gp; // Gnuplot pipe
    int i;
   
     /* 
     * Opens an interface that one can use to send commands as if they were typing into the
     * gnuplot command line. "The -persistent" keeps the plot open even after your
     * C program terminates.
     */
    l_gp = popen(CMD_GNUPLOT, "w");
    if (l_gp == NULL)
    {
        printf("Error opening pipe to GNU plot. Check if you have it!\n");
        return 1;
    }
    
    // TODO: wait, what? the command plots data.temp...
    // So I need a file with data... that would be data.tmp!
    for (i = 0; i < OUTPUT_ARRAY_MAX; i++)
    {
       fprintf(l_gp, "%s \n", l_gcommands[i]); /* Send commands to gnuplot one by one. */
       fflush(l_gp); /* Note: Update in realtime, don't wait until processing is finished. */
    }
    
    /* 
     * Other note: you could also make an actual pipe:
     * mkfifo /tmp/gnuplot
     * while :; do (gnuplot -persist) < /tmp/gnuplot; done
     * and then do a simple
     * echo "plot sin(x)" > /tmp/gnuplot
     */

    /*fprintf(l_gp, "set samples 2000\n");
    fprintf(l_gp, "plot abs(sin(x))\n");
    fprintf(l_gp, "rep abs(cos(x))\n");*/
    fclose(l_gp);
    return 0;
}


/*
 * This function returns a pointer to a substring of the original string.
 * If the given string was allocated dynamically, the caller must not overwrite
 * that pointer with the returned value, since the original pointer must be
 * deallocated using the same allocator with which it was allocated.  The return
 * value must NOT be deallocated using free() etc.
 */
char *trim_whitespace(char *a_string)
{
    char *l_end;

    /* Trim leading space/newline. */
    while(isspace(*a_string) || (*a_string=='\n'))
        a_string++;

    if(*a_string == 0)  // Exit when done.
        return a_string;

    /* Trim trailing space/newline. */
    l_end = a_string + strlen(a_string) - 1;
    while(
        l_end > a_string
        && (isspace(*l_end) || (*l_end == '\n'))
    )
    {
        l_end--;
    }

    /* Write new null terminator. */
    *(l_end + 1) = 0;
    return a_string;
}

/*
 * get_lines_from_file
 * Loads the lines of a file into
 * an array that will be used
 * to send to gnuplot.
 */
int get_lines_from_file(char *a_file, char a_gnu_command[OUTPUT_ARRAY_MAX])
{
    FILE *l_file;
    char l_line[INPUT_LINE_MAX];
    int l_count = 0;
     
    l_file = fopen(a_file, "r");
    if (l_file == NULL)
    {
        printf("Error: could not open output file %s.\n", a_file);
        return 1;
    }
    while (fgets(l_line, INPUT_LINE_MAX, l_file) != NULL)
    {
        printf("test0: %s\n", l_line);
        if (strlen(l_line) > 0)
        {
            printf("test2: %c\n", l_line[0]);
            if (l_line[0] != '#')
            {
                l_count++;
                sprintf(&a_gnu_command[l_count], "%s", trim_whitespace(l_line));
            }
        }
    }
    fclose(l_file);
    return 0;
}

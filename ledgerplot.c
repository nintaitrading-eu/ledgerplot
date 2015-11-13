/* See LICENSE.txt for license and copyright information. */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ledgerplot.h"
#include "docopt.c"
#include "c_generic/functions.c"

#define VERSION "version 0.1"
#define CMD_GNUPLOT "gnuplot -persist"
#define FILE_DATA_TMP "lp_data.tmp"
#define FILE_GNU_TMP "lp_gnu.tmp"
#define FILE_IVE_LAYOUT "gp_income_vs_expenses.gnu"
#define FILE_BARCHART "gp_barchart.gnu"
#define INPUT_LINE_MAX 1024 // MAX line length to copy
#define OUTPUT_ARRAY_MAX 8000 // MAX command length for the dat info
#define NUM_COMMANDS 2


static int prepare_temp_file(
    char *a_input_file,
    FILE *a_output_file,
    int a_start_year,
    int a_end_year
);
static int write_to_gnuplot(char a_gnu_command[OUTPUT_ARRAY_MAX][INPUT_LINE_MAX]);
static int get_lines_from_file(
    char *a_file,
    char a_gnu_command[OUTPUT_ARRAY_MAX][INPUT_LINE_MAX],
    int a_index
);


char *f_cmd_gnuplot =
    "plot for [COL=STARTCOL:ENDCOL] 'lp_data.tmp' u COL:xtic(1) w histogram title columnheader(COL) lc rgb word(COLORS, COL-STARTCOL+1), for [COL=STARTCOL:ENDCOL] 'lp_data.tmp' u (column(0)+BOXWIDTH*(COL-STARTCOL+GAPSIZE/2+1)-1.0):COL:COL notitle w labels textcolor rgb \"gold\"";
char *f_cmd_income_vs_expenses =
    "ledger -f %s bal --real -X EUR -s -p %d -d \"T&l<=1\" expenses income | grep -Eo '[0-9\\.]{1,100}'";


//enum f_dateformats = "%d-%d-%d %d:%d:%d"
//"%d%d%d_%d%d%d"


// TODO: write function that loads info from barchart.gnu and combines it with
// info from income_vs_expenses.gnu => user settings for a specific graph
// barchart.gnu => barchart code
// add the plotting code at the end, through a define.
int main(int argc, char *argv[])
{
    FILE *l_output_file; // Temp dat file, where the final script is written to.
    int l_start_year;
    int l_end_year;
    int l_lines = 0;
    int l_lines_total = 0;
    char l_gnu_command[OUTPUT_ARRAY_MAX][INPUT_LINE_MAX];

    DocoptArgs args = docopt(
        argc,
        argv,
        1, /* help */
        VERSION /* version */
    );

    if (argc == 1)
    {
        printf("%s\n", args.help_message);
        return 1;
    }
    
    l_start_year = atoi(args.startyear);
    l_end_year = atoi(args.endyear);
    
    l_output_file = fopen(FILE_DATA_TMP, "w");
    if (l_output_file == NULL)
    {
        printf("Error: could not open output file %s.\n", FILE_DATA_TMP);
        exit(1);
    }
    
    /* 
     * Note: To update in realtime: use fflush(gp)
     * Otherwise, the application will wait until the processing is finished.
     */
    if (prepare_temp_file(args.file, l_output_file, l_start_year, l_end_year) != 0)
    {
        fprintf(stderr, "Could not prepare temporary data-file %s.", FILE_DATA_TMP);
        exit(1);
    }
   
    /* 1. Load layout commands */ 
    memset(l_gnu_command, '\0', OUTPUT_ARRAY_MAX*INPUT_LINE_MAX*sizeof(char));
    l_lines = get_lines_from_file(FILE_IVE_LAYOUT, l_gnu_command, 0);
    l_lines_total = l_lines;
    if (l_lines == -1)
    {
        fprintf(stderr, "Could not read %s.\n", FILE_IVE_LAYOUT);
        exit(1);
    }
    /* 2. Load barchart commands */ 
    l_lines = get_lines_from_file(FILE_BARCHART, l_gnu_command, l_lines_total);
    l_lines_total += l_lines;
    if ( l_lines == -1)
    {
        fprintf(stderr, "Could not read %s.\n", FILE_BARCHART);
        exit(1);
    }
    /* 3. Load barchart plot command */
    //if (!strncpy(l_gnu_command[l_lines_total + 1], f_cmd_gnuplot, INPUT_LINE_MAX))
    //    exit(1);
    sprintf(
        l_gnu_command[l_lines_total - 1],
        f_cmd_gnuplot,
        FILE_DATA_TMP
    );
    /*for (int i=0; i<l_lines_total+2; i++) 
    {
        printf("-- %s\n", l_gnu_command[i]);
    }*/
    printf("%s:\n", FILE_IVE_LAYOUT);
    printf(">>> Generated using gnuplot chart info from %s.\n", FILE_BARCHART);
    printf(">>> Data exported to png.\n");
    printf(">>> The filename was specified in %s.\n", FILE_IVE_LAYOUT);
    return write_to_gnuplot(l_gnu_command);
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
    char l_line_temp[INPUT_LINE_MAX];
    char l_line_input[INPUT_LINE_MAX];
    char l_line_output[INPUT_LINE_MAX]; /* The output line may be just as long. */
    double l_d1;
    double l_d2;
    double l_d3;
    char *l_tmp;
    char l_current_datetime[20];
    int l_current_year;

    l_records = (a_end_year - a_start_year);    
    l_current_year = a_start_year - 1;
    for (i = 0; i <= l_records; i++)
    {   
        l_current_year += l_records;
        sprintf(l_cmd, f_cmd_income_vs_expenses, a_input_file, l_current_year);
        l_fp = popen(l_cmd, "r");
        if (l_fp == NULL)
        {
            fprintf(stderr, "Could not execute ledger command.\n");
            exit(1);
        }

        *l_line_output = '\0';
        while (fgets(l_line_input, INPUT_LINE_MAX, l_fp) != NULL)
        {
            *l_line_temp = '\0'; /* Make sure temp string is empty. */
            trim_whitespace(l_line_temp, l_line_input, INPUT_LINE_MAX);
            if (strlen(l_line_output) <= 0)
            {
                sprintf(l_line_output, "%s", l_line_temp);
            }
            else
            {
                sprintf(l_line_output, "%s %s", l_line_output, l_line_temp);
            }
        }
        l_d1 = strtod(l_line_output, &l_tmp);
        l_d2 = strtod(l_tmp, &l_tmp);
        l_d3 = strtod(l_tmp, NULL);
        //printf("test: l_d1 = %.2f, l_d2 = %.2f, l_d3 = %.2f\n", l_d1, l_d2, l_d3);
        
        if (pclose(l_fp) == -1)
            fprintf(stderr, "Error reported by pclose().\n");
        
        /* Initialize tmp file */    
        if (i == 0) 
        {
            // TODO: move timestamp function in its own module.
            // TODO: make format an enum?
            timestamp(l_current_datetime, "%02d%02d%02d_%02d%02d%02d", sizeof(l_current_datetime));
            fprintf(a_output_file, "# Generated by ledgerplot %s on %s.\n", VERSION, l_current_datetime);
            fprintf(a_output_file, "year expenses income difference\n");
        }
        /* Write data to tmp file */
        // TODO: check if we need to perform operations on these values?
        fprintf(a_output_file, "%d %.2lf %.2lf %.2lf\n", l_current_year, l_d1, l_d2, l_d3); 
    }
    return 0;
}


/*
 * Writes the generated script lines to a
 * gnuplot pipe.
 */
static int write_to_gnuplot(char a_gnu_command[OUTPUT_ARRAY_MAX][INPUT_LINE_MAX])
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
    
    for (i = 0; i < OUTPUT_ARRAY_MAX; i++)
    {
        if (strncmp(a_gnu_command[i], "", INPUT_LINE_MAX) != 0)
        {
            //printf("%d: %s\n", i, a_gnu_command[i]); /* Used for testing/debugging. */
            fprintf(l_gp, "%s\n", a_gnu_command[i]); /* Send commands to gnuplot one by one. */
            fflush(l_gp); /* Note: Update in realtime, don't wait until processing is finished. */
        }
    }
    
    /* 
     * Note: you could also make an actual pipe:
     * mkfifo /tmp/gnuplot
     * while :; do (gnuplot -persist) < /tmp/gnuplot; done
     * and then do a simple
     * echo "plot sin(x)" > /tmp/gnuplot
     */

    fclose(l_gp);
    return 0;
}


/*
 * get_lines_from_file
 * Loads the lines of a file into
 * an array that will be used
 * to send to gnuplot.
 */
static int get_lines_from_file(char *a_file, char a_gnu_command[OUTPUT_ARRAY_MAX][INPUT_LINE_MAX], int a_index)
{
    FILE *l_file;
    char l_line[INPUT_LINE_MAX];
    char l_line_temp[INPUT_LINE_MAX];
    int l_count = 0;
     
    l_file = fopen(a_file, "r");
    if (l_file == NULL)
    {
        printf("Error: could not open output file %s.\n", a_file);
        return -1;
    }
    while (fgets(l_line, INPUT_LINE_MAX, l_file) != NULL)
    {
        if (
            (strlen(l_line) > 0)
            && (l_line[0] != '#')
        )
        {
            l_count++;
            trim_whitespace(l_line_temp, l_line, INPUT_LINE_MAX);
            sprintf(a_gnu_command[a_index + l_count - 1], "%s", l_line_temp);
        }
    }
    fclose(l_file);
    return l_count;
}

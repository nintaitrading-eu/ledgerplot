/* See LICENSE.txt for license and copyright information. */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ledgerplot.h"
#include "docopt.c"

#define GNUPLOT "gnuplot -persist"
#define NUM_COMMANDS 2
#define TEMP_FILE "data.temp"
#define INPUT_MAX 1024 // MAX line length to copy
#define DAT_MAX 8000 // MAX command length for the dat info

char *f_cmd_gnuplot =
    "plot for [COL=STARTCOL:ENDCOL] 'test.dat' u COL:xtic(1) w histogram title columnheader(COL) lc rgb word(COLORS, COL), \\"
        "for [COL=STARTCOL:ENDCOL] 'test.dat' u (column(0)+BOXWIDTH*(COL-STARTCOL+GAPSIZE/2+1)-0.5):COL:COL notitle w labels";
char *f_l_income_vs_expenses =
    "ledger -f %s bal --real -X EUR -s -p %d -d \"T&l<=1\" expenses income | grep -Eo '[0-9\\.]{1,100}'";
// TODO: call exec... on sprintf(l_cmd_str, f_l_income_vs_expenses, "ledger.dat", 2015);

static int prepare_temp_file(
    char *a_file,
    int a_start_year,
    int a_end_year
);

char *trim_whitespace(char *str);

int main(int argc, char *argv[])
{
    // TODO: write function that loads info from barchart.gnu and combines it with
    // info from income_vs_expenses.gnu => user settings for a specific graph
    // barchart.gnu => barchart code
    // add the plotting code at the end, through a define.
    char *l_gcommands[] = {"set title \"TITLE\"", "plot 'data.temp'"};
    FILE *l_gp; // Gnuplot pipe
    //FILE *l_temp = fopen(TEMP_FILE, "w");
    int l_start_year;
    int l_end_year;
    int i;

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
    /* 
     * Opens an interface that one can use to send commands as if they were typing into the
     * gnuplot command line. "The -persistent" keeps the plot open even after your
     * C program terminates.
     */
    l_gp = popen(GNUPLOT, "w");
    if (l_gp == NULL)
    {
        printf("Error opening pipe to GNU plot. Check if you have it!\n");
        exit(1);
    }
    
    /* 
     * Note: To update in realtime: use fflush(gp)
     * Otherwise, the application will wait until the processing is finished.
     */
    if(prepare_temp_file(args.file, l_start_year, l_end_year) != 0)
    {
        fprintf(stderr, "Could not prepare temporary data-file %s.", TEMP_FILE);
        exit(-1);
    }
   
    // TODO: for line in first gnu-script
    for (i = 0; i < NUM_COMMANDS; i++)
    {
       fprintf(l_gp, "%s \n", l_gcommands[i]); /* Send commands to gnuplot one by one. */
       fflush(l_gp); /* Note: Update in realtime, don't wait until processing is finished. */
    }
    // TODO: for line in 2nd gnu-script
    // TODO: add final plot command
    
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
 * prepare_temp_file:
 * This function prepares the data to be plotted,
 * in a temporary file that can be read by gnuplot.
 */
static int prepare_temp_file(
    char *a_file,
    int a_start_year,
    int a_end_year
)
{
    int l_records;
    int i;
    FILE *l_fp;
    char l_cmd[INPUT_MAX];
    char l_input[INPUT_MAX];
    char l_dat_info[DAT_MAX];
    double l_d1;
    double l_d2;
    double l_d3;
    char *l_tmp;
    // TODO: exec ledgerplot command for startyear
    // exec ledgerplot command for all other years until endyear
    // For each: parse output (scheme app?)
    // write to temp-file.
    // Remove (( and change <space>) to a newline
    // That gives the line to be added to data.temp
    // for a specific year.
    // Or easier: use grep:
    // exec ledgerplot command for startyear with  | grep -Eo '[0-9\.]{1,100}' added to it

    l_records = (a_end_year - a_start_year);    
    for (i = 0; i < l_records; i++)
    {   
        sprintf(l_cmd, f_l_income_vs_expenses, a_file, 2015);
        l_fp = popen(l_cmd, "r");
        if (l_fp == NULL)
        {
            fprintf(stderr, "Could not open pipe to ledger command.\n");
            exit(1);
        }

        while (fgets(l_input, INPUT_MAX, l_fp) != NULL)
        {
            if (strlen(l_dat_info) <= 0)
                sprintf(l_dat_info, "%s", trim_whitespace(l_input));
            else
                sprintf(l_dat_info, "%s %s", l_dat_info, trim_whitespace(l_input));
        }
        l_d1 = strtod(l_dat_info, &l_tmp);
        l_d2 = strtod(l_tmp, &l_tmp);
        l_d3 = strtod(l_tmp, NULL);
        
        if (pclose(l_fp) == -1)
            fprintf(stderr, "Error reported by pclose().\n");
            
        // TODO: check if we need to perform operations on these values?
        fprintf(a_file, "%.2lf %.2lf .2lf\n", l_d1, l_d2, l_d3); //Write the data to a temporary file
    }
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

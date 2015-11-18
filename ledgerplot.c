/* See LICENSE.txt for license and copyright information. */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ledgerplot.h"
#include "docopt.c"
#include "c_generic/functions.c"
#include "modules/income_vs_expenses.c"

#define VERSION "version 0.1"
#define CMD_GNUPLOT "gnuplot -persist"
#define FILE_DATA_TMP "lp_data.tmp"
#define FILE_GNU_TMP "lp_gnu.tmp"
#define FILE_BARCHART "gp_barchart.gnu"

#define NUM_COMMANDS 2 // TODO: is dependent on type of graph I guess? e.g. ive / expenses per product / etc.

#define INPUT_LINE_MAX 1024 // MAX line length to copy
#define OUTPUT_ARRAY_MAX 8000 // MAX command length for the dat info


static int write_to_gnuplot(char a_gnu_command[OUTPUT_ARRAY_MAX][INPUT_LINE_MAX]);
static int get_lines_from_file(
    char *a_file,
    char a_gnu_command[OUTPUT_ARRAY_MAX][INPUT_LINE_MAX],
    int a_index
);


static char *f_cmd_gnuplot =
    "plot for [COL=STARTCOL:ENDCOL] 'lp_data.tmp' u COL:xtic(1) w histogram title columnheader(COL) lc rgb word(COLORS, COL-STARTCOL+1), for [COL=STARTCOL:ENDCOL] 'lp_data.tmp' u (column(0)+BOXWIDTH*(COL-STARTCOL+GAPSIZE/2+1)-1.0):COL:COL notitle w labels textcolor rgb \"gold\"";

/*
 * Main
 */
int main(int argc, char *argv[])
{
    FILE *l_output_file; // Temp dat file, where the final script is written to.
    int l_start_year;
    int l_end_year;
    int l_lines = 0;
    int l_lines_total = 0;
    char l_gnu_command[OUTPUT_ARRAY_MAX][INPUT_LINE_MAX];

    /*
     * Parse arguments
     */
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
    
    /*
     * Prepare temporary data file
     */
    l_output_file = fopen(FILE_DATA_TMP, "w");
    if (l_output_file == NULL)
    {
        printf("Error: could not open output file %s.\n", FILE_DATA_TMP);
        exit(1);
    }
    // TODO: switch with extra parameter, to determine the type of graph to export
    // In a first fase, we could also generate all of them?
    /* income vs expenses */
    if (prepare_temp_file_ive(args.file, l_output_file, l_start_year, l_end_year) != 0)
    {
        fprintf(stderr, "Could not prepare temporary data-file %s.", FILE_DATA_TMP);
        exit(1);
    }
    /* expenses per category */
    /* dividend ... */
    /* ...*/
    fclose(l_output_file);
   
    /*
     * Load layout commands
     */ 
    memset(l_gnu_command, '\0', OUTPUT_ARRAY_MAX*INPUT_LINE_MAX*sizeof(char));
    l_lines = get_lines_from_file(FILE_IVE_LAYOUT, l_gnu_command, 0);
    l_lines_total = l_lines;
    if (l_lines == -1)
    {
        fprintf(stderr, "Could not read %s.\n", FILE_IVE_LAYOUT);
        exit(1);
    }
    
    /*
     * Load barchart commands
     */ 
    l_lines = get_lines_from_file(FILE_BARCHART, l_gnu_command, l_lines_total);
    l_lines_total += l_lines;
    if ( l_lines == -1)
    {
        fprintf(stderr, "Could not read %s.\n", FILE_BARCHART);
        exit(1);
    }
    /*
     * Load barchart plot command
     */
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
    /*
     * Plot data
     */
    printf("%s:\n", FILE_IVE_LAYOUT);
    printf(">>> Generated using gnuplot chart info from %s.\n", FILE_BARCHART);
    printf(">>> Data exported to png.\n");
    printf(">>> The filename was specified in %s.\n", FILE_IVE_LAYOUT);
    return write_to_gnuplot(l_gnu_command);
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

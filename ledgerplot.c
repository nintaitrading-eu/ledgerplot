/* See LICENSE.txt for license and copyright information. */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h> /* For the sleep function. */
#include "ledgerplot.h"
#include "docopt.c"
#include "c_generic/functions.h"
#include "enum.h"
#include "modules/income_vs_expenses.h"

#define CMD_GNUPLOT "gnuplot -persist"
#define FILE_DATA_TMP "lp_data.tmp"
#define FILE_MERGED_TMP "lp_merged.tmp"
#define FILE_BARCHART "/usr/local/share/ledgerplot/gnuplot/gp_barchart.gnu"


static int prepare_data_file(
    const char *a_file,
    enum enum_plot_type_t a_plot_type,
    enum enum_plot_timeframe_t a_plot_timeframe,
    int a_start_year,
    int a_end_year
);
static int write_to_gnuplot(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]);
static int get_lines_from_file(
    const char *a_file,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE],
    int *a_lines_total
);
static int append_content_to_file(const char *a_src, const char *a_dst);
static int merge_data_files(uint32_t a_nargs, ...);
static int load_data(
    int *a_lines_total,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
);
static int remove_tmp_file(const char *a_file_name);


static const char *f_file_ive_layout =
    "/usr/local/share/ledgerplot/gnuplot/gp_income_vs_expenses.gnu";
static char *f_cmd_gnuplot =
    "plot for [COL=STARTCOL:ENDCOL] 'lp_data.tmp' u COL:xtic(1) w histogram title columnheader(COL) lc rgb word(COLORS, COL-STARTCOL+1), for [COL=STARTCOL:ENDCOL] 'lp_data.tmp' u (column(0)+BOXWIDTH*(COL-STARTCOL+GAPSIZE/2+1)-1.0):COL:COL notitle w labels textcolor rgb \"gold\"";

enum enum_return_generic_t
{
    SUCCEEDED,
    FAILED
    /* Other error codes can go here... */
};

/*
 * Main
 */
int main(int argc, char *argv[])
{
    uint32_t l_start_year;
    uint32_t l_end_year;
    int32_t l_lines = 0;
    int32_t l_lines_total = 0;
    char l_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE];
    uint32_t l_status = 0;
    enum enum_plot_type_t l_plot_type;
    enum enum_plot_timeframe_t l_plot_timeframe;

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
        return EXIT_FAILURE;
    }

    l_start_year = atoi(args.startyear);
    l_end_year = atoi(args.endyear);

    // TODO: get l_plot_type from parameters
    // TODO: get l_plot_timeframe from parameters
    l_plot_type = income_vs_expenses;
    l_plot_timeframe = yearly;
    if (prepare_data_file(args.file, l_plot_type, l_plot_timeframe, l_start_year, l_end_year) != SUCCEEDED)
        return EXIT_FAILURE;

    if (merge_data_files(3, f_file_ive_layout,  FILE_DATA_TMP, FILE_DATA_BARCHART) != SUCCEEDED)
       return EXIT_FAILURE;

    if (load_data(&l_lines_total, l_gnu_command) != SUCCEEDED)
        return EXIT_FAILURE;

    /*
     * Load barchart plot command
     */
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
    printf("%s:\n", f_file_ive_layout);
    printf(">>> Generated using gnuplot chart info from %s.\n", FILE_BARCHART);
    l_status = write_to_gnuplot(l_gnu_command);
    printf(">>> Data exported to png.\n");
    printf(">>> The filename was specified in %s.\n", f_file_ive_layout);
    /*
     * Cleanup tmp files.
     */
    printf(">>> Cleaning up temporary file(s)...\n");
    sleep(3); /* Give gnuplot time to read from the temporary file. */
    if (remove(FILE_DATA_TMP) != 0)
    {
        fprintf(stderr, "Could not delete file %s.\n", FILE_DATA_TMP);
        l_status = FAILED;
    }
    printf(">>> Done.\n");
    return l_status; // EXIT_FAILURE when l_status is 0. // TODO: return error code from enum?
}


/*
 * load_data:
 * Load data from merged file with layout data, gnuplot data and gnuplot commands.
 */
static int load_data(
    int *a_lines_total,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
)
{
    if (get_lines_from_file(FILE_MERGED_TMP, a_gnu_command, &a_lines_total) != SUCCEEDED)
    {
        fprintf(stderr, "Could not read %s.\n", FILE_MERGED_TMP);
        return FAILED;
    }
}

/*
 * remove_tmp_file:
 * Remove given tmp file.
 */
static int remove_tmp_file(const char *a_file_name)
{
    if (remove(a_file_name) != 0)
    {
        fprintf(stderr, "Could not delete file %s.\n", a_file_name);
        return FAILED;
    }
    return SUCCEEDED;
}

/*
 * merge_data_files:
 * Load layout, data and gnuplot specific file-data into one temporary file we can plot from.
 */
static int merge_data_files(uint32_t a_nargs, ...)
{
    FILE *l_output_file; // Temp dat file, where the merged data is written to.
    l_output_file = fopen(FILE_MERGED_TMP, "w");
    if (l_output_file == NULL)
    {
        printf("Error: could not open merge-output file %s.\n", FILE_MERGED_TMP);
        return FAILED;
    }

    register int l_i;
    va_list l_ap;
    char *l_current;
    uint32_t l_status = SUCCEEDED;
    va_start(l_ap, a_nargs);
    for (l_i = 0; l_i < a_nargs; l_i++)
    {
         l_current = va_arg(l_ap, char *);
         if(append_content_to_file(l_current) != SUCCEEDED)
            l_status = FAILED;
    }
    va_end(l_ap);
    return l_status;
}

/*
 * load_layout_commands:
 * Load layout commands from gnuplot file with layout data.
 */
/*static int load_layout_commands(
    int *a_lines,
    int *a_lines_total,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
)
{
    memset(a_gnu_command, '\0', MS_OUTPUT_ARRAY*MS_INPUT_LINE*sizeof(char));
    a_lines = get_lines_from_file(f_file_ive_layout, a_gnu_command, 0);
    a_lines_total = a_lines;
    if (l_lines == -1)
    {
        fprintf(stderr, "Could not read %s.\n", f_file_ive_layout);
        return FAILED;
    }
    return SUCCEEDED;
}*/

/*
 * prepare_data_file:
 * Prepare temporary data file
 */
static int prepare_data_file(
    const char *a_file,
    enum enum_plot_type_t a_plot_type,
    enum enum_plot_timeframe_t a_plot_timeframe,
    int a_start_year,
    int a_end_year
)
{
    FILE *l_output_file; // Temp dat file, where the final script is written to.
    bool l_status;

    l_output_file = fopen(FILE_DATA_TMP, "w");
    if (l_output_file == NULL)
    {
        printf("Error: could not open output file %s.\n", FILE_DATA_TMP);
        return FAILED;
    }
    l_status = SUCCEEDED;
    switch(a_plot_type)
    {
        case income_vs_expenses:
            if (ive_prepare_temp_file(a_file, l_output_file, a_start_year, a_end_year, a_plot_timeframe) != 0)
            {
                fprintf(stderr, "Could not prepare temporary data-file %s.", FILE_DATA_TMP);
                l_status = FAILED;
            };
            break;
        /* expenses per category */
        /* dividend ... */
        /* ...*/
        default:
            fprintf(stderr, "Unknown plot type %s.\n", string_plot_type_t[income_vs_expenses]);
            l_status = FAILED;
    }
    fclose(l_output_file);
    return l_status;
}

/*
 * write_to_gnuplot:
 * Writes the generated script lines to a gnuplot pipe.
 */
static int write_to_gnuplot(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE])
{
    FILE *l_gp; // Gnuplot pipe

     /*
     * Opens an interface that one can use to send commands as if they were typing into the gnuplot command line.
     * The "-persistent" keeps the plot open even after your C program terminates.
     */
    l_gp = popen(CMD_GNUPLOT, "w");
    if (l_gp == NULL)
    {
        printf("Error opening pipe to GNU plot. Check if you have it!\n");
        fclose(l_gp);
        return FAILED;
    }

    for (uint32_t i = 0; i < MS_OUTPUT_ARRAY; i++)
    {
        if (strncmp(a_gnu_command[i], "", MS_INPUT_LINE) != 0)
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
    return SUCCEEDED;
}

/*
 * append_content_to_file:
 * Reads a file and appends it's non-comment and non-empty lines output to the tmp merge file.
 */
static int append_content_to_file(const char *a_src, const char *a_dst)
{
    FILE *l_src;
    FILE *l_dst;
    char l_line[MS_INPUT_LINE];
    uint32_t l_count = 0;

    l_src = fopen(a_src, "r");
    if (l_src == NULL)
    {
        printf("Error: could not open source file %s.\n", a_src);
        return FAILED;
    }

    l_dst = fopen(a_dst, "a");
    if (l_dst == NULL)
    {
        fprintf(stderr, "Error: could not open source file %s.\n", a_src);
        fclose(l_src); // Note: Was already opened.
        return FAILED;
    }

    while (fgets(l_line, MS_INPUT_LINE, l_src) != NULL)
    {
        if (
            (strlen(l_line) > 0)
            && (l_line[0] != '#')
        )
        {
            l_count++;
            trim_whitespace(l_line_temp, l_line, MS_INPUT_LINE);
            fprintf(l_dst, "%s\n", l_line_temp);
        }
    }
    fclose(l_dst);
    fclose(l_src);
    fprintf(stdout, "%d lines copied from %s to %s.\n", l_count, a_src, a_dst);
    return SUCCEEDED;
}

/*
 * get_lines_from_file
 * Loads the lines of a file into an array that will be used to send to gnuplot. It returns an integer for the number
 * of lines read.
 */
static int get_lines_from_file(const char *a_file, char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE], int *a_lines_total)
{
    FILE *l_file;
    char l_line[MS_INPUT_LINE];
    char l_line_temp[MS_INPUT_LINE];
    uint32_t l_count = 0;

    l_file = fopen(a_file, "r");
    if (l_file == NULL)
    {
        printf("Error: could not open output file %s.\n", a_file);
        return FAILED;
    }
    while (fgets(l_line, MS_INPUT_LINE, l_file) != NULL)
    {
        if (
            (strlen(l_line) > 0)
            && (l_line[0] != '#')
        )
        {
            l_count++;
            trim_whitespace(l_line_temp, l_line, MS_INPUT_LINE);
            sprintf(a_gnu_command[a_lines_total + l_count - 1], "%s", l_line_temp);
        }
    }
    a_lines_total += l_count;
    fclose(l_file);
    return l_count;
}

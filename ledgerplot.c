/* See LICENSE.txt for license and copyright information. */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h> /* For the sleep function. */
#include <stdarg.h>
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
static int get_lines_from_file(const char *a_file, char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE],
    int *a_lines_total);
static int merge_data_files(uint32_t a_nargs, ...);
static int load_data(int *a_lines_total, char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]);
static int append_plot_cmd(int *a_lines_total, char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]);
static int plot_data(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE], const char *a_file_layout,
    const char *a_file_chart_cmd);
static int remove_tmp_files(uint32_t a_nargs, ...);
static int write_to_gnuplot(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]);
static int append_content_to_file(const char *a_src, const char *a_dst);


static const char *f_file_ive_layout =
    "/usr/local/share/ledgerplot/gnuplot/gp_income_vs_expenses.gnu";
static char *f_cmd_gnuplot_barchart =
    "plot for [COL=STARTCOL:ENDCOL] '%s' u COL:xtic(1) w histogram title columnheader(COL) lc rgb word(COLORS, COL-STARTCOL+1), for [COL=STARTCOL:ENDCOL] '%s' u (column(0)+BOXWIDTH*(COL-STARTCOL+GAPSIZE/2+1)-1.0):COL:COL notitle w labels textcolor rgb \"gold\"";

/*
 * Main
 */
int main(int argc, char *argv[])
{
    uint32_t l_start_year;
    uint32_t l_end_year;
    int32_t l_lines_total = 0;
    char l_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE];
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

    /*
     * Preparation of data.
     */
    uint32_t l_status = EXIT_SUCCESS; // Note: To make sure the cleanup runs.
    printf(">>> Preparing data...\n");
    if (prepare_data_file(args.file, l_plot_type, l_plot_timeframe, l_start_year, l_end_year) != SUCCEEDED)
        l_status = EXIT_FAILURE;
    printf(">>> Preparation %s.\n", string_return_status(l_status));

    printf(">>> Merging data...\n");
    if ((l_status != EXIT_FAILURE)
        && (merge_data_files(2, f_file_ive_layout, FILE_BARCHART) != SUCCEEDED))
        l_status = EXIT_FAILURE;
    printf(">>> Merging %s.\n", string_return_status(l_status));

    printf(">>> Loading data...\n");
    if ((l_status != EXIT_FAILURE)
        && (load_data((int *)l_lines_total, l_gnu_command) != SUCCEEDED))
        l_status = EXIT_FAILURE;
    printf(">>> Loading %s.\n", string_return_status(l_status));

    printf(">>> Appending plot cmd...\n");
    if ((l_status != EXIT_FAILURE)
        && (append_plot_cmd((int *)l_lines_total, l_gnu_command) != SUCCEEDED))
        l_status = EXIT_FAILURE;
    printf(">>> Appending %s.\n", string_return_status(l_status));

    /*for (int i=0; i<l_lines_total+2; i++)
    {
        printf("-- %s\n", l_gnu_command[i]);
    }*/

    /*
     * Plot data
     */
    printf(">>> Plotting...\n");
    if ((l_status != EXIT_FAILURE)
        && (plot_data(l_gnu_command, f_file_ive_layout, FILE_BARCHART) != SUCCEEDED))
        l_status = EXIT_FAILURE;
    printf(">>> Plotting %s.\n", string_return_status(l_status));
    /*
     * Cleanup tmp files.
     */
    sleep(3); /* Give gnuplot time to read from the temporary file. */
    if (remove_tmp_files(2, FILE_DATA_TMP, FILE_MERGED_TMP) != SUCCEEDED)
        l_status = EXIT_FAILURE;
    printf(">>> Done.\n");
    return l_status;
}

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
    uint32_t l_status;

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
            if (ive_prepare_temp_file(a_file, l_output_file, a_start_year, a_end_year, a_plot_timeframe) != SUCCEEDED)
            {
                fprintf(stderr, "Could not prepare temporary data-file %s.\n", FILE_DATA_TMP);
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
 * merge_data_files:
 * Load layout, data and gnuplot specific file-data into one temporary file we can plot from.
 */
static int merge_data_files(uint32_t a_nargs, ...)
{
    register int l_i;
    va_list l_ap;
    char *l_current;
    uint32_t l_status = SUCCEEDED;
    printf(">>> Merging...\n");
    va_start(l_ap, a_nargs);
    for (l_i = 0; l_i < a_nargs; l_i++)
    {
        l_current = va_arg(l_ap, char *);
        printf(">>> [%s] ", l_current);
        if(append_content_to_file(l_current, FILE_MERGED_TMP) != SUCCEEDED)
        {
            printf(" [FAIL]");
            l_status = FAILED;
        }
        else
            printf(" [OK]");
        printf("\n");
    }
    va_end(l_ap);
    printf(">>> Merging done.\n");
    return l_status;
}

/*
 * load_data:
 * Load data from merged file with layout data and gnuplot commands.
 */
static int load_data(
    int *a_lines_total,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
)
{
    memset(a_gnu_command, '\0', MS_OUTPUT_ARRAY*MS_INPUT_LINE*sizeof(char));
    printf("test load_data 1 - a_lines_total = %d\n", a_lines_total);
    if (get_lines_from_file(FILE_MERGED_TMP, a_gnu_command, a_lines_total) != SUCCEEDED)
    {
        printf("test load_data 2 - a_lines_total = %d\n", a_lines_total);
        fprintf(stderr, "Could not read %s.\n", FILE_MERGED_TMP);
        return FAILED;
    }
    printf("test load_data 3 - a_lines_total = %d\n", a_lines_total);
    return SUCCEEDED;
}

/*
 * append_plot_cmd:
 * Append a line to the command array, with the actual plot command.
 */
static int append_plot_cmd(
    int *a_lines_total,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
)
{
    // TODO: different command, according to the plot-type?
    /*
     * Load barchart plot command
     */
    printf("test append_plot_cmd - f_cmd_gnuplot_barchart = %s\n", f_cmd_gnuplot_barchart);
    printf("test append_plot_cmd - a_lines_total = %d\n", a_lines_total);
    sprintf(
        a_gnu_command[(int)a_lines_total - 1],
        f_cmd_gnuplot_barchart,
        FILE_DATA_TMP,
        FILE_DATA_TMP
    );
    return SUCCEEDED;
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
    char l_line_temp[MS_INPUT_LINE];
    uint32_t l_count = 0;

    l_src = fopen(a_src, "r");
    if (l_src == NULL)
    {
        fprintf(stderr, "Error: could not open source file %s.\n", a_src);
        return FAILED;
    }

    l_dst = fopen(a_dst, "a");
    if (l_dst == NULL)
    {
        fprintf(stderr, "Error: could not open destination file %s.\n", a_dst);
        fclose(l_dst); // Note: Was already opened.
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
    fprintf(stdout, "%d lines copied from %s to %s.", l_count, a_src, a_dst);
    return SUCCEEDED;
}

/*
 * plot_data:
 * Plot the data and display information about what's going on.
 */
static int plot_data(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE], const char *a_file_layout,
    const char *a_file_chart_cmd)
{
    printf("%s:\n", a_file_layout);
    printf(">>> Generated using gnuplot chart info from %s.\n", a_file_chart_cmd);
    if (write_to_gnuplot(a_gnu_command) != SUCCEEDED)
    {
        printf(">>> Error exporting data to png!\n");
        return FAILED;
    }
    else
    {
        printf(">>> Data exported to png.\n");
        printf(">>> The filename was specified in %s.\n", f_file_ive_layout);
    }
    return SUCCEEDED;
}

/*
 * remove_tmp_files:
 * Remove given tmp files.
 */
static int remove_tmp_files(uint32_t a_nargs, ...)
{
    register int l_i;
    va_list l_ap;
    char *l_current;
    uint32_t l_status = SUCCEEDED;

    return SUCCEEDED;
    printf(">>> Cleaning up temporary file(s)...\n");
    va_start(l_ap, a_nargs);
    for (l_i = 0; l_i < a_nargs; l_i++)
    {
         l_current = va_arg(l_ap, char *);
        if (remove(l_current) != 0)
        {
            fprintf(stderr, "Could not delete file %s.\n", l_current);
            l_status = FAILED;
        }
    }
    va_end(l_ap);
    printf(">>> Cleaning done.\n");
    return l_status;
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
    /* Note: l_count < MS_OUTPUT_ARRAY -> leave 1 row for the plot command, that is added in a final step. */
    while ((fgets(l_line, MS_INPUT_LINE, l_file) != NULL) && (l_count < MS_OUTPUT_ARRAY))
    {
        if (
            (strlen(l_line) > 0)
            && (l_line[0] != '#')
        )
        {
            l_count++;
            trim_whitespace(l_line_temp, l_line, MS_INPUT_LINE);
            sprintf(a_gnu_command[(int)a_lines_total + l_count - 1], "%s", l_line_temp);
        }
    }
    printf("test get_lines_from_file 1 - l_count = %d\n", l_count);
    a_lines_total += l_count;
    fclose(l_file);
    printf("test get_lines_from_file 2 - a_lines_total = %d\n", a_lines_total);
    return l_count;
}

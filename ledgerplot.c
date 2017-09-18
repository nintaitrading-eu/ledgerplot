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
#include "c_generic/enum.h"
#include "enum.h"
#include "struct.h"
#include "modules/income_vs_expenses.h"
#include "modules/cashflow.h"
#include "modules/wealthgrowth.h"

#define CMD_GNUPLOT "gnuplot -persist"
#ifndef NDEBUG
#define FILE_DATA0_TMP "/var/tmp/lp_data0.tmp"
#define FILE_DATA1_TMP "/var/tmp/lp_data1.tmp"
//#define FILE_MERGED_TMP "/var/tmp/lp_merged.tmp"
#else
#define FILE_DATA0_TMP "lp_data0.tmp"
#define FILE_DATA1_TMP "lp_data1.tmp"
//#define FILE_MERGED_TMP "lp_merged.tmp"
#endif

static uint32_t prepare_data(
    const char *a_file,
    uint32_t *a_verbose, 
    plot_object *a_plot_object);
static uint32_t get_lines_from_file(
    const char *a_file,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE],
    uint32_t *a_lines_total);
//static uint32_t merge_data_files(uint32_t *a_verbose, uint32_t a_nargs, ...);
static uint32_t append_plot_cmd(
    uint32_t *a_lines_total,
    enum enum_plot_type_t a_plot_type,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]);
static uint32_t plot_data(
    uint32_t *a_verbose,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE],
    const char *a_file_chart_cmd);
static uint32_t remove_tmp_files(uint32_t *a_verbose, uint32_t a_nargs, ...);
static uint32_t write_to_gnuplot(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]);
//static uint32_t append_content_to_file(uint32_t *a_verbose, const char *a_src, const char *a_dst);
enum enum_plot_type_t get_plot_type_from_args(DocoptArgs args);
enum enum_plot_timeframe_t get_plot_timeframe_from_args(DocoptArgs args);

#ifndef NDEBUG
static const char *f_gnuplot_cashflow = "/usr/local/share/ledgerplot/gnuplot/gp_cashflow.gnu";
static const char *f_gnuplot_wealthgrowth = "/usr/local/share/ledgerplot/gnuplot/gp_wealthgrowth.gnu";
static const char *f_gnuplot_ipc = "/usr/local/share/ledgerplot/gnuplot/gp_income_per_category.gnu";
static const char *f_gnuplot_epc = "/usr/local/share/ledgerplot/gnuplot/gp_expenses_per_category.gnu";
#else
static const char *f_gnuplot_cashflow = "gnuplot/gp_cashflow.gnu";
static const char *f_gnuplot_wealthgrowth = "gnuplot/gp_wealthgrowth.gnu";
static const char *f_gnuplot_ipc = "gnuplot/gp_income_per_category.gnu";
static const char *f_gnuplot_epc = "gnuplot/gp_expenses_per_category.gnu";
#endif
static char *f_gnuplot_cashflow_cmd = "plot '%s' using 1:2 with filledcurves x1 title \"Income\" linecolor rgb \"#dc322f\", '' using 1:2:2 with labels font \"Liberation Mono,10\" offset 0,0.5 textcolor linestyle 0 notitle, '%s' using 1:2 with filledcurves y1=0 title \"Expenses\" linecolor rgb \"#859900\", '' using 1:2:2 with labels font \"Liberation Mono,10\" offset 0,0.5 textcolor linestyle 0 notitle";
static char *f_gnuplot_wealthgrowth_cmd = "plot '%s' using 1:2 with filledcurves x1 title \"Assets\" linecolor rgb \"#dc322f\", '' using 1:2:2 with labels font \"Liberation Mono,10\" offset 0,0.5 textcolor linestyle 0 notitle, '%s' using 1:2 with filledcurves y1=0 title \"Liabilities\" linecolor rgb \"#859900\", '' using 1:2:2 with labels font \"Liberation Mono,10\" offset 0,0.5 textcolor linestyle 0 notitle";

/*
 * Main
 */
int main(int argc, char *argv[])
{
    char l_gnuplot_instructions[MS_OUTPUT_ARRAY][MS_INPUT_LINE];
    uint32_t l_verbose = 0;
    uint32_t l_status = EXIT_SUCCESS; // Note: To make sure the cleanup runs.
    plot_object l_plot_object;

    /*
     * Parse arguments
     */
    DocoptArgs args = docopt(
        argc,
        argv,
        1, /* help */
        LEDGERPLOT_VERSION /* version */
    );

    if (argc == 1)
    {
        printf("%s\n", args.help_message);
        return EXIT_FAILURE;
    }

    l_verbose = args.verbose ? 1 : 0;

    /*
     * Prepare the plot_object.
     */

    strncpy(args.period, l_plot_object.period, MS_MAX_PARM);
    l_plot_object.plot_type = get_plot_type_from_args(args); /* default: income_vs_expenses */
    l_plot_object.plot_timeframe = get_plot_timeframe_from_args(args); /* default: yearly */

    /*
     * Prepare data
     */
    print_if_verbose(&l_verbose, ">>> Plotting %s...\n", string_plot_type_t[l_plot_object.plot_type]);
    if ((l_status != EXIT_FAILURE)
        && (prepare_data(args.file, &l_verbose, &l_plot_object) != SUCCEEDED))
        l_status = EXIT_FAILURE;
    print_if_verbose(&l_verbose, ">>> Plotting %s.\n", string_return_status(l_status));

    /*
     * Assemble gnuplot instructions
     */
    // TODO: combine data from the temp files + plot command and use it to plot.
    // l_gnuplot_instructions

    /* Plot data
     *
     */
    // TBD

    /*
     * Cleanup tmp files.
     */
    sleep(3); /* Give gnuplot time to read from the temporary file. */
    if (remove_tmp_files(&l_verbose, 2, FILE_DATA0_TMP, FILE_DATA1_TMP) != SUCCEEDED)
        l_status = EXIT_FAILURE;
    print_if_verbose(&l_verbose, ">>> Done.\n");
    return l_status;
}

/*
 * get_plot_type_from_args:
 * Return plot_type, based on given option.
 */
enum enum_plot_type_t get_plot_type_from_args(DocoptArgs args)
{
    if (args.cashflow)
    {
        return cashflow;
    }
    else if (args.wealthgrowth)
    {
        return wealthgrowth;
    }
    else if (args.income_per_category)
    {
        return income_per_category;
    }
    else if (args.expenses_per_category)
    {
        return expenses_per_category;
    }
    else
        return income_vs_expenses;
};

/*
 * get_plot_timeframe_from_args:
 * Return plot_timeframe, based on given option.
 */
enum enum_plot_timeframe_t get_plot_timeframe_from_args(DocoptArgs args)
{
    if (args.quarterly)
    {
        return quarterly;
    }
    else if (args.monthly)
    {
        return monthly;
    }
    else if (args.weekly)
    {
        return weekly;
    }
    else
        return yearly;
}

/*
 * prepare_data:
 * Prepare data, for given plot_type and timeframe.
 */
static uint32_t prepare_data(
    const char *a_file,
    uint32_t *a_verbose, 
    plot_object *a_plot_object)
{
    FILE *l_data0_tmp; // Temp dat file, where the data is written to.
    FILE *l_data1_tmp; // Temp dat file, where the data is written to.
    uint32_t l_status;

    l_data0_tmp = fopen(FILE_DATA0_TMP, "w");
    l_data1_tmp = fopen(FILE_DATA1_TMP, "w");
    if (l_data0_tmp == NULL)
    {
        fprintf(stderr, "Error in prepare_data: could not open output file %s.\n", FILE_DATA0_TMP);
        fclose(l_data0_tmp);
        return FAILED;
    }
    if (l_data1_tmp == NULL)
    {
        fprintf(stderr, "Error in prepare_data: could not open output file %s.\n", FILE_DATA1_TMP);
        fclose(l_data1_tmp);
        return FAILED;
    }
    l_status = SUCCEEDED;
    switch(a_plot_object->plot_type)
    {
        case income_vs_expenses:
            // TODO: use a_plot_object
            if (ive_prepare_data(a_file, l_data0_tmp, l_data1_tmp, &a_plot_object) != SUCCEEDED)
            {
                fprintf(stderr, "Error in prepare_data: ive_prepare__data failed.\n");
                l_status = FAILED;
            };
            break;
        case cashflow:
            // TODO:
            break;
        case wealthgrowth:
            // TODO:
            break;
        case income_per_category:
            // TODO:
            break;
        case expenses_per_category:
            // TODO:
            break;
        default:
            fprintf(stderr, "Error in prepare_data_file: Unknown plot type %s.\n", string_plot_type_t[a_plot_object->plot_type]);
            l_status = FAILED;
    }
    fclose(l_data0_tmp);
    fclose(l_data1_tmp);
    return l_status;
}

/*static uint32_t merge_data_files(uint32_t *a_verbose, uint32_t a_nargs, ...)
{
    register uint32_t l_i;
    va_list l_ap;
    char *l_current_file;
    uint32_t l_status = SUCCEEDED;
    va_start(l_ap, a_nargs);
    for (l_i = 0; l_i < a_nargs; l_i++)
    {
        l_current = va_arg(l_ap, char *);
        print_if_verbose(a_verbose, ">>> [%s] ", l_current_file);
        if (append_content_to_file(a_verbose, l_current_file, FILE_MERGED_TMP) != SUCCEEDED)
        {
            print_if_verbose(a_verbose, " [FAIL]");
            l_status = FAILED;
        }
        else
            print_if_verbose(a_verbose, " [OK]");
        print_if_verbose(a_verbose, "\n");
    }
    va_end(l_ap);
    return l_status;
}*/

/*
 * append_plot_cmd:
 * Append a line to the command array, with the actual plot command.
 */
static uint32_t append_plot_cmd(
    uint32_t *a_lines_total,
    enum enum_plot_type_t a_plot_type,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
)
{
    /*
     * Load barchart plot command
     */
    switch(a_plot_type)
    {
        // TODO: plot command must also move to the modules, so this function might become obsolete. Just use the sprintf statements in each module.
        case income_vs_expenses:
            sprintf(
                a_gnu_command[*a_lines_total - 1],
                f_gnuplot_ive_cmd,
                FILE_DATA0_TMP,
                FILE_DATA0_TMP);
            break;
        case cashflow:
            sprintf(
                a_gnu_command[*a_lines_total - 1],
                f_gnuplot_cashflow_cmd,
                FILE_DATA0_TMP,
                FILE_DATA1_TMP);
            break;
        case wealthgrowth:
            sprintf(
                a_gnu_command[*a_lines_total - 1],
                f_gnuplot_wealthgrowth_cmd,
                FILE_DATA0_TMP,
                FILE_DATA1_TMP);
            break;
        case income_per_category:
            break;
        case expenses_per_category:
            break;
        default:
            fprintf(stderr, "Error in append_plot_cmd: Unknown plot type %s.\n", string_plot_type_t[a_plot_type]);
    }
    return SUCCEEDED;
}

/*
 * write_to_gnuplot:
 * Writes the generated script lines to a gnuplot pipe.
 */
static uint32_t write_to_gnuplot(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE])
{
    FILE *l_gp; // Gnuplot pipe

     /*
     * Opens an interface that one can use to send commands as if they were typing into the gnuplot command line.
     * The "-persistent" keeps the plot open even after your C program terminates.
     */
    l_gp = popen(CMD_GNUPLOT, "w");
    if (l_gp == NULL)
    {
        fprintf(stderr, "Error opening pipe to GNU plot. Check if you have it!\n");
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
/*static uint32_t append_content_to_file(uint32_t *a_verbose, const char *a_src, const char *a_dst)
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
    print_if_verbose(a_verbose, "%d lines copied from %s to %s.", l_count, a_src, a_dst);
    return SUCCEEDED;
}*/

/*
 * plot_data:
 * Plot the data and display information about what's going on.
 */
static uint32_t plot_data(uint32_t *a_verbose, char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE],
    const char *a_file_chart_cmd)
{
    print_if_verbose(a_verbose, ">>> Generated using gnuplot chart info from %s.\n", a_file_chart_cmd);
    if (write_to_gnuplot(a_gnu_command) != SUCCEEDED)
    {
        fprintf(stderr, ">>> Error exporting data to png!\n");
        return FAILED;
    }
    else
    {
        print_if_verbose(a_verbose, ">>> Data exported to png.\n");
    }
    return SUCCEEDED;
}

/*
 * remove_tmp_files:
 * Remove given tmp files.
 */
static uint32_t remove_tmp_files(uint32_t *a_verbose, uint32_t a_nargs, ...)
{
    register uint32_t l_i;
    va_list l_ap;
    char *l_current;
    uint32_t l_status = SUCCEEDED;

    print_if_verbose(a_verbose, ">>> Cleaning up temporary file(s)...\n");
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
    print_if_verbose(a_verbose, ">>> Cleaning done.\n");
    return l_status;
}

/*
 * get_lines_from_file
 * Loads the lines of a file into an array that will be used to send to gnuplot. It returns an integer for the number
 * of lines read.
 */
static uint32_t get_lines_from_file(const char *a_file, char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE],
    uint32_t *a_lines_total)
{
    FILE *l_file;
    char l_line[MS_INPUT_LINE];
    char l_line_temp[MS_INPUT_LINE];
    uint32_t l_count = 0;

    l_file = fopen(a_file, "r");
    if (l_file == NULL)
    {
        fprintf(stderr, "Error: could not open output file %s.\n", a_file);
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
            sprintf(a_gnu_command[*a_lines_total + l_count - 1], "%s", l_line_temp);
        }
    }
    *a_lines_total += l_count;
    fclose(l_file);
    return SUCCEEDED;
}

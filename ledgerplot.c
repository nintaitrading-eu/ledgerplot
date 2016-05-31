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
#define FILE_BARCHART "/usr/local/share/ledgerplot/gnuplot/gp_barchart.gnu"


static int prepare_data_file();
static int write_to_gnuplot(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]);
static int get_lines_from_file(
    const char *a_file,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE],
    int a_index
);
static int load_data(
    int *l_lines,
    int *l_lines_total
);
static int load_layout_commands(
    int *l_lines,
    int *l_lines_total,
    char l_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
);
static int load_barchart_commands(
    int *l_lines,
    int *l_lines_total,
    char l_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
);


static const char *f_file_ive_layout =
    "/usr/local/share/ledgerplot/gnuplot/gp_income_vs_expenses.gnu";
static char *f_cmd_gnuplot =
    "plot for [COL=STARTCOL:ENDCOL] 'lp_data.tmp' u COL:xtic(1) w histogram title columnheader(COL) lc rgb word(COLORS, COL-STARTCOL+1), for [COL=STARTCOL:ENDCOL] 'lp_data.tmp' u (column(0)+BOXWIDTH*(COL-STARTCOL+GAPSIZE/2+1)-1.0):COL:COL notitle w labels textcolor rgb \"gold\"";

enum enum_return_generic_t
{
    FAILED,
    SUCCEEDED
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
    char l_gnu_command_layout[MS_OUTPUT_ARRAY][MS_INPUT_LINE];
    char l_gnu_command_data[MS_OUTPUT_ARRAY][MS_INPUT_LINE];
    char l_gnu_command_plot[MS_OUTPUT_ARRAY][MS_INPUT_LINE];
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

    if (!prepare_data_file())
        return EXIT_FAILURE;

    if (!load_data(
        &l_lines,
        &l_lines_total
    ))
       return EXIT_FAILURE;

    if (!load_layout_commands(
        &l_lines,
        &l_lines_total,
        l_gnu_command_layout
    ))
        return EXIT_FAILURE;

    if (!load_barchart_commands(
        &l_lines,
        &l_lines_total,
        l_gnu_command_data
    ))
       return EXIT_FAILURE;

    /*
     * Load barchart plot command
     */
    //if (!strncpy(l_gnu_command[l_lines_total + 1], f_cmd_gnuplot, INPUT_LINE_MAX))
    //    exit(1);
    sprintf(
        l_gnu_command_plot[l_lines_total - 1],
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
    return l_status; // EXIT_FAILURE when l_status is false. // TODO: return error code from enum?
}


/*
 * load_barchart_commands:
 * Load layout commands from gnuplot file with layout data,
 * with the barchart specific options.
 */
static int load_barchart_commands(
    int *a_lines,
    int *a_lines_total,
    char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE]
)
{
    a_lines += get_lines_from_file(FILE_BARCHART, a_gnu_command, *a_lines_total);
    a_lines_total += a_lines;
    if ( a_lines == -1)
    {
        fprintf(stderr, "Could not read %s.\n", FILE_BARCHART);
        return EXIT_FAILURE;
    }
}
/*
 * load_data:
 * Load layout, data and gnuplot specific options
 * into a temporary file we can plot from.
 */
static int load_data(
    int *a_lines,
    int *a_lines_total
)
{
    return FAILED;
}

/*
 * load_layout_commands:
 * Load layout commands from gnuplot file with layout data.
 */
static int load_layout_commands(
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
}

/*
 * prepare_data_file:
 * Prepare temporary data file
 */
static int prepare_data_file()
{
    FILE *l_output_file; // Temp dat file, where the final script is written to.
    bool l_status;

    l_output_file = fopen(FILE_DATA_TMP, "w");
    if (l_output_file == NULL)
    {
        printf("Error: could not open output file %s.\n", FILE_DATA_TMP);
        return false;
    }
    // TODO: get l_plot_type from parameters
    // TODO: get l_plot_timeframe from parameters
    l_status = true;
    l_plot_type = income_vs_expenses;
    l_plot_timeframe = yearly;
    switch(l_plot_type)
    {
        case income_vs_expenses:
            if (ive_prepare_temp_file(args.file, l_output_file, l_start_year, l_end_year, l_plot_timeframe) != 0)
            {
                fprintf(stderr, "Could not prepare temporary data-file %s.", FILE_DATA_TMP);
                l_status = false;
            };
            break;
        /* expenses per category */
        /* dividend ... */
        /* ...*/
        default:
            fprintf(stderr, "Unknown plot type %s.\n", string_plot_type_t[income_vs_expenses]);
            l_status = false;
    }
    fclose(l_output_file);
    return l_status;
}

/*
 * prepare_income_vs_expenses:
 * Calls the proper prepare function and return
 * wether it succeeded or not.
 */
static int prepare_income_vs_expenses()
{
}

/*
 * Writes the generated script lines to a
 * gnuplot pipe.
 */
static int write_to_gnuplot(char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE])
{
    FILE *l_gp; // Gnuplot pipe

     /*
     * Opens an interface that one can use to send commands as if they were typing into the
     * gnuplot command line. "The -persistent" keeps the plot open even after your
     * C program terminates.
     */
    l_gp = popen(CMD_GNUPLOT, "w");
    if (l_gp == NULL)
    {
        printf("Error opening pipe to GNU plot. Check if you have it!\n");
        return false;
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
    return true;
}

/*
 * get_lines_from_file
 * Loads the lines of a file into
 * an array that will be used
 * to send to gnuplot.
 * It returns an integer for the number of lines read.
 */
static int get_lines_from_file(const char *a_file, char a_gnu_command[MS_OUTPUT_ARRAY][MS_INPUT_LINE], int a_index)
{
    FILE *l_file;
    char l_line[MS_INPUT_LINE];
    char l_line_temp[MS_INPUT_LINE];
    uint32_t l_count = 0;

    l_file = fopen(a_file, "r");
    if (l_file == NULL)
    {
        printf("Error: could not open output file %s.\n", a_file);
        return EXIT_FAILURE;
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
            sprintf(a_gnu_command[a_index + l_count - 1], "%s", l_line_temp);
        }
    }
    fclose(l_file);
    return l_count;
}

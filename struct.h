/* See LICENSE.txt for license and copyright information. */
#pragma once

struct plot_object_t
{
    enum enum_plot_type_t plot_type;
    enum enum_plot_timeframe_t plot_timeframe;
    char *period;
    char gnuplot_data[MS_OUTPUT_ARRAY][MS_INPUT_LINE];
    char *gnuplot_command;
} plot_object_t;

typedef struct plot_object_t plot_object;

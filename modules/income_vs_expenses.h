/* See LICENSE.txt for license and copyright information. */
#pragma once

int ive_prepare_temp_file(
    const char *a_input_file,
    FILE *a_output_file,
    uint32_t a_start_year,
    uint32_t a_end_year,
    enum enum_plot_timeframe_t a_plot_timeframe_t
);

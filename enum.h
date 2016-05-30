/* See LICENSE.txt for license and copyright information. */
#pragma once



enum enum_plot_type_t
{
    income_vs_expenses
};

static const char* string_plot_type_t[] = {
    "income_vs_expenses"
};

enum enum_plot_timeframe_t
{
    yearly,
    monthly,
    weekly,
    daily
};

static const char* string_plot_timeframe[] = {
    "yearly",
    "monthly",
    "weekly",
    "daily"
};

typedef struct
{
    const char *enum_string;
    int enum_value;
} EnumValue;


int string_to_enumvalue(const char* a_string, const EnumValue *a_lst);

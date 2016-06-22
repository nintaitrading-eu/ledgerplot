/* See LICENSE.txt for license and copyright information. */
#pragma once



enum enum_plot_type_t
{
    income_vs_expenses
};

static const char* string_plot_type_t[] = {
    "income_vs_expenses"
};

enum enum_return_status_generic_t
{
    SUCCEEDED,
    FAILED
    /* Other error codes can go here... */
};

static inline const char *string_return_status(enum enum_return_status_generic_t a_return_status)
{
    static const char *l_return_values[] = {
        "SUCCEEDED",
        "FAILED"
        /* continue for rest of values */
    };
    return l_return_values[a_return_status];
};

enum enum_plot_timeframe_t
{
    yearly,
    monthly,
    weekly,
    daily
};

static inline const char *string_plot_timeframe(enum enum_plot_timeframe_t a_return_status)
{
    static const char *l_return_values[] = {
        "yearly",
        "monthly",
        "weekly",
        "daily"
    };
    return l_return_values[a_return_status];
};

/*typedef struct
{
    const char *enum_string;
    int enum_value;
} EnumValue;

int string_to_enumvalue(const char* a_string, const EnumValue *a_lst);
*/

/* See LICENSE.txt for license and copyright information. */

#include <string.h>
#include "enum.h"

/*
 * Note: The below code will give a month as an enum (integer value) and also
 * a lowercase string of the enum name.
 * Example usage:
 * printf("enum month of january as a string: %s\n", MONTH_STRING[january]);
 */
/*#define FOREACH_MONTH(MONTH) \
        MONTH(january)   \
        MONTH(february)  \
        MONTH(march)   \
        MONTH(april)  \
        MONTH(may)  \
        MONTH(june)  \
        MONTH(july)  \
        MONTH(august)  \
        MONTH(september)  \
        MONTH(october)  \
        MONTH(november)  \
        MONTH(december)
 */       
/*#define FOREACH_PLOT_TYPE(PLOT_TYPE) \
        PLOT_TYPE(income_vs_expenses)
        
#define FOREACH_PLOT_TIMEFRAME(PLOT_TIMEFRAME) \
        PLOT_TIMEFRAME(yearly) \
        PLOT_TIMEFRAME(monthly) \
        PLOT_TIMEFRAME(weeky) \
        PLOT_TIMEFRAME(daily)
*/

/*#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum enum_month_t {
    FOREACH_MONTH(GENERATE_ENUM)
};

const char *MONTH_STRING[] = {
    FOREACH_MONTH(GENERATE_STRING)
};
*/
/*enum enum_plot_type_t {
    FOREACH_PLOT_TYPE(GENERATE_ENUM)
};

const char *PLOT_TYPE_STRING[] = {
    FOREACH_PLOT_TYPE(GENERATE_STRING)
};

enum enum_plot_timeframe_t {
    FOREACH_PLOT_TIMEFRAME(GENERATE_ENUM)
};

const char *PLOT_TIMEFRAME_STRING[] = {
    FOREACH_PLOT_TIMEFRAME(GENERATE_STRING)
};
*/

/*int string_to_enumvalue(const char* a_string, const EnumValue *a_lst)
{
    int z_idx;
    for (z_idx = 0; a_lst[z_idx].enum_string; z_idx++)
    {
        if (strcmp(a_lst[z_idx].enum_string, a_string) == 0)
            return z_idx;
    }
    return -1;
};*/

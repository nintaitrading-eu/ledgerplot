/* See LICENSE.txt for license and copyright information. */

/*
 * Note: The below code will give a month as an enum (integer value) and also
 * a lowercase string of the enum name.
 * Example usage:
 * printf("enum month of january as a string: %s\n", MONTH_STRING[january]);
 */
#define FOREACH_MONTH(MONTH) \
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
        
#define FOREACH_PLOT(PLOT) \
        PLOT(income_vs_expenses)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum MONTH_ENUM {
    FOREACH_MONTH(GENERATE_ENUM)
};

static const char *MONTH_STRING[] = {
    FOREACH_MONTH(GENERATE_STRING)
};

enum PLOT_ENUM {
    FOREACH_PLOT(GENERATE_ENUM)
};

static const char *PLOT_STRING[] = {
    FOREACH_PLOT(GENERATE_STRING)
};

enum PLOT_TYPE_ENUM {
    yearly, monthly
};

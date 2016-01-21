/* See LICENSE.txt for license and copyright information. */

/*
 * Note: The below code will give a month as an enum (integer value) and also
 * a lowercase string of the enum name.
 * Example usage:
 * printf("enum month of january as a string: %s\n", MONTH_STRING[january]);
 */
#define FOREACH_MONTH(MONTH) \
        MONTH(JANUARY)   \
        MONTH(FEBRUARY)  \
        MONTH(MARCH)   \
        MONTH(APRIL)  \
        MONTH(MAY)  \
        MONTH(JUNE)  \
        MONTH(JULY)  \
        MONTH(AUGUST)  \
        MONTH(SEPTEMBER)  \
        MONTH(OCTOBER)  \
        MONTH(NOVEMBER)  \
        MONTH(DECEMBER)
        
#define FOREACH_PLOT(PLOT) \
        PLOT(income_vs_expenses)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum enum_month_t {
    FOREACH_MONTH(GENERATE_ENUM)
};

static const char *MONTH_STRING[] = {
    FOREACH_MONTH(GENERATE_STRING)
};

enum enum_plot_t {
    FOREACH_PLOT(GENERATE_ENUM)
};

static const char *PLOT_STRING[] = {
    FOREACH_PLOT(GENERATE_STRING)
};

enum enum_plot_type_t {
    YEARLY,
    MONTHLY,
    WEEKLY
};

static int string_to_enumvalue(const char* a_string, const EnumValue *a_lst);

typedef struct
{
    const char *enum_string;
    int enum_value;
} EnumValue;

static int string_to_enumvalue(const char* a_string, const EnumValue *a_lst)
{
  int z_idx;
  for (z_idx = 0; a_lst[z_idx].enum_string; z_idx++)
  {
    if (strcmp(a_lst[z_idx].enum_string, a_string) == 0)
      return z_idx;
  }
  return -1;
}

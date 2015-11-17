/* See LICENSE file for copyright and license info. */

#define FILE_IVE_LAYOUT "gp_income_vs_expenses.gnu"


static char *f_cmd_income_vs_expenses =
    "ledger -f %s bal --real -X EUR -s -p %d -d \"T&l<=1\" expenses income | grep -Eo '[0-9\\.]{1,100}'";

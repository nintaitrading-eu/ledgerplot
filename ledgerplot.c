#include <stdlib.h>
#include <stdio.h>
#define GNUPLOT "gnuplot -persist"
#define NUM_POINTS 5
#define NUM_COMMANDS 2
 
int main(int argc, char *argv[])
{
    char *l_gcommands[] = {"set title \"TITLEEEEE\"", "plot 'data.temp'"};
    FILE *l_gp; // Gnuplot pipe
    double l_xvals[NUM_POINTS] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double l_yvals[NUM_POINTS] = {5.0 ,3.0, 1.0, 3.0, 5.0};
    FILE *l_temp = fopen("data.temp", "w");
    
    // TODO: l_gcommands and the points, should be dynamic.
    // Prepare the data, by executing  ledgerplot with the proper
    // parameters in batch.
    // Load all files 1 by 1 (from the ledger output folder) and generate the plots.
    /* Opens an interface that one can use to send commands as if they were typing into the
     * gnuplot command line. "The -persistent" keeps the plot open even after your
     * C program terminates.
     */
    l_gp = popen(GNUPLOT, "w");
    if (l_gp == NULL)
    {
        printf("Error opening pipe to GNU plot. Check if you have it!\n");
        exit(0);
    }
    
    // Note: To update in realtime: use fflush(gp)
    // Otherwise, the application will wait until the processing is finished.
    // Other note: you could also make an actual pipe:
    // mkfifo /tmp/gnuplot
    // while :; do (gnuplot -persist) < /tmp/gnuplot; done
    // and then do a simple
    // echo "plot sin(x)" > /tmp/gnuplot
    int i;
    for (i = 0; i < NUM_POINTS; i++)
    {
        fprintf(l_temp, "%lf %lf \n", l_xvals[i], l_yvals[i]); //Write the data to a temporary file
    }
    
    for (i = 0; i < NUM_COMMANDS; i++)
    {
       fprintf(l_gp, "%s \n", l_gcommands[i]); //Send commands to gnuplot one by one.
    }
    
    /*fprintf(gp, "set samples 2000\n");
    fprintf(gp, "plot abs(sin(x))\n");
    fprintf(gp, "rep abs(cos(x))\n");*/
    fclose(gp);
    return 0;
}

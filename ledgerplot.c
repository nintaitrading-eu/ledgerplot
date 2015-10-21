#include <stdio.h>
#define GNUPLOT "gnuplot -persist"
 
int main(int argc, char *argv[])
{
  FILE *gp; // Gnuplot pipe
  gp = popen(GNUPLOT,"w");
  if (gp == NULL)
  {
    printf("Error opening pipe to GNU plot. Check if you have it!\n");
    exit(0);
  }
  fprintf(gp, "set samples 2000\n");
  fprintf(gp, "plot abs(sin(x))\n");
  fprintf(gp, "rep abs(cos(x))\n");
  fclose(gp);
  return 0;
}

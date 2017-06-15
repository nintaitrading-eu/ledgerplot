####################################################################################
# Gnuplot 4.6 script for plotting multiple groups of 3 bars in a barchart.
# This part is dynamic and can be altered to influence the look of the graph.
####################################################################################

### plot setup
#set terminal pngcairo transparent enhanced font "inconsolata,10" fontscale 1.0 size 500, 350
set terminal pngcairo size 800,600 enhanced font 'Liberation Mono,10' background "#002b36"
set output 'income_vs_expenses.png'
COLORS = "red green blue"
STARTCOL = 2
ENDCOL = 4
set border 3 front linetype -1 linewidth 1.000 linecolor rgb "gold"

### titles and labels
set title "Income vs expenses" textcolor rgb "gold"
set xlabel "Year"
set xlabel  offset character 0, -2, 0 font "" textcolor rgb "gold"  norotate
set ylabel "Value (EUR)"
set ylabel textcolor rgb "gold"

### axis / grid
set grid nopolar
set grid xtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
set grid layerdefault linetype 0 linewidth 1.000 linecolor "grey", linetype 0 linewidth 1.000 linecolor "grey"
set xzeroaxis linetype 0 linewidth 1.000 linecolor "gold"
set yzeroaxis linetype 0 linewidth 1.000 linecolor "gold"
set zzeroaxis linetype 0 linewidth 1.000 linecolor "gold"

### legend
set key bmargin center horizontal Left reverse noenhanced autotitles columnhead nobox textcolor rgb "gold"


### main code
GAPSIZE = 3
NCOL = ENDCOL-STARTCOL+1
BOXWIDTH = 2./(GAPSIZE+NCOL)

### barchart layout
set style fill solid 0.75 border lt -1
#set boxwidth BOXWIDTH absolute

### Set tics and histograms
# Note: I can not use GAPSIZE below, for some reason... hardcoded value is used instead
set style histogram clustered gap 3 title offset character 1, 0.25, 0
set datafile missing '-'
set style data histograms
set xtics border in scale 0,0 nomirror rotate by -45  offset character 0, 0, 0 autojustify
set xtics  norangelimit font ",8"
set xtics ()
set ytics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autojustify
set ztics border in scale 0,0 nomirror norotate  offset character 0, 0, 0 autojustify
set cbtics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autojustify
set rtics axis in scale 0,0 nomirror norotate  offset character 0, 0, 0 autojustify

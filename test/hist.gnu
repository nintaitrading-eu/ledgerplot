### TODO: 2014_income_vs_expenses.dat -> 2014 / income_vs_expenses = year for a group of
### histograms / plottitle
### plot setup
#set terminal pngcairo transparent enhanced font "inconsolata,10" fontscale 1.0 size 500, 350 
set terminal pngcairo size 800,600 enhanced font 'Liberation Mono,10' background "#002b36"
set output 'test.png'

STARTCOL=2
ENDCOL=4
GAPSIZE=5
NCOL=ENDCOL-STARTCOL+1
BOXWIDTH=1./(GAPSIZE+NCOL)

#set border 3 front linetype -1 linewidth 1.000
set boxwidth BOXWIDTH absolute
set style fill solid 1.00 border lt -1
set grid nopolar
set grid noxtics nomxtics ytics nomytics noztics nomztics \
 nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
set grid layerdefault linetype 0 linewidth 1.000,  linetype 0 linewidth 1.000
set xzeroaxis linetype 0 linewidth 1.000
set yzeroaxis linetype 0 linewidth 1.000
set zzeroaxis linetype 0 linewidth 1.000

### legend
set key bmargin center horizontal Left reverse noenhanced autotitles columnhead nobox

### Set tics and histograms
set style histogram clustered gap 4 title  offset character 2, 0.25, 0
set datafile missing '-'
set style data histograms
set xtics border in scale 0,0 nomirror rotate by -45  offset character 0, 0, 0 autojustify
set xtics  norangelimit font ",8"
set xtics   ()
set ytics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autojustify
set ztics border in scale 0,0 nomirror norotate  offset character 0, 0, 0 autojustify
set cbtics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autojustify
set rtics axis in scale 0,0 nomirror norotate  offset character 0, 0, 0 autojustify

# TODO: the below text is variable
set title "income\_vs\_expenses\n(Extra info here?)" 
# TODO: the below text is variable
#set xlabel "(Note: )" 
set xlabel  offset character 0, -2, 0 font "" textcolor lt -1 norotate
# TODO: the below text is variable
set ylabel "Value (EUR)" 
x = 0.0
i = 23
### the plot
## Last datafile plotted: "data.dat"
#plot newhistogram "Northern Europe", 'data.dat' using "Sweden":xtic(1) t col, '' u "Denmark" t col, '' u "Norway" t col, newhistogram "Southern Europe", '' u "Greece":xtic(1) t col, '' u "Romania" t col, '' u "Yugoslavia" t col, newhistogram "British Isles", '' u "Ireland":xtic(1) t col, '' u "United_Kingdom" t col
# TODO: the below texts and colors are variable
#plot 'test.dat' using "income":xtic(1) t col lc "#586e75", '' u "expenses" t col lc "#cb4b16", '' u "difference" t col lc "#93a1a1"
#plot 'test.dat' using "income":xtic(1) t col lc "green", '' u "expenses" t col lc "red", '' u "difference" t col lc "gray"
#plot for [COL=STARTCOL:ENDCOL] 'test.dat' using COL:xtic(1) t col lc "green", '' u  t col lc "red", '' u "difference" t col lc "gray"
plot for [COL=STARTCOL:ENDCOL] 'test.dat' u COL:xtic(1) w histogram title columnheader(COL), \
    for [COL=STARTCOL:ENDCOL] 'test.dat' u (column(0)+BOXWIDTH*(COL-STARTCOL+GAPSIZE/2+1)-0.5):COL:COL notitle w labels

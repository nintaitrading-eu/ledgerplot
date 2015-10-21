### TODO: 2014_income_vs_expenses.dat -> 2014 / income_vs_expenses = year for a group of
### histograms / plottitle
# set terminal png transparent nocrop enhanced size 450,320 font "arial,8" 
# set output 'histograms.7.png'
set border 3 front lt black linewidth 1.000 dashtype solid
set boxwidth 0.95 absolute
set style fill   solid 1.00 noborder
set grid nopolar
set grid noxtics nomxtics ytics nomytics noztics nomztics \
 nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
set grid layerdefault   lt 0 linewidth 0.500,  lt 0 linewidth 0.500
set key bmargin center horizontal Left reverse noenhanced autotitle columnhead nobox
set style histogram clustered gap 1 title textcolor lt -1 offset character 2, 0.25, 0
set datafile missing '-'
set style data histograms
set xtics border in scale 0,0 nomirror rotate by -45  autojustify
set xtics  norangelimit font ",8"
set xtics   ()
set ytics border in scale 0,0 mirror norotate  autojustify
set ytics autofreq  norangelimit font ",8"
set ztics border in scale 0,0 nomirror norotate  autojustify
set cbtics border in scale 0,0 mirror norotate  autojustify
set rtics axis in scale 0,0 nomirror norotate  autojustify
set paxis 1 tics border in scale 0,0 nomirror norotate  autojustify
set paxis 1 tics autofreq  rangelimit
set paxis 2 tics border in scale 0,0 nomirror norotate  autojustify
set paxis 2 tics autofreq  rangelimit
set paxis 3 tics border in scale 0,0 nomirror norotate  autojustify
set paxis 3 tics autofreq  rangelimit
#set paxis 4 tics border in scale 0,0 nomirror norotate  autojustify
#set paxis 4 tics autofreq  rangelimit
#set paxis 5 tics border in scale 0,0 nomirror norotate  autojustify
#set paxis 5 tics autofreq  rangelimit
#set paxis 6 tics border in scale 0,0 nomirror norotate  autojustify
#set paxis 6 tics autofreq  rangelimit
#set paxis 7 tics border in scale 0,0 nomirror norotate  autojustify
#set paxis 7 tics autofreq  rangelimit
set title "income_vs_expenses\n(Extra info here?)" 
set xlabel "(note: histogram titles have specified offset relative to X-axis label)" 
set xlabel  offset character 0, -2, 0 font "" textcolor lt -1 norotate
set ylabel "Value (EUR)" 
x = 0.0
i = 23
## Last datafile plotted: "data.tmp"
#plot newhistogram "Northern Europe", 'immigration.dat' using "Sweden":xtic(1) t col, '' u "Denmark" t col, '' u "Norway" t col, newhistogram "Southern Europe", '' u "Greece":xtic(1) t col, '' u "Romania" t col, '' u "Yugoslavia" t col, newhistogram "British Isles", '' u "Ireland":xtic(1) t col, '' u "United_Kingdom" t col
plot newhistogram "2014", 'data.tmp' using "income":xtic(1) t col, '' u "expenses" t col, '' u "difference" t col, newhistogram "2015", '' u "income":xtic(1) t col, '' u "expenses" t col, '' u "difference" t col, newhistogram "2016", '' u "income":xtic(1) t col, '' u "expenses" t col, '' u "difference" t col

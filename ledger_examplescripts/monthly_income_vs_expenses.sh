#!/bin/sh

FILE=/home/rockwolf/doc/ledger/ledger.dat

ledger -f $FILE -j reg ^Income -M --collapse --plot-amount-format="%(format_date(date, \"%Y-%m-%d\")) %(abs(quantity(scrub(display_amount))))\n" > ledgeroutput1.tmp
ledger -f $FILE -j reg ^Expenses -M --collapse > ledgeroutput2.tmp

(cat <<EOF) | gnuplot
  set terminal pngcairo size 800,600 enhanced font 'Liberation Mono,10'
  set output 'monthly_income_vs_expenses.png'
  set style data histogram
  set style histogram clustered gap 1
  set style fill transparent solid 0.4 noborder
  set xtics nomirror scale 0 center
  set ytics add ('' 0) scale 0
  set border 1
  set grid ytics
  set title "Monthly Income and Expenses"
  set ylabel "Amount"
  plot "ledgeroutput1.tmp" using 2:xticlabels(strftime('%b', strptime('%Y-%m-%d', strcol(1)))) title "Income" linecolor rgb "light-salmon", '' using 0:2:2 with labels left font "Courier,8" rotate by 15 offset -4,0.5 textcolor linestyle 0 notitle, "ledgeroutput2.tmp" using 2 title "Expenses" linecolor rgb "light-green", '' using 0:2:2 with labels left font "Courier,8" rotate by 15 offset 0,0.5 textcolor linestyle 0 notitle
EOF

rm ledgeroutput*.tmp

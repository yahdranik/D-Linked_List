set terminal pngcairo size 1200,700
set grid
set xlabel 'Number of operations'
set ylabel 'Time'
set xrange [0:50000]
set yrange [0:1000]
set boxwidth 80 absolute
set style fill solid 0.5 border
set title 'Perfomance test'

set output 'histograms.png'
plot 'SLOWLY_LIST/slowly_list.txt' using ($0*100):1 with boxes notitle lc rgb "blue", \
     'QUICK_LIST/quick_list.txt'   using ($0*100):1 with boxes notitle lc rgb "pink"


#set output 'quick_histogram.png'
#plot 'quick_list.txt' using ($0*100):1 with boxes notitle
#set title 'Slowly list'
#set output 'slowly_histogram.png'
#plot 'slowly_list.txt' using ($0*100):1 with boxes notitle

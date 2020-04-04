set datafile separator ','
plot "comparison.csv" using 12:2 with lines
set key autotitle columnhead

set xlabel "Number of Cores" 
set ylabel 'Time (seconds)'
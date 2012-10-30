set terminal postscript eps enhanced solid color
set output "gcbench_bad_cache_mark.eps"
set title "GCBench - caching" font "Helvetica,26"
set datafile separator "," #csv is comma separated
set yrange [0:300]      #plot starting from 0
set ylabel "Average execution time (s)" font "Helvetica,20"
set style data histograms  #plot histogram style
set style fill solid 1.00 border 0 #fill bars
set style histogram errorbars gap 2 lw 1
set key left
plot "benchmark_results/mark_sweep_nocache_noTagging_gcbench.csv" using ($2/1000):($3/1000):xtic(1) ti "SOM++ mark sweep (no cache)", \
     "benchmark_results/mark_sweep_badcache_noTagging_gcbench.csv" using ($2/1000):($3/1000) ti "SOM++ mark sweep (bad cache)", \
     "benchmark_results/mark_sweep_cache_noTagging_gcbench.csv" using ($2/1000):($3/1000) ti "SOM++ mark sweep (with cache)"

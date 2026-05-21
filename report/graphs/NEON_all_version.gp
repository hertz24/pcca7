set terminal pngcairo size 1200,800 enhanced font 'arial,15'
set datafile separator ','
set key top left
set title 'Execution time for b = 1422166701 and p = 1451478673'
set output 'all_versions.png'
set xlabel 'Size of the vector'
set ylabel 'Time in microseconds'
plot '-' title 'Variant 1 (NEON)' with points pt 7 ps 0.5 linecolor 'red', '-' title 'Variant 2 (NEON)' with points pt 7 ps 0.5 linecolor 'dark-green', '-' title 'Variant 3 (NEON)' with points pt 7 ps 0.5 linecolor 'orange'

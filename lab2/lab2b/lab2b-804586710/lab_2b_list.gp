#! /usr/local/cs/bin/gnuplot
#
# purpose:
#        generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#       1. test name
#       2. # threads
#       3. # iterations per thread
#       4. # lists
#       5. # operations performed (threads x iterations x (ins + lookup + delete))
#       6. run time (ns)
#       7. run time per operation (ns)
#
# output:
#       lab2_list-1.png ... cost per operation vs threads and iterations
#       lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#       lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#       lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#       Managing data is simplified by keeping all of the results in a single
#       file.  But this means that the individual graphing commands have to
#       grep to select only the data they want.
#

# general plot parameters
set terminal png
set datafile separator ","

set title "Throughput vs number of threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'
set key left top

plot \
     "< grep 'list-none-m,' lab2_list.csv" using ($2):(1000000000/($6)) \
        title 'list mutex' with linespoints lc rgb 'red', \
     "< grep list-none-s, lab2_list.csv" using ($2):(1000000000/($6)) \
        title 'list spin' with linespoints lc rgb 'green', \
     "< grep add-m lab2_add.csv" using ($2):(1000000000/($6)) \
        title 'add mutex' with linespoints lc rgb 'blue', \
     "< grep add-s lab2_add.csv" using ($2):(1000000000/($6)) \
        title 'add spin' with linespoints lc rgb 'orange'





set title "wait for lock time and time per operation vs threads"
set xlabel "wait for lock time and time per operation vs threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_2.png'
set key left top

plot \
     "< grep list lab2b_list.csv" using ($2):($7) \
        title 'list mutex avg lock time' with linespoints lc rgb 'red', \
     "< grep list lab2b_list.csv" using ($2):($6) \
        title 'list time per operation' with linespoints lc rgb 'green'





set title "number of iterations to cause it fail vs # threads"
set xlabel " threads"
set logscale x 2
set xrange [0.75:]
set ylabel "iterations"
set logscale y 10
set output 'lab2b_3.png'
set key left top

plot \
     "< grep list-id-m lab2b_4list.csv" using ($2):($3) \
        title 'list mutex ' with points lc rgb 'red', \
     "< grep list-id-s lab2b_4list.csv" using ($2):($3) \
        title 'list spin' with points lc rgb 'green', \
     "< grep list-id-none lab2b_4list.csv" using ($2):($3) \
     	title 'list non_synchronized ' with points lc rgb 'orange'




set title "Throughput vs number of threads for different list in mutex"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_4.png'
set key left top

plot \
     "< grep list-none-m, lab2b_4list.csv | grep ,1000,1," using ($2):(1000000000/($6)) \
        title '1 list' with linespoints lc rgb 'red', \
     "< grep list-none-m, lab2b_4list.csv | grep ,1000,4" using ($2):(1000000000/($6)) \
        title '4 list' with linespoints lc rgb 'green', \
	"< grep list-none-m, lab2b_4list.csv | grep ,1000,8" using ($2):(1000000000/($6))title '8 list' with linespoints lc rgb 'blue', \
     "< grep list-none-m, lab2b_4list.csv | grep ,1000,16" using ($2):(1000000000/($6)) \
     title '16 list' with linespoints lc rgb 'orange'

set title "Throughput vs number of threads for different list in spin"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_5.png'
set key left top

plot \
     "< grep list-none-s, lab2b_4list.csv | grep ,1000,1," using ($2):(1000000000/($6)) \
     title '1 list' with linespoints lc rgb 'red', \
     "< grep list-none-s, lab2b_4list.csv | grep ,1000,4" using ($2):(1000000000/($6)) \
     title '4 list' with linespoints lc rgb 'green', \
       "< grep list-none-s, lab2b_4list.csv | grep ,1000,8" using ($2):(1000000000/($6)) title '8 list' with linespoints lc rgb 'blue', \
     "< grep list-none-s, lab2b_4list.csv | grep ,1000,16" using ($2):(1000000000/($6)) \
     title '16 list' with linespoints lc rgb 'orange'
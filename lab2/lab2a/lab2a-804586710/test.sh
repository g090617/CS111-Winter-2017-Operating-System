#!/bin/bash


#thread[0]=1
#thread[1]=2
#thread[2]=3
#thread[3]=4

thread=(2 4 8 12)

cp empty lab2_add.csv

for i in 2 4 8 12                                                                        
do                                                                                       
    iteration=100                                                                        
    for j in 1 2 3 4                                                                     
    do                                                                                   
        ./lab2_add \
	    --iterations="$iteration" \
	    --threads="$i" >>lab2_add.csv               
        iteration=$(($iteration*10))                                                     
        #echo "$i"                                                                       
    done                                                                                 
done                                                                                     
                                                                                         
                                                                                        
for i in 2 4 8 12                                                                        
do                                                                                       
    iteration=100                                                                        
    for j in 1 2 3 4                                                                     
    do                                                                                   
        ./lab2_add \
	    --yield \
	    --iterations="$iteration" \
	    --threads="$i" >>lab2_add.csv       
        iteration=$(($iteration*10))                                                     
        #echo "$i"                                                                       
    done                                                                                 
done  

iteration=100
for j in 1 2 3 4
do
    ./lab2_add \
	--iterations="$iteration" \
	--threads=1 >>lab2_add.csv
    iteration=$(($iteration*10))
    #echo "$i"                                                                        
done

for i in 2 4 8 12
do
    iteration=100
    for j in 1 2 3 4
    do
	./lab2_add \
            --yield \
	    --sync=m \
            --iterations="$iteration" \
            --threads="$i" >>lab2_add.csv
        iteration=$(($iteration*10))
	#echo "$i"                                                                       
    done
done

for i in 2 4 8 12
do
    iteration=100
    for j in 1 2 3 4
    do
	./lab2_add \
            --sync=m \
            --iterations="$iteration" \
            --threads="$i" >>lab2_add.csv
        iteration=$(($iteration*10))
	#echo "$i"                                                                       
    done
done

for i in 2 4 8 12
do
    iteration=100
    for j in 1 2 3 4
    do
        ./lab2_add \
            --sync=s \
            --iterations="$iteration" \
            --threads="$i" >>lab2_add.csv
        iteration=$(($iteration*10))
        #echo "$i"                                                                       
    done
done

for i in 2 4 8 12
do
    iteration=100
    for j in 1 2 3 4
    do
        ./lab2_add \
	    --yield \
            --sync=s \
            --iterations="$iteration" \
            --threads="$i" >>lab2_add.csv
        iteration=$(($iteration*10))
        #echo "$i"                                                                       
    done
done


for i in 2 4 8 12
do
    iteration=100
    for j in 1 2 3 4
    do
        ./lab2_add \
            --sync=c \
            --iterations="$iteration" \
            --threads="$i" >>lab2_add.csv
        iteration=$(($iteration*10))
        #echo "$i"                                                                       
    done
done

for i in 2 4 8 12
do
    iteration=100
    for j in 1 2 3 4
    do
        ./lab2_add \
            --yield \
            --sync=c \
            --iterations="$iteration" \
            --threads="$i" >>lab2_add.csv
        iteration=$(($iteration*10))
        #echo "$i"                                                                       
    done
done

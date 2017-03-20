touch lab2b_4list.csv
rm lab2b_4list.csv
touch lab2b_4list.csv

for i in 1 4 8 12 16
do
    for j in 10 20 40 80 
    do
        ./lab2_list \
            --yield=id \
            --sync=m \
            --iterations="$j" \
            --threads="$i" \
	    --lists=4 >> lab2b_4list.csv
        #echo "$i"                                                                       
    done
done

for i in 1 4 8 12 16
do
    for j in 10 20 40 80 
    do
	./lab2_list \
            --yield=id \
            --sync=s \
            --iterations="$j" \
            --threads="$i" \
            --lists=4 >> lab2b_4list.csv
        #echo "$i"                                                                       
    done
done

for i in 1 4 8 12 16
do
    for j in 1 2 4 8 16
    do
	./lab2_list \
            --yield=id \
            --iterations="$j" \
            --threads="$i" \
            --lists=4 >> lab2b_4list.csv
        #echo "$i"                                                                       
    done
done

for i in 1 2 4 8 12
do
    for j in 1 4 8 16
    do
        ./lab2_list \
            --sync=m \
            --iterations=1000 \
            --threads="$i" \
            --lists="$j" >> lab2b_4list.csv
        #echo "$i"                                                                       
    done
done

for i in 1 2 4 8 12 
do
    for j in 1 4 8 16
    do
        ./lab2_list \
            --sync=s \
            --iterations=1000 \
            --threads="$i" \
            --lists="$j" >> lab2b_4list.csv
        #echo "$i"                                                                       
    done
done

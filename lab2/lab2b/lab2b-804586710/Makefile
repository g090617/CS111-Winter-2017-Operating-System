CC = gcc
FLAGS = -g -o

TARGET = lab2_add
TARGET2 = lab2_list

build:$(TARGET).c
	$(CC) $(FLAGS) $(TARGET) $(TARGET).c -lpthread

	$(CC) $(FLAGS) $(TARGET2) $(TARGET2).c SortedList.c -lpthread

tests:
	-./t2.sh
	@-./t3.sh
	@-./t4.sh
	@-t_list.sh

graphs:
	./lab_2b_list.gp

profile:
	LD_PRELOAD=/u/cs/ugrad/haoranz/gperftools/lib/libprofiler.so CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s

	/u/cs/ugrad/haoranz/gperftools/bin/pprof --text ./lab2_list ./raw.gperf > profile.gperf
	/u/cs/ugrad/haoranz/gperftools/bin/pprof --list={{ mission }} ./lab2_list ./raw.gperf >> profile.gperf


clean:
	rm lab2_add lab2_list

tarball:
	tar -czvf lab2b-804586710.tar.gz *.png *.c *.csv *.sh *.h *.gp Makefile README profile.gperf 

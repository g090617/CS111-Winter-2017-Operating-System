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
graphs:
	./lab2_add.gp
	./lab2_list.gp

clean:
	rm lab2_add lab2_list

tarball:
	tar -czvf lab2a-804586710.tar.gz *.png *.c *.csv *.sh *.h *.gp Makefile README

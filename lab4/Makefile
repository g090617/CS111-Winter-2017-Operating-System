build:
	gcc -o lab4_1 lab4_1.c -lm -lmraa -pthread
	gcc -o lab4_2 lab4_2.c -lmraa -lm -lpthread
	gcc -o lab4_3 lab4_3.c -lm -lmraa -lpthread -lssl -lcrypto

dist:
	tar -czvf lab4-804586710.tar.gz lab4_1.c lab4_2.c lab4_3.c \
		lab4_1.log lab4_2.log lab4_3.log README Makefile 

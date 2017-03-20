CC = gcc
CFLAGS = -g -o

TARGET = simpsh

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET) $(TARGET).c

check:
	@-./test.sh
clean:
	@rm in_file.txt out_file.txt error.txt simpsh out_message.txt

dist:
	tar -zcvf lab1-haoran.tar.gz --transform 's,^,lab1-haoran/,' Makefile simpsh.c \
	test.sh README benchmark.pdf 

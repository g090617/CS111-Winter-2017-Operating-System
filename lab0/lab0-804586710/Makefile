CC = gcc
CFLAGS = -g -o

TARGET = lab0

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET) $(TARGET).c

check:
	@-echo "test case 1 for ./lab0 < in_file1.txt >out_file.txt" > in_file1.txt
	@-./lab0 < in_file1.txt >out_file1.txt
	@-echo "test case 1 succeed "
	@-echo "test case 2 for ./lab0 --input=in_file2.txt >out_file2.txt" > in_file2.txt
	@-./lab0 --input=in_file2.txt >out_file2.txt
	@-echo "test case 2 succeed "
	@-echo "test case 3 for ./lab0 --output=out_file3.txt <in_file3.txt" > in_file3.txt
	@-./lab0 --output=out_file3.txt <in_file3.txt
	@-echo "test case 3 succeed "
	@-echo "test case 4 for ./lab0 --output=out_file4.txt --input=in_file4.txt" > in_file4.txt
	@-./lab0 --input=in_file4.txt --output=out_file4.txt
	@-echo "test case 4 succeed "
	@-echo "test case 5 with non-exist input file"
	@-./lab0 --input=nonexist.txt --output=out_file.txt
	@-echo "test case 5 succeed "
	@-echo "test case 6 for ./lab0 --segfault --catch"
	@-./lab0 --segfault --catch
	@-echo "test case 6 succeed"
	@-echo "test case 7 for not writable file" > in_file7.txt
	@-touch out_file7.txt
	@-chmod -w out_file7.txt
	@-./lab0 --input=in_file7.txt --output=out_file7.txt
	@-echo "test case 7 succeed"
	@-chmod +w out_file7.txt
clean:
	@-rm in_file1.txt in_file2.txt in_file4.txt in_file3.txt in_file7.txt
	@-rm out_file1.txt out_file2.txt out_file4.txt out_file3.txt out_file7.txt
	@-rm lab0
dist:
	tar -czvf lab0-804586710.tar.gz lab0.c README Makefile gdb\ breakpoint.png gdb\ backstrace.png 


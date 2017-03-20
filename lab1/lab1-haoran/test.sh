#!/bin/bash

echo "test test" > in_file.txt        
touch out_file.txt        
touch error.txt        
touch out_message.txt

echo "test 1"        
./simpsh --rdonly in_file.txt > out_file.txt        
cat out_file.txt

        
echo "test 2"        
./simpsh --rdonly non_exist.txt > out_file.txt        
cat out_file.txt
        
echo "test 3"        
./simpsh \
    --rdonly in_file.txt \
    --wronly out_file.txt \
    --wronly error.txt \
    --verbose --command 0 1 9 cat > out_message.txt
        
echo "in_file.txt is "        
cat in_file.txt
        
echo " "        
echo "out_file.txt is "
        
cat out_file.txt        
echo " "
        
echo "error.txt is "        
cat error.txt
        
echo "out_message.txt is"
cat out_message.txt

echo "test 4"
./simpsh \
    --rdonly in_file.txt \
    --wronly out_file.txt \
    --wronly error.txt \
    --verbose --command 0 1 2 cat > out_message.txt

echo "in_file.txt is "
cat in_file.txt

echo " "
echo "out_file.txt is "

cat out_file.txt
echo " "

echo "error.txt is "
cat error.txt

echo "out_message.txt is"
cat out_message.txt


echo " "
echo "test 5"

echo "not test test" > in_file.txt
./simpsh --rdonly in_file.txt --wronly out_file.txt --pipe --wronly error.txt \
--command 0 3 1 cat --command 2 4 1 cat

echo "in_file.txt is "
cat in_file.txt

 
echo "out_file.txt is "

cat out_file.txt
echo " "

echo "error.txt is "
cat error.txt

echo "out_message.txt is"
cat out_message.txt

echo " "
echo "test 6"
        
echo "not test test" > in_file.txt
./simpsh --verbose --rdonly in_file.txt --wronly out_file.txt --wronly error.txt --pipe \
--command 0 4 2 cat --command 3 1 2 cat --wait

echo "in_file.txt is "
cat in_file.txt


echo "out_file.txt is "

cat out_file.txt
echo " "

echo "error.txt is "
cat error.txt

#echo "out_message.txt is"
#cat out_message.txt


echo " "
echo "test 7"

echo "not test test" > in_file.txt
./simpsh --verbose --rdonly in_file.txt --wronly out_file.txt --wronly error.txt \
--command 0 1 2 sleep 2 --wait

echo "in_file.txt is "
cat in_file.txt


echo "out_file.txt is "

cat out_file.txt
echo " "

echo "error.txt is "
cat error.txt

echo "out_message.txt is"
cat out_message.txt



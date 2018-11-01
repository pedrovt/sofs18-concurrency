#!/bin/bash

if [ "$1" == "-b" ]; then
    echo -e "Using binary version\n"
    ./createDisk dddd 1000 > /dev/null
    ./mksofs -b -n "binary" dddd > /dev/null

    freeInodes=$(./showblock dddd | grep "Number of free inodes" | sed 's/.*: //1')
    echo $freeInodes

    dz_first_block=$(./showblock dddd | grep "First block of the data zone" | sed 's/.*: //1')
    echo $dz_first_block

    # allocate an inode
    echo -e "ai\n1\nq" | ./testtool dddd -q1 -b -p 202-202 

    # show direntries of root
    echo -e "sb\nd\n$dz_first_block\nq" | ./testtool dddd -q1 -b -p 0-999 

    # add several direntries to root
    for i in $(seq 1 18); do
        #echo -e "ade\n0\n$i\n1\n" 
        echo -e "ade\n0\n$i\n1\nq\n" | ./testtool dddd -q1 -b -p 202-202 
    done

    # test repetead files
    echo -e "ade\n0\n3\n1\nq\n" | ./testtool dddd -q1 -b -p 202-202 

    # show direntries of root again
    echo -e "sb\nd\n$dz_first_block-$((dz_first_block + 1))\nq" | ./testtool dddd -q1 -b -p 202-202 
    exit 0

else
    echo -e "Using work version\n"
    ./createDisk dddd 1000 > /dev/null
    ./mksofs -b -n "work" dddd > /dev/null

    freeInodes=$(./showblock dddd | grep "Number of free inodes" | sed 's/.*: //1')
    echo $freeInodes

    dz_first_block=$(./showblock dddd | grep "First block of the data zone" | sed 's/.*: //1')
    echo $dz_first_block

    # allocate an inode
    echo -e "ai\n1\nq" | ./testtool dddd -q1 -b -r 202-202 -p 202-202 

    # show direntries of root
    echo -e "sb\nd\n$dz_first_block\nq" | ./testtool dddd -q1 -b -r 202-202 -p 202-202 

    # add several direntries to root
    for i in $(seq 1 18); do
        #echo -e "ade\n0\n$i\n1\n" 
        echo -e "ade\n0\n$i\n1\nq\n" | ./testtool dddd -q1 -b -r 202-202 -p 202-202 
    done

    # test repetead files
    echo -e "ade\n0\n3\n1\nq\n" | ./testtool dddd -q1 -b -r 202-202 -p 202-202 

    # show direntries of root again
    echo -e "sb\nd\n$dz_first_block-$((dz_first_block + 1))\nq" | ./testtool dddd -q1 -r 202-202 -b -p 202-202 
fi


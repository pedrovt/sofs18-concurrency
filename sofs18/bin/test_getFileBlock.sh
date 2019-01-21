#!/bin/bash

echo -e "Testing Get File Block"

if [ "$1" == "-b" ]; then
    echo -e "Using binary version\n"
    ./createDisk dddd 24235 > /dev/null 2>&1
    ./mksofs -b -n "binary" dddd > /dev/null 2>&1

    freeBlocks=$(./showblock dddd | grep "Number of free data blocks:" | sed 's/.*: //1')
    echo "Number of free blocks before allocation:" $freeBlocks

    prefix="[0m[01;34m"
    suffix="allocated"

    # allocate blocks
    echo -e "Allocing blocks...\n"
    for i in $(seq 1 $((freeBlocks - 1))); do
        #echo -e "ade\n0\n$i\n1\n" 
        echo -e "afb\n0\n$i\nq" | ./testtool dddd -q1 -b -p 0-0 | sed -e 's/^$prefix//' -e 's/$suffix$//' >> alloc_results_bin 
    done

    echo -e "\n===============================\n"
    verify=$(./showblock dddd | grep "Number of free data blocks:" | sed 's/.*: //1')
    echo "Number of free blocks after allocation:" $verify
 
    # allocate blocks: no more free blocks!
    echo -e "Should give error: \n"
    echo -e "afb\n0\n$((freeBlocks + 1))\nq" | ./testtool dddd -q1 -b -p 1-1
    echo -e "\n===============================\n"

    suffix="retrieved"

    # retrieve blocks
    echo -e "Retrieving blocks...\n"
    for i in $(seq 1 $((freeBlocks - 1))); do
        #echo -e "ade\n0\n$i\n1\n" 
        echo -e "gfb\n0\n$i\nq" | ./testtool dddd -q1 -b -p 0-0 | sed -e 's/^$prefix//' -e 's/$suffix$//' >> get_results_bin
    done

    #retrieve non existing block
    echo -e "\n===============================\n"
    echo -e "Should give error: \n"
    echo -e "gfb\n0\n242350000000\nq" | ./testtool dddd -q1 -b -p 1-1     

    meld alloc_results_bin get_results_bin
    rm alloc_results 
    rm get_results
    exit 0

else
    echo -e "Using work version\n"
    
    ./createDisk dddd 24235 > /dev/null 2>&1
    ./mksofs -b -r 301-301 -n "binary" dddd > /dev/null 2>&1

    freeBlocks=$(./showblock dddd | grep "Number of free data blocks:" | sed 's/.*: //1')
    echo "Number of free blocks before allocation:" $freeBlocks

    prefix="[0m[01;34m"
    suffix="allocated"
    
    # allocate blocks
    echo -e "Allocing blocks...\n"
    for i in $(seq 1 $((freeBlocks - 1))); do
        #echo -e "ade\n0\n$i\n1\n" 
        echo -e "afb\n0\n$i\nq" | ./testtool dddd -q1 -b -r 301-301 -p 1-1 >> alloc_results 
    done

    echo -e "\n===============================\n"
    verify=$(./showblock dddd | grep "Number of free data blocks:" | sed 's/.*: //1')
    echo "Number of free blocks after allocation:" $verify
 
    # allocate blocks: no more free blocks!
    echo -e "Should give error: \n"
    echo -e "afb\n0\n$((freeBlocks + 1))\nq" | ./testtool dddd -q1 -b -r 301-301 -p 1-1
    echo -e "\n===============================\n"

    suffix="retrieved"

    # retrieve blocks
    echo -e "Retrieving blocks...\n"
    for i in $(seq 1 $((freeBlocks - 1))); do
        #echo -e "ade\n0\n$i\n1\n" 
        echo -e "gfb\n0\n$i\nq" | ./testtool dddd -q1 -b -r 301-301 -p 1-1  >> get_results
    done

    #retrieve non existing block
    echo -e "\n===============================\n"
    echo -e "Should give error: \n"
    echo -e "gfb\n0\n242350000000\nq" | ./testtool dddd -q1 -b -r 301-301 -p 1-1     

    meld alloc_results get_results
    rm alloc_results 
    rm get_results
    exit 0
fi


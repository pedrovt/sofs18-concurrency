#!/bin/bash

echo -e "Testing replenish ircache (test 2)"

if [ "$1" == "-b" ]; then
    echo -e "Using binary version\n"
    ./createDisk dddd 1000 > /dev/null
    ./mksofs -n "bin" -b -i 8 dddd > /dev/null

    freeInodes=$(./showblock dddd | grep "Number of free inodes" | sed 's/.*: //1')

    echo -e "sb\ns\n0\nric\nsb\ns\n0\nq" | ./testtool dddd -q1 -b -p 403-403 # -p0-1000
    
else
    echo -e "Using work version\n"
    ./createDisk ddd 1000 > /dev/null
    ./mksofs -n "wo" -b -i 8 ddd > /dev/null

    freeInodes=$(./showblock ddd | grep "Number of free inodes" | sed 's/.*: //1')
    #echo $freeInodes
        
    echo -e "sb\ns\n0\nric\nsb\ns\n0\nq" | ./testtool ddd -q1 -b -r 403-403 -p 403-403 
fi


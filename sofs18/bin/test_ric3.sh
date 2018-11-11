#!/bin/bash

echo -e "Testing replenish ircache (test 1)"

if [ "$1" == "-b" ]; then
    echo -e "Using binary version\n"
    ./createDisk ddd 1000 > /dev/null 2>&1
    ./mksofs -b -n "BINARY" ddd > /dev/null 2>&1

    freeInodes=$(./showblock ddd | grep "Number of free inodes" | sed 's/.*: //1')
    echo $freeInodes

    # first replenish from FILT cache
    echo -e "ai\n1\nq" | ./testtool ddd -q1 -b -p 401-403 # -p0-1000

    # free inode to insertion cache
    echo -e "fi\n1\nq" | ./testtool ddd -q1 -b -p 401-403 # -p0-1000

    # replenish from FILT cache (when applied)
    for i in $(seq 3 $freeInodes); do
        echo -e "ai\n1\nq" | ./testtool ddd -q1 -b  -p 401-403 # -p0-1000
    done

    # replenish from insertion cache
    echo -e "ai\n1\nq" | ./testtool ddd -q1 -b -p403-403
    echo -e "sb\ns\n0\nq\n" | ./testtool ddd -q1 -b -p 401-403
    echo -e "ai\n1\nq" | ./testtool ddd -q1 -b -p403-403
    echo -e "sb\ns\n0\nq\n" | ./testtool ddd -q1 -b -p 401-403
    exit 0

else
    echo -e "Using work version\n"
    ./createDisk dddd 1000 > /dev/null 2>&1
    ./mksofs -b -n "work" dddd > /dev/null 2>&1

    freeInodes=$(./showblock dddd | grep "Number of free inodes" | sed 's/.*: //1')
    echo $freeInodes

    # first replenish from FILT cache
    echo -e "ai\n1\nq" | ./testtool dddd -q1 -b -r 403-403 -p 401-403 # -p0-1000

    # free inode to insertion cache
    echo -e "fi\n1\nq" | ./testtool dddd -q1 -b -r 403-403 -p 401-403 # -p0-1000

    # replenish from FILT cache (when applied)
    for i in $(seq 3 $freeInodes); do
    echo -e "ai\n1\nq" | ./testtool dddd -q1 -b -r 403-403 -p 401-403 # -p0-1000
    done

    # replenish from insertion cache
    echo -e "ai\n1\nq" | ./testtool dddd -q1 -b -r 403-403 -p403-403
    echo -e "sb\ns\n0\nq\n" | ./testtool dddd -q1 -b -p 401-403
    echo -e "ai\n1\nq" | ./testtool dddd -q1 -b -r 403-403 -p403-403
    echo -e "sb\ns\n0\nq\n" | ./testtool dddd -q1 -b -p 403-403
fi


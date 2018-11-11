#!/bin/bash

echo -e "Test 1...\nBinary"
./test_ric.sh -b > binary.txt
echo -e "Done!\n\nWork\n"
./test_ric.sh > work.txt
meld work.txt binary.txt

echo -e "Done!\nTest 2...\nBinary"
./test_ric2.sh -b > binary.txt
echo -e "Done!\n\nWork\n"
./test_ric2.sh > work.txt
echo -e "Done!\n"
meld work.txt binary.txt

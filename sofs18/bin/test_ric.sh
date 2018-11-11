#!/bin/bash

echo -e "Testing replenish ircache\nTest 1..."


./test_ric.sh >> work.txt
./test_ric.sh -b >> binary.txt
meld work.txt binary.txt
rm work.txt
rm binary.txt

echo -e "\nDone!\nTest 2..."
./test_ric2.sh >> work.txt
./test_ric2.sh -b >> binary.txt
meld work.txt binary.txt
rm work.txt
rm binary.txt

echo -e "\nDone!"


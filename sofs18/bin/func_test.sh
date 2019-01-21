#!/bin/bash
<<<<<<< HEAD
#seq commands
#
BOLD_BLUE="\e[1;34m"
=======

function call_menu
{
    echo -e "Usage: ./func_test.sh [OPTIION] DISK_NAME"
    echo -e "  OPTIONS:"
    echo -e "   -fi    --- check calls and superblock difference in free_inode (only) with teacher version"
    echo -e "   -fb    --- check calls and superblock difference in free_block (only) with teacher version"
    echo -e "   -ai    --- check calls and superblock difference in alloc_inode (only) with teacher version"
    echo -e "   -ab    --- check calls and superblock difference in alloc_block (only) with teacher version"
    echo -e "   -dbi   --- check calls and superblock difference in deplete_block_insertion_cache (only) with teacher version"
    echo -e "   -dii   --- check calls and superblock difference in deplete_inode_insertion_cache (only) with teacher version"
    echo -e "   -rbr   --- check calls and superblock difference in replenish_block_retrieval_cache (only) with teacher version"
    echo -e "   -rir   --- check calls and superblock difference in replenish_inode_retrieval_cache (only) with teacher version"
    exit 1
}

if [ "$#" != "2" ]; then
  call_menu
  exit 1
fi

BOLD_BLUE="\033[1;34m"
>>>>>>> master
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
REGULAR="\033[1;37m"
BOLD_BLINKING_INVERTED="\e[1;7;5m"
STOP="\e[0m"
<<<<<<< HEAD

function format_dsk
{
  DISK_NAME=$1
  ./mksofs -z $1 > a
  rm a
}

function fill_n_inodes 
{
  DISK_NAME=$1
  INODES=$2
  if [[ ! "$INODES" =~ ^[0-9]+$ ]]
  then
    echo "No number for inodes given"
    exit 1
  fi
  while [ $INODES -gt 0 ]
  do
    let INODES--
    echo -e "ai\n1\nq" | ./testtool $DISK_NAME -q1
  done
}
=======
>>>>>>> master

function check_bin_free_block 
{
  DISK_NAME=$1
  FREES=32
<<<<<<< HEAD
  
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/FREE_BLOCK WITHOUT WORK_DEPLETE:${STOP}"
  ./mksofs $DISK_NAME > iol
  
  while [ $FREES -gt 1 ] 
  do
    echo -e "api\n400\n700\nfdb\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -b -q1 > iol
=======
 
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/FREE_BLOCK WITHOUT WORK_DEPLETE:${STOP}"
  ./mksofs $DISK_NAME > iol

  while [ $FREES -gt 1 ] 
  do
    echo -e "fdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 > iol
>>>>>>> master
    let FREES--
  done

  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
<<<<<<< HEAD
  echo -e "${BOLD_RED}Teacher:"
  echo -e "api\n400\n700\nabi\n400\n700\nfdb\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Data .*//g'
=======
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "fdb\n1\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 | sed -e 's/Data .*//g' | sed -e 's/.*(444)/(444)/g' | sed -e 's/.*(442)/(442)/g' | tail -n+4 | head -n-5 > c_t.txt 
  cat c_t.txt
>>>>>>> master
  ./showblock $DISK_NAME > t.txt
 
  ./mksofs $DISK_NAME > iol
  
  let FREES=32
  while [ $FREES -gt 1 ] 
  do
<<<<<<< HEAD
    echo -e "api\n400\n700\nfdb\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 > iol
=======
    echo -e "fdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -a 444-444 > iol
>>>>>>> master
    let FREES--
  done
  rm iol
 
<<<<<<< HEAD
  echo -e "${BOLD_BLUE}Yours:"
  echo -e "abi\n444\n444\napi\n400\n700\nfdb\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Data .*//g'
=======
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "fdb\n1\nq\n" | ./testtool $DISK_NAME -q1 -a 444-444 -p400-700 | sed -e 's/Data .*//g' | sed -e 's/.*(444)/(444)/g' | sed -e 's/.*(442)/(442)/g' | tail -n+4 | head -n-5 > c_m.txt
>>>>>>> master
  ./showblock $DISK_NAME > m.txt
  cat c_m.txt

  diff t.txt m.txt > a
<<<<<<< HEAD
  if [ -s a ]
  then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:"
    echo -e -n $REGULAR
    cat a
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't fuck up!!!"
=======
  diff c_t.txt c_m.txt > b
  if [ -s b ]; then
    echo -e "${BOLD_RED}Your calls differ from teacher:${STOP}"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm c_t.txt c_m.txt b
  fi
  echo -e "${BOLD_GREEN}Diffs in superblock struct:${STOP}"
  if [ -s a ]; then
    echo -e "${BOLD_RED}Yeah...You screwed up here:${STOP}"
    cat a | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
>>>>>>> master
  fi
  rm t.txt m.txt a
}

function check_bin_free_inode 
{
  DISK_NAME=$1
  FREES=21

<<<<<<< HEAD
=======
  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/FREE_INODE WITHOUT WORK_DEPLETE:${STOP}"
  
  while [ $FREES -gt 1 ] 
  do
    echo -e "fi\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "fi\n${FREES}\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/Inode .*//g' | sed -e 's/.*(402)/(402)/g' | sed -e 's/.*(404)/(404)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 
  ./mksofs $DISK_NAME > iol
  
  let FREES=21
  while [ $FREES -gt 1 ] 
  do
    echo -e "fi\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 > iol
    let FREES--
  done
  rm iol
 
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "fi\n1\nq\n" | ./testtool $DISK_NAME -q1 -a 404-404 -p400-700 | sed -e 's/Inode .*//g' | sed -e 's/.*(404)/(404)/g' | sed -e 's/.*(402)/(402)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
}

function check_bin_alloc_inode 
{
  DISK_NAME=$1

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/ALLOC_INODE WITHOUT WORK_REPLENISH:${STOP}"
  
  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "ai\n1\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/Inode .*//g' | sed -e 's/.*(401)/(401)/g' | sed -e 's/.*(403)/(403)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 
  ./mksofs $DISK_NAME > iol
  
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "ai\n1\nq\n" | ./testtool $DISK_NAME -q1 -a403-403 -p400-700 | sed -e 's/Inode .*//g' | sed -e 's/.*(401)/(401)/g' | sed -e 's/.*(403)/(403)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!${STOP}"
    cat a | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!${STOP}"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:"
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
}

function check_bin_alloc_block 
{
  DISK_NAME=$1

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/ALLOC_BLOCK WITHOUT WORK_REPLENISH:${STOP}"
  
  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "adb\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/Data .*//g' | sed -e 's/.*(441)/(441)/g' | sed -e 's/.*(443)/(443)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 
  ./mksofs $DISK_NAME > iol
  
  echo -e "${BOLD_BLUE}Yours:"
  echo -e "adb\nq\n" | ./testtool $DISK_NAME -q1 -a443-443 -p400-700 | sed -e 's/Data .*//g' | sed -e 's/.*(441)/(441)/g' | sed -e 's/.*(443)/(443)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
}

function check_bin_deplete_bi
{
  DISK_NAME=$1
  FREES=10

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/DEPLETE_BI_CACHE:${STOP}"
  
  echo -e "${REGULAR}Showing what diffs in dal calls between teacher and yours in the 2 cases:"
  echo -e "\tIf regular deplete\n\tIf filt has not enough space"
 
  # test regular deplete
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 1 (REGULAR DEPLETE):${STOP}"
  echo -e "${BOLD_RED}Teacher:${STOP}"
  while [ $FREES -gt 1 ] 
  do
    echo -e "adb\nfdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  echo -e "dbc\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(444)/(444)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 

>>>>>>> master
  ./mksofs $DISK_NAME > iol
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  let FREES=10
  while [ $FREES -gt 1 ] 
  do
    echo -e "adb\nfdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -r444-444 -p400-700 > iol
    let FREES--
  done
  echo -e "dbc\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 | sed -e 's/.*(444)/(444)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

<<<<<<< HEAD
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/FREE_INODE WITHOUT WORK_DEPLETE:${STOP}"
  
  while [ $FREES -gt 1 ] 
  do
    echo -e "api\n400\n700\nabi\n400\n700\nfi\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 > iol
    let FREES--
  done
  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:"
  echo -e "api\n400\n700\nfi\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -b -q1# | sed -e 's/Inode .*//g' > t1.txt
=======
  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
  
  # test limited deplete
  ./mksofs $DISK_NAME > iol
  echo -e $STOP
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 2 (NOT FILLED CACHE):${STOP}"
  let FREES=15
  while [ $FREES -gt 10 ] 
  do
    echo -e "adb\nfdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  while [ $FREES -gt 0 ]; do
    echo -e "fdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "dbc\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(444)/(444)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
  
  ./mksofs $DISK_NAME > iol
  let FREES=15
  while [ $FREES -gt 10 ] 
  do
    echo -e "adb\nfdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -r444-444 -p400-700 > iol
    let FREES--
  done
  while [ $FREES -gt 0 ]; do
    echo -e "fdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -r444-444 -p400-700 > iol
    let FREES--
  done
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "dbc\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 | sed -e 's/.*(444)/(444)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
  
}

function check_bin_replenish_br
{
  DISK_NAME=$1
  ALLOCS=$(./showblock $DISK_NAME | grep 'Number of free data ' | sed -e 's/.*blocks: //g')
  ORIG_ALLOCS=$(($ALLOCS-1))

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/REPLENISH_BR_CACHE:${STOP}"
  
  echo -e "${REGULAR}Showing what diffs in dal calls between teacher and yours in the 3 cases:"
  echo -e "\tIf regular replenish\n\tIf cache is half filled\n\tIf it has not enough blocks to fill the cache"
 
  # test regular replenish
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 1 (REGULAR REPLENISH):${STOP}"
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "rbc\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(443)/(443)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 

  ./mksofs $DISK_NAME > iol
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "rbc\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 | sed -e 's/.*(443)/(443)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
  
  # test limited replenish 
  ./mksofs $DISK_NAME > iol
  echo -e $STOP
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 2 (HALF FULL CACHE):${STOP}"
  let ALLOCS=10
  while [ $ALLOCS -gt 0 ] 
  do
    echo -e "adb\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let ALLOCS--
  done
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "rbc\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(443)/(443)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
  
  ./mksofs $DISK_NAME > iol
  let ALLOCS=10
  while [ $ALLOCS -gt 0 ] 
  do
    echo -e "adb\nq\n" | ./testtool $DISK_NAME -q1 -b -r443-443 -p400-700 > iol
    let ALLOCS--
  done
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "rbc\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 -r443-443 | sed -e 's/.*(443)/(443)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
  
  # test if it does not have enough blocks to replenish 
  ./mksofs $DISK_NAME > iol
  echo -e $STOP
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 3 (NOT ENOUGH FREE BLOCKS TO FILL CACHE):${STOP}"
  let ALLOCS=$(($ORIG_ALLOCS-5))
  while [ $ALLOCS -gt 0 ] 
  do
    echo -e "adb\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let ALLOCS--
  done
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "rbc\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(443)/(443)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
  
  ./mksofs $DISK_NAME > iol
  let ALLOCS=$(($ORIG_ALLOCS-5))
  while [ $ALLOCS -gt 0 ] 
  do
    echo -e "adb\nq\n" | ./testtool $DISK_NAME -q1 -b -r443-443 -p400-700 > iol
    let ALLOCS--
  done
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "rbc\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 | sed -e 's/.*(443)/(443)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
}

function check_bin_deplete_ii
{
  DISK_NAME=$1
  FREES=10

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/DEPLETE_II_CACHE:${STOP}"
  
  echo -e "${REGULAR}Showing what diffs in dal calls between teacher and yours in the 2 cases:"
  echo -e "\tIf regular deplete\n\tIf filt has not enough space"
 
  # test regular deplete
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 1 (REGULAR DEPLETE):${STOP}"
  echo -e "${BOLD_RED}Teacher:${STOP}"
  while [ $FREES -gt 1 ] 
  do
    echo -e "ai\n1\nfi\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  echo -e "dic\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(404)/(404)/g' | head -n-5 | tail -n+4 > t1.txt
>>>>>>> master
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 

  ./mksofs $DISK_NAME > iol
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  let FREES=10
  while [ $FREES -gt 1 ] 
  do
    echo -e "ai\n1\nfi\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  echo -e "dic\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 | sed -e 's/.*(404)/(404)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
  
<<<<<<< HEAD
  let FREES=21
  while [ $FREES -gt 1 ] 
  do
    echo -e "api\n400\n700\nabi\n400\n700\nfi\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 > iol
=======
  # test limited deplete
  ./mksofs $DISK_NAME > iol
  echo -e $STOP
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 2 (TAIL TO HIGH):${STOP}"
  let FREES=15
  while [ $FREES -gt 10 ] 
  do
    echo -e "ai\n1\nfi\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
>>>>>>> master
    let FREES--
  done
  while [ $FREES -gt 0 ]; do
    echo -e "fi\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "dic\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(404)/(404)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
  
  ./mksofs $DISK_NAME > iol
  let FREES=15
  while [ $FREES -gt 10 ] 
  do
    echo -e "ai\n1\nfi\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  while [ $FREES -gt 0 ]; do
    echo -e "fi\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let FREES--
  done
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "dic\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 | sed -e 's/.*(404)/(404)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt


  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
}

function check_bin_replenish_ir
{
  DISK_NAME=$1
  ALLOCS=$(./showblock $DISK_NAME | grep 'Number of free inodes' | sed -e 's/.*inodes: //g')
  ORIG_ALLOCS=$(($ALLOCS-1))

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/REPLENISH_IR_CACHE:${STOP}"
  
  echo -e "${REGULAR}Showing what diffs in dal calls between teacher and yours in the 3 cases:"
  echo -e "\tIf regular replenish\n\tIf cache is half filled\n\tIf it has not enough inodes to fill the cache"
 
  # test regular replenish
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 1 (REGULAR REPLENISH):${STOP}"
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "ric\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(403)/(403)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 
<<<<<<< HEAD
  echo -e "${BOLD_BLUE}Yours:"
  echo -e "abi\n404\n404\napi\n400\n700\nfi\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Inode .*//g' > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  sed -i -e 's/\[01;31m//g' t1.txt
  sed -i -e 's/\[01;34m//g' t1.txt
  sed -i -e 's/\[01;34m//g' m1.txt
  sed -i -e 's/\[01;31m//g' m1.txt
  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  echo -e "Diffs in calls or superblock struct:"
  if [ -s a ]
  then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
    rm t2.txt m2.txt
  fi
  if [ -s b ]
  then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:"
    echo -e -n $REGULAR
    cat b
    rm t1.txt m1.txt a b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't fuck up!!!"
    rm t1.txt m1.txt a t2.txt m2.txt b 
  fi
}

function check_bin_alloc_inode 
{
  DISK_NAME=$1

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/ALLOC_INODE WITHOUT WORK_REPLENISH:${STOP}"

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:"
  echo -e "api\n400\n700\nai\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 -b | sed -e 's/Inode .*//g' > t1.txt
  echo -e "api\n400\n700\nai\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 -b | sed -e 's/Inode .*//g' > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 
  ./mksofs $DISK_NAME > iol
 
  echo -e "${BOLD_BLUE}Yours:"
  echo -e "abi\n403\n403\napi\n400\n700\nai\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Inode .*//g' > m1.txt
  echo -e "abi\n403\n403\napi\n400\n700\nai\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Inode .*//g' > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  sed -i -e 's/\[01;31m//g' t1.txt
  sed -i -e 's/\[01;34m//g' t1.txt
  sed -i -e 's/\[01;34m//g' m1.txt
  sed -i -e 's/\[01;31m//g' m1.txt
  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  echo -e "Diffs in calls or superblock struct:"
  if [ -s a ]
  then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
    rm t2.txt m2.txt
  fi
  if [ -s b ]
  then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:"
    echo -e -n $REGULAR
    cat b
    rm t1.txt m1.txt a b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't fuck up!!!"
    rm t1.txt m1.txt a t2.txt m2.txt b 
  fi
}

function check_bin_alloc_block 
{
  DISK_NAME=$1

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/ALLOC_BLOCK WITHOUT WORK_REPLENISH:${STOP}"

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:"
  echo -e "api\n400\n700\nadb\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q2 -b | sed -e 's/Data .*//g' > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 
  ./mksofs $DISK_NAME > iol
  
  echo -e "${BOLD_BLUE}Yours:"
  echo -e "abi\n443\n443\napi\n400\n700\nadb\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q2 | sed -e 's/Data .*//g' > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  sed -i -e 's/\[01;31m//g' t1.txt
  sed -i -e 's/\[01;34m//g' t1.txt
  sed -i -e 's/\[01;34m//g' m1.txt
  sed -i -e 's/\[01;31m//g' m1.txt
  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  echo -e "Diffs in calls or superblock struct:"
  if [ -s a ]
  then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
    #cat a
    #rm t2.txt m2.txt b
  fi
  if [ -s b ]
  then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:"
    echo -e -n $REGULAR
    cat b
    rm t1.txt m1.txt a
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't fuck up!!!"
    rm t1.txt m1.txt a t2.txt m2.txt b 
=======

  ./mksofs $DISK_NAME > iol
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "ric\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 | sed -e 's/.*(403)/(403)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
  
  # test limited replenish 
  ./mksofs $DISK_NAME > iol
  echo -e $STOP
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 2 (HALF FULL CACHE):${STOP}"
  let ALLOCS=10
  while [ $ALLOCS -gt 0 ] 
  do
    echo -e "ai\n1\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let ALLOCS--
  done
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "ric\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(403)/(403)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
  
  ./mksofs $DISK_NAME > iol
  let ALLOCS=10
  while [ $ALLOCS -gt 0 ] 
  do
    echo -e "ai\n1\nq\n" | ./testtool $DISK_NAME -q1 -b -r403-403 -p400-700 > iol
    let ALLOCS--
  done
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "ric\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 -r403-403 | sed -e 's/.*(403)/(403)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    rm t2.txt m2.txt b 
  fi
  
  # test if it does not have enough inodes to replenish 
  ./mksofs $DISK_NAME > iol
  echo -e $STOP
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING CASE 3 (NOT ENOUGH FREE INODES TO FILL CACHE):${STOP}"
  let ALLOCS=$(($ORIG_ALLOCS-10))
  while [ $ALLOCS -gt 0 ] 
  do
    echo -e "ai\n1\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 > iol
    let ALLOCS--
  done
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "ric\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 | sed -e 's/.*(403)/(403)/g' | head -n-5 | tail -n+4 > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
  
  ./mksofs $DISK_NAME > iol
  let ALLOCS=$(($ORIG_ALLOCS-10))
  while [ $ALLOCS -gt 0 ] 
  do
    echo -e "ai\n1\nq\n" | ./testtool $DISK_NAME -q1 -b -r403-403 -p400-700 > iol
    let ALLOCS--
  done
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "ric\nq\n" | ./testtool $DISK_NAME -q1 -p400-700 | sed -e 's/.*(403)/(403)/g' | tail -n+4 | head -n-5 > m1.txt
  cat m1.txt
  ./showblock $DISK_NAME > m2.txt

  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:${STOP}"
    echo -e -n $REGULAR
    cat b | sed -e 's/>/Your version:/g' | sed -e 's/</Teacher Version:/g'
    rm b
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
    #rm t2.txt m2.txt b 
>>>>>>> master
  fi
}

case $1 in
  -fi)
    check_bin_free_inode $2 
  ;;

<<<<<<< HEAD
  -fi)
    fill_n_inodes $2 $3
=======
  -fb)
    check_bin_free_block $2
>>>>>>> master
  ;;

  -ai)
    check_bin_alloc_inode $2
  ;;
  
  -ab)
    check_bin_alloc_block $2
  ;;

  -dbi)
    check_bin_deplete_bi $2
  ;;

  -dii)
    check_bin_deplete_ii $2
  ;;

  -rbr)
    check_bin_replenish_br $2
  ;;

  -rir)
    check_bin_replenish_ir $2
  ;;

  -bai)
    check_bin_alloc_inode $2
  ;;

  -bab)
    check_bin_alloc_block $2
  ;;

  -f)
    format_dsk $2
  ;;

  *)
<<<<<<< HEAD
    echo -e "Usage: ./func_test.sh [OPTIION] DISK_NAME [VALUES]"
    echo -e "   OPTIONS:"
    echo -e "     -bfi    --- check calls difference in free_inode with teacher version"
    echo -e "     -bfb    --- check calls difference in free_block with teacher version"
    echo -e "     -fi VAL --- fills all inodes in given value"
    echo -e "     -bai    --- check calls difference in alloc_inode with teacher version"
    echo -e "     -bab    --- check calls difference in alloc_block with teacher version"
    echo -e "     -f      --- formats disk with default values"
    exit 1
=======
    call_menu
>>>>>>> master

esac

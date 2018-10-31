#!/bin/bash
#seq commands
#
BOLD_BLUE="\e[1;34m"
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
REGULAR="\033[1;37m"
BOLD_BLINKING_INVERTED="\e[1;7;5m"
STOP="\e[0m"

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

function check_bin_free_block 
{
  DISK_NAME=$1
  FREES=32
  
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/FREE_BLOCK WITHOUT WORK_DEPLETE:${STOP}"
  ./mksofs $DISK_NAME > iol
  
  while [ $FREES -gt 1 ] 
  do
    echo -e "api\n400\n700\nfdb\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -b -q1 > iol
    let FREES--
  done

  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:"
  echo -e "api\n400\n700\nabi\n400\n700\nfdb\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Data .*//g'
  ./showblock $DISK_NAME > t.txt
 
  ./mksofs $DISK_NAME > iol
  
  let FREES=32
  while [ $FREES -gt 1 ] 
  do
    echo -e "api\n400\n700\nfdb\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 > iol
    let FREES--
  done
  rm iol
 
  echo -e "${BOLD_BLUE}Yours:"
  echo -e "abi\n444\n444\napi\n400\n700\nfdb\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Data .*//g'
  ./showblock $DISK_NAME > m.txt

  echo -e "Diffs in superblock struct:"
  diff t.txt m.txt > a
  if [ -s a ]
  then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:"
    echo -e -n $REGULAR
    cat a
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't fuck up!!!"
  fi
  rm t.txt m.txt a
}

function check_bin_free_inode 
{
  DISK_NAME=$1
  FREES=21

  ./mksofs $DISK_NAME > iol

  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/FREE_INODE WITHOUT WORK_DEPLETE:${STOP}"
  
  while [ $FREES -gt 1 ] 
  do
    echo -e "api\n400\n700\nabi\n400\n700\nfi\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 > iol
    let FREES--
  done
  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:"
  echo -e "api\n400\n700\nfi\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -b -q1# | sed -e 's/Inode .*//g' > t1.txt
  cat t1.txt
  ./showblock $DISK_NAME > t2.txt
 
  ./mksofs $DISK_NAME > iol
  
  let FREES=21
  while [ $FREES -gt 1 ] 
  do
    echo -e "api\n400\n700\nabi\n400\n700\nfi\n${FREES}\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 > iol
    let FREES--
  done
  rm iol
 
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
  fi
}

case $1 in
  -bfi)
    check_bin_free_inode $2 
  ;;

  -fi)
    fill_n_inodes $2 $3
  ;;

  -bfb)
    check_bin_free_block $2
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
    echo -e "Usage: ./func_test.sh [OPTIION] DISK_NAME [VALUES]"
    echo -e "   OPTIONS:"
    echo -e "     -bfi    --- check calls difference in free_inode with teacher version"
    echo -e "     -bfb    --- check calls difference in free_block with teacher version"
    echo -e "     -fi VAL --- fills all inodes in given value"
    echo -e "     -bai    --- check calls difference in alloc_inode with teacher version"
    echo -e "     -bab    --- check calls difference in alloc_block with teacher version"
    echo -e "     -f      --- formats disk with default values"
    exit 1

esac

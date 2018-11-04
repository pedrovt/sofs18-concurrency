#!/bin/bash

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
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
REGULAR="\033[1;37m"
BOLD_BLINKING_INVERTED="\e[1;7;5m"
STOP="\e[0m"

function check_bin_free_block 
{
  DISK_NAME=$1
  FREES=32
 
  echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FREELISTS/FREE_BLOCK WITHOUT WORK_DEPLETE:${STOP}"
  ./mksofs $DISK_NAME > iol

  while [ $FREES -gt 1 ] 
  do
    echo -e "fdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -b -q1 -p400-700 > iol
    let FREES--
  done

  echo -e "${BOLD_BLUE}Showing what diffs in dal calls between teacher and yours:"
  echo -e "${BOLD_RED}Teacher:${STOP}"
  echo -e "fdb\n1\nq\n" | ./testtool $DISK_NAME -q1 -b -p400-700 | sed -e 's/Data .*//g' | sed -e 's/.*(444)/(444)/g' | sed -e 's/.*(442)/(442)/g' | tail -n+4 | head -n-5 > c_t.txt 
  cat c_t.txt
  ./showblock $DISK_NAME > t.txt
 
  ./mksofs $DISK_NAME > iol
  
  let FREES=32
  while [ $FREES -gt 1 ] 
  do
    echo -e "fdb\n${FREES}\nq\n" | ./testtool $DISK_NAME -q1 -a 444-444 > iol
    let FREES--
  done
  rm iol
 
  echo -e "${BOLD_BLUE}Yours:${STOP}"
  echo -e "fdb\n1\nq\n" | ./testtool $DISK_NAME -q1 -a 444-444 -p400-700 | sed -e 's/Data .*//g' | sed -e 's/.*(444)/(444)/g' | sed -e 's/.*(442)/(442)/g' | tail -n+4 | head -n-5 > c_m.txt
  ./showblock $DISK_NAME > m.txt
  cat c_m.txt

  diff t.txt m.txt > a
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
    cat a
  else
    echo -e "${BOLD_GREEN}None! It is possible that you didn't screw up!!!"
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

  sed -i -e 's/\[01;31m//g' t1.txt
  sed -i -e 's/\[01;34m//g' t1.txt
  sed -i -e 's/\[01;34m//g' m1.txt
  sed -i -e 's/\[01;31m//g' m1.txt
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
    cat b
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

  sed -i -e 's/\[01;31m//g' t1.txt
  sed -i -e 's/\[01;34m//g' t1.txt
  sed -i -e 's/\[01;34m//g' m1.txt
  sed -i -e 's/\[01;31m//g' m1.txt
  diff t1.txt m1.txt > a
  diff t2.txt m2.txt > b
  if [ -s a ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher!${STOP}"
  else
    echo -e "${BOLD_GREEN}Your calls coincide with the teacher version!${STOP}"
    rm t1.txt m1.txt a
  fi
  if [ -s b ]; then
    echo -e "${BOLD_RED}Yeah...\nYou screwed up here:"
    cat b
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

  sed -i -e 's/\[01;31m//g' t1.txt
  sed -i -e 's/\[01;34m//g' t1.txt
  sed -i -e 's/\[01;34m//g' m1.txt
  sed -i -e 's/\[01;31m//g' m1.txt
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
}

case $1 in
  -fi)
    check_bin_free_inode $2 
  ;;

  -fb)
    check_bin_free_block $2
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

  *)
    call_menu

esac

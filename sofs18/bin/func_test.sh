#!/bin/bash
#seq commands
#
BOLD_BLUE="\033[1;34m"
BOLD_RED="\033[1;31m"

function fill_all_inodes 
{
  DISK_NAME=$1
  INODES=$2
  while [ $INODES -gt 0 ]
  do
    let INODES--
    echo -e "ai\n1\nq" | ./testtool $DISK_NAME -q2
  done
}

function check_bin_inode 
{
  DISK_NAME=$1
  echo -e -n $BOLD_BLUE
  echo -e "Showing what diffs in dal calls between teacher and yours:"
  echo -e -n $BOLD_RED
  echo "Teacher:"
  echo -e "api\n400\n700\nabi\n400\n700\nfi\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Inode .*//g'
  echo -e -n $BOLD_BLUE
  echo "Yours:"
  echo -e "api\n400\n700\nfi\n1\nrpi\n400\n700\nq\n" | ./testtool $DISK_NAME -q1 | sed -e 's/Inode .*//g'
}

case $1 in
  -bi)
    check_bin_inode $2 
  ;;

  -fi)
    fill_all_inodes $2 $3
  ;;

  *)
    echo -e "Usage: ./func_test.sh [OPTIION] DISK_NAME [VALUES]"
    echo -e "\tOPTIONS:"
    echo -e "\t\t-bi     --- check calls difference with teacher version"
    echo -e "\t\t-fi VAL --- fills all inodes in given value"
    exit 1

esac

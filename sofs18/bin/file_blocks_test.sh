#!/bin/bash

BOLD_BLUE="\033[1;34m"
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
REGULAR="\033[1;37m"
BOLD_BLINKING_INVERTED="\e[1;7;5m"
STOP="\e[0m"

function call_menu
{
    echo -e "Usage: ./file_blocks_test.sh [OPTIION] DISK_NAME"
    echo -e "  OPTIONS:"
    echo -e "   -gfb  ---  compares calls and results with bin version of GetFileBlock" 
    echo -e "   -afb  ---  compares calls and results with bin version of AllocFileBlock"
    echo -e "   -ffb  ---  compares calls and results with bin version of FreeFileBlock"
    echo -e "   -rfb  ---  compares calls and results with bin version of ReadFileBlock"
    echo -e "   -wfb  ---  compares calls and results with bin version of WriteFileBlock"
    exit 1
}

if [ "$#" != "2" ]; then
  call_menu
  exit 1
fi

function check_bin_gfb
{
  DISK_NAME=$1
  AFBS=100
  echo -e "${BOLD_BLINKING_INVERTED}TESTING CALLS OF FILEBLOCKS/GET_FILE_BLOCK${STOP}"
  while [ $AFBS -gt 1 ]; do
    echo -e "afb\n1\n${AFBS}\nq" | ./testtool -q2 -b $DISK_NAME > iol
    echo -e "gfb\n1\n${AFBS}\nq" | ./testtool -q2 -b -p400-700 $DISK_NAME | tail -n+4 | head -n-5 > t_${AFBS}.txt 
    let AFBS--
  done

}

function check_bin_afb
{
  DISK_NAME=$1
}

function check_bin_ffb
{
  DISK_NAME=$1
}

function check_bin_rfb
{
  DISK_NAME=$1
}

function check_bin_wfb
{
  DISK_NAME=$1
}

case $1 in
  -gfb)
    check_bin_gfb $2 
  ;;
  -afb)
    check_bin_afb $2 
  ;;
  -ffb)
    check_bin_ffb $2 
  ;;
  -rfb)
    check_bin_rfb $2 
  ;;
  -wfb)
    check_bin_wfb $2 
  ;;
  *)
    call_menu
esac

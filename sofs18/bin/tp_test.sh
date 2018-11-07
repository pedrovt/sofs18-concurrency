#/bin/bash

BOLD_BLUE="\033[1;34m"
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
REGULAR="\033[1;37m"
BOLD_BLINKING_INVERTED="\e[1;7;5m"
STOP="\e[0m"

FLAG_C=1
FLAG_SM=1

function call_menu
{
    echo -e "Usage: ./tp_test.sh [OPTIONS] DISK_NAME"
    echo -e "  OPTIONS:"
    echo -e "    -c  --- disables calls testing"
    echo -e "    -sm --- disables sofsmount testing"
    exit 1
}

if [ $# -lt 1 ]; then
  call_menu
  exit 1
fi

Args=("$@")
for arg in "${Args[@]}"; do
  case $arg in
    -c) 
      FLAG_C=0
    ;;
    
    -sm) 
      FLAG_SM=0
    ;;
    
    -h)
      call_menu
    ;;

    *)
    ;;
  esac
done

FIN=${*: -1}

./mksofs -b -z $FIN > iol
if [ ! -d "mnt" ]; then
  mkdir mnt
fi
./sofsmount -b $FIN mnt
cd mnt
mkdir a
mkdir a/b
mkdir a/b/c
mkdir a/b/c/d
mkdir a/b/c/d/e
mkdir a/b/c/d/e/f
cd ..
fusermount -u mnt

echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING DIRENTIRE/TRAVERSE_PATH${STOP}"

if [ $FLAG_C -ne 0 ]; then
  echo -e "${BOLD_GREEN}Testing traversePath calls:${STOP}"
  echo -e "tp\n/a/b/c/d/e/f\nq\n" | ./testtool -b -r221-221 -p200-600 -q2 $1 | head -n-10 | tail -n+6 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(221)/(221)/g' > calls_m
  #cat calls_m
  echo -e "tp\n/a/b/c/d/e/f\nq\n" | ./testtool -b -q2 -p200-600 $1 | head -n-10 | tail -n+6 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(221)/(221)/g' > calls_t
  #cat calls_t
  diff calls_t calls_m > diffs
  if [ -s diffs ];then
    echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
    cat diffs | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
    rm calls_m calls_t diffs
  fi
fi

rm iol

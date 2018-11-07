#/bin/bash

BOLD_BLUE="\033[1;34m"
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
REGULAR="\033[1;37m"
BOLD_BLINKING_INVERTED="\e[1;7;5m"
STOP="\e[0m"

FLAG_D=1
FLAG_I1=1
FLAG_I2=1

function call_menu
{
    echo -e "Usage: ./ffb_test.sh [OPTIONS] DISK_NAME"
    echo -e "  OPTIONS:"
    echo -e "    -d  --- disables fileblocks/freeFileBlock starting from direct test"
    echo -e "    -i1 --- disables fileblocks/freeFileBlock starting from indirect test"
    echo -e "    -i2 --- disables fileblocks/freeFileBlock starting from double indirect test"
    exit 1
}

if [ $# -lt 1 ]; then
  call_menu
  exit 1
fi

Args=("$@")
for arg in "${Args[@]}"; do
  case $arg in
    -d) 
      FLAG_D=0
    ;;
    
    -i1) 
      FLAG_I1=0
    ;;
    
    -i2) 
      FLAG_I2=0
    ;;

    -h)
      call_menu
    ;;

    *)
    ;;
  esac
done

echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FILEBLOCKS/FREE_FILE_BLOCKS${STOP}"

FIN=${*: -1}

if [ $FLAG_D -ne 0 ]; then
  echo -e "${BOLD_GREEN}Testing freeFileBlock calls starting in direct blocks:${STOP}"
  ./mksofs -q -b $FIN 
  echo -e "afb\n1\n0\nafb\n1\n1\nafb\n1\n2\nafb\n1\n100\nafb\n1\n1000\nffb\n1\n0\nq\n" | ./testtool -b -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_t
  ./mksofs -q -b $FIN 
  echo -e "afb\n1\n0\nafb\n1\n1\nafb\n1\n2\nafb\n1\n100\nafb\n1\n1000\nffb\n1\n0\nq\n" | ./testtool -b -r303-303 -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_m
  diff calls_t calls_m > differences
  if [ -s differences ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
    cat differences | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
    rm calls_m calls_t differences
  fi
fi

if [ $FLAG_I1 -ne 0 ]; then
  echo -e "${BOLD_GREEN}Testing freeFileBlock calls starting in indirect blocks:${STOP}"
  ./mksofs -q -b $FIN 
  echo -e "afb\n1\n100\nafb\n1\n99\nffb\n1\n99\nq\n" | ./testtool -b -p0-699 -q2 $FIN | tail -n+48 | head -n-4 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g'> calls_t
  ./mksofs -q -b $FIN 
  echo -e "rbi\n303\n303\nafb\n1\n100\nafb\n1\n99\nffb\n1\n99\nq\n" | ./testtool -b -p0-699 -q2 $FIN | tail -n+48 | head -n-4 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g'> calls_m
  diff calls_t calls_m > differences
  if [ -s differences ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
    cat differences | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
    rm calls_m calls_t differences
  fi
fi

if [ $FLAG_I2 -ne 0 ]; then
  echo -e "${BOLD_GREEN}Testing freeFileBlock calls starting in double indirect blocks:${STOP}"
  ./mksofs -q -b $FIN 
  echo -e "afb\n1\n1000\nafb\n1\n999\nafb\n1\n998\nffb\n1\n998\nq\n" | ./testtool -b -p0-699 -q2 $FIN | tail -n+80 | head -n-4 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_t
  ./mksofs -q -b $FIN 
  echo -e "rbi\n303\n303\nafb\n1\n1000\nafb\n1\n999\nafb\n1\n998\nffb\n1\n998\nq\n" | ./testtool -b -p0-699 -q2 $FIN | tail -n+80 | head -n-4 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_m
  diff calls_t calls_m > differences
  if [ -s differences ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
    cat differences | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
    rm calls_m calls_t differences
  fi
fi

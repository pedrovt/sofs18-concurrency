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
if [ -d "mnt" ]; then
  rm -r mnt
fi

if [ $FLAG_D -ne 0 ]; then
  echo -e "\n${BOLD_GREEN}Testing freeFileBlock calls starting in direct blocks:${STOP}"
  # create test file with non contiguous blocks for teacher_test
  ./mksofs -b -z -q $FIN 
  if [ ! -d "mnt" ]; then
    mkdir mnt
  fi
  ./sofsmount -b $FIN mnt
  cd mnt
  dd if=/dev/zero of=test_file bs=512 count=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=100 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=1000 status=none
  cd ..
  fusermount -u mnt
  rm -r mnt
  echo -e "ffb\n1\n0\nq\n" | ./testtool -b -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_t_dir
  
  # create test file with non contiguous blocks for student_test
  ./mksofs -b -z -q $FIN 
  if [ ! -d "mnt" ]; then
    mkdir mnt
  fi
  ./sofsmount -b $FIN mnt
  cd mnt
  dd if=/dev/zero of=test_file bs=512 count=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=100 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=1000 status=none
  cd ..
  fusermount -u mnt
  rm -r mnt
  echo -e "ffb\n1\n0\nq\n" | ./testtool -b -r303-303 -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_m_dir
  
  # check calls 
  diff calls_t_dir calls_m_dir > differences
  if [ -s differences ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
    cat differences | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
    rm calls_m_dir calls_t_dir differences
  fi
fi

if [ $FLAG_I1 -ne 0 ]; then
  echo -e "\n${BOLD_GREEN}Testing freeFileBlock calls starting in indirect blocks:${STOP}"
  # create test file with non contiguous blocks for teacher_test
  ./mksofs -b -z -q $FIN 
  if [ ! -d "mnt" ]; then
    mkdir mnt
  fi
  ./sofsmount -b $FIN mnt
  cd mnt
  dd if=/dev/zero of=test_file bs=512 count=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=100 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=1000 status=none
  cd ..
  fusermount -u mnt
  rm -r mnt
  echo -e "ffb\n1\n10\nq\n" | ./testtool -b -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_t_i1
  
  # create test file with non contiguous blocks for student_test
  ./mksofs -b -z -q $FIN 
  if [ ! -d "mnt" ]; then
    mkdir mnt
  fi
  ./sofsmount -b $FIN mnt
  cd mnt
  dd if=/dev/zero of=test_file bs=512 count=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=100 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=1000 status=none
  cd ..
  fusermount -u mnt
  rm -r mnt
  echo -e "ffb\n1\n10\nq\n" | ./testtool -b -r303-303 -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_m_i1
  
  # check calls 
  diff calls_t_i1 calls_m_i1 > differences
  if [ -s differences ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
    cat differences | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
    rm calls_m_i1 calls_t_i1 differences
  fi
fi

if [ $FLAG_I2 -ne 0 ]; then
  echo -e "\n${BOLD_GREEN}Testing freeFileBlock calls starting in double indirect blocks:${STOP}"
  # create test file with non contiguous blocks for teacher_test
  ./mksofs -b -z -q $FIN 
  if [ ! -d "mnt" ]; then
    mkdir mnt
  fi
  ./sofsmount -b $FIN mnt
  cd mnt
  dd if=/dev/zero of=test_file bs=512 count=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=100 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=1000 status=none
  cd ..
  fusermount -u mnt
  rm -r mnt
  echo -e "ffb\n1\n1000\nq\n" | ./testtool -b -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_t_i2
  
  # create test file with non contiguous blocks for student_test
  ./mksofs -b -z -q $FIN 
  if [ ! -d "mnt" ]; then
    mkdir mnt
  fi
  ./sofsmount -b $FIN mnt
  cd mnt
  dd if=/dev/zero of=test_file bs=512 count=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=10 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=100 status=none
  dd if=/dev/zero of=test_file bs=512 count=10 seek=1000 status=none
  cd ..
  fusermount -u mnt
  rm -r mnt
  echo -e "ffb\n1\n1000\nq\n" | ./testtool -b -r303-303 -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(303)/(303)/g' > calls_m_i2
  
  # check calls 
  diff calls_t_i2 calls_m_i2 > differences
  if [ -s differences ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
    cat differences | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
    rm calls_m_i2 calls_t_i2 differences
  fi
fi

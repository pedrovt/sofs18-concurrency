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
FLAG_MN=0

function call_menu
{
    echo -e "Usage: ./afb_test.sh [OPTIONS] DISK_NAME"
    echo -e "  OPTIONS:"
    echo -e "    -d  --- disables fileblocks/allocFileBlock starting from direct test"
    echo -e "    -i1 --- disables fileblocks/allocFileBlock starting from indirect test"
    echo -e "    -i2 --- disables fileblocks/allocFileBlock starting from double indirect test"
    echo -e "    -dd --- test with sofsmount and dd command, will use all ways to alloc"
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

    -dd)
      FLAG_MN=1
      FLAG_D=0
      FLAG_I1=0
      FLAG_I2=0
    ;;
    
    -h)
      call_menu
    ;;

    *)
    ;;
  esac
done

echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING FILEBLOCKS/ALLOC_FILE_BLOCKS${STOP}"

FIN=${*: -1}

if [ $FLAG_MN -ne 0 ]; then
  echo -e "\n${BOLD_GREEN}Testing allocFileBlock calls by mounting and using dd command:${STOP}"
  if [ ! -d "mnt" ]; then
    mkdir mnt
  fi
  # teacher version
  ./mksofs -q $FIN
  ./sofsmount -b $FIN mnt
  cd mnt
  dd if=/dev/zero of=a count=10 bs=512 status=none
  dd if=/dev/zero of=a count=10 bs=512 seek=10 status=none
  dd if=/dev/zero of=a count=10 bs=512 seek=1000 status=none
  cd ..
  fusermount -u mnt
  IT_START=$(./showblock $FIN | grep 'inode table' | sed -e 's/.*: //g' | head -n-1)
  DZ_START=$(./showblock $FIN | grep zone | sed -e 's/.*: //g')
  BLKS_INODE=$(./showblock -i $IT_START $FIN | grep 'd\[' | tail -n-7 | head -n-6)
  echo $BLKS_INODE | sed -e 's/.*d\[\*\] = {//g' | sed -e 's/\}, i1.*//g' | sed -e 's/(nil)//g' > teacher_direct_blocks
  T_I1_BLKS=$(echo $BLKS_INODE | sed -e 's/.*i1\[\*\] = {//g' | sed -e 's/\}, , i2.*//g' | sed -e 's/(nil)//g')
  T_I2_BLKS=$(echo $BLKS_INODE | sed -e 's/.*i2\[\*\] = {//g' | sed -e 's/\},//g' | sed -e 's/(nil)//g')
  ./showblock -r $(($DZ_START+$T_I1_BLKS)) $FIN > teacher_indirect_blocks
  ./showblock -r $(($DZ_START+$T_I2_BLKS)) $FIN > teacher_double_indirect_blocks

  # student version
  ./mksofs -q $FIN
  ./sofsmount $FIN mnt
  cd mnt
  dd if=/dev/zero of=a count=10 bs=512 status=none
  dd if=/dev/zero of=a count=10 bs=512 seek=10 status=none
  dd if=/dev/zero of=a count=10 bs=512 seek=1000 status=none
  cd ..
  fusermount -u mnt
  IT_S_START=$(./showblock $FIN | grep 'inode table' | sed -e 's/.*: //g' | head -n-1)
  DZ_S_START=$(./showblock $FIN | grep zone | sed -e 's/.*: //g')
  S_BLKS_INODE=$(./showblock -i $IT_START $FIN | grep 'd\[' | tail -n-7 | head -n-6)
  echo $S_BLKS_INODE | sed -e 's/.*d\[\*\] = {//g' | sed -e 's/\}, i1.*//g' | sed -e 's/(nil)//g' > student_direct_blocks
  S_I1_BLKS=$(echo $S_BLKS_INODE | sed -e 's/.*i1\[\*\] = {//g' | sed -e 's/\}, , i2.*//g' | sed -e 's/(nil)//g')
  S_I2_BLKS=$(echo $S_BLKS_INODE | sed -e 's/.*i2\[\*\] = {//g' | sed -e 's/\},//g' | sed -e 's/(nil)//g')
  ./showblock -r $(($DZ_START+$S_I1_BLKS)) $FIN > student_indirect_blocks
  ./showblock -r $(($DZ_START+$S_I2_BLKS)) $FIN > student_double_indirect_blocks
  
  rm -r mnt

  diff student_direct_blocks teacher_direct_blocks > direct_diff
  if [ -s direct_diff ]; then
    echo -e "${BOLD_RED}Blocks differ in direct array:${STOP}"
    cat direct_diff | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}Allocating correctly in direct blocks!!!${STOP}"
    rm student_direct_blocks teacher_direct_blocks direct_diff
  fi
  
  diff student_indirect_blocks teacher_indirect_blocks > indirect_diff
  if [ -s indirect_diff ]; then
    echo -e "${BOLD_RED}Blocks differ in indirect array:${STOP}"
    cat indirect_diff | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}Allocating correctly in indirect blocks!!!${STOP}"
    rm student_indirect_blocks teacher_indirect_blocks indirect_diff
  fi
  
  diff student_double_indirect_blocks teacher_double_indirect_blocks > double_diff
  if [ -s double_diff ]; then
    echo -e "${BOLD_RED}Blocks differ in double indirect array:${STOP}"
    cat double_diff | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}Allocating correctly in direct blocks!!!${STOP}"
    rm student_double_indirect_blocks teacher_double_indirect_blocks double_diff
  fi
fi


if [ $FLAG_D -ne 0 ]; then
  echo -e "\n${BOLD_GREEN}Testing allocFileBlock calls starting in direct blocks:${STOP}"
  # check calls 
  ./mksofs -b -z -q $FIN 
  echo -e "afb\n1\n0\nq\n" | ./testtool -b -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(302)/(302)/g' | sed -e 's/.*(441)/(441)/g' | sed -e 's/.*(443)/(443)/g' > calls_t_dir
  
  ./mksofs -b -z -q $FIN 
  echo -e "afb\n1\n0\nq\n" | ./testtool -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(302)/(302)/g' | sed -e 's/.*(441)/(441)/g' | sed -e 's/.*(443)/(443)/g' > calls_m_dir
  
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
  echo -e "\n${BOLD_GREEN}Testing allocFileBlock calls starting in indirect blocks:${STOP}"
  # check calls 
  ./mksofs -b -z -q $FIN 
  echo -e "afb\n1\n10\nq\n" | ./testtool -b -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(302)/(302)/g' | sed -e 's/.*(441)/(441)/g' | sed -e 's/.*(443)/(443)/g' > calls_t_i1
  
  ./mksofs -b -z -q $FIN 
  echo -e "afb\n1\n10\nq\n" | ./testtool -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(302)/(302)/g' | sed -e 's/.*(441)/(441)/g' | sed -e 's/.*(443)/(443)/g' > calls_m_i1
  
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
  echo -e "\n${BOLD_GREEN}Testing allocFileBlock calls starting in double indirect blocks:${STOP}"
  # check calls 
  ./mksofs -b -z -q $FIN 
  echo -e "afb\n1\n1000\nq\n" | ./testtool -b -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(302)/(302)/g' | sed -e 's/.*(441)/(441)/g' | sed -e 's/.*(443)/(443)/g' > calls_t_i2
  
  ./mksofs -b -z -q $FIN 
  echo -e "afb\n1\n1000\nq\n" | ./testtool -p0-699 -q1 $FIN | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(302)/(302)/g' | sed -e 's/.*(441)/(441)/g' | sed -e 's/.*(443)/(443)/g' > calls_m_i2
  
  diff calls_t_i2 calls_m_i2 > differences
  if [ -s differences ]; then
    echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
    cat differences | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
  else
    echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
    rm calls_m_i2 calls_t_i2 differences
  fi
fi

#/bin/bash

BOLD_BLUE="\033[1;34m"
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
REGULAR="\033[1;37m"
BOLD_BLINKING_INVERTED="\e[1;7;5m"
STOP="\e[0m"

function call_menu
{
    echo -e "Usage: ./tp_test.sh DISK_NAME"
    exit 1
}

if [ $# -lt 1 ]; then
  call_menu
  exit 1
fi

Args=("$@")
for arg in "${Args[@]}"; do
  case $arg in
    -h)
      call_menu
    ;;

    *)
    ;;
  esac
done

FIN=${*: -1}

./mksofs -b -z -q $FIN 
if [ ! -d "mnt" ]; then
  mkdir mnt
fi

./sofsmount -b $FIN mnt
cd mnt
# regular path
mkdir a
mkdir a/b
# will remove x perms in this one
mkdir a/b/c
touch a/b/c/d
cd ..
#pwd

fusermount -u mnt
rm -r mnt
# gives rw-rw-rw perms to inode 3 (dir c)
echo -e "sia\n3\n666\nq\n" | ./testtool -q2 -b $FIN > a

echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING DIRENTIRE/TRAVERSE_PATH${STOP}"

# regular path testing
echo -e "${BOLD_GREEN}Testing regular traversePath calls (/a/b):${STOP}"
echo -e "tp\n/a/b\nq\n" | ./testtool -b -r221-221 -p200-600 -q2 $1 | head -n-6 | tail -n+5 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadFile.*/soReadFile/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(221)/(221)/g' > calls_m
echo -e "tp\n/a/b\nq\n" | ./testtool -b -q2 -p200-600 $1 | head -n-6 | tail -n+5 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/soReadFile.*/soReadFile/g' | sed -e 's/.*(221)/(221)/g' > calls_t
sed -i '/581/d' calls_m
diff calls_t calls_m > diffs
if [ -s diffs ];then
  echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
  cat diffs | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
else
  echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
  rm calls_m calls_t diffs
fi

# non-existent path testing
echo -e "\n${BOLD_GREEN}Testing non-existent path traversePath calls (/a/b/d):${STOP}"
echo -e "${BOLD_GREEN}MUST throw no such file or dir exception TWICE:${STOP}"
echo -e "tp\n/a/b/d/\nq\n" | ./testtool -b -r221-221 -p200-600 -q2 $1 | head -n-6 | tail -n+5 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadFile.*/soReadFile/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(221)/(221)/g' > calls_m
echo -e "tp\n/a/b/d/\nq\n" | ./testtool -b -q2 -p200-600 $1 | head -n-6 | tail -n+5 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/soReadFile.*/soReadFile/g' | sed -e 's/.*(221)/(221)/g' > calls_t
sed -i '/581/d' calls_m
diff calls_t calls_m > diffs
if [ -s diffs ];then
  echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
  cat diffs | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
else
  echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
  rm calls_m calls_t diffs
fi

# access denied path testing
echo -e "\n${BOLD_GREEN}Testing permission denied traversePath calls (/a/b/c):${STOP}"
echo -e "${BOLD_GREEN}MUST throw permission denied exception TWICE:${STOP}"
echo -e "tp\n/a/b/c/d\nq\n" | ./testtool -b -r221-221 -p200-600 -q2 $1 | head -n-6 | tail -n+5 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadFile.*/soReadFile/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/.*(221)/(221)/g' > calls_m
echo -e "tp\n/a/b/c/d\nq\n" | ./testtool -b -q2 -p200-600 $1 | head -n-5 | tail -n+5 | sed -e 's/soWriteData.*$/soWriteData/g' | sed -e 's/soReadData.*/soReadData/g' | sed -e 's/soReadFile.*/soReadFile/g' | sed -e 's/.*(221)/(221)/g' > calls_t
sed -i '/581/d' calls_m
diff calls_t calls_m > diffs
if [ -s diffs ];then
  echo -e "${BOLD_RED}Your calls differ from the teacher:${STOP}"
  cat diffs | sed -e 's/</Teacher Version:/g' | sed -e 's/>/Your Version:/g'
else
  echo -e "${BOLD_GREEN}No differences, you might have not screwed up!${STOP}"
  rm calls_m calls_t diffs
fi

#/bin/bash

BOLD_BLUE="\033[1;34m"
BOLD_RED="\033[1;31m"
BOLD_GREEN="\033[1;32m"
REGULAR="\033[1;37m"
BOLD_BLINKING_INVERTED="\e[1;7;5m"
STOP="\e[0m"

FLAG_CS=1
FLAG_SB=1
FLAG_FILT=1
FLAG_IT=1
FLAG_FBLT=1
FLAG_RD=1
FLAG_RB=1

function call_menu
{
    echo -e "Usage: ./test_mksofs.sh [OPTIIONS] DISK_NAME"
    echo -e "  OPTIONS:"
    echo -e "    -cs   --- disables mksofs/computeStructure test"
    echo -e "    -sb   --- disables mksofs/fillInSuperBlock test"
    echo -e "    -filt --- disables mksofs/fillInFreeInodeListTable test"
    echo -e "    -it   --- disables mksofs/fillInInodeTable test"
    echo -e "    -fblt --- disables mksofs/fillInFreeBlockListTable test"
    echo -e "    -rd   --- disables mksofs/fillInRootDir test"
    exit 1
}

echo -e "${BOLD_BLINKING_INVERTED}NOW TESTING MKSOFS${STOP}"

Args=("$@")
for arg in "${Args[@]}"; do
  case $arg in
    -cs) 
      FLAG_CS=0
    ;;
    
    -sb) 
      FLAG_SB=0
    ;;
    
    -filt) 
      FLAG_FILT=0
    ;;

    -it) 
      FLAG_IT=0
    ;;

    -fblt) 
      FLAG_FBLT=0
    ;;

    -rd) 
      FLAG_RD=0
    ;;
    
    *)
      echo $arg
      echo -e "Invalid Argument"
    ;;
  esac
done

echo -en "${BOLD_BLUE}Size os disks (default=1M):${STOP} "
read SIZE
if [ -z $SIZE ]; then
  SIZE=1M
fi

echo -e "${BOLD_GREEN}Creating Disks (${SIZE})" 
rm t m > l 2> l
rm l
./createDisk t $SIZE 2> a
cp t m

echo -e "${BOLD_GREEN}Formatting disks"
./mksofs -z -b -d t | sed -e 's/.*(751).*//g' | sed -e 's/.*(752).*//g' | sed -e 's/.*(791).*//g' | sed -e 's/.*(60/(60/g' | sed -e 's/.*SOFS18.*/Installing SOFS18/g' | sed '/^$/d' > c_a
./mksofs -z -d m | sed -e 's/.*(751).*//g' | sed -e 's/.*(752).*//g' | sed -e 's/.*(791).*//g' | sed -e 's/.*(60/(60/g' | sed -e 's/.*SOFS18.*/Installing SOFS18/g' | sed '/^$/d' > c_b
diff c_a c_b > calls

echo -e "${BOLD_GREEN}Checking call Diffs:"
if [ -s calls ]; then
  echo -e "${BOLD_RED}There are differences in the calls!${STOP}"
  CS=$(cat calls | grep 601 | sed -e 's/</Teacher Version:/g' | sed -e 's/>/| Your Version:/g')
  SB=$(cat calls | grep 602 | sed -e 's/</Teacher Version:/g' | sed -e 's/>/| Your Version:/g')
  FILT=$(cat calls | grep 603 | sed -e 's/</Teacher Version:/g' | sed -e 's/>/| Your Version:/g')
  IT=$(cat calls | grep 604 | sed -e 's/</Teacher Version:/g' | sed -e 's/>/| Your Version:/g')
  FBLT=$(cat calls | grep 605 | sed -e 's/</Teacher Version:/g' | sed -e 's/>/| Your Version:/g') 
  RD=$(cat calls | grep 606 | sed -e 's/</Teacher Version:/g' | sed -e 's/>/| Your Version:/g')
  RB=$(cat calls | grep 607 | sed -e 's/</Teacher Version:/g' | sed -e 's/>/| Your Version:/g')

  if [ ! -z "$CS" ]; then
    echo -e "${BOLD_RED}Error in ComputeStructure Call:${STOP}"
    echo -e  $CS
  fi
  if [ ! -z "$SB" ]; then
    echo -e "${BOLD_RED}Error in SuperBlock Call:${STOP}"
    echo -e  $SB
  fi
  if [ ! -z "$FILT" ]; then
    echo -e "${BOLD_RED}Error in FillInFreeInodeTable Call:${STOP}"
    echo -e  $FILT
  fi
  if [ ! -z "$IT" ]; then
    echo -e "${BOLD_RED}Error in InodeTable Call:${STOP}"
    echo -e  $IT
  fi
  if [ ! -z "$FBLT" ]; then
    echo -e "${BOLD_RED}Error in FillInFreeBlockListTable Call:${STOP}"
    echo -e  $FBLT
  fi
  if [ ! -z "$RD" ]; then
    echo -e "${BOLD_RED}Error in FillInRootDir Call:${STOP}"
    echo -e  $RD
  fi
  if [ ! -z "$RB" ]; then
    echo -e "${BOLD_RED}Error in ResetBlocks Call:${STOP}"
    echo -e  $RB
  fi
else
  echo -e "${BOLD_GREEN}There are no differences in the calls!${STOP}"
  rm c_a c_b calls
fi

./showblock t > s_a
./showblock m > s_b

# FILT stuff
FILT_T_POS=$(cat s_a | grep 'First block of the free inode' | sed -e 's/.*table: //g')
FILT_T_SIZE=$(cat s_a | grep 'blocks of the free inode' | sed -e 's/.*table: //g')
FILT_M_POS=$(cat s_b | grep 'First block of the free inode' | sed -e 's/.*table: //g')
FILT_M_SIZE=$(cat s_b | grep 'blocks of the free inode' | sed -e 's/.*table: //g')

# FBLT stuff
FBLT_T_POS=$(cat s_a | grep 'First block of the free block' | sed -e 's/.*table: //g')
FBLT_T_SIZE=$(cat s_a | grep 'blocks of the free block' | sed -e 's/.*table: //g')
FBLT_M_POS=$(cat s_b | grep 'First block of the free block' | sed -e 's/.*table: //g')
FBLT_M_SIZE=$(cat s_b | grep 'blocks of the free block' | sed -e 's/.*table: //g')

# IT stuff
IT_T_POS=$(cat s_a | grep 'First block of the inode' | sed -e 's/.*table: //g')
IT_T_SIZE=$(cat s_a | grep 'blocks of the inode' | sed -e 's/.*table: //g')
IT_M_POS=$(cat s_b | grep 'First block of the inode' | sed -e 's/.*table: //g')
IT_M_SIZE=$(cat s_b | grep 'blocks of the inode' | sed -e 's/.*table: //g')

# RD stuff
RD_T_POS=$(cat s_a | grep 'data zone' | sed -e 's/.*zone: //g')
RD_M_POS=$(cat s_b | grep 'data zone' | sed -e 's/.*zone: //g')

# SuperBlock tests
if [ $FLAG_SB -ne 0 ]; then
  diff s_a s_b > superblock
  if [ -s superblock ]; then
    echo -e "${BOLD_RED}There are differences in the superblock!${STOP}"
    cat superblock
  else
    echo -e "${BOLD_GREEN}There are no differences in the superblock!${STOP}"
    rm superblock
  fi
fi

# Free Inode List Table tests
if [ $FLAG_FILT -ne 0 ]; then
  if [ $FILT_T_POS -ne $FILT_M_POS ]; then
    echo -e "${BOLD_RED}Starting points of FILT's don't match!${STOP}"
  elif [ $FILT_T_SIZE -ne $FILT_M_SIZE ]; then
    echo -e "${BOLD_RED}Size of FILT's don't match!${STOP}"
  fi
  if [ $FILT_T_SIZE -gt 1 ]; then
    GOOD=1
    INDEX=0
    for i in $(seq 0 $(($FILT_T_SIZE-1))); do
      let INDEX=$(($FILT_T_POS+$i))
      ./showblock -r $INDEX t > filt_a
      ./showblock -r $INDEX m > filt_b
      diff filt_a filt_b > filt_diffs
      if [ -s filt_diffs ]; then
        break
      fi
    done
    if [ $GOOD ]; then
      echo -e "${BOLD_GREEN}There are no differences in the FILT!${STOP}"
      rm filt_a filt_b filt_diffs
    else
      echo -e "${BOLD_RED}There are differences in the FILT starting on block ${INDEX}!${STOP}"
      cat filt_diffs
    fi
  else
    ./showblock -r $FILT_T_POS t > filt_a
    ./showblock -r $FILT_M_POS m > filt_b
    diff filt_a filt_b > filt_diffs
    if [ -s filt_diffs ]; then
      echo -e "${BOLD_RED}Error in the FILT!${STOP}"
      cat filt_diffs
    else
      echo -e "${BOLD_GREEN}There are no differences in the FILT!${STOP}"
      rm filt_a filt_b filt_diffs
    fi
  fi
fi

# Inode List Table tests
if [ $FLAG_IT -ne 0 ]; then
  if [ $IT_T_POS -ne $IT_M_POS ]; then
    echo -e "${BOLD_RED}Starting points of IT's don't match!${STOP}"
  elif [ $IT_T_SIZE -ne $IT_M_SIZE ]; then
    echo -e "${BOLD_RED}Size of IT's don't match!${STOP}"
  fi
  if [ $IT_T_SIZE -gt 1 ]; then
    GOOD=1
    INDEX=0
    for i in $(seq 0 $(($IT_T_SIZE-1))); do
      let INDEX=$(($IT_T_POS+$i))
      ./showblock -r $INDEX t > it_a
      ./showblock -r $INDEX m > it_b
      diff it_a it_b > it_diffs
      if [ -s it_diffs ]; then
        break
      fi
    done
    if [ $GOOD ]; then
      echo -e "${BOLD_GREEN}There are no differences in the IT!${STOP}"
      rm it_a it_b it_diffs
    else
      echo -e "${BOLD_RED}There are differences in the IT starting on block ${INDEX}!${STOP}"
      cat it_diffs
    fi
  else
    ./showblock -r $IT_T_POS t > it_a
    ./showblock -r $IT_M_POS m > it_b
    diff it_a it_b > it_diffs
    if [ -s it_diffs ]; then
      echo -e "${BOLD_RED}Error in the IT!${STOP}"
      cat it_diffs
    else
      echo -e "${BOLD_GREEN}There are no differences in the IT!${STOP}"
      rm it_a it_b it_diffs
    fi
  fi
fi

# Free BLock List Table tests
if [ $FLAG_FBLT -ne 0 ]; then
  if [ $FBLT_T_POS -ne $FBLT_M_POS ]; then
    echo -e "${BOLD_RED}Starting points of FBLT's don't match!${STOP}"
  elif [ $FBLT_T_SIZE -ne $FBLT_M_SIZE ]; then
    echo -e "${BOLD_RED}Size of FBLT's don't match!${STOP}"
  fi
  if [ $FBLT_T_SIZE -gt 1 ]; then
    GOOD=1
    INDEX=0
    for i in $(seq 0 $(($FBLT_T_SIZE-1))); do
      let INDEX=$(($FBLT_T_POS+$i))
      ./showblock -r $INDEX t > fblt_a
      ./showblock -r $INDEX m > fblt_b
      diff fblt_a fblt_b > fblt_diffs
      if [ -s fblt_diffs ]; then
        break
      fi
    done
    if [ $GOOD ]; then
      echo -e "${BOLD_GREEN}There are no differences in the FBLT!${STOP}"
      rm fblt_a fblt_b fblt_diffs
    else
      echo -e "${BOLD_RED}There are differences in the FBLT starting on block ${INDEX}!${STOP}"
      cat fblt_diffs
    fi
  else
    ./showblock -r $FBLT_T_POS t > fblt_a
    ./showblock -r $FBLT_M_POS m > fblt_b
    diff fblt_a fblt_b > fblt_diffs
    if [ -s fblt_diffs ]; then
      echo -e "${BOLD_RED}Error in the FBLT!${STOP}"
      cat fblt_diffs
    else
      echo -e "${BOLD_GREEN}There are no differences in the FBLT!${STOP}"
      rm fblt_a fblt_b fblt_diffs
    fi
  fi
fi

# Root Directory tests
if [ $FLAG_RD -ne 0 ]; then
  if [ $RD_T_POS -ne $RD_M_POS ]; then
    echo -e "${BOLD_RED}Root Directory BLock does not coincide!${STOP}"
  fi
  ./showblock -d $RD_T_POS t > directory_a
  ./showblock -d $RD_M_POS m > directory_b
  diff directory_a directory_b > directory_diffs
  if [ -s directory_diffs ]; then
    echo -e "${BOLD_RED}Error in the root directory${STOP}"
    cat directory_diffs
  else
    echo -e "${BOLD_GREEN}There are no differences in the root directory!${STOP}"
    rm directory_a directory_b directory_diffs
  fi
fi

rm m t s_a s_b

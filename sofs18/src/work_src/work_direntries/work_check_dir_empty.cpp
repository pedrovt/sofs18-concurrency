#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs18{
    namespace work{
        bool soCheckDirEmpty(int ih){
            soProbe(205, "%s(%d)\n", __FUNCTION__, ih);

            /* change the following line by your code */
            SODirEntry targetDirEntry;
            SODirEntry blockDirEntries[DirentriesPerBlock];				// array to store entries of a block
            sofs18::soReadFileBlock(ih, 0, blockDirEntries);			// read block to the array
            for (uint32_t i = 2; i < DirentriesPerBlock; i++) {			// cycling each dirEntry
                targetDirEntry = blockDirEntries[i];
                if (targetDirEntry.in != NullReference){				// targetDirEntry should have name equal to the name passed as argument
                    return false;               						// return inode number of the dirEntry to delete
                }	
            }
            return true;
            //return bin::soCheckDirEmpty(ih);
        }
    };
};
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
            SODirEntry blockDirEntries[DirentriesPerBlock];				// array to store entries of a block
            sofs18::soReadFileBlock(ih, i, blockDirEntries);			// read block to the array
            for (uint32_t t = 0; t < DirentriesPerBlock; t++) {			// cycling each dirEntry
                targetDirEntry = blockDirEntries[i];
                if (strcmp(".", targetDirEntry.name) == 0 or strcmp("..", targetDirEntry.name) == 0){	// targetDirEntry should be either "." or ".."
                    return false;               														
                }	
            }
            return true;
            //return bin::soCheckDirEmpty(ih);
        }
    };
};


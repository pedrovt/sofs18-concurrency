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
        uint32_t soDeleteDirEntry(int pih, const char *name){
            soProbe(203, "%s(%d, %s, %s)\n", __FUNCTION__, pih, name);

            /* change the following line by your code */
            SODirEntry targetDirEntry;										// DirEntry to delete

            if (strlen(name) > SOFS18_MAX_NAME){							// check if name is within the supposed length range
                throw SOException(ENAMETOOLONG, __FUNCTION__);
            }

            SOInode* inode = soITGetInodePointer(pih);						// get inode
            uint32_t numBlocks = (inode->size)/BlockSize;					// number of blocks with dirEntries
            for (uint32_t i = 0; i < numBlocks; i++) {						// cycling all the blocks with dirEntries
                SODirEntry blockDirEntries[DirentriesPerBlock];				// array to store entries of a block
                soReadFileBlock(pih, i, blockDirEntries);					// read block to the array
                for (uint32_t t = 0; t < DirentriesPerBlock; t++) {			// cycling each dirEntry
                    targetDirEntry = blockDirEntries[i];
                    if (strcmp(name, targetDirEntry.name) == 0){			// targetDirEntry should have name equal to the name passed as argument
                        strcpy(targetDirEntry.name, "\0");		 			// delete its name
                        return targetDirEntry.in;                   		// return inode number of the dirEntry to delete
                    }	
                }
            }
            throw SOException(ENOENT, __FUNCTION__);					// if targetDirEntry not found, throw exception
            //return bin::soDeleteDirEntry(pih, name);
        }
    };
};


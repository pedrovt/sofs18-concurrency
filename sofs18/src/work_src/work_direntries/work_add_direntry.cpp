#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs18
{
    namespace work
    {

        void soAddDirEntry(int pih, const char *name, uint32_t cin)
        {
            soProbe(202, "%s(%d, %s, %u)\n", __FUNCTION__, pih, name, cin);
            printf("\tUsing work version\n");

            /* verify sanity of arguments */
            if (!name) {
                throw SOException(EINVAL, __FUNCTION__);
            } 

            if (strlen(name) > SOFS18_MAX_NAME) {
                throw SOException(ENAMETOOLONG, __FUNCTION__);
            }

            // todo verify sanity of number of the inode??
            // if (cin < 0 || cin > numInodes)

            /* get inode of the directory */
            SOInode* inode = soITGetInodePointer(pih);

            /* get number of blocks associated to the dir */ 
            uint32_t numBlocks = (inode->size) / BlockSize;
            printf("\tNumber of blocks: %d", numBlocks);

            /* number and position in the block of the first free entry */
            int numFreeBlock = -1;
            int posFreeBlock = -1;

            /* for each block with the directory contents */
            for (uint32_t i = 0; i < numBlocks; i++) {
                /* read data block */
                SODirEntry entries[DirentriesPerBlock];
                soReadFileBlock(pih, i, entries);

                /* verify if there's a free position in the block */
                for (uint32_t j = 0; j < DirentriesPerBlock; j++) {
                    SODirEntry entry = entries[i];

                    /* found free position */ 
                    if (numFreeBlock == -1 && entry.in == 0) {
                        printf("\tFree position at block %d, pos %d", i, j);
                        numFreeBlock = i;
                        posFreeBlock = j;
                        // todo consider optimization of copying the entries array
                    }

                    /* can't have repeated file names in the same dir! */
                    else if (strcmp(name, entry.name) == 0) {
                        throw SOException(EEXIST, __FUNCTION__);
                        /* Tested binary version. 
                         * The file system is not case sensitive, 
                         * ie file /path/A != /path/a 
                         * Therefore, there's no need to use strcasecmp,
                         * strcmp function is enough */
                    }
                }
            }

            /* add the entry */
            SODirEntry entry;
            entry.in = cin;
            strcpy(entry.name, name);

            /* no free position was found -> write a new file block */
            if (numFreeBlock == -1) {
                /* create new block contents */
                SODirEntry entries[DirentriesPerBlock];

                // first position has the new entry
                entries[0] = entry;

                // others are initialized as free 
                SODirEntry freeEntry;
                entry.in = NullReference;
                memset(entry.name, 0, SOFS18_MAX_NAME);

                for (uint32_t i = 1 ; i < DirentriesPerBlock; i++) {
                    entries[i] = freeEntry;
                }

                /* write the block */
                soWriteFileBlock(pih, numBlocks, entries);
            }

            /* free position was found */
            else {
                SODirEntry entries[DirentriesPerBlock];
                soReadFileBlock(pih, numFreeBlock, entries);
                entries[posFreeBlock] = entry;
                soWriteFileBlock(pih, numFreeBlock, entries); 
            }

            /* change the following line by your code */
            //bin::soAddDirEntry(pih, name, cin);
        }

    };

};


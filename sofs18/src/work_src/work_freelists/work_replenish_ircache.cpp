/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2018
 */

#include "work_freelists.h"

#include <string.h>
#include <errno.h>
#include <iostream>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs18
{
    namespace work
    {

        using namespace std;

        /*  Considerations
                Nothing should be done if the retrieval cache is not empty.
                The insertion cache should only be used if the free inode list table is empty.
                Only a single block should be processed, even if it is not enough to fulfill the retrieval cache.
                The block to be processes is the one pointed to by the filt_head field of the superblock.
            Steps
                1. If cache is not empty (ie idx != size of cache), return
                2. Else, if table is not free (ie head != tail), copy 1 block (max) in the end of the cache and update idx
                3. Else, if insertion cache has elements, copy 1 block (max) from insertion cache and update idx
                4. Else, no Inodes are free, throw an exception [see man errno]
        */
        void soReplenishIRCache(void)
        {
            soProbe(403, "%s()\n", __FUNCTION__);
            printf("\tUsing work version\n");

            SOSuperBlock* sb = soSBGetPointer();
            SOInodeReferenceCache ircache = sb -> ircache;

            /* If cache is not empty (idx != size of cache), 
            there's no need to replenish the cache! ------------------------- */
            if (ircache.idx != INODE_REFERENCE_CACHE_SIZE - 1) {
                return;
            }

            /* If no inodes are free, can't replenish the cache! ------------ */
            // Considering no free inodes as "No space left on device"
            if (sb->ifree == 0) {
                throw SOException(ENOSPC, __FUNCTION__);
            }
           
            /* If FILT is not empty (ie head != tail), copy from it --------- */
            if (sb -> filt_head != sb -> filt_tail) {
                // Number of the block
                uint32_t numBlock    = (sb -> filt_head) / ReferencesPerBlock; 
                
                // Position in the block
                uint32_t posBlock    = (sb -> filt_head) % ReferencesPerBlock; 
                
                // Determine number of elements to be moved
                // - Max number of references that can be copied from that block
                uint32_t numRefsToMove = ReferencesPerBlock - posBlock;

                // - Max number of references that can be copied from the FILT
                uint32_t numRefsInFilt  = sb -> filt_tail - sb -> filt_head;
                
                // - Head and tail can be in the same block, which means there
                // will be less than numRefsToMove valid references
                if (numRefsInFilt < numRefsToMove) {
                    numRefsToMove = numRefsInFilt;
                }

                // Move from FILT to the cache
                uint32_t* block = soFILTOpenBlock(numBlock);

                // If there's enough references, completely replenish the cache
                if (numRefsToMove >= INODE_REFERENCE_CACHE_SIZE)
                {
                    uint32_t numBytes = INODE_REFERENCE_CACHE_SIZE * sizeof(uint32_t);

                    // Copy from FILT to cache
                    memcpy(&(ircache.ref), &block[posBlock], numBytes);

                    // Remove from FILT (no need to use NullReference macro)
                    memset(&block[posBlock], 0xFF, numBytes);

                    // Update counters
                    ircache.idx = 0;
                    sb -> filt_head += INODE_REFERENCE_CACHE_SIZE; 
                }

                // Otherwise, copy to the end of the cache
                else {
                    uint32_t numBytes = numRefsToMove * sizeof(uint32_t);
                    uint32_t posCache = INODE_REFERENCE_CACHE_SIZE - numRefsToMove;

                    // Copy from FILT to cache
                    memcpy(&(ircache.ref[posCache]), &block[posBlock], numBytes);

                    // Remove from FILT (no need to use NullReference macro)
                    memset(&block[posBlock], 0xFF, numBytes);

                    // Update counters
                    ircache.idx = posCache;
                    sb->filt_head += numRefsToMove;
                }
            }

            /* Else, if insertion cache is not empty, copy from it ---------- */
            SOInodeReferenceCache iicache = sb->iicache;
            if (iicache.idx != 0)
            {
                uint32_t numBytes = iicache.idx * sizeof(uint32_t);
                uint32_t posCache = INODE_REFERENCE_CACHE_SIZE - iicache.idx;

                // Copy from iicache
                memcpy(&(ircache.ref[posCache]), iicache.ref, numBytes);

                // Remove from iicache
                memset(&(iicache.ref), 0xFF, numBytes);

                // Update counters
                ircache.idx = posCache;
                sb -> filt_head += INODE_REFERENCE_CACHE_SIZE - iicache.idx;
            }
            
            else {
                // INTERNAL ERROR. Should not happen!
                printf("Internal Error in Replenish ircache!");
                exit(1);
            }

            soFILTSaveBlock();
            soFILTCloseBlock();
            soSBSave();

            /* To use the given binary version ------------------------------ */
            //bin::soReplenishIRCache();
        }

    };

};


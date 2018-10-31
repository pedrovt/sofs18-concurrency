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

        void soReplenishIRCache(void)
        {
            // Pedro Teixeira 84715

            soProbe(403, "%s()\n", __FUNCTION__);
            printf("\tUsing work version\n");

            SOSuperBlock* sb = soSBGetPointer();
            SOInodeReferenceCache ircache = sb -> ircache;

            /* the 2 following verifications could/should be done in the 
             * allocInode function but to ensure everything works properly 
             * they're repeated. */ 

            /* if cache is not empty (idx != size of cache), 
             * there's no need to replenish the cache */
            if (ircache.idx != INODE_REFERENCE_CACHE_SIZE) {
                //printf("\tCache not empty. No need to replenish!\n");
                return;
            }

            /* if no inodes are free, can't replenish the cache */
            // Considering no free inodes as "No space left on device"
            if (sb->ifree == 0) {
                //printf("\tNo inodes are free! Replenish cache is not possible!\n");
                throw SOException(ENOSPC, __FUNCTION__);
            }

            printf("\tBefore FILT head: %d\n\tBefore FILT tail: %d\n", sb->filt_head, sb->filt_tail);
            /* copy from FILT if it's not empty (ie head != tail) */
            if (sb -> filt_head != sb -> filt_tail) {
                printf("\tReplenishing from FILT\n");

                // Number of the block
                uint32_t numBlock    = (sb -> filt_head) / ReferencesPerBlock; 
                
                // Position in the block
                uint32_t posBlock    = (sb -> filt_head) % ReferencesPerBlock; 
                
                // Determine number of elements to be moved
                // - Max number of references that can be copied from that block
                uint32_t numRefsToMove = ReferencesPerBlock - posBlock;

                // - Max number of references that can be copied from the FILT
                uint32_t numRefsInFilt  = sb -> filt_tail - sb -> filt_head;
                
                // printf("\tnumRefsToMove: %d\n\tnumRefsInFilt: %d\n", numRefsToMove, numRefsInFilt);
                // - Head and tail can be in the same block, which means there
                // will be less than numRefsToMove valid references
                if (numRefsToMove > numRefsInFilt)
                {
                   // printf("\tIF\n");
                    numRefsToMove = numRefsInFilt;
                }

                //printf("\tnumRefsToMove: %d\n\tnumRefsInFilt: %d\n", numRefsToMove, numRefsInFilt);
                // Move from FILT to the cache
                uint32_t* block = soFILTOpenBlock(numBlock);

                // If there's enough references, completely replenish the cache
                if (numRefsToMove >= INODE_REFERENCE_CACHE_SIZE)
                {
                    printf("\t\tEnough references on FILT. Replenish will be full.\n");
                    uint32_t numBytes = INODE_REFERENCE_CACHE_SIZE * sizeof(uint32_t);

                    // Copy from FILT to cache
                    memcpy(&(sb -> ircache), &block[posBlock], numBytes);

                    // Remove from FILT 
                    memset(&block[posBlock], NullReference, numBytes);

                    // Update counters
                    sb -> ircache.idx = 0;
                    //printf("FILTHEAD: %d", sb->filt_head);
                    sb -> filt_head = sb -> filt_head + INODE_REFERENCE_CACHE_SIZE;
                    //printf("FILTHEAD: %d", sb->filt_head);
                }

                // Otherwise, copy to the end of the cache
                else {
                    printf("\t\tNot enough references on FILT. Replenish will be partial.\n");
                    //printf("\t\tnumRefsToMove= %d\n", numRefsToMove);
                    //printf("\t\tfilt head= %d filt tail= %d\n", sb -> filt_head, sb -> filt_tail);
                    uint32_t numBytes = numRefsToMove * sizeof(uint32_t);
                    uint32_t posCache = INODE_REFERENCE_CACHE_SIZE - numRefsToMove;

                    // Copy from FILT to cache
                    memcpy(&((sb -> ircache).ref[posCache]), &block[posBlock], numBytes);

                    // Remove from FILT
                    memset(&block[posBlock], NullReference, numBytes);

                    //!BUG
                    // Update counters
                    sb -> ircache.idx = posCache;
                    sb -> filt_head = sb->filt_head + numRefsToMove;
                    if (sb -> filt_head == sb ->filt_tail) {
                        sb -> filt_head = 0;
                        sb -> filt_tail = 0;
                    }
                }

                soFILTSaveBlock();
                soFILTCloseBlock();
                soSBSave();
               // printf("\tAfter FILT head: %d\n\tAfter FILT tail: %d\n", sb->filt_head, sb->filt_tail);
                return;
            }

            /* copy from insertion cache if it's not empty (idx != 0) */
            SOInodeReferenceCache iicache = sb->iicache;
            printf("%d, %d, %d, ", sb->filt_head, sb->filt_tail, iicache.idx);
            if (iicache.idx != 0)
            {
                //printf("\tReplenishing from insertion cache\n");
                uint32_t numBytes = iicache.idx * sizeof(uint32_t);
                uint32_t posCache = INODE_REFERENCE_CACHE_SIZE - iicache.idx;

                // Copy from iicache
                // todo use a for
                memcpy(&(sb-> ircache).ref[posCache], iicache.ref, numBytes);

                // Remove from iicache
                memset(&(sb -> iicache.ref), NullReference, numBytes);

                // Update counters
                sb -> ircache.idx = posCache;
                //sb -> filt_head = sb-> filt_head + INODE_REFERENCE_CACHE_SIZE - iicache.idx;
                sb -> iicache.idx = 0;

                soSBSave();
                //printf("\tAfter FILT head: %d\n\tAfter FILT tail: %d\n", sb->filt_head, sb->filt_tail);
                return;
            }
            
            /* internal error. should not happen */
            printf("Internal Error in Replenish ircache!");
            exit(1);

            /* binary version */
            //bin::soReplenishIRCache();
        }

    };

};


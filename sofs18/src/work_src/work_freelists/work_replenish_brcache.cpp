/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2018
 */

#include "work_freelists.h"

#include <string.h>
#include <errno.h>
#include <iostream>
#include <cstdlib>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs18{
    namespace work{

        using namespace std;

        void soReplenishBRCache(void){
            soProbe(443, "%s()\n", __FUNCTION__);

            /* change the following line by your code */

			// memcpy(dest, src, number of bytes)
			// memset(dest, src, number of bytes)

            SOSuperBlock* sb = soSBGetPointer();           					// pointer to superblock 
            SOBlockReferenceCache biCache = sb -> bicache; 					// insertion cache
            SOBlockReferenceCache brCache = sb -> brcache; 					// retrieval cache
            uint32_t head = sb -> fblt_head;								// free data block list table head

            if(sb->dz_free == 0){                                           // if there aren free data blocks
                throw SOException(EINVAL, __FUNCTION__);
            }

            if(brCache.idx != BLOCK_REFERENCE_CACHE_SIZE - 1){				// if cache not empty
            	return;														// do nothing
            }

            if (head != sb -> fblt_tail){												                        // else if free block list table not empty
            	uint32_t* block = soFBLTOpenBlock(head);										                // open list head block
                uint32_t numBlock = head/ReferencesPerBlock;                                                    // block number
                uint32_t posBlock = head%ReferencesPerBlock;                                                    // first non NullReference in the block
                uint32_t numRefsToMove = ReferencesPerBlock - posBlock;											// max number of references that can be copied from that block
                uint32_t numRefsInFblt  = sb -> fblt_tail - sb -> fblt_head;									// max number of references that can be copied from the fblt
 				if (numRefsInFblt < numRefsToMove) {															// when max number of references that can be copied is greater than the number of references in the block
                    numRefsToMove = numRefsInFblt;																// copy just the ones on the block
                }
                if (numRefsToMove >= BLOCK_REFERENCE_CACHE_SIZE){												// if there's enough references completely replenish the cache
                    memcpy(&(brCache.ref), &block[posBlock], BLOCK_REFERENCE_CACHE_SIZE*sizeof(uint32_t));		// replenish retrieval cache with references from table
            		memset(&block[posBlock], NullReference, BLOCK_REFERENCE_CACHE_SIZE*sizeof(uint32_t));		// resets table entries
                    brCache.idx = 0;																			// update idx
                    sb -> fblt_head += BLOCK_REFERENCE_CACHE_SIZE; 												// update fblt head
                }
                else {																							// copy to the end of the cache
                    uint32_t posCache = BLOCK_REFERENCE_CACHE_SIZE - numRefsToMove;								// position in the cache to end copying
                    memcpy(&(brCache.ref[posCache]), &block[posBlock], numRefsToMove * sizeof(uint32_t));		// replenish retrieval cache with references from table
                    memset(&block[posBlock], NullReference, numRefsToMove * sizeof(uint32_t));					// resets table entries
                    brCache.idx = posCache;																		// update idx
                    sb->fblt_head += numRefsToMove;																// update fblt head
                }     
            }

            else if(biCache.idx != 0){																								// else, insertion cache not empty
            	memcpy(&(brCache.ref[BLOCK_REFERENCE_CACHE_SIZE - biCache.idx]), biCache.ref, biCache.idx*sizeof(uint32_t));		// replenish retrieval cache with references from insertion cache
            	memset(&(biCache.ref), NullReference, biCache.idx*sizeof(uint32_t));												// resets insertion cache references //
                sb->fblt_head = BLOCK_REFERENCE_CACHE_SIZE - biCache.idx;                                                           // update table head
                brCache.idx += BLOCK_REFERENCE_CACHE_SIZE - biCache.idx;                                                           	// update idx
                biCache.idx = 0;                                                           											// update idx
            }

            else{
                printf("Internal Error in Replenish brcache!");				// INTERNAL ERROR. Should not happen!
                exit(1);
            }

            soFBLTSaveBlock();					// save table head
            soFBLTCloseBlock();					// close table head
            soSBSave();							// save superblock
            //bin::soReplenishBRCache();
        }
    };
};


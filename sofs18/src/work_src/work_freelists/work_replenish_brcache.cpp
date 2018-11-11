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

            SOSuperBlock* sb = soSBGetPointer();           			// pointer to superblock 

            if(sb->dz_free == 0){                                   // if there aren free data blocks
                throw SOException(EINVAL, __FUNCTION__);
            }

            if((sb -> brcache).idx != BLOCK_REFERENCE_CACHE_SIZE){			// if cache not empty
            	return;														// do nothing
            }

            if ((sb -> fblt_head) != sb -> fblt_tail){												            	// else if free block list table not empty
            	uint32_t numBlock = (sb -> fblt_head)/ReferencesPerBlock;                                       	// block number
              	uint32_t posBlock = (sb -> fblt_head)%ReferencesPerBlock;                                       	// first non NullReference in the block
              	uint32_t numRefsToMove = ReferencesPerBlock - posBlock;												// max number of references that can be copied from that block
              	uint32_t numRefsInFblt  = sb -> fblt_tail - sb -> fblt_head;										// max number of references that can be copied from the fblt
              	uint32_t* block = sofs18::soFBLTOpenBlock(numBlock);										    	// open list head block
              	if (numRefsInFblt < numRefsToMove) {																// when max number of references that can be copied is greater than the number of references in the block
                    numRefsToMove = numRefsInFblt;																	// copy just the ones on the block
              	}
              	if (numRefsToMove >= BLOCK_REFERENCE_CACHE_SIZE){													// if there's enough references completely replenish the cache
                    for (uint32_t i = 0; i < BLOCK_REFERENCE_CACHE_SIZE; i++){
                        sb -> brcache.ref[i] = block[posBlock + i];													// replenish retrieval cache with references from table
                        block[posBlock + i] = NullReference;														// resets table entries
                    }
                    (sb -> brcache).idx = 0;																		// update idx
                    sb -> fblt_head += BLOCK_REFERENCE_CACHE_SIZE; 													// update fblt head
                }
                else {																								// copy to the end of the cache
                    uint32_t posCache = BLOCK_REFERENCE_CACHE_SIZE - numRefsToMove;									// position in the cache to end copying
                    for (uint32_t i = 0; i < numRefsToMove; i++){
                        sb -> brcache.ref[posCache + i] = block[posBlock + i];										// replenish retrieval cache with references from table
                        block[posBlock + i] = NullReference;														// resets table entries
                    }
                    (sb -> brcache).idx = posCache;																	// update idx
                    sb -> fblt_head += numRefsToMove;																// update fblt head
                    if (sb -> fblt_head == sb -> fblt_tail) {
                        sb -> fblt_head = 0;
                        sb -> fblt_tail = 0;
                    }
                }  
                soFBLTSaveBlock();					// save table head
            	soFBLTCloseBlock();					// close table head   
            }
            else if((sb -> bicache).idx != 0){																													// else, insertion cache not empty
            	for (uint32_t i = 0; i < BLOCK_REFERENCE_CACHE_SIZE; i++){
                    sb -> brcache.ref[BLOCK_REFERENCE_CACHE_SIZE - sb -> bicache.idx + i] = sb -> bicache.ref[i];												// replenish retrieval cache with references from insertion cache
                    sb -> bicache.ref[BLOCK_REFERENCE_CACHE_SIZE - sb -> bicache.idx + i] = NullReference;														// resets insertion cache entries
                }									
                (sb -> brcache).idx += BLOCK_REFERENCE_CACHE_SIZE - (sb -> bicache).idx;                                                           				// update idx
                (sb -> bicache).idx = 0;                                                           																// update idx
            }
            soSBSave();								// save superblock
        }
    };
};


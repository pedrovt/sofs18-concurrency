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

            sofs18::soSBOpen();												// open superblock
            SOSuperBlock* sb = sofs18::soSBGetPointer();           			// pointer to superblock 

            if(sb->dz_free == 0){                                           // if there aren free data blocks
                throw SOException(EINVAL, __FUNCTION__);
            }

            if((sb -> brcache).idx != BLOCK_REFERENCE_CACHE_SIZE){			// if cache not empty
            	return;														// do nothing
            }

            if ((sb -> fblt_head) != sb -> fblt_tail){												            // else if free block list table not empty
                uint32_t numBlock = (sb -> fblt_head)/ReferencesPerBlock;                                       // block number
                uint32_t posBlock = (sb -> fblt_head)%ReferencesPerBlock;                                       // first non NullReference in the block
                uint32_t numRefsToMove = ReferencesPerBlock - posBlock;											// max number of references that can be copied from that block
                uint32_t numRefsInFblt  = sb -> fblt_tail - sb -> fblt_head;									// max number of references that can be copied from the fblt
 				uint32_t* block = sofs18::soFBLTOpenBlock(numBlock);										    // open list head block
 				if (numRefsInFblt < numRefsToMove) {															// when max number of references that can be copied is greater than the number of references in the block
                    numRefsToMove = numRefsInFblt;																// copy just the ones on the block
                }
                if (numRefsToMove >= BLOCK_REFERENCE_CACHE_SIZE){													// if there's enough references completely replenish the cache
                    memcpy(&((sb -> brcache).ref), &block[posBlock], BLOCK_REFERENCE_CACHE_SIZE*sizeof(uint32_t));	// replenish retrieval cache with references from table
            		memset(&block[posBlock], NullReference, BLOCK_REFERENCE_CACHE_SIZE*sizeof(uint32_t));			// resets table entries
                    (sb -> brcache).idx = 0;																		// update idx
                    sb -> fblt_head += BLOCK_REFERENCE_CACHE_SIZE; 													// update fblt head
                }
                else {																								// copy to the end of the cache
                    uint32_t posCache = BLOCK_REFERENCE_CACHE_SIZE - numRefsToMove;									// position in the cache to end copying
                    memcpy(&((sb -> brcache).ref[posCache]), &block[posBlock], numRefsToMove * sizeof(uint32_t));	// replenish retrieval cache with references from table
                    memset(&block[posBlock], NullReference, numRefsToMove * sizeof(uint32_t));						// resets table entries
                    (sb -> brcache).idx = posCache;																	// update idx
                    sb -> fblt_head += numRefsToMove;																// update fblt head
                    if (sb -> fblt_head == sb -> fblt_tail) {
                        sb -> fblt_head = 0;
                        sb -> fblt_tail = 0;
                    }
                }  
                sofs18::soFBLTSaveBlock();					// save table head
            	sofs18::soFBLTCloseBlock();					// close table head   
            }

            else if((sb -> bicache).idx != 0){																													// else, insertion cache not empty
            	memcpy(&((sb -> brcache).ref[BLOCK_REFERENCE_CACHE_SIZE - (sb -> bicache).idx]), (sb -> bicache).ref, (sb -> bicache).idx*sizeof(uint32_t));	// replenish retrieval cache with references from insertion cache
            	memset(&((sb -> brcache).ref), NullReference, (sb -> bicache).idx*sizeof(uint32_t));															// resets insertion cache references //
                (sb -> brcache).idx += BLOCK_REFERENCE_CACHE_SIZE - (sb -> bicache).idx;                                                           				// update idx
                (sb -> bicache).idx = 0;                                                           																// update idx
            }

            sofs18::soSBSave();							// save superblock
            sofs18::soSBClose();						// close superblock
            //bin::soReplenishBRCache();
        }
    };
};


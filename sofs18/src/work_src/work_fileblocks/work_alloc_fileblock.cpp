#include "work_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>
#include <math.h>
#include <iostream>

namespace sofs18{
    namespace work{
        static uint32_t soAllocIndirectFileBlock(SOInode * ip, uint32_t afbn);
        static uint32_t soAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn);
        /* ********************************************************* */
		/* Associate a data block to the given file block position.
		   Parameters:
		      ih	inode handler.
		      fbn	file block number.
		   Returns:
		      the number of the allocated block.
		*/
        uint32_t soAllocFileBlock(int ih, uint32_t fbn){
            soProbe(302, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);
            /* change the following two lines by your code */

            SOInode* inode = soITGetInodePointer(ih);
            if (fbn > N_DOUBLE_INDIRECT * pow(ReferencesPerBlock, 2) + N_INDIRECT * ReferencesPerBlock + N_DIRECT){
                throw SOException(EINVAL, __FUNCTION__);    
            }

            if (fbn < N_DIRECT) {																			// if block is directly referencied (d)
                return sofs18::soAllocDataBlock();															// alloc file block (from the cache) ??
                // there should be something to do with the fbn
                // Associate a data block !to the given file block position!
            }

            uint32_t maxIndirectIndex = (N_INDIRECT * ReferencesPerBlock) + N_DIRECT;
            uint32_t maxDoubleIndirectIndex = (N_DOUBLE_INDIRECT * pow(ReferencesPerBlock, 2)) + maxIndirectIndex;
            if (fbn < maxIndirectIndex) {																	// if block is indirectly referencied (i1)
                return soAllocIndirectFileBlock(inode, fbn - N_DIRECT);
            }

            else if (fbn < maxDoubleIndirectIndex){															// if block is double indirectly referencied (i2)
                return soAllocDoubleIndirectFileBlock(inode, fbn - maxIndirectIndex);
            }
            
            //return bin::soAllocFileBlock(ih, fbn);
        }
        /* ********************************************************* */
        /*
         */
        static uint32_t soAllocIndirectFileBlock(SOInode * ip, uint32_t afbn){
            soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);
            /* change the following two lines by your code */

            throw SOException(ENOSYS, __FUNCTION__); 
            return 0;
        }
        /* ********************************************************* */
        /*
         */
        static uint32_t soAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn){
            soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);
            /* change the following two lines by your code */

            throw SOException(ENOSYS, __FUNCTION__); 
            return 0;
        }
    };
};


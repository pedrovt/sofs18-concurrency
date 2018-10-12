#include "work_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <string.h>

namespace sofs18
{
    namespace work
    {

    	uint32_t determineNumBlocks(uint32_t total, uint32_t rpb){ return total/rpb + (total%rpb != 0 ? 1 : 0); }
        uint32_t fillInFreeInodeListTable(uint32_t first_block, uint32_t itotal)
        {
            soProbe(603, "%s(%u, %u)\n", __FUNCTION__, first_block, itotal);

            itotal -= 1; //Decremented by 1, since one of the inodes is being used for the root directory
            uint32_t num_blocks = determineNumBlocks(itotal,ReferencesPerBlock); //Number of necessary blocks for all the references
            uint32_t block[ReferencesPerBlock]; //Contains all the references of a given block
            int last_block = first_block + num_blocks - 1;
            uint32_t num = ReferencesPerBlock;
            uint32_t index = 1; //Starts with 1, because the first inode has been used for the root directory
            for(uint32_t i = first_block; i <= last_block; i++){
            	if(i == last_block) num = itotal - ReferencesPerBlock * (num_blocks-1);
            	uint32_t j = 0;
            	for(; j < num; j++){
            		block[j] = index++;
            	}
            	for(uint32_t k = j; k < ReferencesPerBlock; k++){
            		block[j++] = NullReference;
            	}
            	sofs18::soWriteRawBlock(i,&block);
            }
            return num_blocks;
        }

    };

};


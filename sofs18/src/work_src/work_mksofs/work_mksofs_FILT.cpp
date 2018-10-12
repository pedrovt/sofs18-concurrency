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
    	//Determines the number of blocks necessary, given the number of total inodes and the number of references per block
    	uint32_t determineNumBlocks(uint32_t total, uint32_t rpb){ return total/rpb + (total%rpb != 0 ? 1 : 0); }
		/*
	    	[in]	first_block	physical number of the first block used by the table
			[in]	itotal	the total number of inodes
			[out]   The number of blocks occupied by the table
		*/
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
            	/* Checks if the current block is the last. If so, calculates how many references must be filled.
            	   Else it just fills all ReferencesPerBlock references */
            	if(i == last_block) num = itotal - ReferencesPerBlock * (num_blocks-1);
            	uint32_t j = 0;
            	/*Fills the blocks dedicated to the Free INode Table List*/
            	for(; j < num; j++){
            		block[j] = index++;
            	}
            	/*Fills the remaining references with NullReference*/
            	for(uint32_t k = j; k < ReferencesPerBlock; k++){
            		block[j++] = NullReference;
            	}
            	/*Writes each buffer to the corresponding block*/
            	sofs18::soWriteRawBlock(i,&block);
            }
            return num_blocks;
        }

    };

};


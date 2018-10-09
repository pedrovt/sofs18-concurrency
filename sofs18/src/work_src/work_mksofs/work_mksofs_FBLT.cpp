#include "work_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <string.h>
#include<iostream>

namespace sofs18
{
    namespace work
    {
		
		uint32_t roundNBlocksUp(uint32_t);

        uint32_t fillInFreeBlockListTable(uint32_t first_block, uint32_t btotal, uint32_t rdsize)
        {
            soProbe(605, "%s(%u, %u, %u)\n", __FUNCTION__, first_block, btotal, rdsize);
          		
			uint32_t n_blocks = roundNBlocksUp(btotal);
			uint32_t table [n_blocks * ReferencesPerBlock];
			for(uint32_t j = rdsize; j < btotal; j++) {
				table[j - rdsize] = j;
			}
			//Fill all blocks but the last because the last may have NullReferences
			for(uint32_t i = 0; i < n_blocks-1; i++) {
				soWriteRawBlock(first_block + i, &table[128*i]);
			}

			//the address of the last block
			uint32_t last_block = first_block + n_blocks - 1;
			//table with the last block's references
			uint32_t table_last_block [ReferencesPerBlock];
			//amount of references that exist in last block
			uint32_t remainder = btotal % ReferencesPerBlock-1;

			for(uint32_t k = 0; k < remainder; k++) {
				//this sum makes us continue form the last reference
				table_last_block[k] = k + table[(n_blocks-1)*128];
			}
			//fill the rest of the array with NullReference
			for(uint32_t l = remainder; l < ReferencesPerBlock; l++) {
				table_last_block[l] = NullReference;
			}
			//write the last block
			soWriteRawBlock(last_block, &table_last_block);

			return n_blocks;
            //return bin::fillInFreeBlockListTable(first_block, btotal, rdsize);
			
        }

		uint32_t roundNBlocksUp(uint32_t size)
		{
			if(size % ReferencesPerBlock == 0)
				return size/ReferencesPerBlock;
			else 
				return size/ReferencesPerBlock + 1;
			
		}

    };

};


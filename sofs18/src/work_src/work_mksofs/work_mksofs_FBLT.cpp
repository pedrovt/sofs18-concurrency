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
          uint32_t block[ReferencesPerBlock];
          uint32_t last_reference_number = rdsize; 
          for(uint32_t i = 0; i < n_blocks-1; i++) {
            for(uint32_t j=0; j < ReferencesPerBlock; j++) {
              block[j] = last_reference_number++;	
            }
            soWriteRawBlock(first_block + i, &block);	
          }
      
          uint32_t last_block = first_block + n_blocks - 1;
          uint32_t remainder = btotal % ReferencesPerBlock-1;
  
          if(btotal%ReferencesPerBlock != 0) {
            for(uint32_t i=0; i<ReferencesPerBlock; i++) {
              if(i < remainder) {
                block[i] = last_reference_number++;
              }else {
                block[i] = NullReference;
              }	
            }
            soWriteRawBlock(last_block, &block);
          }
          else {
            for(uint32_t i=0; i < ReferencesPerBlock; i++) {
              block[i] = last_reference_number++;	
            }
            soWriteRawBlock(last_block, &block);	
          }
          return n_blocks;
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


#include "work_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

namespace sofs18
{
    namespace work
    {

        void resetBlocks(uint32_t first_block, uint32_t cnt)
        {
            soProbe(607, "%s(%u, %u)\n", __FUNCTION__, first_block, cnt);
            printf("\t[PVT/DEBUG] Using my version of resetBlocks!!!!");
            
            uint32_t size = BlockSize / sizeof(uint32_t);
            uint32_t array[size];  // Macro ReferencesPerBlock wouldn't be clear
            for (uint32_t i = 0; i < size; i++)
            {
                array[i] = 0;
                //printf("\t[PVT/DEBUG] array[%d]=%d\n", i, array[i]);
            }
            
            for (; first_block < cnt; first_block++) 
            {
                soWriteRawBlock(first_block, &array);
            }

            /* change the following line by your code */
            //bin::resetBlocks(first_block, cnt);
        }

    };

};


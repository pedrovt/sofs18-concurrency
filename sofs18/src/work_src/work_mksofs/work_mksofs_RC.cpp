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
            printf("\tUsing work version of resetBlocks\n");

            /* Creates an array of BlockSize with 0s and writes it from LBA 
               Address first_block until LBA Address first_block + cnt - 1 -- */
            uint8_t array[BlockSize];  
            memset(array, 0, BlockSize);
            for (uint32_t i = first_block; i < cnt; i++)
            {
                soWriteRawBlock(i, &array);
            }

            /* To use the given binary version ------------------------------ */
            //bin::resetBlocks(first_block, cnt);
        }

    };

};


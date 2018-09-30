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

        uint32_t fillInFreeBlockListTable(uint32_t first_block, uint32_t btotal, uint32_t rdsize)
        {
            soProbe(605, "%s(%u, %u, %u)\n", __FUNCTION__, first_block, btotal, rdsize);
            
            /* change the following line by your code */
            return bin::fillInFreeBlockListTable(first_block, btotal, rdsize);
        }

    };

};


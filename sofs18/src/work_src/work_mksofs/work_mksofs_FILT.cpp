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

        uint32_t fillInFreeInodeListTable(uint32_t first_block, uint32_t itotal)
        {
            soProbe(603, "%s(%u, %u)\n", __FUNCTION__, first_block, itotal);
            
            /* change the following line by your code */
            return bin::fillInFreeInodeListTable(first_block, itotal);
        }

    };

};


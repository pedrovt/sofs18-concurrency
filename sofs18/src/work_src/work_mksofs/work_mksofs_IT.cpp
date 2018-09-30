#include "work_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <inttypes.h>

namespace sofs18
{
    namespace work
    {

        uint32_t fillInInodeTable(uint32_t first_block, uint32_t itotal, uint32_t rdsize)
        {
            soProbe(604, "%s(%u, %u, %u)\n", __FUNCTION__, first_block, itotal, rdsize);
            
            /* change the following line by your code */
            return bin::fillInInodeTable(first_block, itotal, rdsize);
        }

    };

};


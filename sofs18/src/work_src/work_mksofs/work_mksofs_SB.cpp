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

        void fillInSuperBlock(const char *name, uint32_t ntotal, uint32_t itotal, uint32_t rdsize)
        {
            soProbe(602, "%s(%s, %u, %u, %u)\n", __FUNCTION__, name, ntotal, itotal, rdsize);
            
            /* change the following line by your code */
            bin::fillInSuperBlock(name, ntotal, itotal, rdsize);
        }

    };

};


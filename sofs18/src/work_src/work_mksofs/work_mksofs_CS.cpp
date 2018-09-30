#include "work_mksofs.h"

#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <iostream>

namespace sofs18
{
    namespace work
    {

        void computeStructure(uint32_t ntotal, uint32_t & itotal, uint32_t & btotal, uint32_t & rdsize)
        {
            soProbe(601, "%s(%u, %u, ...)\n", __FUNCTION__, ntotal, itotal);
            
            /* change the following line by your code */
            bin::computeStructure(ntotal, itotal, btotal, rdsize);
        }

    };

};


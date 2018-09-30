#include "work_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>

#include <iostream>

namespace sofs18
{
    namespace work
    {

#if false
        static uint32_t soAllocIndirectFileBlock(SOInode * ip, uint32_t afbn);
        static uint32_t soAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn);
#endif

        /* ********************************************************* */

        uint32_t soAllocFileBlock(int ih, uint32_t fbn)
        {
            soProbe(302, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);

            /* change the following two lines by your code */
            return bin::soAllocFileBlock(ih, fbn);
        }

        /* ********************************************************* */

#if false
        /*
         */
        static uint32_t soAllocIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {
            soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);

            /* change the following two lines by your code */
            throw SOException(ENOSYS, __FUNCTION__); 
            return 0;
        }
#endif

        /* ********************************************************* */

#if false
        /*
         */
        static uint32_t soAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {
            soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);

            /* change the following two lines by your code */
            throw SOException(ENOSYS, __FUNCTION__); 
            return 0;
        }
#endif

    };

};


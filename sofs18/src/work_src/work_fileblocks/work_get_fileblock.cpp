#include "work_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>

namespace sofs18
{
    namespace work
    {

        /* ********************************************************* */

#if false
        static uint32_t soGetIndirectFileBlock(SOInode * ip, uint32_t fbn);
        static uint32_t soGetDoubleIndirectFileBlock(SOInode * ip, uint32_t fbn);
#endif

        /* ********************************************************* */

        uint32_t soGetFileBlock(int ih, uint32_t fbn)
        {
            soProbe(301, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);

            /* change the following line by your code */
            return bin::soGetFileBlock(ih, fbn);
        }

        /* ********************************************************* */

#if false
        static uint32_t soGetIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {
            soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);

            /* change the following two lines by your code */
            throw SOException(ENOSYS, __FUNCTION__); 
            return 0;
        }
#endif

        /* ********************************************************* */

#if false
        static uint32_t soGetDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {
            soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);

            /* change the following two lines by your code */
            throw SOException(ENOSYS, __FUNCTION__); 
            return 0;
        }
#endif

    };

};


#include "work_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <inttypes.h>
#include <errno.h>
#include <assert.h>

namespace sofs18
{
    namespace work
    {

#if false
        /* free all blocks between positions ffbn and ReferencesPerBlock - 1
         * existing in the block of references given by i1.
         * Return true if, after the operation, all references become NullReference.
         * It assumes i1 is valid.
         */
        static bool soFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn);
        
        /* free all blocks between positions ffbn and ReferencesPerBloc**2 - 1
         * existing in the block of indirect references given by i2.
         * Return true if, after the operation, all references become NullReference.
         * It assumes i2 is valid.
         */
        static bool soFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn);
#endif

        /* ********************************************************* */

        void soFreeFileBlocks(int ih, uint32_t ffbn)
        {
            soProbe(303, "%s(%d, %u)\n", __FUNCTION__, ih, ffbn);

            /* change the following line by your code */
            bin::soFreeFileBlocks(ih, ffbn);
        }

        /* ********************************************************* */

#if false
        static bool soFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn)
        {
            soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i1, ffbn);

            /* change the following line by your code */
            throw SOException(ENOSYS, __FUNCTION__); 
        }
#endif

        /* ********************************************************* */

#if false
        static bool soFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn)
        {
            soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i2, ffbn);

            /* change the following line by your code */
            throw SOException(ENOSYS, __FUNCTION__); 
        }
#endif

        /* ********************************************************* */

    };

};


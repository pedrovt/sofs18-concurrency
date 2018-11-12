#include "work_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "fileblocks.h"
#include "bin_fileblocks.h"

#include <string.h>
#include <inttypes.h>

namespace sofs18
{
    namespace work
    {

        void soReadFileBlock(int ih, uint32_t fbn, void *buf)
        {
            soProbe(331, "%s(%d, %u, %p)\n", __FUNCTION__, ih, fbn, buf);
            uint32_t block_number = sofs18::soGetFileBlock(ih, fbn);
            if(block_number != NullReference) 
                sofs18::soReadDataBlock(block_number, buf);
            else
                memset(buf, '\0', 512);
            //bin::soReadFileBlock(ih, fbn, buf);
        }

    };

};


#include "work_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "fileblocks.h"
#include "bin_fileblocks.h"

#include <string.h>
#include <inttypes.h>
#include <stdio.h>

namespace sofs18
{
    namespace work
    {

        void soWriteFileBlock(int ih, uint32_t fbn, void *buf)
        {
           	soProbe(332, "%s(%d, %u, %p)\n", __FUNCTION__, ih, fbn, buf);

			uint32_t block_number = sofs18::soGetFileBlock(ih, fbn);
	
			//if the file block is nil we need to allocate it
			if(block_number == NullReference){
		 		block_number = sofs18::soAllocFileBlock(ih, fbn);
			}
				
			printf("BN : %d \n", block_number);
			sofs18::soWriteDataBlock(block_number, buf);

            /* change the following line by your code */
            //bin::soWriteFileBlock(ih, fbn, buf);
        }

    };

};


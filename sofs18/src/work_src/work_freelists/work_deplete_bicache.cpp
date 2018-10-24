/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2018
 */

#include "work_freelists.h"

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
using namespace std;

namespace sofs18
{
    namespace work
    {

        /* only fill the current block to its end */
        void soDepleteBICache(void)
        {
		soProbe(444, "%s()\n", __FUNCTION__);
		
		soSUperBlock *sb = soSBGetPointer();
		soBlockReferenceCache bicache = sb->bicache;

		uint32_t block_number = sb.fblt_tail / ReferencesPerBlock;
		uint32_t position_in_block = sb.fblt_tail % ReferencesPerBlock;
		uint32_t *block = soFBLTOpenBlock(block_number);

		if(bicache.idx > 0) {
			if((ReferencesPerBlock - position_in_block) < bicache.idx) {
				uint32_t numBytes = (ReferencesPerBlock - position_in_block) * sizeof(uint32_t);
				//copy references from cache to the FBLT
				memcpy(&block[position_in_block], &bicache, numBytes);
				//delete those references from the cache
				memset(&bicache, NullReference, numBytes);
			}else {
				uint32_t numBytes = bicache.idx * sizeof(uint32_t);
				//copy references from the cache to the FBLT
				memcpy(&block[position_in_block], &bicache, numBytes);
				//delete those references from the cache
				memset(&bicache, NullReference, numBytes);
			}
		}
		
		
		soSBSave();
            /* change the following line by your code */
            //bin::soDepleteBICache();
        }

    };

};


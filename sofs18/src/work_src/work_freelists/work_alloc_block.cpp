/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2009-2018
 */

#include "work_freelists.h"

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs18
{
    namespace work
    {
    	/*
			Allocate a free data block.
			The block reference is retrieved from the data block retrieval cache.
			If the cache is empty, it has to be replenished before the retrieval takes place.
			Returns
			    the number (reference) of the data block allocated
    	*/
        uint32_t soAllocDataBlock()
        {
            soProbe(441, "%s()\n", __FUNCTION__);
            SOSuperBlock* sb = soSBGetPointer();
            if(sb->dz_free <= 0){
            	throw SOException(ENOSPC,__FUNCTION__);
         	}
         	if(sb->brcache.idx == BLOCK_REFERENCE_CACHE_SIZE-1){
            sofs18::soReplenishBRCache();
         		sb = soSBGetPointer();
         	}
         	uint32_t retrieved_block = sb->brcache.ref[sb->brcache.idx];
         	sb->brcache.ref[sb->brcache.idx] = NullReference;
         	sb->brcache.idx += 1;
         	sb->dz_free -= 1;
         	soSBSave();
         	return retrieved_block;
        }

    };

};


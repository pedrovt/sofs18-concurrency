/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2018
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

        void soFreeDataBlock(uint32_t bn)
        {
            soProbe(442, "%s(%u)\n", __FUNCTION__, bn);
            
            SOSuperBlock *spb_pointer = soSBGetPointer();
            spb_pointer->dz_free++;
            if (spb_pointer->bicache.ref[BLOCK_REFERENCE_CACHE_SIZE-1] != NullReference) {soDepleteBICache();}
            spb_pointer->bicache.ref[spb_pointer->bicache.idx++] = bn;
            soSBSave();
        }
    };

};


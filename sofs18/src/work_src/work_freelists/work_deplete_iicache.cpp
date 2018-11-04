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
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

namespace sofs18
{
    namespace work
    {

        void soDepleteIICache(void)
        {

            soProbe(404, "%s()\n", __FUNCTION__);
            SOSuperBlock *sb = soSBGetPointer();
            uint32_t block_number = sb->filt_tail / ReferencesPerBlock;
            uint32_t position_in_block = sb->filt_tail % ReferencesPerBlock;
            uint32_t *block = soFILTOpenBlock(block_number);

            // Spaces left in freelist
            int blockFull = 0;

            // It will move the whole cache into the FILT
            uint32_t refsToCopy = sb->iicache.idx;
            // If the block pointed by filt_tail has not space enough for the whole cache
            if(ReferencesPerBlock - position_in_block < refsToCopy) {
                refsToCopy = ReferencesPerBlock - position_in_block;
                blockFull = 1;
            }

			memcpy(&block[position_in_block], &(sb->iicache.ref), refsToCopy * sizeof(uint32_t));
            memset(&(sb->iicache.ref), NullReference, refsToCopy * sizeof(uint32_t));

            // Moves the cache that could not be removed to the start of the cache
            if(blockFull) {
                int index = 0;
                for(int i = refsToCopy; i < sb->iicache.idx; i++) {
                    sb->iicache.ref[index++] = sb->iicache.ref[i];
                    sb->iicache.ref[i] = NullReference;
                }
                sb->iicache.idx -= refsToCopy;
            } else
                sb->iicache.idx = 0;

            sb->filt_tail = (sb->filt_tail + refsToCopy) % (sb->filt_size * 128);

            soFILTSaveBlock();
            soFILTCloseBlock();
            soSBSave();
            //bin::soDepleteI   ICache();

        }

    };

};
/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2018
 */

#include "work_freelists.h"

#include <cstring>
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
<<<<<<< HEAD
            SOBlockReferenceCache bicache = sb->bicache;

            uint32_t block_number = sb->fblt_tail / ReferencesPerBlock;
            uint32_t position_in_block = sb->fblt_tail % ReferencesPerBlock;
            uint32_t *block = soFBLTOpenBlock(block_number);

            if(bicache.idx > 0) {
              //if the FBLT is full just deplete the cache
              if(sb->fblt_tail == sb->fblt_head) {
                for(uint32_t i=0; i<bicache.idx; i++) {
                    bicache.ref[i] = NullReference;
                }
              bicache.idx = 0;
              sb->bicache = bicache;
            
              }else {
                  //if the block has less space than the amount of references in cache
                if((ReferencesPerBlock - position_in_block) < bicache.idx) {
                  uint32_t numBytes = (ReferencesPerBlock - position_in_block) * sizeof(uint32_t);
                  //copy references from cache to the FBLT
                  memcpy(&block[position_in_block], &(bicache.ref), numBytes);
                  //save the references that wont be deleted
                  uint32_t arr[bicache.idx - (ReferencesPerBlock - position_in_block)];
                  uint32_t index = 0;
                  for(uint32_t i = (ReferencesPerBlock - position_in_block); i<bicache.idx; i++) {
                    arr[index] = bicache.ref[i];
                    index += 1;
                  }
                  //delete all references
                  for(uint32_t i=0; i<bicache.idx; i++) {
                    bicache.ref[i] = NullReference;
                  }
                  //restore the ones saved previously
                  bicache.idx = bicache.idx - (ReferencesPerBlock - position_in_block);
                  for(uint32_t i = 0; i<bicache.idx; i++) {
                    bicache.ref[i] = arr[i];	
                  }
                  sb->bicache = bicache;
                }else {
                  uint32_t numBytes = bicache.idx * sizeof(uint32_t);
                  //copy references from the cache to the FBLT
                  memcpy(&block[position_in_block], &(bicache.ref), numBytes);
                  //delete those references from the cache
                  for(uint32_t i=0; i<bicache.idx; i++) {
                    bicache.ref[i] = NullReference;
                  }
                  bicache.idx = 0;
                  sb->bicache = bicache;
                }
              }
            }
          
            soFBLTSaveBlock();
            soFBLTCloseBlock();
            soSBSave();

            /* change the following line by your code */
            //bin::soDepleteIICache();
=======
            uint32_t block_number = sb->filt_tail / ReferencesPerBlock;
            uint32_t position_in_block = sb->filt_tail % ReferencesPerBlock;
            uint32_t *block = soFILTOpenBlock(block_number);
            int blockFull = 0;

            // It will move the whole cache into the FILT
            uint32_t refsToCopy = sb->iicache.idx;
            // If the block pointed by filt_tail has not space enough for the whole cache
            if(ReferencesPerBlock - position_in_block < refsToCopy) {
                refsToCopy = ReferencesPerBlock - position_in_block;
                blockFull = 1;
            }

            // Copies x refs from the cache to the filt (x = refsToCopy)
			memcpy(&block[position_in_block], &(sb->iicache.ref), refsToCopy * sizeof(uint32_t));
            // Turns the cache coppied into NullReference
            memset(&(sb->iicache.ref), NullReference, refsToCopy * sizeof(uint32_t));

            // Moves the cache that could not be moved to the start of the cache
            if(blockFull) {
                int index = 0;
                for(uint32_t i = refsToCopy; i < sb->iicache.idx; i++) {
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
            //bin::soDepleteIICache();

>>>>>>> master
        }

    };

};
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

        /* ********************************************************* */

        void soFreeFileBlocks(int ih, uint32_t ffbn)
        {
          soProbe(303, "%s(%d, %u)\n", __FUNCTION__, ih, ffbn);

          SOInode *inode = soITGetInodePointer(ih);
          SOSuperBlock *sb;
          // if ffbn out of range
          if (ffbn < 0 || ffbn > (N_DIRECT + N_INDIRECT*ReferencesPerBlock + N_DOUBLE_INDIRECT*ReferencesPerBlock*ReferencesPerBlock)) { return; }
          // if ffbn is after direct
          else if (ffbn > N_DIRECT && ffbn < N_DIRECT + N_INDIRECT*ReferencesPerBlock) 
          {
            for (int i = 0; i < N_INDIRECT; i++)
            {
              uint32_t blk = inode->i1[i];
              if (blk != NullReference) 
              {
                uint32_t rm = ffbn-((i+1)*ReferencesPerBlock);
                if (rm==0) 
                { 
                  inode->i1[i] = NullReference; 
                  soFreeIndirectFileBlocks(inode, i, rm);
                }
                else if (rm>ffbn) 
                { 
                  inode->i1[i] = NullReference;
                  soFreeIndirectFileBlocks(inode, i, 0); 
                }
                else { 
                  printf("%u\n", rm);
                  printf("%u\n", ffbn);
                  printf("%lu\n", (i+1)*ReferencesPerBlock);
                  soFreeIndirectFileBlocks(inode, i, rm); 
                } 
              }
            }
            for (int i = 0; i < N_DOUBLE_INDIRECT; i++)
            {
              uint32_t blk = inode->i2[i];
              if (blk != NullReference) 
              {
                inode->i1[i] = NullReference;
                soFreeDoubleIndirectFileBlocks(inode, i, 0); 
              }
            }
          }
          // if ffbn is after indirect
          else if (ffbn > N_DIRECT + N_INDIRECT*ReferencesPerBlock && ffbn < N_DIRECT*ReferencesPerBlock*ReferencesPerBlock)
          {
            for (int i = 0; i < N_DOUBLE_INDIRECT; i++)
            {
              uint32_t blk = inode->i2[i];
              if (blk != NullReference) 
              {
                uint32_t rm = ffbn-(i+1)*(N_DIRECT*ReferencesPerBlock+ReferencesPerBlock);
                if (rm==0) 
                { 
                  inode->i1[i] = NullReference; 
                  soFreeDoubleIndirectFileBlocks(inode, i, 0); 
                }
                else if (rm>ffbn)
                {
                  inode->i1[i] = NullReference;
                  soFreeDoubleIndirectFileBlocks(inode, i, 0);
                }
                else{ soFreeDoubleIndirectFileBlocks(inode, i, rm); }
              }
            }
          }
          // if ffbn starts inside direct
          else
          {
            sb = soSBGetPointer();
            // indirect
            for (int i = 0; i < N_INDIRECT; i++)
            {
              uint32_t blk = inode->i1[i];
              if (blk != NullReference) 
              {
                soFreeIndirectFileBlocks(inode, i, 0); 
              }
            }
            // double indirect
            for (int i = 0; i < N_DOUBLE_INDIRECT; i++)
            {
              uint32_t blk = inode->i2[i];
              if (blk != NullReference) 
              {
                inode->i1[i] = NullReference;
                soFreeDoubleIndirectFileBlocks(inode, i, 0); 
              }
            }
            // direct
            for (int i = ffbn; i < N_DIRECT; i++)
            {
              uint32_t blk = inode->d[i];
              if (blk != NullReference)
              {
                uint32_t block = sb->dz_start + blk;
                inode->d[i] = NullReference;
                sofs18::soFreeDataBlock(block);
              }
            }
          }
        }

        /* ********************************************************* */

        static bool soFreeIndirectFileBlocks(SOInode * ip, uint32_t i1, uint32_t ffbn)
        {
          soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i1, ffbn);

          uint32_t refs[ReferencesPerBlock], block;
          sofs18::soReadDataBlock(i1,&refs);
          bool to_ret = false;
          for (uint32_t i = ffbn; i < ReferencesPerBlock; i++)
          {
            block = refs[i];
            if (block != NullReference)
            {
              sofs18::soFreeDataBlock(block);
              to_ret=true;
            }
          }
          return to_ret;
        }

        /* ********************************************************* */

        static bool soFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn)
        {
          soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i2, ffbn);
          
          uint32_t refs[ReferencesPerBlock], block;
          soReadDataBlock(i2,&refs);
          bool to_ret=false;
          for (uint32_t i = ffbn; i < ReferencesPerBlock; i++)
          {
            block = refs[i];
            if (block != NullReference)
            {
              sofs18::soFreeDataBlock(block);
              to_ret=true;
            }
          }
          return to_ret;
        }
        /* ********************************************************* */
    };
};


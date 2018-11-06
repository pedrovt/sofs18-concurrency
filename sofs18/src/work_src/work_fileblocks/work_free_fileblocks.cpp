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
          // if ffbn out of range
          if (ffbn < 0 || ffbn > (N_DIRECT + N_INDIRECT*ReferencesPerBlock + N_DOUBLE_INDIRECT*ReferencesPerBlock*ReferencesPerBlock)) { return; }
          // if ffbn is after indirect -- working correctly 
          else if (ffbn > N_DIRECT + N_INDIRECT*ReferencesPerBlock && ffbn < N_DIRECT*ReferencesPerBlock*ReferencesPerBlock)
          {
            for (int i = 0; i < N_DOUBLE_INDIRECT; i++)
            {
              uint32_t blk = inode->i2[i];
              bool del=false;
              if (blk != NullReference) 
              {
                uint32_t rm = ffbn-(i+1)*(N_INDIRECT*ReferencesPerBlock+4);
                if (rm==0 || rm>ffbn) { del=soFreeDoubleIndirectFileBlocks(inode, blk, 0); }
                else { del=soFreeDoubleIndirectFileBlocks(inode, blk, rm); }
                if (del) 
                { 
                  inode->i2[i] = NullReference;
                  sofs18::soFreeDataBlock(blk);
                }
              }
            }
          }
          // if ffbn is after direct
          else if (ffbn > N_DIRECT && ffbn < N_DIRECT + N_INDIRECT*ReferencesPerBlock) 
          {
            for (int i = 0; i < N_INDIRECT; i++)
            {
              uint32_t blk = inode->i1[i];
              if (blk != NullReference)
              {
                bool del=false;
                uint32_t rm = ffbn-N_DIRECT-(i*ReferencesPerBlock);
                if (rm > ffbn || rm==0) { del=soFreeIndirectFileBlocks(inode, blk, 0); }
                else {del = soFreeIndirectFileBlocks(inode, blk, rm); }
                if (del)
                {
                  inode->i1[i] = NullReference;
                  sofs18::soFreeDataBlock(blk);
                }
              }
            }
          }
          // if ffbn starts inside direct
          else
          {
            // direct
            for (int i = ffbn; i < N_DIRECT; i++)
            {
              uint32_t blk = inode->d[i];
              if (blk != NullReference)
              {
                inode->d[i] = NullReference;
                sofs18::soFreeDataBlock(blk);
              }
            }
            // indirect
            for (int i = 0; i < N_INDIRECT; i++)
            {
              uint32_t blk = inode->i1[i];
              if (blk != NullReference) 
              {
                soFreeIndirectFileBlocks(inode, blk, 0);
                inode->i1[i]=NullReference;
                sofs18::soFreeDataBlock(blk);
              }
            }
            // double indirect
            for (int i = 0; i < N_DOUBLE_INDIRECT; i++)
            {
              uint32_t blk = inode->i2[i];
              if (blk != NullReference) 
              {
                soFreeDoubleIndirectFileBlocks(inode, blk, 0); 
                inode->i2[i]=NullReference;
                sofs18::soFreeDataBlock(blk);
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
          bool become_Null=true;
          for (uint32_t i=0; i < ReferencesPerBlock; i++)
          {

            block = refs[i];
            if (i>=ffbn) 
            {
              if (block != NullReference)
              {
                refs[i]=NullReference;
                sofs18::soFreeDataBlock(block);
              }
            }
            else 
            {
              if (block!=NullReference) { become_Null = false; }
            }
          }
          soWriteDataBlock(i1,&refs);
          return become_Null;
        }

        /* ********************************************************* */

        static bool soFreeDoubleIndirectFileBlocks(SOInode * ip, uint32_t i2, uint32_t ffbn)
        {
          soProbe(303, "%s(..., %u, %u)\n", __FUNCTION__, i2, ffbn);
          
          uint32_t refs[ReferencesPerBlock], block;
          soReadDataBlock(i2,&refs);
          uint32_t start = ffbn/ReferencesPerBlock;
          bool become_Null=true;
          bool del=false;
          for (uint32_t i=0; i < ReferencesPerBlock; i++)
          {
            block = refs[i];
            if (i>=start)
            {
              if (block != NullReference)
              {
                uint32_t rm = ffbn-(i)*ReferencesPerBlock;
                if (rm==0 || rm > ffbn)
                {
                  refs[i]=NullReference;
                  del=soFreeIndirectFileBlocks(ip, block,0);
                }
                else
                {
                  del=soFreeIndirectFileBlocks(ip, block,rm);
                }
                if (del) { sofs18::soFreeDataBlock(block); }
              }
            }
            else
            {
              if (block!=NullReference)
              {
                become_Null=false;
              }
            }
          }
          return become_Null;
        }
    };
};


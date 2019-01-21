#include "work_fileblocks.h"

#include "freelists.h"
#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>
#include <math.h>
#include <iostream>
#include <cstring>

namespace sofs18{
    namespace work{
        static uint32_t soAllocIndirectFileBlock(SOInode * ip, uint32_t afbn);
        static uint32_t soAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn);
        /* ********************************************************* */
		/* Associate a data block to the given file block position.
		   Parameters:
		      ih	inode handler.
		      fbn	file block pos.
		   Returns:
		      the number of the allocated block.
		*/
        uint32_t soAllocFileBlock(int ih, uint32_t fbn){
            soProbe(302, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);

            SOInode* inode = soITGetInodePointer(ih);
            if (fbn > N_DOUBLE_INDIRECT * pow(ReferencesPerBlock, 2) + N_INDIRECT * ReferencesPerBlock + N_DIRECT)
            {
              throw SOException(EINVAL, __FUNCTION__);    
            }
            uint32_t block;
            uint32_t maxIndirectIndex = (N_INDIRECT * ReferencesPerBlock) + N_DIRECT;

            if (fbn < N_DIRECT) 
            {
              block=sofs18::soAllocDataBlock();
              inode->d[fbn]=block;
              inode->blkcnt++;
            }
            else if (fbn >=N_DIRECT && fbn < maxIndirectIndex) 
            {										
              block=soAllocIndirectFileBlock(inode, fbn - N_DIRECT);
            }
            else
            {
              block=soAllocDoubleIndirectFileBlock(inode, fbn - maxIndirectIndex);
            }

            soITSaveInode(ih);
            return block;
        }
        /* ********************************************************* */
        /*
         */
        static uint32_t soAllocIndirectFileBlock(SOInode * ip, uint32_t afbn){
            soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);
            
            // allocating block in i1
            uint32_t pos = afbn/ReferencesPerBlock;
            uint32_t refs[ReferencesPerBlock];
            uint32_t blk_i1;
            if (ip->i1[pos]==NullReference)
            {
              blk_i1 = sofs18::soAllocDataBlock();
              ip->blkcnt++;
              ip->i1[pos]=blk_i1;
              memset(&refs, NullReference, ReferencesPerBlock*sizeof(uint32_t));
              soWriteDataBlock(blk_i1, &refs);
            }
            else
            {
              blk_i1=ip->i1[pos];
              soReadDataBlock(blk_i1, &refs);
            }
            
            // allocating block in i1_block
            uint32_t pos_blk = afbn - ReferencesPerBlock*pos;
            uint32_t blk_data=sofs18::soAllocDataBlock();
            refs[pos_blk] = blk_data;
            soWriteDataBlock(blk_i1, &refs);
            ip->blkcnt++;

            return blk_data;
        }
        /* ********************************************************* */
        /*
         */
        static uint32_t soAllocDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn){
            soProbe(302, "%s(%d, ...)\n", __FUNCTION__, afbn);
            
            // allocating the i2 blk
            uint32_t pos_i2 = afbn/pow(ReferencesPerBlock, 2);
            uint32_t blk_i2;
            uint32_t refs_i2[ReferencesPerBlock];
            if (ip->i2[pos_i2]==NullReference)
            {
              blk_i2 = sofs18::soAllocDataBlock();
              ip->blkcnt++;
              ip->i2[pos_i2] = blk_i2;
              memset(&refs_i2, NullReference, ReferencesPerBlock*sizeof(uint32_t));
              soWriteDataBlock(blk_i2, &refs_i2);
            }
            else
            {
              blk_i2 = ip->i2[pos_i2];
              soReadDataBlock(blk_i2, &refs_i2);
            }
            
            // allocating block in i2_block
            uint32_t pos_i_blk = afbn/ReferencesPerBlock - pos_i2*ReferencesPerBlock;
            uint32_t refs[ReferencesPerBlock];
            uint32_t blk_refs;
            if (refs_i2[pos_i_blk] == NullReference)
            {
              blk_refs = sofs18::soAllocDataBlock();
              ip->blkcnt++;
              refs_i2[pos_i_blk] = blk_refs;
              memset(&refs, NullReference, ReferencesPerBlock*sizeof(uint32_t));
              soWriteDataBlock(blk_refs, &refs);
            }
            else
            {
              blk_refs = refs_i2[pos_i_blk];
              soReadDataBlock(blk_refs, &refs);
            }
            
            // allocating block in i2_block_refs
            uint32_t pos_refs = afbn - pos_i2*ReferencesPerBlock - pos_i_blk*ReferencesPerBlock;
            uint32_t blk = sofs18::soAllocDataBlock();
            refs[pos_refs] = blk;
            ip->blkcnt++;
            soWriteDataBlock(blk_refs, &refs);
            soWriteDataBlock(blk_i2, &refs_i2);

            return blk;
        }
    };
};


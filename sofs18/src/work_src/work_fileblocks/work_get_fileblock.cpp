#include "work_fileblocks.h"

#include "dal.h"
#include "core.h"
#include "bin_fileblocks.h"

#include <errno.h>

#include <math.h>

namespace sofs18
{
    namespace work
    {

        /* ********************************************************* */
        static uint32_t soGetIndirectFileBlock(SOInode * ip, uint32_t fbn);
        static uint32_t soGetDoubleIndirectFileBlock(SOInode * ip, uint32_t fbn);
        static uint32_t powerOfRefPerBlock = ReferencesPerBlock * ReferencesPerBlock;
        static uint32_t maxFBN = N_DOUBLE_INDIRECT * powerOfRefPerBlock + N_INDIRECT * ReferencesPerBlock + N_DIRECT;
        /* ********************************************************* */

        uint32_t soGetFileBlock(int ih, uint32_t fbn)
        {
            // Pedro Teixeira 84715

            soProbe(301, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);
            //printf("\tUsing work version\n");
            //printf("\tTrying to find block %d\n", fbn);

            /* verify sanity of given file block number */
            if (fbn > maxFBN)
            {
                //EINVAL checked with bin version
                throw SOException(EINVAL, __FUNCTION__);    
            }

            SOInode* inode = soITGetInodePointer(ih);

            /* determine if the block is directly or indirectly referenced */ 
            /* direct (d) */ 
            if (fbn < N_DIRECT) {
                return inode -> d[fbn];
            }

            /* indirect (i1) */
            uint32_t maxIndirectIndex = N_INDIRECT * ReferencesPerBlock + N_DIRECT;
            if (fbn < maxIndirectIndex) {
                return soGetIndirectFileBlock(inode, fbn - N_DIRECT);
            }

            /* double indirect (i2) */
            else if (fbn < N_DOUBLE_INDIRECT * ReferencesPerBlock * ReferencesPerBlock + maxIndirectIndex)
            {
                return soGetDoubleIndirectFileBlock(inode, fbn - maxIndirectIndex);
            }
            
            /* error */
            else
            {
                return NullReference;
            }

            /* binary version */
            // return bin::soGetFileBlock(ih, fbn);
        }

        /* ********************************************************* */
        static uint32_t soGetIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {   
            soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);
            //printf("\tIndirect.\n\t\tTrying to find i1 index %d\n", afbn);
            
            /* get indirect index (i1[0] or i1[1]?) */
            uint32_t i1Index = afbn / ReferencesPerBlock;
           
            uint32_t i1[ReferencesPerBlock];

            /* verify if i1[0]/i1[1] is null */
            if (ip->i1[i1Index] == NullReference)
            {
                return NullReference;
            }
            sofs18::soReadDataBlock(ip->i1[i1Index], i1);

            uint32_t i1Pos = afbn % ReferencesPerBlock;
            return i1[i1Pos];

            /* original version */
            //throw SOException(ENOSYS, __FUNCTION__); 
            //return 0;
        }

        /* ********************************************************* */
        static uint32_t soGetDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {
            soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);
            //printf("\tDouble Indirect.\n\t\tTrying to find i2 index %d\n", afbn);

            /* get double indirect index (i2[0] or i2[1]?) */
            uint32_t iiIndex = afbn / (N_DOUBLE_INDIRECT * powerOfRefPerBlock);
            
            /* get position in i2[0]/i2[1] */
            uint32_t iiPos = afbn / ReferencesPerBlock;

            /* get index in the indirect referencing array */
            uint32_t iIndex = afbn / ReferencesPerBlock;     
           
            /* get position in the indirect referencing array */
            uint32_t iPos = afbn % ReferencesPerBlock;

            /* retrieve block */
            uint32_t i2[ReferencesPerBlock * ReferencesPerBlock];   // retrieve i2[0]/i2[1]
            
            /* verify if i2[0]/i2[1] is null */
            if (ip->i2[iiIndex])
            {
                return NullReference;
            }
            sofs18::soReadDataBlock(ip->i2[iiIndex], i2);

            uint32_t i2_array[ReferencesPerBlock];  // retrieve position in i2 -> indirect
            sofs18::soReadDataBlock(i2[iiPos], i2_array); // referencing array

            uint32_t i2_i1[ReferencesPerBlock];     // retrieve position in the previous array
            sofs18::soReadDataBlock(i2_array[iIndex], i2_i1);

            //return i2_i1[iPos];
            return i2_array[iPos];

            /* original version */
            //throw SOException(ENOSYS, __FUNCTION__); 
            //return 0;
            
        }

    };

};


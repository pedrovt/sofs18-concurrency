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
        static uint32_t maxFBN = N_DOUBLE_INDIRECT * pow(ReferencesPerBlock, N_DOUBLE_INDIRECT) + N_INDIRECT * ReferencesPerBlock + N_DIRECT;
        /* ********************************************************* */
        // *    -> Tested
        // ?    -> Under Testing
        // !    -> Bugs
        // TODO -> Not implemented

        uint32_t soGetFileBlock(int ih, uint32_t fbn)
        {
            soProbe(301, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);
            printf("\tUsing work version\n");
            printf("\tTrying to find block %d\n", fbn);

            /* verify sanity of given file block number */
            if (fbn < 0 || fbn > maxFBN)
            {
                throw SOException(EINVAL, __FUNCTION__);
            }

            SOInode* inode = soITGetInodePointer(ih);

            /* determine if the block is directly or indirectly referenced */ 
            /* direct (d) */ 
            if (fbn < N_DIRECT) {
                return inode -> d[fbn];
            }

            /* indirect (i1) */
            uint32_t maxIndirectIndex = N_INDIRECT * ReferencesPerBlock + N_DIRECT - 1;
            if (fbn < maxIndirectIndex) {
                return soGetIndirectFileBlock(inode, fbn - N_DIRECT);
            }

            /* double indirect (i2) */
            else if (fbn < ReferencesPerBlock * ReferencesPerBlock + maxIndirectIndex)
            {
                return soGetDoubleIndirectFileBlock(inode, fbn - N_INDIRECT * ReferencesPerBlock - N_DIRECT);
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
            printf("\tIndirect.\n\t\tTrying to find i1 index %d\n", afbn);
            
            /* get indirect index (i1[0] or i1[1]?) */
            uint32_t i1Index = afbn / ReferencesPerBlock;
            printf("\t\ti1Index = %d\n", i1Index);

            // !bug
            uint32_t* i1;
            soReadDataBlock(ip->i1[i1Index], i1);
            
            int i1Pos = afbn % ReferencesPerBlock;
            printf("\t\tpos in i1 = %d\n", i1Pos);

            return i1[i1Pos];

            /* original version */
            //throw SOException(ENOSYS, __FUNCTION__); 
            //return 0;
        }

        /* ********************************************************* */
        static uint32_t soGetDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {
            soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);
            printf("\tDouble Indirect.\n\t\tTrying to find i2 index %d\n", afbn);

            static uint32_t powerOfRefPerBlock = ReferencesPerBlock *ReferencesPerBlock;

            /* get double indirect index (i2[0] or i2[1]?) */
            uint32_t iiIndex = afbn / (N_DOUBLE_INDIRECT * powerOfRefPerBlock);
            printf("\t\tiiIndex = %d\n", iiIndex);

            /* get position in i2[0]/i2[1] */
            uint32_t iiPos = afbn % (N_DOUBLE_INDIRECT * powerOfRefPerBlock);
            printf("\t\tiiPos = %d\n", iiPos);

            /* get index in the indirect referencing array */
            uint32_t iIndex = afbn / (ReferencesPerBlock);
            printf("\t\tiIndex = %d\n", iIndex);

            /* get position in the indirect referencing array */
            uint32_t iPos = afbn % (ReferencesPerBlock);
            printf("\t\tiPos = %d\n", iPos);

            /* retrieve block */
            
            uint32_t* i2;        // retrieve i2[0]/i2[1]
            soReadDataBlock(ip->i2[iiIndex], i2);   // * ok

            //!bug
            uint32_t *i2_array;  // retrieve position in i2 -> indirect
            soReadDataBlock(i2[iiPos], i2_array); //referencing array
                                                    //! error

            uint32_t* i2_i1;    // retrieve position in the previous array
            soReadDataBlock(i2_array[iIndex], i2_i1);
                                                    //!error
            

            return (i2_i1[iPos]);

            /* get position in selected i2 */
            //uint32_t iIndex_Index = afbn % ReferencesPerBlock;
            //printf("\t\tiIndex_Index= %d\n", iIndex_Index);

            //uint32_t i1Pos = iIndex_Index / ReferencesPerBlock;
            //printf("\t\tpos= %d\n", i1Pos);
            //return i1[];

            /* original version */
            //throw SOException(ENOSYS, __FUNCTION__); 
            //return 0;
            
        }

    };

};


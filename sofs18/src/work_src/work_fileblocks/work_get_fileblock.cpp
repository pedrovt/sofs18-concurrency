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

        /* ********************************************************* */
        // *    -> Tested
        // ?    -> Under Testing
        // !    -> Bugs
        // TODO -> Not implemented

        uint32_t soGetFileBlock(int ih, uint32_t fbn)
        {
            soProbe(301, "%s(%d, %u)\n", __FUNCTION__, ih, fbn);
            printf("\tUsing work version\n");

            SOInode* inode = soITGetInodePointer(ih);

            /* Determine if the block is in direct field, indirect field or
            double indirect field */
            
            /* direct, d */ 
            if (fbn < N_DIRECT)         
            {
                return inode -> d[fbn];
            }
            
            /* indirect, i */
            uint32_t maxIndirectIndex = N_INDIRECT * ReferencesPerBlock + N_DIRECT - 1;
            if (fbn < maxIndirectIndex) 
            {
                return soGetIndirectFileBlock(inode, fbn - N_DIRECT);
            }

            /* double indirect, i2 */  //--> here? 
            else if (fbn < ReferencesPerBlock*ReferencesPerBlock + maxIndirectIndex)    
            {
                return soGetDoubleIndirectFileBlock(inode, fbn - N_INDIRECT * 2 - N_DIRECT);
            }

            /* error */
            else                        
            {
                return NullReference;
            }

            /* change the following line by your code */
            // return bin::soGetFileBlock(ih, fbn);
        }

        /* ********************************************************* */
        static uint32_t soGetIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {   
            soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);
            printf("\tIndirect. Trying to find index %d\n", afbn);
            
            /* get i1[0]/i1[1] */
            uint32_t* i = &ip->i1[afbn / ReferencesPerBlock];
            
            /* get position in selected i1 */
            return i[afbn % ReferencesPerBlock];
            
            /* change the following two lines by your code */
            //throw SOException(ENOSYS, __FUNCTION__); 
            //return 0;
        }

        /* ********************************************************* */
        static uint32_t soGetDoubleIndirectFileBlock(SOInode * ip, uint32_t afbn)
        {
            soProbe(301, "%s(%d, ...)\n", __FUNCTION__, afbn);
            printf("\nDouble Indirect. Trying to find index %d\n", afbn);


            /* change the following two lines by your code */
            throw SOException(ENOSYS, __FUNCTION__); 
            return 0;
        }

    };

};


#include "work_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"

#include <string.h>
#include <inttypes.h>

namespace sofs18{
    namespace work{

        /*
         filling in the contents of the root directory:
         the first 2 entries are filled in with "." and ".." references
         the other entries are empty.
         If rdsize is 2, a second block exists and should be filled as well.
         */
        uint32_t fillInRootDir(uint32_t first_block, uint32_t rdsize){
            soProbe(606, "%s(%u, %u)\n", __FUNCTION__, first_block, rdsize);
            // change the following line by your code 

            SODirEntry first, second, other;
            first.in = 0;											// first direntry is filled with "." and points to inode 0
            strcpy(first.name, ".");
            second.in = 0;											// second direntry is filled with ".." and points to inode 0
            strcpy(second.name, "..");
            other.in = NullReference;								// other entries are empty
            strcpy(other.name, "\0");								// empty name
            SODirEntry array[DirentriesPerBlock] = {first, second}; 
            for (uint32_t i = 2; i < DirentriesPerBlock; i++){
                array[i] = other;
            }
            for (uint32_t i = 0; i < rdsize; i++) {
                sofs18::soWriteRawBlock(first_block, &array);
                array[0] = other;									// if second block exists the first and the second direntries are reseted
                array[1] = other;
            }
            return rdsize;
            //return bin::fillInRootDir(first_block, rdsize);
        }

    };

};


#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define INODE_REF_SIZE 4

namespace sofs18
{
    namespace work
    {

        uint32_t soGetDirEntry(int pih, const char *name)
        {
            soProbe(201, "%s(%d, %s)\n", __FUNCTION__, pih, name);
		 	
			/*checks if the char '/' is in the name in which case
			an exception is thrown because the name should be a base name*/	
			if(strchr(name, '/')){
				throw SOException(EINVAL, __FUNCTION__);
			}
			
			/*check if pih is a directory
			if not throw an exception*/
			SOInode* parent_inode = sofs18::soITGetInodePointer(pih);
			if(!S_ISDIR(parent_inode -> mode)){
				throw SOException(ENOTDIR, __FUNCTION__);
			}

			uint32_t blocks_to_iterate = (parent_inode -> size)/BlockSize;
			printf("N allocated blocks : %d \n",blocks_to_iterate);

			for(uint32_t w=0; w<blocks_to_iterate; w++){
				//read the direntries block
				char buffer[BlockSize];
				sofs18::soReadFileBlock(pih,w,buffer);
				
				//go through each line
				for(uint32_t i=0; i<DirentriesPerBlock; i++){
					
					//the first INODE_REF_SIZE bytes are the number of the inode
					uint32_t inode_number = 0;
					for(int k=0; k<INODE_REF_SIZE; k++){
						inode_number += buffer[k + i*sizeof(SODirEntry)] << 8*k;
					}

					//the rest of the bytes are the base name of the link
					char string[sizeof(SODirEntry) - INODE_REF_SIZE];
					int index = 0;
					for(uint32_t j=INODE_REF_SIZE; j<sizeof(SODirEntry); j++){
						string[index] =  buffer[j + i*sizeof(SODirEntry)];
						index +=1;
					}
					//if the same name is found return the number of the inode
					if(strcmp(string, name) == 0){
						return inode_number;
					}	
				
				}
			
			}

						
			//return NullReference if we didn't find the dir entry
			return NullReference;
        }

    };

};


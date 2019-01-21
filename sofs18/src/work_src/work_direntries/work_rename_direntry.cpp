#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <string.h>
#include <errno.h>
#include <sys/stat.h>

namespace sofs18
{
    namespace work
    {
		/*
		Rename an entry of a directory.
		A direntry associated from the given directory is renamed.

		Parameters:
		    [in]	pih	inode handler of the parent inode
		    [in]	name	current name of the entry
		    [in]	newName	new name for the entry

		Remarks:
		    Assume pih is a valid inode handler of a directory where the user has write access
		    Error ENOENT should be thrown if name does not exist
		    when calling a function of any layer, use the main version (sofs18::«func»(...)).
		*/
        void soRenameDirEntry(int pih, const char *name, const char *newName)
        {
            soProbe(204, "%s(%d, %s, %s)\n", __FUNCTION__, pih, name, newName);
            //If names are NULL references
            if(strcmp(name, "") == 0 || strcmp(newName, "") == 0){
            	throw SOException(EINVAL,__FUNCTION__);
            }
            //In case the new name is too long
            if(strlen(newName) > SOFS18_MAX_NAME || strlen(name) > SOFS18_MAX_NAME ){
            	throw SOException(ENAMETOOLONG,__FUNCTION__);
            }
            //Exception ENOENT will be thrown by soDeleteDirEntry if name does not exist

            SOInode* inode = sofs18::soITGetInodePointer(pih);	
            // Num blocks with direntries			
            uint32_t numBlocks = (inode->size)/BlockSize;					
            for (uint32_t i = 0; i < numBlocks; i++) {				
                // Block of direntries		
                SODirEntry blockDirEntries[DirentriesPerBlock];				
                sofs18::soReadFileBlock(pih, i, blockDirEntries);			
                for (uint32_t t = 0; t < DirentriesPerBlock; t++) {	
                    // Current direntry 
                    SODirEntry targetDirEntry = blockDirEntries[t];    
                    // If the current dirEntry has the name to replace, replaces it with newname, and writes the modified block into the respective data block
                    if(strcmp(name, targetDirEntry.name) == 0) {
                        strcpy(targetDirEntry.name, newName);
                        blockDirEntries[t] = targetDirEntry;
                        sofs18::soWriteFileBlock(pih, i, blockDirEntries);
                        return;
                    }
                }
            }

            throw SOException(ENOENT,__FUNCTION__);

            //bin::soRenameDirEntry(pih, name, newName);
        }

    };

};


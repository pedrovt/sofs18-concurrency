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
            if(name == NULL || newName == NULL){
            	throw SOException(EINVAL,__FUNCTION__);
            }
            //In case the new name is too long
            if(sizeof(newName) > SOFS18_MAX_NAME + 1){
            	throw SOException(ENAMETOOLONG,__FUNCTION__);
            }
            //Exception ENOENT will be thrown by soDeleteDirEntry if name does not exist
            soAddDirEntry(pih,newName,soDeleteDirEntry(pih,name));
            //bin::soRenameDirEntry(pih, name, newName);
        }

    };

};


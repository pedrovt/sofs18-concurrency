#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "direntries.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

namespace sofs18
{
    namespace work
    {
        uint32_t soTraversePath(char *path)
        {
            soProbe(221, "%s(%s)\n", __FUNCTION__, path);
           
            char *parent_dir = dirname(strdupa(path));
            char *base_dir = basename(strdupa(path));
            if (strcmp(base_dir, parent_dir)==0) { return 0; }
            uint32_t parent_inode = soTraversePath(parent_dir);
            int ih = soITOpenInode(parent_inode);
            if (soCheckInodeAccess(ih, 1))
            {
              uint32_t dir=sofs18::soGetDirEntry(ih,base_dir);
              soITCloseInode(ih);
              if (dir != NullReference)
              {
                return dir;
              }
              throw SOException(ENOENT, __FUNCTION__);
            }
            throw SOException(EACCES, __FUNCTION__);
        }
    };
};


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

        void soRenameDirEntry(int pih, const char *name, const char *newName)
        {
            soProbe(204, "%s(%d, %s, %s)\n", __FUNCTION__, pih, name, newName);

            /* change the following line by your code */
            bin::soRenameDirEntry(pih, name, newName);
        }

    };

};


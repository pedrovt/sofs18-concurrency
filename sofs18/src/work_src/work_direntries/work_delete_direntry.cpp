#include "direntries.h"

#include "core.h"
#include "dal.h"
#include "fileblocks.h"
#include "bin_direntries.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace sofs18
{
    namespace work
    {

        uint32_t soDeleteDirEntry(int pih, const char *name)
        {
            soProbe(203, "%s(%d, %s, %s)\n", __FUNCTION__, pih, name);

            /* change the following line by your code */
            return bin::soDeleteDirEntry(pih, name);
        }

    };

};


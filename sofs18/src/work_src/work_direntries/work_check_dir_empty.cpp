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

        bool soCheckDirEmpty(int ih)
        {
            soProbe(205, "%s(%d)\n", __FUNCTION__, ih);

            /* change the following line by your code */
            return bin::soCheckDirEmpty(ih);
        }

    };

};


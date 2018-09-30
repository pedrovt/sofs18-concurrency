/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2018
 */

#include "work_freelists.h"

#include <string.h>
#include <errno.h>
#include <iostream>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs18
{
    namespace work
    {

        using namespace std;

        void soReplenishIRCache(void)
        {
            soProbe(403, "%s()\n", __FUNCTION__);

            /* change the following line by your code */
            bin::soReplenishIRCache();
        }

    };

};


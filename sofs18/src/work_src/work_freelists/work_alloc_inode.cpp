/*
 *  \author António Rui Borges - 2012-2015
 *  \authur Artur Pereira - 2016-2018
 */

#include "work_freelists.h"

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <iostream>

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs18
{
    namespace work
    {
    	/*

		Allocate a free inode.
		An inode reference is retrieved from inode retrieval cache and the inode is initialized.
		If the cache is empty, it has to be replenished before the retrieval takes place.
		Parameters
		    [in]	type	the inode type (it must represent either a file, or a directory, or a symbolic link)
		Returns
		    the number (reference) of the inode allocated 

		*/
        uint32_t soAllocInode(uint32_t type)
        {
            soProbe(401, "%s(%x)\n", __FUNCTION__, type);
            //return bin::soAllocInode(type);
            //Retrieve the inode reference from the inode retrieval cache
            soSBOpen();
            SOSuperBlock* sb = soSBGetPointer();
            //Checks for free inodes
            if(sb->ifree <= 0){
            	throw SOException(ENOSPC,__FUNCTION__);
            }
            //If the cache is empty, it has to be replenished before the retrieval takes place.
            if(sb->ircache.idx == INODE_REFERENCE_CACHE_SIZE) {
            	soReplenishIRCache();
                soSBOpen();
                //Updates with the new information after the replenish
                sb = soSBGetPointer();
                sb->ircache = sb->ircache;
            }
            uint32_t retrieved_node = sb->ircache.ref[sb->ircache.idx];
            int handler = soITOpenInode(retrieved_node);
            //soITCheckHandler(handler,__FUNCTION__); --> apparently doesn't exist. But it's in the documentation
            sb->ircache.ref[sb->ircache.idx] = NullReference;
            sb->ircache.idx += 1;
            sb->ifree -= 1;
            SOInode* inode = soITGetInodePointer(handler);
            //INode is initialized
			inode->mode = uint16_t(type);
			inode->lnkcnt = 0;
			inode->owner = getuid();
			inode->group = getgid();
			inode->size = 0;
			inode->blkcnt = 0;
			inode->atime = inode->mtime = inode->ctime = time(NULL);
            for(int i = 0; i < N_DIRECT; i++) inode->d[i] = NullReference;
            for(int i = 0; i < N_INDIRECT; i++) inode->i1[i] = NullReference;
            for(int i = 0; i < N_DOUBLE_INDIRECT; i++) inode->i2[i] = NullReference;

			//Save inode
			soITSaveInode(handler);
			soITCloseInode(handler);
            //Save SB
			soSBSave();
            return retrieved_node;
        }

    };

};


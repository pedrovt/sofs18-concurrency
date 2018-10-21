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

#include "core.h"
#include "dal.h"
#include "freelists.h"
#include "bin_freelists.h"

namespace sofs18
{
    namespace work
    {

        void soFreeInode(uint32_t in)
        {
            soProbe(402, "%s(%u)\n", __FUNCTION__, in);

            // operations over the superblock
            // change free inodes
            // change free inode table
            SOSuperBlock *spb_pointer = soSBGetPointer();                                                             // get superblock pointer
            spb_pointer->ifree++;                                                                                     // increase amount of free inodes
            if (spb_pointer->iicache.ref[INODE_REFERENCE_CACHE_SIZE-1] != NullReference) {soDepleteIICache();}        // check if inode insertion cache is full, depletes if it is
            spb_pointer->iicache.ref[spb_pointer->iicache.idx++] = in;                                                // puts inode ref in insertion cache and increase its index
            soSBSave();                                                                                               // save SuperBlock                                   

            int inode = soITOpenInode(in);                            // get Inode handler (int)
            SOInode *inode2free = soITGetInodePointer(inode);         // get pointer to Inode(SOInode *)

            // free Inode
            inode2free->mode = INODE_FREE;                  
            inode2free->lnkcnt = 0;                     
            inode2free->owner = 0;              
            inode2free->group = 0;             
            inode2free->size = 0;   
            inode2free->blkcnt = 0;            
            inode2free->atime = inode2free->ctime = inode2free->mtime = 0;              
            for (int j = 0; j < N_DIRECT; j++) { inode2free->d[j] = NullReference; }
            for (int j = 0; j < N_INDIRECT; j++) { inode2free->i1[j] = NullReference; }
            for (int j = 0; j < N_DOUBLE_INDIRECT; j++) { inode2free->i2[j] = NullReference; }
    
            // save and close Inode
            soITSaveInode(inode);
            soITCloseInode(inode);

            // save superblock

            /* change the following line by your code */
            //printf("If printing this, it's using teacher version")
//            bin::soFreeInode(in);
        }

    };

};


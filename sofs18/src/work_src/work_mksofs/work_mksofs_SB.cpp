#include "work_mksofs.h"
#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"
#include <string.h>
#include <inttypes.h>

namespace sofs18
{
    namespace work
    {   
        
        // *    -> Tested
        // ?    -> Under Testing
        // !    -> Bugs
        // TODO -> Not implemented

        void fillInSuperBlock(const char *name, uint32_t ntotal, uint32_t itotal, uint32_t rdsize)
        {
            soProbe(602, "%s(%s, %u, %u, %u)\n", __FUNCTION__, name, ntotal, 
                    itotal, rdsize);

            // Create SuperBlock
            SOSuperBlock sb;

            /* Header ------------------------------------------------------- */
            // *Magic Number/File System ID Number
            /* Note : Not the SOFS18 magic (MAGIC_NUMBER macro) number to make  
                      sure that, if the formatting process fails, the file is   
                      not viewed as a valid SOFS18 device. */
            sb.magic   = 0xFFFF;  
            
            // *Version Number
            sb.version = VERSION_NUMBER;

            // *Volume Name
            // Default name
            if (!name) 
            {
                strncpy(sb.name, "sofs18_disk", PARTITION_NAME_SIZE);
            }
            // Verifies if size of the given name is less than 
            // PARTITION_NAME_SIZE 
            strncpy(sb.name, name, PARTITION_NAME_SIZE);

            // *Mount Status
            sb.mntstat = 1;

            // *Number of mounts since the last file system check
            sb.mntcnt = 0;

            // *Number of blocks in the device
            sb.ntotal = ntotal;

            /* Free Inodes Table (FILT) ------------------------------------- */
            // *Physical number of the block where the FILT starts
            sb.filt_start = 1;                          // by definition

            // *Number of blocks that the FILT comprises
            sb.filt_size = (itotal / ReferencesPerBlock) + 
                            ((itotal % ReferencesPerBlock != 0)? 1 : 0);

            // *First filled FILT position
            sb.filt_head = 0;

            // *First empty FILT position
            sb.filt_tail = itotal - 1;

            /* Inodes Table (IT) -------------------------------------------- */
            // *Number of blocks that the inode table comprises
            sb.it_size  = itotal / InodesPerBlock;

            // *Physical number of the block where the inode table starts
            sb.it_start = sb.filt_start + sb.filt_size; // inode table starts 
                                                        // after the FILT

            // *Total number of inodes
            sb.itotal = itotal;

            // *Number of free inodes
            sb.ifree = itotal - 1;                      // inode of the Root 
                                                        // dir is not free

            /* Data Zone & Free Block List Table (FBLT) --------------------- */
            // *Physical number of the block where the FBLT starts
            sb.fblt_start = sb.it_start + sb.it_size;   // FBLT starts after
                                                        // the inode table

            // *Number of blocks that in data zone and that the FBLT comprises
            // First estimate of the number of blocks of the FBLT
            uint32_t remainingLBA = ntotal - sb.fblt_start - rdsize;
           
            sb.fblt_size = remainingLBA / (ReferencesPerBlock + 1) + 
                    ((remainingLBA % (ReferencesPerBlock + 1) != 0 ? 1 : 0));

            sb.dz_total = remainingLBA - sb.fblt_size + rdsize;

            // *First filled FBLT position
            sb.fblt_head = 0; 

            // *Physical number of the block where the data zone starts
            sb.dz_start = sb.fblt_start + sb.fblt_size;  // DZ starts after
                                                         // the FBLT

            // *Number of free blocks in data zone
            sb.dz_free = sb.dz_total - rdsize;

            // *First empty FBLT position
            sb.fblt_tail = sb.dz_total - 1;

            /* Caches ------------------------------------------------------- */
            // Should be put empty

            // *Free inodes ################################
            // Creates caches of references to free inodes
            SOInodeReferenceCache irCache;               // Retrieval cache
            SOInodeReferenceCache iiCache;               // Insert cache

            irCache.idx = INODE_REFERENCE_CACHE_SIZE;
            iiCache.idx = 0;

            // Cleans up the caches
            for (int i = 0; i < INODE_REFERENCE_CACHE_SIZE; i++)
            {
                irCache.ref[i] = NullReference;
                iiCache.ref[i] = NullReference;
            }

            sb.ircache = irCache;
            sb.iicache = iiCache;

            // *Free Blocks ################################
            // Creates caches of references to free blocks
            SOBlockReferenceCache brCache;              // Retrieval cache 
            SOBlockReferenceCache biCache;              // Insert cache 

            brCache.idx = BLOCK_REFERENCE_CACHE_SIZE;
            biCache.idx = 0;

            // Cleans up the caches 
            for (int i = 0; i < BLOCK_REFERENCE_CACHE_SIZE; i++)
            {
                brCache.ref[i] = NullReference;
                biCache.ref[i] = NullReference;
            }
            
            sb.brcache = brCache;
            sb.bicache = biCache;
            
            /* Writes the Superblock in the storage devices ----------------- */
            soWriteRawBlock(0, &sb);

            /* To use the given binary version ------------------------------ */
            //bin::fillInSuperBlock(name, ntotal, itotal, rdsize);
        }

        

    };

};


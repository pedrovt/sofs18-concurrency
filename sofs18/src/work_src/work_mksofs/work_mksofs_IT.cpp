#include "work_mksofs.h"

#include "rawdisk.h"
#include "core.h"
#include "bin_mksofs.h"
#include "superblock.h"

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <inttypes.h>

namespace sofs18
{
    namespace work
    {

        uint32_t fillInInodeTable(uint32_t first_block, uint32_t itotal, uint32_t rdsize)
        {
            soProbe(604, "%s(%u, %u, %u)\n", __FUNCTION__, first_block, itotal, rdsize);
            SOInode inode_table[InodesPerBlock]; // estrutura dos nós
           
            // primeiro meter tudo a 0's
            // esta linha dá todas as permissões necessárias 
            inode_table[0].mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IFDIR;
            inode_table[0].lnkcnt = 2;                      // quantos apontam para este bloco (neste caso ele mesmo [o dir .] e ele mesmo [o dir ..])
            inode_table[0].owner = getuid();                // owner ID
            inode_table[0].group = getgid();                // group ID
            inode_table[0].size = BlockSize;       // size of the inode 
            inode_table[0].blkcnt = rdsize;                 // blocos utilizados, nese caso é igual aos blocos utilizados pela raíz
            inode_table[0].atime = inode_table[0].ctime = inode_table[0].mtime = time(NULL);              // data do último ACESSO AO FICHEIRO
            
            // array de referências a blocos diretos
            inode_table[0].d[0] = 0;
            for (int i = 1; i < N_DIRECT; i++) 
            {
              inode_table[0].d[i] = NullReference;
            }
            
            // array de referências a blocos indiretos
            for (int i = 0; i < N_INDIRECT; i++)
            {
              inode_table[0].i1[i] = NullReference;
            }

            // array de referências a blocos duplamente indiretos
            for (int i = 0; i < N_DOUBLE_INDIRECT; i++)
            {
              inode_table[0].i2[i] = NullReference;
            }

            // preencher o resto dos nós
            for (uint32_t i = 1; i < InodesPerBlock; i++)
            {
              inode_table[i].mode = INODE_FREE;                  
              inode_table[i].lnkcnt = 0;                     
              inode_table[i].owner = 0;              
              inode_table[i].group = 0;             
              inode_table[i].size = 0;   
              inode_table[i].blkcnt = 0;            
              inode_table[i].atime = inode_table[i].ctime = inode_table[i].mtime = 0;              
              for (int j = 0; j < N_DIRECT; j++) { inode_table[i].d[j] = NullReference; }
              for (int j = 0; j < N_INDIRECT; j++) { inode_table[i].i1[j] = NullReference; }
              for (int j = 0; j < N_DOUBLE_INDIRECT; j++) { inode_table[i].i2[j] = NullReference; }
            }
            soWriteRawBlock(first_block, &inode_table);
            
            // fill in the rest of the inode table
            for (uint32_t block = first_block+1; block <= itotal/InodesPerBlock+1; block++)
            {
              for (uint32_t inode = 0; inode < InodesPerBlock; inode++)
              {
                inode_table[inode].mode = INODE_FREE;                  
                inode_table[inode].lnkcnt = 0;                     
                inode_table[inode].owner = 0;              
                inode_table[inode].group = 0;             
                inode_table[inode].size = 0;   
                inode_table[inode].blkcnt = 0;            
                inode_table[inode].atime = inode_table[inode].ctime = inode_table[inode].mtime = 0;              
                for (int j = 0; j < N_DIRECT; j++) { inode_table[inode].d[j] = NullReference; }
                for (int j = 0; j < N_INDIRECT; j++) { inode_table[inode].i1[j] = NullReference; }
                for (int j = 0; j < N_DOUBLE_INDIRECT; j++) { inode_table[inode].i2[j] = NullReference; }
              }
              
              // escrita da IT no disco
              soWriteRawBlock(block, &inode_table);
            }

            return itotal/InodesPerBlock;
        }
    };
};


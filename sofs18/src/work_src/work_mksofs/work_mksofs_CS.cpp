#include "work_mksofs.h"

#include "core.h"
#include "bin_mksofs.h"

#include <inttypes.h>
#include <iostream>
#include <math.h>

namespace sofs18
{
    namespace work
    {

        uint32_t roundUp(int numToRound, int multiple){
            if (multiple == 0) return numToRound;
            int remainder = numToRound % multiple;
            if (remainder == 0) return numToRound;
            return numToRound + multiple - remainder;
        }

        uint32_t roundDown(int numToRound, int multiple){
            if (multiple == 0) return numToRound;
            int remainder = numToRound % multiple;
            if (remainder == 0) return numToRound;
            return numToRound - remainder;
        }

        void computeStructure(uint32_t ntotal, uint32_t & itotal, uint32_t & btotal, uint32_t & rdsize)
        {
            soProbe(601, "%s(%u, %u, ...)\n", __FUNCTION__, ntotal, itotal);
            
            uint32_t max_itotal = roundDown(ntotal >> 2, 8); // Maximo de inodes é 1/4 do size disk, arredondado para baixo pois como tem de ser multiplo de 8 ao arredondar para cima poderia passar o 1/4
            if(max_itotal == 0) max_itotal = 8; // Permita ao user criar discos com menos de 32 blocos mesmo que obrigue que os inodes > 1/4 blocos totais
            itotal = (itotal == 0) ? roundUp(ntotal >> 3, 8) : (itotal > max_itotal) ? max_itotal : roundUp(itotal, 8);
            
            uint32_t blocksLeftData = ntotal - 1 - (roundUp(itotal, ReferencesPerBlock) >> 7) - (itotal >> 3); // ntotal - sb - blocos ocupados por inodes - blocos ocupados por referencias a free inodes
            btotal = blocksLeftData*ReferencesPerBlock/(ReferencesPerBlock + 1); // DATA_BLOCKS = N * RPB / RPB + 1  (Das equacoes N = DATA_BLOCKS + FILT_BLOCKS e FILT_BLOCKS = DATA_BLOCKS / RPB)
            uint32_t dataReferences = (uint32_t)ceil(btotal/(float)ReferencesPerBlock); // DATA_BLOCKS / 128 arredondado para cima
            uint32_t blocksLeft = blocksLeftData - (btotal + dataReferences); 

            // Caso em que ira faltar um bloco para por, mas o nr de blocos de data livre ja e multiplo de 128. 
            // Logo ao adicionar la iria necessitar de aumentar o nr de blocos para free inodes
            if(blocksLeft == 1) { 
                printf("%d\n", itotal);
                if(itotal % ReferencesPerBlock != 0) {  // Se os inodes ja forem multiplos de 128 o root tem de ficar com 2 blocos de data
                    itotal += 8;
                    rdsize = 1;
                } else rdsize = 2;
            } else {
                rdsize = 1;
            }

<<<<<<< HEAD
//            std::cout << "Nr of inodes: " << itotal << " using a total of " << (itotal >> 3) << " blocks\n"; 
//            std::cout << "Nr of blocks for free inodes references: " << (roundUp(itotal, 128) >> 7) << "\n";
//            std::cout << "Nr of blocks for data: " << btotal << "\n";
//            std::cout << "Nr of blocks for free data blocks references: " << (roundUp(blocksLeftData, 128) >> 7) << "\n";
//            std::cout << "Nr of blocks for the root: " << rdsize << "\n";
=======
            /* printf("%d\n", blocksLeftData);
            printf("%d\n", btotal);
            printf("%d\n", dataReferences);
            printf("%d\n", blocksLeft); */
>>>>>>> master

            //bin::computeStructure(ntotal, itotal, btotal, rdsize);
        }

    };

};
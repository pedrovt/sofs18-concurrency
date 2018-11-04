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
            
            uint32_t blocksLeftData = ntotal - 1 - (roundUp(itotal, 128) >> 7) - (itotal >> 3); // ntotal - sb - blocos ocupados por inodes - blocos ocupados por referencias a free inodes
            uint32_t dataReferences = blocksLeftData >> 7; // O nr de referencias minimo para inodes vai ser blocos que faltam / 128
            uint32_t blocksLeft = blocksLeftData - (dataReferences << 7) - dataReferences; 
            // Caso em que ira faltar um bloco para por, mas o nr de blocos de data livre ja e multiplo de 128. 
            // Logo ao adicionar la iria necessitar de aumentar o nr de blocos para free inodes
            if(blocksLeft == 1) { 
                btotal = blocksLeftData - (uint32_t)dataReferences;
                if(itotal % 128 != 0) {  // Se os inodes ja forem multiplos de 128 o root tem de ficar com 2 blocos de data
                    itotal++;
                    rdsize = 1;
                } else rdsize = 2;
            } else {
                rdsize = 1;
                // Caso em que faltam 0 blocos para por depois de retirar o nr de blocos para dados e referencias
                // (Casos em que as referencias ficam preenchidas perfeitamente, sem a existencia de null references)
                if(blocksLeft < 1) btotal = blocksLeftData - (uint32_t)dataReferences;
                // Casos em que faltem mais do que um bloco, e necessario aumentar o nr de blocos para referencias por um
                // E o resto para blocos
                else if(blocksLeft > 1) btotal = blocksLeftData - (dataReferences + 1);
            }
            //bin::computeStructure(ntotal, itotal, btotal, rdsize);
        }

    };

};
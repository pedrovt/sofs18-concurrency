/**
 *  \file
 *  \brief Student version of the \b sofs18 formatting functions.
 *
 *  \author Artur Pereira - 2018
 *  \author ...
 */

#ifndef __SOFS18_MKSOFS_WORK__
#define __SOFS18_MKSOFS_WORK__

#include <inttypes.h>

namespace sofs18
{
    namespace work
    {

        /**
         * \brief computes the structural division of the disk
         * \details The following rules should be observed:
         *     \li if \c itotal is zero, the value ntotal / 8 should is used as a start value;
         *     \li itotal must be always lower than or equal to ntotal / 4;
         *     \li rdsize represents the number of blocks used by the root dir for formatting;
         *          it is normally 1, but can be 2 if a block can not be assigned to any other 
         *          purpose.
         * \param [in] ntotal Total number of blocks of the disk
         * \param [in, out] itotal Total number of inodes
         * \param [out] btotal Total number of data blocks
         * \param [out] rdsize Initial number of blocks of the root directory
         */
        void computeStructure(uint32_t ntotal, uint32_t & itotal, uint32_t & btotal, uint32_t & rdsize);

        /**
         * \brief Format the superblock
         * \details The magic number should be put at 0xFFFF.
         * \param [in] name Volume name
         * \param [in] ntotal Total number of blocks of the disk
         * \param [in] itotal Total number of inodes
         * \param [in] rdsize Initial number of blocks of the root directory
         */
        void fillInSuperBlock(const char *name, uint32_t ntotal, uint32_t itotal, uint32_t rdsize);

        /**
         * \brief Format the free inode list table
         * \details The list should represent the sequence 1..itotal-1
         * \param [in] first_block Number of the block where the table starts
         * \param [in] itotal Total number of inodes
         * \return The number of blocks occupied by the table
         */
        uint32_t fillInFreeInodeListTable(uint32_t first_block, uint32_t itotal);

        /**
         * \brief Format the inode table
         * \details The inode 0 must be filled assuming it is used by the root directory.
         *      The other inodes are free
         * \param [in] first_block Number of the block where the table starts
         * \param [in] itotal Total number of inodes
         * \param [in] rdsize Initial number of blocks of the root directory
         * \return The number of blocks occupied by the table
         */
        uint32_t fillInInodeTable(uint32_t first_block, uint32_t itotal, uint32_t rdsize);

        /**
         * \brief Format the free data block list table
         * \param [in] first_block Number of the block where the table starts
         * \param [in] btotal Total number of data blocks
         * \param [in] rdsize Initial number of blocks of the root directory
         * \return The number of blocks occupied by the table
         */
        uint32_t fillInFreeBlockListTable(uint32_t first_block, uint32_t btotal, uint32_t rdsize);

        /**
         * \brief Format the root directory
         * \details The root directory occupies one or two contiguous blocks,
         *      with the first two slots filled in with
         *      "." and ".." directory entries.
         *      The other slots must be cleaned: field \c name filled with zeros and field \c inode
         *      filled with \c NullReference.
         * \param [in] first_block Number of the first data block of the root directory
         * \param [in] rdsize Initial number of blocks of the root directory
         * \return The number of blocks occupied by the root directory
         */
        uint32_t fillInRootDir(uint32_t first_block, uint32_t rdsize);

        /**
         * \brief Fill in with zeros the given range of data blocks
         * \param [in] first_block Number of the first block
         * \param [in] cnt Number of blocks
         */
        void resetBlocks(uint32_t first_block, uint32_t cnt);

    };

};

#endif   /* __SOFS18_MKSOFS_WORK__ */

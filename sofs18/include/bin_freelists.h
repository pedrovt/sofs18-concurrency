/**
 * \file
 * \brief Binary version of functions to manage the list of free inodes 
 *      and the list of free blocks
 * 
 *  \author Artur Pereira 2008-2009, 2016-2018
 *  \author Miguel Oliveira e Silva 2009, 2017
 *  \author António Rui Borges - 2010-2015
 *
 */

/*
 * \defgroup freelists freelists
 *
 * @{
 *
 *  \remarks In case an error occurs, every function throws an SOException
 */

#ifndef __SOFS18_FREELISTS_BIN__
#define __SOFS18_FREELISTS_BIN__

#include <inttypes.h>

namespace sofs18
{
    namespace bin
    {

        /**
         *  \brief Allocate a free inode.
         *
         *  An inode is retrieved from the list of free inodes, marked in use, 
         *  associated to the legal file type passed as
         *  a parameter and is generally initialized. 
         *
         *  \param type the inode type (it must represent either a file, 
         *          or a directory, or a symbolic link)
         *  \return the number of the allocated inode
         */
        uint32_t soAllocInode(uint32_t type);

        /* *************************************************** */

        /**
         *  \brief Free the referenced inode.
         *
         *  The inode is inserted into the list of free inodes.
         *
         *  \param in number of the inode to be freed
         */
        void soFreeInode(uint32_t in);

        /* *************************************************** */

        /**
         *  \brief Allocate a free data block.
         *
         *  The block is retrieved from the retrieval cache of free block references. 
         *  If the cache is empty, it has to be replenished before the retrieval may take place.
         *
         *  \return the number of the allocated block
         */
        uint32_t soAllocDataBlock();

        /* *************************************************** */

        /**
         * \brief replenish the inode retrieval cache
         * \details References to free inode should be transfered from the free inode table
         *      or from the inode insertion cache to the inode retrieval cache.
         *      Nothing should be done if the retrieval cache is not empty.
         *      The insertion cache should only be used if the free inode table is empty.
         *      Only a single block should be processed, 
         *      even if it is not enough to fulfill the retrieval cache.
         *      The block to be processes is the one pointed to by the \c filt_head field 
         *      of the superblock.
         */
        void soReplenishIRCache();

        /* *************************************************** */

        /**
         * \brief replenish the data block retrieval cache
         * \details References to free data blocks should be transfered from the free data block table
         *      or from the data block insertion cache to the data block retrieval cache.
         *      Nothing should be done if the retrieval cache is not empty.
         *      The insertion cache should only be used if the free data block table is empty.
         *      Only a single block should be processed, 
         *      even if it is not enough to fulfill the retrieval cache.
         *      The block to be processed is the one pointed to by the \c fblt_head field 
         *      of the superblock.
         */
        void soReplenishBRCache();

        /* *************************************************** */

        /**
         *  \brief Free the referenced data block.
         *
         *  \param cn the number of the block to be freed
         */
        void soFreeDataBlock(uint32_t cn);

        /* *************************************************** */

        /**
         * \brief Deplete the inode insertion cache
         * \details References to free inodes should be transfered from the inode insertion cache
         *      to the free inode list table.
         *      Only a single block should be processed, 
         *      even if it is not enough to empty the insertion cache.
         *      The block to be processed is the one pointed to by the \c filt_tail field 
         *      of the superblock.
         */
        void soDepleteIICache();

        /* *************************************************** */

        /**
         * \brief Deplete the data block insertion cache
         * \details References to free data blocks should be transfered from the data block insertion cache
         *      to the free data block list table.
         *      Only a single block should be processed, 
         *      even if it is not enough to empty the insertion cache.
         *      The block to be processed is the one pointed to by the \c fblt_tail field 
         *      of the superblock.
         */
        void soDepleteBICache();

        /* *************************************************** */
        /* @} */
        /* *************************************************** */

    };

};

#endif				/* __SOFS18_FREELISTS_BIN__ */

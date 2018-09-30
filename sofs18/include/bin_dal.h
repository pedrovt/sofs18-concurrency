/**
 * \file
 * \brief Binary version of the disk abstraction layer module
 */

#ifndef __SOFS18_BIN_DAL__
#define __SOFS18_BIN_DAL__

#include <inttypes.h>

#include "superblock.h"
#include "inode.h"

namespace sofs18
{
    namespace bin
    {
        /* ***************************************** */

        /*
         * \brief Open the superblock dealer
         *
         * Prepare the internal data structure of the superblock dealer
         */
        void soSBOpen();

        /* ***************************************** */

        /*
         * \brief Close the superblock dealer
         *
         * Save superblock to disk and close dealer
         * Do nothing if not loaded
         */
        void soSBClose();

        /* ***************************************** */

        /*
         * \brief Save superblock to disk
         *
         * Do nothing if not loaded
         */
        void soSBSave();

        /* ***************************************** */

        /*
         * \brief Get a pointer to the superblock
         *
         * Load the superblock, if not done yet
         *
         * \return Pointer to the superblock
         */
        SOSuperBlock * soSBGetPointer();

        /* ***************************************** */
        /* ***************************************** */

        /*
         * \brief Open (load) a FILT block
         *
         * A FILT block is loaded from disk.
         * Throws an error if a block is opened.
         *
         * \param[in] bn relative number of block to be open
         */
        uint32_t * soFILTOpenBlock(uint32_t bn);

        /* ***************************************** */

        /*
         * \brief Save the open FILT block to disk.
         *
         * Throws an error if no block is open.
         */
        void soFILTSaveBlock();

        /* ***************************************** */

        /*
         * \brief Close the open FILT block.
         *
         * Throws an error if no block is open.
         */
        void soFILTCloseBlock();

        /* ***************************************** */
        /* ***************************************** */

        /*
         * \brief Open inode table dealer
         *
         * Prepare the internal data structure for the inode table dealer
         */
        void soITOpen();

        /* ***************************************** */

        /*
         * \brief Close the inode table dealer
         *
         * Save to disk all openning inodes and close dealer
         */
        void soITClose();

        /* ***************************************** */

        /*
         * \brief open inode
         *
         * If inode is already open, just increment usecount;
         * otherwise, transfer the inode from disk and 
         * put usecount at 1.
         *
         * \param in the number of the inode to open
         * \return inode handler
         */
        int soITOpenInode(uint32_t in);

        /* ***************************************** */

        /*
         * \brief get pointer to an open inode
         *
         * A pointer to the SOInode structured where the inode
         * is loaded is returned.
         *
         * \param ih inode handler
         * \return pointer to the inode
         */
        SOInode* soITGetInodePointer(int ih);

        /* ***************************************** */

        /*
         * \brief Save an open inode to disk
         *
         * The inode is not closed.
         *
         * \param ih inode handler
         */
        void soITSaveInode(int ih);

        /* ***************************************** */

        /*
         * \brief Close an open inode
         *
         * Decrement usecount of given inode,
         * releasing slot if 0 is reached.
         *
         * \param ih inode handler
         */
        void soITCloseInode(int ih);

        /* ***************************************** */

        /*
         * \brief Return the number of the inode associated to the given handler
         * \param ih inode handler
         * \return inode number
         */
        uint32_t soITGetInodeID(int ih);

        /* ***************************************** */
        /* ***************************************** */

        /*
         * \brief check an open inode against a requested access
         * \details access is a bitwise OR of one or more of R_OK, W_OK, and X_OK
         * \sa man 2 access
         * \param ih inode handler
         * \param access requested access
         * \return true, for access granted; false for access denied
         */
        bool soCheckInodeAccess(int ih, int access);
        
        /* ***************************************** */
        /* ***************************************** */

        /*
         * \brief Open (load) a FBLT block
         *
         * A FBLT block is loaded from disk.
         * Throws an error if a block is opened.
         *
         * \param[in] bn relative number of block to be open
         */
        uint32_t * soFBLTOpenBlock(uint32_t bn);

        /* ***************************************** */

        /*
         * \brief Save the open FBLT block to disk.
         *
         * Throws an error if no block is open.
         */
        void soFBLTSaveBlock();

        /* ***************************************** */

        /*
         * \brief Close the open FBLT block.
         *
         * Throws an error if no block is open.
         */
        void soFBLTCloseBlock();

        /* ***************************************** */
        /* ***************************************** */

        /*
         * \brief Read a block of the data zone
         *
         * \param[in] bn number of block to be read
         * \param[in] buf pointer to the buffer where the data must be read into
         */
        void soReadDataBlock(uint32_t bn, void *buf);

        /* ***************************************** */

        /*
         * \brief Write a block of the data zone
         *
         * \param[in] bn number of block to be read
         * \param[in] buf pointer to the buffer where the data must be written from
         */
        void soWriteDataBlock(uint32_t bn, void *buf);

        /* ***************************************** */
    };

};


#endif    /* __SOFS18_BIN_DAL__ */

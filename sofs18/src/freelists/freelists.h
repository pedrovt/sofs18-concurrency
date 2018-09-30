/**
 * \file
 * \brief Functions to manage the list of free inodes and the list of free blocks
 * 
 *  \author Artur Pereira 2008-2009, 2016-2018
 *  \author Miguel Oliveira e Silva 2009, 2017
 *  \author António Rui Borges - 2010-2015
 *
 */

#ifndef __SOFS18_FREELISTS__
#define __SOFS18_FREELISTS__

#include <inttypes.h>

namespace sofs18
{
    /**
     * \defgroup ilayers ilayers
     * \brief Intermediate functions
     */
    
    /**
     * \defgroup freelists freelists
     * \ingroup ilayers
     * \brief Functions to manage the list of free inodes and the list of free blocks.
     * \remarks In case an error occurs, every function throws an \c SOException.
     * @{ 
     */
 
    /* *************************************************** */

    /**
     *  \brief Allocate a free inode.
     *
     *  An inode reference is retrieved from inode retrieval cache and the inode is initialized.
     *  If the cache is empty, it has to be replenished before the retrieval takes place.
     *
     *  \param [in] type the inode type (it must represent either a file, 
     *          or a directory, or a symbolic link)
     *  \return the number (reference) of the inode allocated
     */
    uint32_t soAllocInode(uint32_t type);

    /* *************************************************** */

    /**
     *  \brief Free the referenced inode.
     *
     * \details
     *  The inode reference is inserted into the inode insertion cache.
     *  If the cache is full, it has to be depleted before the insertion takes place.
     *
     *  \param [in] in number (reference) of the inode to be freed
     */
    void soFreeInode(uint32_t in);

    /* *************************************************** */

    /**
     *  \brief Allocate a free data block.
     *
     *  \details 
     *  The block reference is retrieved from the data block retrieval cache. 
     *  If the cache is empty, it has to be replenished before the retrieval takes place.
     *
     *  \return the number (reference) of the data block allocated
     */
    uint32_t soAllocDataBlock();

    /* *************************************************** */

    /**
     *  \brief Free the referenced data block.
     *
     * \details
     *  The data block reference is inserted into the data block insertion cache.
     *  If the cache is full, it has to be depleted before the insertion takes place.
     *
     *  \param bn the number (reference) of the data block to be freed
     */
    void soFreeDataBlock(uint32_t bn);

    /* *************************************************** */

    /**
     * \brief Replenish the inode retrieval cache
     * \details References to free inode should be transferred from the free inode list table
     *      or from the inode insertion cache to the inode retrieval cache.
     *  \remarks
     *      \li Nothing should be done if the retrieval cache is not empty.
     *      \li The insertion cache should only be used if the free inode list table is empty.
     *      \li Only a single block should be processed, 
     *      even if it is not enough to fulfill the retrieval cache.
     *      \li The block to be processes is the one pointed to by the \c filt_head field 
     *      of the superblock.
     */
    void soReplenishIRCache();

    /* *************************************************** */

    /**
     * \brief Replenish the data block retrieval cache
     * \details References to free data blocks should be transferred from the free data block list table
     *      or from the data block insertion cache to the data block retrieval cache.
     * \remarks
     *      \li Nothing should be done if the retrieval cache is not empty.
     *      \li The insertion cache should only be used if the free data block list table is empty.
     *      \li Only a single block should be processed, 
     *          even if it is not enough to fulfill the retrieval cache.
     *      \li The block to be processed is the one pointed to by the \c fblt_head field 
     *      of the superblock.
     */
    void soReplenishBRCache();

    /* *************************************************** */

    /**
     * \brief Deplete the inode insertion cache
     * \details References to free inodes should be transferred from the inode insertion cache
     *      to the free inode list table.
     *  \remarks
     *      \li Only a single block should be processed, 
     *          even if it is not enough to empty the insertion cache.
     *      \li The block to be processed is the one pointed to by the \c filt_tail field 
     *      of the superblock.
     */
    void soDepleteIICache();

    /* *************************************************** */

    /**
     * \brief Deplete the data block insertion cache
     * \details References to free data blocks should be transferred from the data block insertion cache
     *      to the free data block list table.
     *  \remarks
     *      \li Only a single block should be processed, 
     *          even if it is not enough to empty the insertion cache.
     *      \li The block to be processed is the one pointed to by the \c fblt_tail field 
     *      of the superblock.
     */
    void soDepleteBICache();

    /* *************************************************** */
    /** @} close group freelists */
    /* *************************************************** */

};

#endif				/* __SOFS18_FREELISTS__ */

/**
 *  \file 
 *  \brief Functions to manage directory entries
 *
 *  \author Artur Pereira 2008-2009, 2016-2018
 *  \author Miguel Oliveira e Silva 2009, 2017
 *  \author António Rui Borges - 2012-2015
 *
 *  \remarks In case an error occurs, every function throws an SOException
 */

#ifndef __SOFS18_DIRENTRIES__
#define __SOFS18_DIRENTRIES__

#include <inttypes.h>

namespace sofs18
{

    /* ************************************************** */
    /** \defgroup direntries direntries 
     *  \ingroup ilayers
     *  \brief Functions to manage directory entries
     *  \remarks In case an error occurs, every function throws an SOException
     * @{ 
     */
    /* ************************************************** */

    /**
     *  \brief Get the inode associated to the given path
     *
     *  The directory hierarchy of the file system is traversed to find
     *  an entry whose name is the rightmost component of
     *  <tt>path</tt>.
     *  The path is supposed to be absolute and each component of <tt>path</tt>,
     *  with the exception of the rightmost one,
     *  should be a directory name or symbolic link name to a path.
     *
     *  The process that calls the operation must have execution
     *  (x) permission on all the components of the path with
     *  exception of the rightmost one.
     *
     *  \param [in] path the path to be traversed
     *  \return the corresponding inode number
     */
    uint32_t soTraversePath(char *path);

    /* ************************************************** */

    /**
     *  \brief Get the inode associated to the given name
     *
     *  The directory contents, seen as an array of directory entries,
     *  is parsed to find an entry whose name is <tt>name</tt>.
     *
     *  The <tt>name</tt> must also be a <em>base name</em> and not a <em>path</em>,
     *  that is, it can not contain the character '/'.
     *
     *  \param [in] pih inode handler of the parent directory
     *  \param [in] name the name of the entry to be searched for
     *  \return the corresponding inode number
     */
    uint32_t soGetDirEntry(int pih, const char *name);

    /* ************************************************** */

    /**
     *  \brief Add a new entry to the parent directory.
     *
     *  A direntry is added connecting the parent inode to the child inode.
     *  The refcount of the child inode is not incremented by this function.
     *
     *  \param [in] pih inode handler of the parent inode
     *  \param [in] name the name of the entry to be created
     *  \param [in] cin inode number of the entry to be created
     */
    void soAddDirEntry(int pih, const char *name, uint32_t cin);

    /* ************************************************** */

    /**
     *  \brief Remove an entry from a parent directory.
     *
     *  A direntry associated from the given directory is deleted.
     *  The \c linkcnt of the child inode is not decremented by this function.
     *
     *  \param [in] pih inode handler of the parent inode
     *  \param [in] name name of the entry
     *  \return the inode number of the deleted entry
     */
    uint32_t soDeleteDirEntry(int pih, const char *name);

    /* ************************************************** */

    /**
     *  \brief Rename an entry of a directory.
     *
     *  A direntry associated from the given directory is renamed.
     *
     *  \param [in] pih inode handler of the parent inode
     *  \param [in] name current name of the entry
     *  \param [in] newName new name for the entry
     */
    void soRenameDirEntry(int pih, const char *name, const char *newName);

    /* ************************************************** */

    /**
     *  \brief Check directory emptiness
     *
     *  The directory is traversed to verified if the only existing entries are "." and "..".
     *
     *  \param [in] ih handler of the inode to be checked
     */
    bool soCheckDirEmpty(int ih);

    /* ************************************************** */
    /** @} close group direntries */
    /* ************************************************** */

};

#endif             /* __SOFS18_DIRENTRIES__ */

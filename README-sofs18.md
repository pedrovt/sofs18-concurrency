# The **sofs18** file system supporting software

******
******

## Prerequisites

On Ubuntu you need the following packages installed: 
_build-essential_, _cmake_, _doxygen_, _libfuse-dev_, and _git_.

```
sudo apt install build-essential cmake doxygen libfuse-dev git
```

In other Linux distributions you need equivalent packages installed.

******

## Cloning the repo

In a directory of your choice, clone the project to your computer

```
cd «directory-of-your-choice»
git clone https://code.ua.pt/git/XXXXX
```

where **XXXXX** must be your project name.

******

## Compiling the code

In a terminal, enter the **build** directory of your project

```
cd XXXXX/sofs18/build
```

Then compile the code

```
cmake ../src
make
```

******

## Generating documentation

The code is documented in **doxygen**. So, you can easily generate **html** documentation pages.

```
cd XXXXX/sofs18/doc
doxygen
firefox html/index.html &
```

Of course, you can change firefox by your favourite browser.

******

## Testing the code

The following sequence of commands, where XXXXX is your project's name, allows you to mount a **sofs** file system

```
cd XXXXX/sofs18/bin
./createDisk /tmp/zzz 1000      # /tmp/zzz will be a disk with 1000 blocks
./mksofs /tmp/zzz               # format the disk as a sofs18 file system
mkdir /tmp/mnt                  # our mount point
./sofsmount /tmp/zzz /tmp/mnt   # mount the disk in the mount point
```
Now, everything created inside the mount point will be stored in disk (the /tmp/zzz file). You can use the **showblock** tool to check that out.

******

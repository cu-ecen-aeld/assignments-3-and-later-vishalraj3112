#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
#KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_REPO=https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git #---Adding my part here
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

#---Adding my part here
if ! [ -d "$OUTDIR" ]; then #directory could not be created, return error
	echo " $OUTDIR Director could not be created"
	exit 1
fi
#---Adding my part here	

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    # Step 1: Clean tree
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper
    # Step 2: Configure 
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig
    # Step 3: Build kernal image
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
    # Step 4: Build modules
    make ARCH=arm64 CROSS_COMPILE= aarch64-none-linux-gnu- modules
    # Step 5: Build device tree
    make ARCH=arm64 CROSS_COMPILE= aarch64-none-linux-gnu - dtbs
fi 

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories

#---Adding my part here
if ! [ -d "${OUTDIR}/rootfs" ]
then
	mkdir ${OUTDIR}/rootfs
fi

cd ${OUTDIR}/rootfs
mkdir bin dev etc home lib proc sbin sys tmp usr var
mkdir usr/bin usr/lib usr/sbin
mkdir -p var/log
cd ${OUTDIR}/rootfs
#sudo chown -R root:root *

#---Adding my part here

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and insatll busybox
make distclean
make defconfig
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
cd ${OUTDIR}/rootfs
cp -a $SYSROOT/lib/ld-linux-armhf.so.3 lib
cp -a $SYSROOT/lib/ld-2.19.so lib
cp -a $SYSROOT/lib/libc.so.6 lib
cp -a $SYSROOT/lib/libc-2.19.so lib
cp -a $SYSROOT/lib/libm.so.6 lib
cp -a $SYSROOT/lib/libm.so.6 lib


# TODO: Make device nodes
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
make clean
make CROSS_COMPILE-aarch64-none-linux-gnu- all

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs

# TODO: Chown the root directory
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
cd "${OUTDIR}/rootfs"
find . | cpio -H newc -ov --owner root:root > ../initramfs.cpio


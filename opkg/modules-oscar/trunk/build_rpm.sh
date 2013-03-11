#!/bin/bash - 
#===============================================================================
#
#          FILE: build_rpm.sh
# 
#         USAGE: ./build_rpm.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: DongInn Kim (), dikim@cs.indiana.edu
#  ORGANIZATION: Center for Research in Extreme Scale Technologies
#       CREATED: 03/09/2013 10:48:43 PM EST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

manual=0

while getopts "h?m" opt; do
    case "$opt" in
        h|\?)
            echo "Running in OSCAR: $0"
            echo "Running in command line manually: $0 m"
            exit 0
            ;;
        m) 
            manual=1
            ;;
    esac
done

shift $((OPTIND-1))

BUILD_ROOT=/tmp/build_modules
SRC_BUILD=`/bin/rpm --eval %{_sourcedir}`
CWD="/tmp/oscar-packager/modules-oscar"
if [[ $manual == 1 ]];then
    CWD=`pwd`
    SRC_BUILD=$BUILD_ROOT/SOURCES
    mkdir -p $SRC_BUILD
fi


DL_URL="http://svn.oscar.openclustergroup.org/pkgs/downloads"

SRC_FILE_1="modules-oscar"
SRC_FILE_VER="1.0.5"
SRC_FILE="$SRC_FILE_1-$SRC_FILE_VER" 
SRC_DIR="$CWD/$SRC_FILE"
cd $SRC_DIR
./configure
make dist
cp $SRC_FILE.tar.gz $SRC_BUILD/

make distclean
rm -f $SRC_FILE.tar.gz


SRC_FILE_2="modules-default-manpath-oscar"
SRC_FILE_VER="1.0.1"
SRC_FILE="$SRC_FILE_2-$SRC_FILE_VER" 
SRC_DIR="$CWD/$SRC_FILE"
cd $SRC_DIR
./configure
make dist
cp $SRC_FILE.tar.gz $SRC_BUILD/
make distclean
rm -f $SRC_FILE.tar.gz


if [[  $manual == 1 ]]; then
    cd $SRC_BUILD/
    wget $DL_URL/$SRC_FILE_1/modules-3.2.9c.tar.bz2
    wget $DL_URL/$SRC_FILE_1/Modules-Paper.doc
    wget $DL_URL/$SRC_FILE_1/Modules-Paper.pdf
    wget $DL_URL/$SRC_FILE_1/modules-3.2.9_bashrc.patch

    SPEC_DIR="$CWD/rpm"
    rpmbuild -bb --define "_topdir $BUILD_ROOT" "$SPEC_DIR/$SRC_FILE_1.spec"
    rpmbuild -bb --define "_topdir $BUILD_ROOT" "$SPEC_DIR/$SRC_FILE_2.spec"

    cp $BUILD_ROOT/RPMS/x86_64/*.rpm $CWD/
    rm -rf $BUILD_ROOT
fi

cd $CWD

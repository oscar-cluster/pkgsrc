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
#       CREATED: 03/13/2013 10:48:43 PM EST
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

BUILD_ROOT=/tmp/build_rpms
SRC_BUILD=`/bin/rpm --eval %{_sourcedir}`
CWD="/tmp/oscar-packager/jobmonarch"
if [[ $manual == 1 ]];then
    CWD=`pwd`
    SRC_BUILD=$BUILD_ROOT/SOURCES
    mkdir -p $SRC_BUILD
fi


DL_URL="http://svn.oscar.openclustergroup.org/pkgs/downloads"

SRC_FILE_1="jobmonarch"
SRC_FILE_2="pbs_python"

cp $CWD/rpm/jobmonarch-*.patch $SRC_BUILD/

if [[  $manual == 1 ]]; then
    cd $SRC_BUILD/
    wget $DL_URL/jobmonarch-0.4-pre.tar.gz
    wget $DL_URL/jobmonarch-0.4.tar.gz
    wget $DL_URL/pbs_python-4.3.3.tar.gz

    SPEC_DIR="$CWD/rpm"
    rpmbuild -bb --define "_topdir $BUILD_ROOT" "$SPEC_DIR/$SRC_FILE_1.spec"
    rpmbuild -bb --define "_topdir $BUILD_ROOT" "$SPEC_DIR/$SRC_FILE_2.spec"

    cp $BUILD_ROOT/RPMS/*/$SRC_FILE_1*.rpm $CWD/
    cp $BUILD_ROOT/RPMS/*/$SRC_FILE_2*.rpm $CWD/
    rm -rf $BUILD_ROOT
fi

cd $CWD

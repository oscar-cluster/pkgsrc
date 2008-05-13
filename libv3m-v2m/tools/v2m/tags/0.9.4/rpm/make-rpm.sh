#!/bin/sh
#

export PKG_CONFIG_PATH=/usr/lib/pkgconfig
rm -f ../v2m*tar.gz ../../../libv3m/trunk/libv3m*tar.gz
cd ..
V2M_DIR=`pwd`
echo $V2M_DIR
cd ../../libv3m/trunk; ./autogen.sh; ./configure; make dist
sudo cp libv3m*tar.gz /usr/src/redhat/SOURCES
cd $V2M_DIR; ./autogen.sh; ./configure --with-libv3m=../../libv3m; make dist
sudo cp v2m*tar.gz /usr/src/redhat/SOURCES
sudo rpmbuild -ba ./rpm/v2m.spec

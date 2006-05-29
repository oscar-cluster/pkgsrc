#!/bin/sh
#
#
# Copyright (c) 2006 Oak Ridge National Laboratory, Geoffroy Vallee <valleegr@ornl.gov>
#                    All rights reserved
# Modified and generalized for rapt by:
#
# Copyright (c) 2006 Erich Focht <efocht@hpce.nec.com>
#                    All rights reserved
#

# we get the version number from the debian/control file. This file must be updated before the creation of a new package# 
version=`grep "Standards-Version:" debian/control | sed 's/Standards-Version: //' | sed 's/ //'`
path=`pwd`


package=rapt

echo "Be also sure before to create the package that package information is up-to-date."
echo "Read the file deb/README for more details."
echo "Press enter to continue or Ctrl+C to abord."
read $toto

echo "Creating Debian package for $package version " $version

# we first copy everything in /tmp/deb-packman and prepare a directory for packaging
rm -rf /tmp/deb$$
mkdir -p /tmp/deb$$/$package-$version
cp -rf ../* /tmp/deb$$/$package-$version
rm -rf /tmp/deb$$/$package-$version/deb
tar czf /tmp/deb$$/$package-$version.tar.gz /tmp/deb-packman/$package-$version

cd  /tmp/deb$$/$package-$version

# we clean up the stub
rm -f debian/*.ex debian/*.EX

# we create all the files for the package
dh_make --copyright gpl --single

# a complete tree of directories is now ready. We update that thanks to information we already have.
cd $path
cp Makefile /tmp/deb$$/$package-$version
cp debian/control /tmp/deb$$/$package-$version/debian
cp debian/copyright /tmp/deb$$/$package-$version/debian
  
# we then really create the package
cd  /tmp/deb$$/$package-$version
dpkg-buildpackage -rfakeroot

# then we grab the created files :-)
cd .. 
cp -rf *.changes *.deb *.orig.tar.gz $path
cd $path
rm -rf /tmp/deb$$

echo "Package(s) created and available in the deb directory"


#!/bin/bash

BASEDIR=/local/debian
INCOMING=$BASEDIR/incoming

DIST_HOST=gforge.inria.fr
DIST_INCOMING=/home/groups/oscar/incoming
DIST_REPOS=/home/groups/oscar/htdocs/debian-test

MAIL_TO=oscar-package@lists.sourceforge.net
MAIL_FROM=oscar-package@lists.sourceforge.net

# If true, check for signatures
#SIGNATURES=true
SIGNATURES=false

DEBUG=false

#
# reprepro options
#
reprepro_opts=
if $SIGNATURES; then
    reprepro_opts=$reprepro_opts --ignore=brokensignatures
fi

#
# logging facilities
#

log_info() {
    echo -e "[INFO]   $*"
}

log_error() {
    echo -e "\033[31m[ERROR]  \033[0m$*"
}

log_success() {
    echo -e "\033[32m[SUCESS] \033[0m$*"
}

log_debug() {
    if $DEBUG; then
	echo -e "[DEBUG]  $*"
    fi
}

#
# Return first component of first distro in conf/distributions
#
get_default_component() {
    ret=`awk '/^Components:/ { print $2; exit }' $BASEDIR/conf/distributions`
    echo $ret
}

#
# Return value of a duet: tag::value in a file.
# If no tag, return default
# $1: file
# $2: tag name
# $3: default
#
get_tag_value() {
    file=$1
    tag=$2
    default=$3

    if egrep $tag'::' $file > /dev/null; then
	echo `sed -e 's/.*'$tag'::\(\S\+\).*/\1/' $file`
    else
	echo $default
    fi
}

#
# Check package with lintian
# $1: .changes file
#
check_package() {
    changefile=$1
    base=`basename $changefile .changes`
    
    log_info "Check $base with lintian"
    lintian $changefile > `dirname $changefile`/$base.lintian
    ret=$?
    if test $ret = 0; then
	log_success "Package $base is clean"
    else
	log_error "Package $base contains errors"
    fi
    return $ret
}

#
# Called if package has been accepted
#
package_success() {
    changefile=$1
    base=`basename $changefile .changes`
    dist=$2
    to=`sed '/^Maintainer: /!d; s/^Maintainer:\s\+//' < $changefile`

    reportfile=`dirname $changefile`/$base.lintian

    mailfile=/tmp/oscar.mail
    echo "Package $base has been accepted in distribution $dist on OSCAR repositories." > $mailfile
    echo '' >> $mailfile
    if [ `cat $reportfile | wc -l` -lt 1 ]; then
	echo 'lintian report was clean' >> $mailfile
    else
	echo 'lintian report was: ' >> $mailfile
	cat $reportfile >> $mailfile
    fi
    echo '' >> $mailfile
    echo 'Thank  you for your contribution to OSCAR.' >> $mailfile

    mail -a "From: $MAIL_FROM" \
	 -s "[oscar-repos] $base in $dist: ACCEPTED" \
         "$to" < $mailfile

    rm -f $mailfile
	 
    log_info "Package $base integrated into $dist"
}

#
# Called if package has not been accepted
#
package_error() {
    changefile=$1
    base=`basename $changefile .changes`
    dist=$2
    to=`sed '/^Maintainer: /!d; s/^Maintainer:\s\+//' < $changefile`

    reportfile=`dirname $changefile`/$base.lintian

    mailfile=/tmp/oscar.mail
    echo "Package $base inclusion in distribution $dist has been refused." > $mailfile
    echo '' >> $mailfile
    if [ `cat $reportfile | wc -l` -lt 1 ]; then
	echo 'lintian report was clean' >> $mailfile
    else
	echo 'lintian report was: ' >> $mailfile
	cat $reportfile >> $mailfile
    fi
    echo '' >> $mailfile
    echo 'Thank  you for your contribution to OSCAR.' >> $mailfile

    mail -a "From: $MAIL_FROM" \
	 -s "[oscar-repos] $base in $dist: REFUSED" \
         "$to" < $mailfile

    rm -f $mailfile

    log_error "Inclusion of $base in $dist has failed"
}

#
# rm_incoming: delete a file locally and on the distant
# incoming dir
# $1: file path, relative to incoming dir
#
rm_incoming() {
    file=$1
    log_debug "Remove $INCOMING/$file"
    rm $INCOMING/$file || return 1
    log_debug "Remove $DIST_HOST:$DIST_INCOMING/$file"
    ssh $DIST_HOST "rm $DIST_INCOMING/$file" || return 1
}

#
# Synchronize incoming dirs
#
log_info "Sync incoming/ dir"
rsync -av --exclude='*.rpm' $DIST_HOST:$DIST_INCOMING/ $INCOMING/

#
# Make sure we're in the base directory
#
cd $BASEDIR

#
# Get default values for dist and component
#
comp=`get_default_component`

#
# Get every distribution
#
for d in $INCOMING/*; do
    dist=`basename $d`
    #
    # See if we find any new packages.
    # If we found none, pass
    #
    found=`find $INCOMING/$dist -name '*.changes' | wc -l`
    if [ "$found" -ge 1 ]; then
	for i in $INCOMING/$dist/*.changes; do
	    base=`basename $i .changes`
	    
            # Import package
	    if check_package $i; then 
		log_info "Including $base into dist: $dist"
		reprepro -Vb . --comp $comp $reprepro_opts include $dist $i 
		ret=$?
		if test $ret = 0; then
		    package_success $i $dist
		else
		    package_error $i $dist
		fi
	    else
		package_error $i $dist
	    fi
	    
            # Delete the referenced files
	    for NAME in `awk '/BEGIN PGP SIGNATURE/ {files=0} 
                              files==1              {print $5} 
                              /^Files:/ {files=1}' $i`; do
        
		if [ -z "$NAME" ]; then
		    continue
		fi

                #
                #  Delete the referenced file
                #
		if [ -f "$INCOMING/$dist/$NAME" ]; then
		    rm_incoming "$dist/$NAME"  || exit 1
		fi
	    done

            # Delete the .changes file itself.
	    rm_incoming  $dist/$base.changes || exit 1
	    
	    # Delete the .lintian file
	    rm $INCOMING/$dist/$base.lintian || exit 1
	done
    fi
done

rsync -av --delete $BASEDIR/dists/ $DIST_HOST:$DIST_REPOS/dists/
rsync -av --delete $BASEDIR/pool/ $DIST_HOST:$DIST_REPOS/pool/

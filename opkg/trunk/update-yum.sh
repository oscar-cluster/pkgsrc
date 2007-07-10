#!/bin/bash

BASEDIR=/local/yum
INCOMING=$BASEDIR/incoming
POOL=$BASEDIR/pool

if [ -z "$DIST_USER" ]; then
    DIST_USER=$USER
fi
DIST_HOST=gforge.inria.fr
DIST_INCOMING=/home/groups/oscar/incoming
DIST_REPOS=/home/groups/oscar/htdocs/yum

MAIL_TO=oscar-package@lists.gforge.inria.fr

LOCK=/tmp/update-yum.lock

DEBUG=false

KEYRING=http://oscar.gforge.inria.fr/oscar-keyring.gpg

#
# createrepo options
#
createrepo_opts="--update --checkts"

#
# exit if an instance is already running
#
if [ -e $LOCK ]; then 
    echo "An instance of update-yum.sh is already running, exiting..."
    exit 0
fi
touch $LOCK

#
# Get keyring
#
wget -O ~/.gnupg/pubring.gpg $KEYRING

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
# Called if repository has been succesfully updated
#
package_success() {
    rpmfile=$1
    base=`basename $rpmfile .rpm`
    dist=$2
    to=`rpm -qp --qf "%{Packager}" $rpmfile`

    mailfile=/tmp/oscar.mail
    echo "Package $base has been accepted in distribution $dist on OSCAR repositories." > $mailfile
    echo '' >> $mailfile
    echo 'Thank  you for your contribution to OSCAR.' >> $mailfile

    log_info "Sending success notification to $to and $MAIL_TO"
    mail -s "$base in $dist: ACCEPTED" \
	-c "$MAIL_TO" \
	"$to" < $mailfile
    
    rm -f $mailfile
	 
    log_info "Package $base integrated into $dist"
}

#
# Called if error occured while updating repository
#
package_error() {
    rpm=$1
    base=`basename $rpmfile .rpm`
    dist=$2
    msg=$3
    to=`rpm -qp --qf "%{Packager}" $rpmfile`

    mailfile=/tmp/oscar.mail
    echo "Package $base inclusion in distribution $dist has been refused." > $mailfile
    if [ -n "$msg" ]; then
	echo "$msg" >> $mailfile
    fi
    echo '' >> $mailfile
    echo 'Thank  you for your contribution to OSCAR.' >> $mailfile

    log_info "Sending error notification to $to and $MAIL_TO"
    mail -s "$base in $dist: REFUSED" \
	-c "$MAIL_TO" \
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
    ssh $DIST_USER@$DIST_HOST "rm $DIST_INCOMING/$file" || return 1
}

#
# Synchronize incoming dirs
#
log_info "Sync incoming/ dir"
rsync -av \
    --exclude='*.deb' \
    --exclude='*.dsc' \
    --exclude='*.tar.gz' \
    --exclude='*.diff.gz' \
    --exclude='*.changes' \
    $DIST_USER@$DIST_HOST:$DIST_INCOMING/ $INCOMING/

#
# Make sure we're in the base directory
#
cd $BASEDIR
mkdir -p $POOL

#
# Get every distribution
#
for d in $INCOMING/*; do
    dist=`basename $d`
    #
    # See if we find any new packages.
    # If we found none, pass
    #
    found=`find $INCOMING/$dist -name '*.rpm' | wc -l`
    if [ "$found" -ge 1 ]; then
	mkdir -p $BASEDIR/dists/$dist

	for i in $INCOMING/$dist/*.rpm; do
	    base=`basename $i .rpm`
	    
            # Import package
	    log_info "Check package signature"
	    rpm --checksig $i || \
		package_error $i $dist "Package signature is incorrect".

	    log_info "Including $base into dist: $dist"
	    cp -f $i $BASEDIR/pool/
	    
	    arch=`echo $base | sed 's/.*\.\([^\.]*\)$/\1/'`
	    rpmdir=RPMS
	    if [ "$arch" = "src" ]; then
		arch=source
		rpmdir=SRPMS
	    fi
	    mkdir -p $BASEDIR/dists/$dist/$arch/$rpmdir
	    ( cd $BASEDIR/dists/$dist/$arch/$rpmdir && ln -fs ../../../../pool/$base.rpm $base.rpm )
	    package_success $i $dist

            # Update repository metadatas
	    log_info "Update repository metadatas"
	    createrepo $createrepo_opts $BASEDIR/dists/$dist/$arch/

	    rm_incoming $dist/$base.rpm
	done
    fi
done

rsync -av --delete $BASEDIR/dists/ $DIST_USER@$DIST_HOST:$DIST_REPOS/dists/
rsync -av --delete $BASEDIR/pool/ $DIST_USER@$DIST_HOST:$DIST_REPOS/pool/

#
# Remove lock
#
rm $LOCK

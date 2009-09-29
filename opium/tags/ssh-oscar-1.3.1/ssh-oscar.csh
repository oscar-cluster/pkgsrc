#!/bin/csh

set user=`whoami`
set home=`getent passwd | egrep "^$user\:" | awk -F: '{print $6}' | tail -1`
if ( "$user" == "nobody" ) then
    echo Not creating SSH keys for user $user
else if ( -d "$home" ) then
    set file=$home/.ssh/id_dsa
    set type=dsa
    if ( ! -r $file ) then
        echo generating ssh file $file ...
        ssh-keygen -t $type -N '' -f $file
    endif

    set file=$home/.ssh/identity
    set type=rsa1
    if ( ! -r $file ) then
        echo generating ssh file $file ...
        ssh-keygen -t $type -N '' -f $file
    endif

    set file=$home/.ssh/id_rsa
    set type=rsa
    if ( ! -r $file ) then
        echo generating ssh file $file ...
        ssh-keygen -t $type -N '' -f $file
    endif

    set id="`cat $home/.ssh/id_dsa.pub`"
    set file=$home/.ssh/authorized_keys2
    if ( ! -r $file ) then
	echo touch $file
	touch $file
	echo chmod 700 $file
	chmod 700 $file
    endif
#    egrep -qi "$id" $file
    set key=`echo $id | awk '{print $NF}'`
    egrep -qi "$key" $file
    if ( $? != 0 ) then
        echo adding id to ssh file $file
        echo $id >> $file
    endif

    set id="`cat $home/.ssh/identity.pub`"
    set file=$home/.ssh/authorized_keys
    if ( ! -r $file ) then
	echo touch $file
	touch $file
	echo chmod 700 $file
	chmod 700 $file
    endif
#    egrep -qi "$id" $file
    set key=`echo $id | awk '{print $NF}'`
    egrep -qi "$key" $file
    if ( $? != 0 ) then
        echo adding id to ssh file $file
        echo $id >> $file
    endif

    # echo chmod 600 $home/.ssh/authorized_keys*
    chmod 600 $home/.ssh/authorized_keys*

else
    echo cannot determine home directory of user $user
endif

#!/bin/sh
#
# Configuration manager simple utility
#
# Copyright (C) 2012, Florian Fainelli <florian@openwrt.org>
#			Xavier Carcelle <xavier.carcelle@gmail.com>

action=${1:-help}
shift 1

PERSIST_CACHE=/firmware/confmgr.cache
CONF_CACHE=/tmp/confmgr.cache

touch $CONF_CACHE

list_contains() {
	local var="$1"
	local str="$2"
	local val

	eval "val=\" \${$var} \""
	[ "${val%% $str *}" != "$val" ]
}

do_delete_one() {
	local key="$1"

	grep -v "$key" $CONF_CACHE > $CONF_CACHE.tmp
	fsync $CONF_CACHE.tmp
	# use rename to provide an atomic change
	mv $CONF_CACHE.tmp $CONF_CACHE
	fsync $CONF_CACHE
}

do_set() {
	local key
	local value
	local argc=$#

	while [ $argc != 0 ]; do
		key="$1"
		value="$2"

		# delete the previous key value
		do_delete_one $key

		# append the new key value
		echo "\"$key\"=\"$value\"" >> $CONF_CACHE
		fsync $CONF_CACHE

		# shift to the next 2 pair
		shift 2
		argc=$(($argc - 2))
	done
}

do_get() {
	local key="$1"
	local value

	[ -z "$key" ] && return 1

	value=$(grep $1 $CONF_CACHE | cut -d= -f2 | sed s/\"//g)
	echo -n $value
	return 0
}

#
# $1: source file
# $2: temporary file before rename
# $3: final file destination
do_fetch_store() {
	local src=$1
	local tmp_dst=$2
	local final_dst=$3

	cp $src $tmp_dst
	fsync $tmp_dst
	# use a rename to provide an atomic operation
	mv $tmp_dst $final_dst
}

do_store() {
	do_fetch_store "$CONF_CACHE" "$CONF_CACHE.persist" "$PERSIST_CACHE"
	return 0
}

do_fetch() {
	do_fetch_store "$PERSIST_CACHE" "$CONF_CACHE.persist" "$CONF_CACHE"
	return 0
}

do_delete() {
	local key
	local argc=$#

	while [ $argc != 0 ]; do
		key="$1"

		# delete one value at a time
		do_delete_one $key

		# shift to next argument
		shift 1
		argc=$(($argc - 1))
	done
}

do_show() {
	cat $CONF_CACHE
}

do_help() {
	cat <<EOF
Available commands:
	set <key1> <value1> .. <keyN> <valueN>
	get <key>
	fetch
	store
	delete <key1> .. <keyN>
	show
EOF
	exit 1
}

ALL_COMMANDS="get set fetch store delete show help"
list_contains ALL_COMMANDS "$action" || action="help"
do_$action "$@"

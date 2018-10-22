#!/bin/sh
for i in $(cat .gitignore);
do
	[ -e $i ] && rm -rf $i;
done

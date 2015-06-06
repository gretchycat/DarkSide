#!/bin/bash
rm *.pbw
for i in appinfo.[A-Z]*.json
do
	FACE=`echo $i|cut -f2 -d'.'`
	ln -sf $i appinfo.json
	pebble clean &&pebble build &&cp build/Darkside.pbw $FACE.pbw
done


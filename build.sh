#!/bin/bash
for i in appinfo.[A-Z]*.json
do
	FACE=`echo $i|cut -f2 -d'.'`
	pebble clean &&pebble build &&cp build/Darkside.pbw $FACE.pbw
done


#!/bin/bash
rm *.pbw
for i in appinfo.[A-Z]*.json
do
	FACE=`echo $i|cut -f2 -d'.'`
	UUID=`cat ${FACE}.uuid`
	cat appinfo.json.template|sed -e "s/FACENAME/$FACE/g"|sed -e "s/UUID/$UUID/g">appinfo.${FACE}.json
	ln -sf $i appinfo.json
	echo "#include \"defines/${FACE}.h\"">src/defines.h
	pebble clean &&pebble build &&cp build/watchface.pbw $FACE.pbw
done


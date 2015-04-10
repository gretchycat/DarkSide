#!/bin/bash
cd resources/fonts/
ln -sf DarkSide $1
cd ../images
cp -av DarkSide $1
cd ../..
cat appinfo.template.json|sed -e "s/FACENAME/$1/g">appinfo.${1}.json
ln -sf appinfo.${1}.json appinfo.json
convert ~/Downloads/$1-icon.png -dither FloydSteinberg -remap resources/images/6bit.png resources/images/$1/color/darkside-icon.png
convert ~/Downloads/${1}.png -dither FloydSteinberg -remap resources/images/6bit.png resources/images/$1/color/darkside.png
convert ~/Downloads/$1-icon.png -dither FloydSteinberg -brightness-contrast +10 -monochrome resources/images/$1/darkside-icon.png
convert ~/Downloads/${1}.png -dither FloydSteinberg -contrast +10 -monochrome resources/images/$1/darkside.png
pebble clean
pebble build
git add resources/fonts/$1
git add respource/images/$1
git add appinfo.${1}.json
git commit . -m "adding $1 watchface"
pebble install --emulator basalt


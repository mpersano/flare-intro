#!/bin/bash

FONT=./DejaVuSansMono.ttf

SMALL_POINTSIZE=22
MEDIUM_POINTSIZE=48
LARGE_POINTSIZE=64

gen_image()
{
	convert \
		-background black \
		-fill white \
		-font ${FONT} \
		-size $1 \
		-pointsize $2 \
		-gravity center \
		label:$3 \
		$4
}

for i in x 7 d f; do
	gen_image 64x96 ${LARGE_POINTSIZE} $i $i.png
done

gen_image 96x32 ${SMALL_POINTSIZE} "code" code.png
gen_image 192x64 ${MEDIUM_POINTSIZE} "fuse" fuse.png
gen_image 96x32 ${SMALL_POINTSIZE} "music" music.png
gen_image 320x64 ${MEDIUM_POINTSIZE} "e:mortal" emortal.png

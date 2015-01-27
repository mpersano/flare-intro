#!/bin/bash

FONT=./DejaVuSansMono.ttf

SMALL_SIZE=96x32
SMALL_POINTSIZE=22

MEDIUM_SIZE=256x64
MEDIUM_POINTSIZE=48

convert \
	-background black \
	-fill white \
	-font ${FONT} \
	-size ${SMALL_SIZE} \
	-pointsize ${SMALL_POINTSIZE} \
	-gravity center \
	label:"code" \
	1.png

convert \
	-background black \
	-fill white \
	-font ${FONT} \
	-size 192x64 \
	-pointsize ${MEDIUM_POINTSIZE} \
	-gravity center \
	label:"fuse" \
	2.png

convert \
	-background black \
	-fill white \
	-font ${FONT} \
	-size ${SMALL_SIZE} \
	-pointsize ${SMALL_POINTSIZE} \
	-gravity center \
	label:"music" \
	3.png

convert \
	-background black \
	-fill white \
	-font ${FONT} \
	-size 320x64 \
	-pointsize ${MEDIUM_POINTSIZE} \
	-gravity center \
	label:"e:mortal" \
	4.png

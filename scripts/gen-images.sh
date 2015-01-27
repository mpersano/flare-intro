#!/bin/bash

FONT=./DejaVuSansMono.ttf

SMALL_SIZE=96x32
SMALL_POINTSIZE=22

MEDIUM_SIZE=192x64
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
	-size ${MEDIUM_SIZE} \
	-pointsize ${MEDIUM_POINTSIZE} \
	-gravity center \
	label:"fuse" \
	2.png

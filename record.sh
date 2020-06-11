#!/bin/sh

arecord -c 1 -f S16_LE -r 8000 -t wav --duration=5 $1

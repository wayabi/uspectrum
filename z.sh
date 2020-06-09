#!/bin/sh

#device='Sound Blaster X-Fi Go! Pro, USB Audio (CARD=MIC0,DEV=0)'
device='USB Audio Device, USB Audio (CARD=MIC0,DEV=0)'

./a.out \
--log_level trace \
--log_file 0 \
--speaker "" \
--size_buf_fft 256 \
--wav ./a.wav \
--path_out_spectrum ./out/0

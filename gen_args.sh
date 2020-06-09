#!/bin/sh

gen_arg_parse_h \
	log_level,string,trace \
	log_file,int,0 \
	wav,string, \
	size_buf_fft,int,man \
	offset_fft,int,0 \
	speaker,string, \
	path_out_spectrum,string,man \
	> src/arg_parse.h

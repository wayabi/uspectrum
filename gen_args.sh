#!/bin/sh

gen_arg_parse_h \
	log_level,string,trace \
	log_file,int,0 \
	speaker,string, \
	> src/arg_parse.h

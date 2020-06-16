#!/bin/sh

gen_arg_parse_h \
	f_in,string,man \
	f_out,string,man \
	offset,int,man \
	> src/arg_parse.h

% This is a script file for compiling the mex versions of the Steerable
% Pyramid Tools.
% 
% Usage:>> compilePyrTools
%
% Tested for gcc and lcc.
%
% Rob Young, 9/08

mex -largeArrayDims upConv.c convolve.c wrap.c edges.c
mex -largeArrayDims corrDn.c convolve.c wrap.c edges.c
mex -largeArrayDims histo.c
%mex innerProd.c
mex -largeArrayDims pointOp.c
mex -largeArrayDims range2.c

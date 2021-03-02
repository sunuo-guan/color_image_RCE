clc
clear
close all
im = imread('Lena.bmp');

addpath('libsvm-mat-3.0-1');
addpath('matlabPyrTools');

score = C_DIIVINE_image_score(im)


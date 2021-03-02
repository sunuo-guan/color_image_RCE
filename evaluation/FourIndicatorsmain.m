clc;
clear;
close all;

X=imread('C:\Users\zhujiu\Desktop\712image\1.bmp\1.bmp');
Y=imread('C:\Users\zhujiu\Desktop\712image\1.bmp\image-foreground\2.bmp');
[M,N]=size(X);
histX=imhist(X);
histY=imhist(Y);%obtain the histogram of the original and the enhanced image.
pdfX=histX/(M*N);
pdfY=histY/(M*N);%obtain the probability distribution function
ree=REE(pdfX,pdfY)
[rce,rmbe]=RCE(pdfX,pdfY,X,Y)
rss=RSS(X,Y)
[mssim ssim_map] = ssim_index(X, Y);
mssim
psnr=psnr(X,Y)
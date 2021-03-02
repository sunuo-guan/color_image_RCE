clc;
clear;
close all;

ree = zeros(64,1);
rce = zeros(64,1);
rmbe = zeros(64,1);
rss = zeros(64,1);
mssim = zeros(64,1);
Psnr = zeros(64,1);

X=imread('E:\experiment\bows2-1c\10.bmp');
XR = X(:,:,1);
XG = X(:,:,2);
XB = X(:,:,1);

[M,N]=size(XR);
histXR=imhist(XR);
pdfXR=histXR/(M*N);

s1_1 = 'E:\experiment\0807\baboon\fore\';
s1_3 = '.bmp';

for n1 = 1: 5
    s1_2 = int2str(n1);
    s11 = [s1_1 s1_2];
    s12 = [s11 s1_3];
    Y = imread(s12);
    histY=imhist(Y);
    pdfY=histY/(M*N);
    ree(n1,1) = REE(pdfX,pdfY);
    [rce(n1,1),rmbe(n1,1)]=RCE(pdfX,pdfY,X,Y);
    rss(n1,1)=RSS(X,Y);
    [mssim(n1,1), ssim_map] = ssim_index(X, Y);
    Psnr(n1,1)=psnr(X,Y);
end
    


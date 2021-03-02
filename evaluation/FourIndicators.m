
X=imread('D:\Acer D\2015-NUST\RDHwCEfMI-Demo\medicalt.bmp');
Y=imread('D:\Acer D\2015-NUST\RDHwCEfMI-Demo\medicalt-2106dB-445bpp.bmp');
[M,N]=size(X);
histX=imhist(X);
histY=imhist(Y);%obtain the histogram of the original and the enhanced image.
pdfX=histX/(M*N);
pdfY=histY/(M*N);%obtain the probability distribution function
ree=REE(pdfX,pdfY)
[rce,rmbe]=RCE(pdfX,pdfY,X,Y)
rss=RSS(X,Y)


% pv=psnr(X,Y)
% [mssim, ssim_map] = ssim_index(X,Y);
% mssim
% Z=imadjust(X);
% [nssim, nssim_map] = ssim_index(X,Z);
% nssim
% Z=histeq(X);
% [nssim, nssim_map] = ssim_index(X,Z);
% nssim
% Z=adapthisteq(X);
% [nssim, nssim_map] = ssim_index(X,Z);
% nssim
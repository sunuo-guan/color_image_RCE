X=imread('E:\experiment\¥Ú‘”\9 CT images\medicalt.bmp');
Y=imread('E:\experiment\¥Ú‘”\9 CT images\medicalt-S50-R05-1916dB-494bpp-5848-5992.bmp');
[M,N]=size(X);
histX=imhist(X);
histY=imhist(Y);%obtain the histogram of the original and the enhanced image.
pdfX=histX/(M*N);
pdfY=histY/(M*N);%obtain the probability distribution function
ree=REE(pdfX,pdfY);
[rce,rmbe]=RCE(pdfX,pdfY,X,Y);
rss=RSS(X,Y);
% a=X(512,1)
% a=X(512,2)
% a=X(512,3)
% a=X(512,4)
% a=X(1,511)
% a=X(1,512)
% a=Y(512,1)
% a=Y(512,2)
% a=Y(512,3)
% a=Y(512,4)
% a=Y(1,511)
% a=Y(1,512)
pv=psnr(X,Y);
[mssim, ssim_map] = ssim_index(X,Y);
mssim
% Z=imadjust(X);
% [nssim, nssim_map] = ssim_index(X,Z);
% nssim
% Z=histeq(X);
% [nssim, nssim_map] = ssim_index(X,Z);
% nssim
% Z=adapthisteq(X);
% [nssim, nssim_map] = ssim_index(X,Z);
% nssim
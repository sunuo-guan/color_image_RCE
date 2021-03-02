clc;
clear;
close all;

ree = zeros(1,3);
rce = zeros(1,3);
rmbe = zeros(1,3);
rss = zeros(1,3);
mssim = zeros(1,3);

image_ree_means = zeros(64,24);
image_rce_means = zeros(64,24);
image_rmbe_means = zeros(64,24);
image_rss_means = zeros(64,24);
image_ssim_means = zeros(64,24);
image_ssim2_means = zeros(64,24);
image_psnr = zeros(64,24);

for n1 = 1: 24
    %X = imread(['E:\experiment\task\result\sipi-origin\sipi',num2str(n1),'.bmp']);
    X = imread(['E:\experiment\task\result-reversible\color-origin\kodim',num2str(n1),'.bmp']);
    %X = imread('E:\experiment\task\result\color-origin\kodim24.bmp');
    %n2=50;
    for n2 = 1:64
    %while n2<60
        %address = ['E:\experiment\task\result-reversible\uce-newPre\kodak\kodim',num2str(n1),'_',num2str(n2),'.bmp'];
        %address = ['E:\experiment\task\result-reversible\uce-newPre\sipi\sipi',num2str(n1),'_',num2str(n2),'.bmp'];
        address = ['E:\experiment\task\result-reversible\huePreservation\kodak-new\kodim',num2str(n1),'_',num2str(n2),'.bmp'];
        %address = 'E:\experiment\task\result-reversible\color-origin\kodim24-30.bmp';
        if exist(address,'file') == 0 
            %n2 = n2+10;
            continue
        end

        Y = imread(address);

        XR = X(:,:,1);
        XG = X(:,:,2);
        XB = X(:,:,3);

        YR = Y(:,:,1);
        YG = Y(:,:,2);
        YB = Y(:,:,3);

        [M,N]=size(XR);
        histXR=imhist(XR);
        pdfXR=histXR/(M*N);

        [M,N]=size(XG);
        histXG=imhist(XG);
        pdfXG=histXG/(M*N);

        [M,N]=size(XB);
        histXB=imhist(XB);
        pdfXB=histXB/(M*N);

        [M,N]=size(YR);
        histYR=imhist(YR);
        pdfYR=histYR/(M*N);

        [M,N]=size(YG);
        histYG=imhist(YG);
        pdfYG=histYG/(M*N);

        [M,N]=size(YB);
        histYB=imhist(YB);
        pdfYB=histYB/(M*N);

        ree(1,1) = REE(pdfXR,pdfYR);
        ree(1,2) = REE(pdfXG,pdfYG);
        ree(1,3) = REE(pdfXB,pdfYB);
        image_ree_means(n2,n1) = mean(ree);
        
        [rce(1,1),rmbe(1,1)]=RCE(pdfXR,pdfYR,XR,YR);
        [rce(1,2),rmbe(1,2)]=RCE(pdfXG,pdfYG,XG,YG);
        [rce(1,3),rmbe(1,3)]=RCE(pdfXB,pdfYB,XB,YB);
        image_rce_means(n2,n1) = mean(rce);
        image_rmbe_means(n2,n1) = mean(rmbe);
        
        rss(1,1)=RSS(XR,YR);
        rss(1,2)=RSS(XG,YG);
        rss(1,3)=RSS(XB,YB);
        image_rss_means(n2,n1) = mean(rss);
        
        [mssim(1,1), ssim_mapR] = ssim_index(XR, YR);
        [mssim(1,2), ssim_mapG] = ssim_index(XG, YG);
        [mssim(1,3), ssim_mapB] = ssim_index(XB, YB);
        image_ssim_means(n2,n1) = mean(mssim);
        
        %[image_ssim2_means(n2,n1), ssim_map] = ssim(X,Y);
        
        image_psnr(n2,n1) = psnr(X,Y);
        
        disp(n2);
        %n2 = n2+10;
    end
    disp(n1);
end
    



%==========================================================================
% 1) Please cite the paper (K. Gu, G. Zhai, X. Yang, and W. Zhang, "Using 
% free energy principle for blind image quality assessment," IEEE Trans. 
% Multimedia, vol. 17, no. 1, pp. 50-63, Jan. 2015.)
% 2) If any question, please contact me through guke.doctor@gmail.com; 
% gukesjtuee@gmail.com. 
% 3) Welcome to cooperation, and I am very willing to share my experience.
%==========================================================================

clear;
clc;

%I = imread('img.png');
I = imread('D:\Acer D\CSVT\JVCIR images\hill\1.bmp');
score = nferm_index(I);

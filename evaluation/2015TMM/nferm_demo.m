
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
nferm_A = zeros(64,6);
nferm_G = zeros(64,6);
nferm_J = zeros(64,6);
nferm_N = zeros(64,6);

for imgnum =3:5
for numt = 51:64

%I = imread(['D:\downloads\NIH\ta\bmp30_1024\m1\',num2str(numt),'-1.bmp']);
%I = imread(['D:\downloads\NIH\ta\bmp30_1024\Fin\JPCLN',num2str(numt,'%03d'),'_O1_0015.bmp']);
I = imread(['D:\bpp\JPCLN003_A_bpp_58\JPCLN00',num2str(imgnum),'_A_bpp_',num2str(numt),'.bmp']);
score = nferm_index(I);
if numel(score) == 0
    score = -1;
end
nferm_N(numt,imgnum) = score;
disp(numt)
end
end

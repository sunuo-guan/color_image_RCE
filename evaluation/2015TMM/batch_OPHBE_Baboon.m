
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

scoreOPHBEbaboon = zeros(64,1);

s1_1 = 'D:\CSVT\OPHBE images\baboon\';
s1_3 = '.bmp';
for n1 = 1: 64
    s1_2 = int2str(n1);
    s11 = [s1_1 s1_2];
    s12 = [s11 s1_3];
    img = imread(s12);
scoreOPHBEbaboon(n1,1) = nferm_index(img);
end


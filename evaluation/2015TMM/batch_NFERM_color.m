 
%==========================================================================
% 1) Please cite the paper (K. Gu, G. Zhai, X. Yang, and W. Zhang, "Using 
% free energy principle for blind image quality assessment," IEEE Trans. 
% Multimedia, vol. 17, no. 1, pp. 50-63, Jan. 2015.)
% 2) If any question, please contact me through guke.doctor@gmail.com; 
% gukesjtuee@gmail.com. 
% 3) Welcome to cooperation, and I am very willing to share my experience.
%==========================================================================

%clear;
%clc;

nferm_A = zeros(64,24);
nferm_G = zeros(64,24);
nferm_J = zeros(64,24);
nferm_N = zeros(64,24);

for imgnum =1:24
    for numt = 1:64
        address = ['E:\experiment\task\result\recovery\kodim',num2str(imgnum),'\kodim',num2str(imgnum),'_',num2str(numt),'.bmp'];
        if exist(address,'file') == 0 
            continue
        end
    I = imread(address);
    %I = imread(['D:\bpp\30\JPCLN',num2str(numt,'%03d'),'_G_P3_BG1_0015_bpp_30.bmp']);
    score = nferm_index(I);
    if numel(score) == 0
        score = -1;
    end
    nferm_N(numt,imgnum) = score;
    %nferm_N(numt) = score;
    disp(numt)
    end
    disp(imgnum)
end
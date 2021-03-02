scoreEntropy = zeros(64,24);


for n1 = 1:24
    for n2 = 1:64
        %address = ['E:\experiment\task\result\sipi-minS-pi\sipi',num2str(n1),'_',num2str(n2),'.bmp'];
        %address = ['E:\experiment\task\result\color-3-pi\kodim',num2str(n1),'_',num2str(n2),'.bmp'];
        address = ['E:\experiment\task\result-reversible\uce-newPre\kodak\kodim',num2str(n1),'_',num2str(n2),'.bmp'];
        %address = ['E:\experiment\task\result-reversible\uce-newPre\sipi\sipi',num2str(n1),'_',num2str(n2),'.bmp'];
        %address = ['E:\experiment\task\result\color-origin\kodim',num2str(n1),'.bmp'];
        if exist(address,'file') == 0 
            continue
        end
%        sum=0;
        I = imread(address);
        %I_gray = rgb2gray(I);
        %score = entropy(I_gray);
        score = yentropy(I);
%         XR = I(:,:,1);
%         XG = I(:,:,2);
%         XB = I(:,:,3);
%         
%         score = entropy(XR);
%         sum=score+sum;
%         score = entropy(XG);
%         sum=score+sum;
%         score = entropy(XB);
%         sum=score+sum;
%         sum=sum/3;
        if numel(score) == 0
            score = -1;
        end       
        scoreEntropy(n2,n1) = score;        
        disp(n2)
    end
end
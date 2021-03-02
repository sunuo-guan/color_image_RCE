clear;
clc;

A = zeros(1,3);
B = zeros(1,3);
image_size = zeros(1,3);
image_deltaE_means = zeros(64,24);

for n1 = 1:24
    standard = imread(['E:\experiment\task\result-reversible\color-origin\kodim',num2str(n1),'.bmp']);
    %standard = imread(['E:\experiment\task\result\sipi-origin\sipi',num2str(n1),'.bmp']);
    standard = rgb2lab(standard);
    image_size = size(standard);
    image_deltaE = zeros(image_size(1,1),image_size(1,2));    
    %n2=50;
    for n2 = 1:64
    %while n2<60
        %address = ['E:\experiment\task\result-reversible\hsPreservation\sipi-recovery\sipi',num2str(n1),'_',num2str(n2),'.bmp'];
        address = ['E:\experiment\task\result-reversible\huePreservation\kodak-new\kodim',num2str(n1),'_',num2str(n2),'.bmp'];
        %address = 'E:\experiment\task\result-reversible\color-origin\kodim7-40.bmp';
        if exist(address,'file') == 0 
            %n2=n2+10;
            continue
        end
        sample = imread(address);
        sample = rgb2lab(sample);
        for m1 =1:image_size(1,1)
            for m2 = 1:image_size(1,2)
            A(1,:) = standard(m1,m2,:);
            B(1,:) = sample(m1,m2,:);
            image_deltaE(m1,m2) = deltaE2000(A,B);
            end
        end
        image_deltaE_means(n2,n1) = mean2(image_deltaE);
        disp(n2);
        %n2=n2+10;
    end
    disp(n1);
end


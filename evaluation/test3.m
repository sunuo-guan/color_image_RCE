clc;
clear;

Z = imread('C:\Users\fhxzh\Desktop\image color  RDH with CE\图集\kodim彩色bmp\kodim23.bmp');
I = imread('C:\Users\fhxzh\Desktop\image color  RDH with CE\图集\kodim彩色bmp\kodim23-re.bmp');
%比较两个图像是否一样
if(Z==I)
    A = 0;
else
    A = 1;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%[count,x] = imhist(I);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% X = double(X);
% Y = double(Y);
% Z = double(Z);
% % 
%  [m,n,o]=size(X);
% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% XR = X(:,:,1);
% XG = X(:,:,2);
% XB = X(:,:,3);
%     
% YR = Y(:,:,1);
% YG = Y(:,:,2);
% YB = Y(:,:,3);
%     
% ZR = Z(:,:,1);
% ZG = Z(:,:,2);
% ZB = Z(:,:,3);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% num = 0;
% for n1 = 1:m
%     for n2 = 1:n
%         if(ZR(n1,n2)==255 || ZG(n1,n2)==255||ZB(n1,n2)==255)
%             num = num + 1;
%             en(num,1)=ZR(n1,n2);
%             en(num,2)=ZG(n1,n2);
%             en(num,3)=ZB(n1,n2);
%             ori(num,1)=YR(n1,n2);
%             ori(num,2)=YG(n1,n2);
%             ori(num,3)=YB(n1,n2);
%             record(num,1) = n1;
%             record(num,2) = n2;
%         end
%     end
% end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% num = 0;
% for n1 = 1:m
%     for n2 = 1:n
%         if(YR(n1,n2)<80 || YG(n1,n2)<80||YB(n1,n2)<80)
%             num = num + 1;
%             en(num,1)=YR(n1,n2);
%             en(num,2)=YG(n1,n2);
%             en(num,3)=YB(n1,n2);
%         end
%     end
% end

% image_ssim_means = zeros(64,24);
% image_ssim2_means = zeros(64,24);
% 
% [image_ssim_means(1,1), ssim_map] = ssim_index(Z, Y);
% [image_ssim2_means(1,1), ssim_map2] = ssim(Z,Y);

% XR = X(:,:,1);
% XG = X(:,:,2);
% XB = X(:,:,3);
%     
% YR = Y(:,:,1);
% YG = Y(:,:,2);
% YB = Y(:,:,3);
% 
% [m,n,o]=size(X);
% i=1;
% for n1 = 1:m
%     for n2 = 1:n
%         if(XR(n1,n2) ~= YR(n1,n2) || XG(n1,n2) ~= YG(n1,n2) || XB(n1,n2) ~= YB(n1,n2))
%             B(i,1)=XR(n1,n2);
%             B(i,2)=XG(n1,n2);
%             B(i,3)=XB(n1,n2);
%             C(i,1)=YR(n1,n2);
%             C(i,2)=YG(n1,n2);
%             C(i,3)=YB(n1,n2);
%             i=i+1;
%         end
%     end
% end


% C = 0;
% 
% if A == B
%     C = 1;
% end

% XR = X(:,:,1);
% XG = X(:,:,2);
% XB = X(:,:,3);
%     
% YR = Y(:,:,1);
% YG = Y(:,:,2);
% YB = Y(:,:,3);
% 
% ZR = Z(:,:,1);
% ZG = Z(:,:,2);
% ZB = Z(:,:,3);
% 
% [m,n,o]=size(Y);
% 
% for width = m
%     for height = n
%         
%     end
% end


Y = imread('E:\experiment\task\result\color-origin\kodim15.bmp');
Z = imread('E:\experiment\task\result\recovery\15-50.bmp');

Y = double(Y);
Z = double(Z);

[m,n,o]=size(Y);

YR = Y(:,:,1);
YG = Y(:,:,2);
YB = Y(:,:,3);
    
ZR = Z(:,:,1);
ZG = Z(:,:,2);
ZB = Z(:,:,3);

M = zeros(m,n,o);
M = uint8(M);

X = zeros(m,n,o);
X = double(X);

XR = X(:,:,1);
XG = X(:,:,2);
XB = X(:,:,3);
MC = M(:,:,1);
MM = M(:,:,2);
MY = M(:,:,3);

sum = 0;
sum2 = 0;
rgb = 0;
rgben = 0;

for n1 = 1:m
    for n2 = 1:n
        rgb = YR(n1,n2)+YG(n1,n2)+YB(n1,n2);
        rgben = ZR(n1,n2)+ZG(n1,n2)+ZB(n1,n2);
        if(rgben <= rgb)
            XR(n1,n2) = rgben / rgb * YR(n1,n2);
            XG(n1,n2) = rgben / rgb * YG(n1,n2);
            XG(n1,n2) = rgben / rgb * YB(n1,n2);
%             MR(n1,n2) = XR(n1,n2) * rgb /rgben;
%             MG(n1,n2) = XG(n1,n2) * rgb /rgben;
%             MB(n1,n2) = XB(n1,n2) * rgb /rgben;
%             sum = sum + 1;
%             if (MR(n1,n2) ~= YR(n1,n2) || MG(n1,n2) ~= YG(n1,n2) || MG(n1,n2) ~= YG(n1,n2))
%                 sum2 = sum2 + 1;
%             end
        end
        if(rgben > rgb)
            MC(n1,n2) = 255 - YR(n1,n2);
            MM(n1,n2) = 255 - YG(n1,n2);
            MY(n1,n2) = 255 - YB(n1,n2);
            lcmy = MC(n1,n2) + MM(n1,n2) + MY(n1,n2);
            gcmy = 3 * 255 - rgben;
            XR(n1,n2) = 255 - gcmy / lcmy * MC(n1,n2);
            XG(n1,n2) = 255 - gcmy / lcmy * MM(n1,n2);
            XB(n1,n2) = 255 - gcmy / lcmy * MY(n1,n2);
        end        
    end
end

result(:,:,1) = XR;
result(:,:,2) = XG;
result(:,:,3) = XB;
figure,imshow(result);
            
            
    
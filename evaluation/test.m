clc;
clear;

X = imread('E:\experiment\task\result-reversible\color-origin\kodim1.bmp');
Y = imread('E:\experiment\task\result-reversible\color-origin\kodim1-re.bmp');
Z = imread('E:\experiment\task\result-reversible\color-origin\kodim14.bmp');

X = double(X);
Y = double(Y);
Z = double(Z);

[m,n,o]=size(X);

    XR = X(:,:,1);
    XG = X(:,:,2);
    XB = X(:,:,3);
    
    YR = Y(:,:,1);
    YG = Y(:,:,2);
    YB = Y(:,:,3);
    
    ZR = Z(:,:,1);
    ZG = Z(:,:,2);
    ZB = Z(:,:,3);
    ori = zeros();
    re = zeros();
    en = zeros();
    maxError=0;
    max = 0;
    num=0;
    min=0;
    median=0;
    RR = 0;
    GG = 0;
    BB = 0;
    sum = 0;
    xx=0;
    yy=0;
    errorMax=0;
    
for n1 = 1:m
    for n2 = 1:n
        A = [XR(n1,n2),XG(n1,n2),XB(n1,n2)];
        B = [YR(n1,n2),YG(n1,n2),YB(n1,n2)];
        A=sort(A,'descend');
        B=sort(B,'descend');
        error=abs(A(1)-A(3));
        if(error>errorMax)
            errorMax=error;
            maxn1=n1;
            maxn2=n2;
            maxError=A;
        end
%             if(XR(n1,n2)>=XG(n1,n2) && XR(n1,n2)>=XB(n1,n2))
%                 max = max + 1;
%             end
            if (XR(n1,n2) ~= YR(n1,n2) || XG(n1,n2) ~= YG(n1,n2) || XB(n1,n2) ~= YB(n1,n2))
                %if(ZR(n1,n2) == 31 && ZG(n1,n2) == 25 && ZB(n1,n2) == 28)
                if(XR(n1,n2) - YR(n1,n2)>maxError)
                    maxError=XR(n1,n2) - YR(n1,n2);
                end
                if(XG(n1,n2) - YG(n1,n2)>maxError)
                    maxError=XG(n1,n2) - YG(n1,n2);
                end
                if(XR(n1,n2) - YR(n1,n2)>maxError)
                    maxError=XB(n1,n2) - YB(n1,n2);
                end
                    num = num + 1;
                    re(num,1)=XR(n1,n2);
                    re(num,2)=XG(n1,n2);
                    re(num,3)=XB(n1,n2);
                    ori(num,1)=YR(n1,n2);
                    ori(num,2)=YG(n1,n2);
                    ori(num,3)=YB(n1,n2);
                    en(num,1)=ZR(n1,n2);
                    en(num,2)=ZG(n1,n2);
                    en(num,3)=ZB(n1,n2);
                %end
            end
%             if (abs(A(2) - B(2))>0)
%                 xx=n1;
%                 yy=n2;
%                 median = median + 1;
%             end
%             if (abs(A(3) - B(3))>0)
%                 min = min + 1;
%             end
%             if (XR(n1,n2) ~= YR(n1,n2))
%                 RR = RR+1;
%             end
%             if (XG(n1,n2) ~= YG(n1,n2))
%                 GG = GG + 1;
%             end
%             if (XB(n1,n2) ~= YB(n1,n2))
%                 BB = BB + 1;
%             end

        sumX = XR(n1,n2)+XG(n1,n2)+XB(n1,n2);
        sumY = YR(n1,n2)+YG(n1,n2)+YB(n1,n2);
        
        if (sumX > 255)
%             C = [XR(n1,n2),XG(n1,n2),XB(n1,n2)];
%             D = [YR(n1,n2),YG(n1,n2),YB(n1,n2)];
%             E = n1;
%             F = n2;

            sum = sum + 1;
        end
    end
end



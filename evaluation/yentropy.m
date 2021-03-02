function entr=yentropy(a)
a=uint8(a); %这里a为8位的单色图像或24为的RGB彩色图像
[m, n, l]=size(a);
entr=0;
for k=1:l
    hi=zeros(1,256);
    for i=1:m
        for j=1:n
            hi(a(i,j,k)+1)=hi(a(i,j,k)+1)+1; %求每种值的在图像中出现的次数
        end
    end
    hi=sort(hi,'descend');
    hi=hi./m./n; %求概率
    en=0.0;
    for i=1:256
        if hi(i)>0
            en=en-hi(i).*log2(hi(i)); %概率不为0 累加求熵
        else
            i=257; %否则停止
        end
    end
    entr=entr+en;
end
entr=entr/l; %当l=1时a为单色图像；当l=3时a为彩色图像，三个页面的熵平均
end
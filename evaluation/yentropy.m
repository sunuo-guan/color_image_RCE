function entr=yentropy(a)
a=uint8(a); %����aΪ8λ�ĵ�ɫͼ���24Ϊ��RGB��ɫͼ��
[m, n, l]=size(a);
entr=0;
for k=1:l
    hi=zeros(1,256);
    for i=1:m
        for j=1:n
            hi(a(i,j,k)+1)=hi(a(i,j,k)+1)+1; %��ÿ��ֵ����ͼ���г��ֵĴ���
        end
    end
    hi=sort(hi,'descend');
    hi=hi./m./n; %�����
    en=0.0;
    for i=1:256
        if hi(i)>0
            en=en-hi(i).*log2(hi(i)); %���ʲ�Ϊ0 �ۼ�����
        else
            i=257; %����ֹͣ
        end
    end
    entr=entr+en;
end
entr=entr/l; %��l=1ʱaΪ��ɫͼ�񣻵�l=3ʱaΪ��ɫͼ������ҳ�����ƽ��
end
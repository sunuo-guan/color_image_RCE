clc
clear
close all


list1 = dir('D:\CSVT\test image set 2\*.bmp');

% dircell=struct2cell(dirs)' ;    % �ṹ��(struct)ת����Ԫ������(cell)��ת��һ�������ļ����������С�
% filenames=dircell(:,1)   

list1 = list1(3:size(list1,1),1);

s1_1 = 'D:\CSVT\test image set 2\';

addpath('libsvm-mat-3.0-1');
addpath('matlabPyrTools');

i=1;
scorese=zeros(1,size(list1,1));

for iter =1:size(list1,1)
   s1_2=list1(iter).name;
   s1 = [s1_1 s1_2];
   [a,b,c]=fileparts(s1);

   im = imread(s1);
   scorese(1,iter) = C_DIIVINE_image_score(im);

end


for iter =1:size(list1,1)
   s1_3=list1(iter).name;
   sc=scorese(1,iter);
   
   s1_3
   sc

end



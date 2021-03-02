clc
clear
close all


list1 = dir('D:\CSVT\test image set 1\*.bmp');

% dircell=struct2cell(dirs)' ;    % 结构体(struct)转换成元胞类型(cell)，转置一下是让文件名按列排列。
% filenames=dircell(:,1)   

list1 = list1(3:size(list1,1),1);

s1_1 = 'D:\CSVT\test image set 1\';

addpath('libsvm-mat-3.0-1');
addpath('matlabPyrTools');

i=1;
scores=zeros(1,size(list1,1));

for iter =1:size(list1,1)
   s1_2=list1(iter).name;
   s1 = [s1_1 s1_2];
   [a,b,c]=fileparts(s1);

   im = imread(s1);
   scores(1,iter) = C_DIIVINE_image_score(im);

end


for iter =1:size(list1,1)
   s1_3=list1(iter).name;
   sc=scores(1,iter);
   
   s1_3
   sc

end



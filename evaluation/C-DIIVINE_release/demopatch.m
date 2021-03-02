clc
clear
close all


list1 = dir('D:\CSVT\test image set 1');
list1 = list1(3:size(list1,1),1);

s1_1 = 'D:\CSVT\test image set 1\';

addpath('libsvm-mat-3.0-1');
addpath('matlabPyrTools');

i=1;
score=zeros(1,size(list1,1));

for iter =1:size(list1,1)
   s1_2=list1(iter).name;
   s1 = [s1_1 s1_2];
   [a,b,c]=fileparts(s1);
   if strcmp(c, '.bmp')
       im = imread(s1);
       score(1,i) = C_DIIVINE_image_score(im);
       i=i+1;
   end
end


clc
clear
close all


list1 = dir('E:\experiment\task\result\color-max\');
list1 = list1(3:size(list1,1),1);

s1_1 = 'E:\experiment\task\result\color-max\';

addpath('libsvm-mat-3.0-1');
addpath('matlabPyrTools');

i=1;
score=zeros(size(list1,1),1);
iter=1;
while iter <25
   s1_2=list1(iter).name;
   s1 = [s1_1 s1_2];
   [a,b,c]=fileparts(s1);
   if strcmp(c, '.bmp')
       im = imread(s1);
       score(iter,1) = C_DIIVINE_image_score(im);
   end
   iter=iter+1;
end

%scoreC_DIIVINE = zeros(64,1);

%s1_1 = 'E:\experiment\task\result\color-max\';
%s1_3 = '.bmp';
%for n1 = 1: 24
%    s1_2 = int2str(n1);
%    s11 = [s1_1 s1_2]; 
%    s12 = [s11 s1_3];
%    img = imread(s12);
%    scoreC_DIIVINE(n1,1) = C_DIIVINE_image_score(img);
%end


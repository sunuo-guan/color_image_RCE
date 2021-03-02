% function  [D,L,testmtr,trainingmtr,indexP,dic,H] = imrp

function  [Training_Acc, Testing_Class1_Acc, Testing_Class2_Acc, trainingLabel, testingLabel_1, testingLabel_2] = imrp_jingyu


% if xxxxxx == 1
options = statset('MaxIter',1000);


% list1 = dir('C:\Jingyu Ye\Image Data Set\CU\Au-1');
% list1 = list1(3:size(list1,1),1);
% 
% s1_1 = 'C:\Jingyu Ye\Image Data Set\CU\Au-1\';
% 
% list2 = dir('C:\Jingyu Ye\Image Data Set\CU\Sp-2');
% list2 = list2(3:size(list2,1),1);
% 
% s2_1 = 'C:\Jingyu Ye\Image Data Set\CU\Sp-2\';
% 
% 
% m1 = size(list1,1);
% m2 = size(list2,1);
% trainingmtr_c1 = [];
% trainingmtr_c2 = [];
% mtrP = [];
% rpm_c1 = [];
% rpm_c2 = [];
% trainingLabel = [];
% testingLabel_1 = [];
% testingLabel_2 = [];
% 
% trainingSet = 50;
% % testingSet = 10;
% textonSize = 50;
% PatchSize = 15;
% DimDeduction = 20;
% rmSize = PatchSize^2;
% 
% ClassifyType = 2;
% PatchType_tr = 1;
% PatchType_ts = 1;
% type = 4;
%%%%%%%%%%%%%%%%%%%%%%
list1 = dir('G:\毕设\程序数据\BOWS2-lsbpmadfti750022');
list1 = list1(3:size(list1,1),1);

s1_1 = 'G:\毕设\程序数据\BOWS2-lsbpmadfti750022\';

list2 = dir('G:\毕设\程序数据\BOWS2-jpg');
list2 = list2(3:size(list2,1),1);

s2_1 = 'G:\毕设\程序数据\BOWS2-jpg\';


m1 = size(list1,1);
m2 = size(list2,1);
trainingmtr_c1 = [];
trainingmtr_c2 = [];
mtrP = [];
rpm_c1 = [];
rpm_c2 = [];
trainingLabel = [];
testingLabel_1 = [];
testingLabel_2 = [];

trainingSet = 100;
testingSet = 100;
textonSize = 50;
PatchSize = 15;
DimDeduction = 20;
rmSize = PatchSize^2;

ClassifyType = 2;
PatchType_tr = 1;
PatchType_ts = 1;
type = 4;
%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%% Initialization
% for textonSize = 50:50:400
%     for PatchSize = 5:5:20
%         for DimDeduction = 10:10:50
%             rmSize = PatchSize^2;
%%%%%%%%%%%%%%%%%%%%%%%%%%    
    
rm = randmtr(rmSize,DimDeduction,1);    %%%%%%%% Random Matrix - Deduct to 200D
% rm = 1;

for iter = 1:1
    sampleSet_c1(iter,:) = randperm(m1);
    sampleSet_c2(iter,:) = sampleSet_c1(iter,:); %randperm(m2);     
    %%%%%%%%randomly select training and testing samples 
end

%%%%%%%%%%%%% Training Textons %%%%%%%%%%%%%%%
%%%%%%%% Class #1
for n1 = 1:trainingSet
    n1
    
    tic
    
    trainingSample = sampleSet_c1(n1);
    
    s1_2 = list1(trainingSample).name;
    s1 = [s1_1 s1_2];
    
    mtrP = patchv(s1,PatchSize,PatchType_tr);
%     trainingmtr_c1 = [trainingmtr_c1 mtrP];
    rpm = rm'* mtrP;  %%%%%%%%% Random Projection for Class #1
    rpm_c1 = [rpm_c1 rpm];
    indexP_c1(n1) = size(mtrP,2);
    clear mtrP s1_2
    
    toc
end

% rpm_c1 = rm'* trainingmtr_c1;       %%%%%%%%% Random Projection for Class #1

clear trainingmtr_c1

%%%%%%% Class #2
for n1 = 1:trainingSet
    n1
    
    tic
    
    trainingSample = sampleSet_c2(n1);
    
    s2_2 = list2(trainingSample).name;
    s2 = [s2_1 s2_2];
    
    mtrP = patchv(s2,PatchSize,PatchType_tr);

%     trainingmtr_c2 = [trainingmtr_c2 mtrP];
    rpm = rm'* mtrP;  %%%%%%%%% Random Projection for Class #2
    rpm_c2 = [rpm_c2 rpm];
    indexP_c2(n1) = size(mtrP,2);
    clear mtrP s2_2
    
    toc
end

% rpm_c2 = rm'* trainingmtr_c2;       %%%%%%%%% Random Projection for Class #2

%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% Generate Texton dictionary 

tic

% [idx_c1,ctrs_c1] = kmeans(rpm_c1',50,'Options',options,'emptyaction','singleton');
% [idx_c2,ctrs_c2] = kmeans(rpm_c2',50,'Options',options,'emptyaction','singleton');

[idx_c1,ctrs_c1] = kmeans(rpm_c1',textonSize,'Options',options,'emptyaction','singleton');
[idx_c2,ctrs_c2] = kmeans(rpm_c2',textonSize,'Options',options,'emptyaction','singleton');


toc

dic = [ctrs_c1;ctrs_c2];

%%%%%%%%%%%%% Creating Histogram Set %%%%%%

k = 1;
[Dtr_c1,Ltr_c1] = pdist2(dic,rpm_c1','euclidean','Smallest',textonSize);
[Dtr_c2,Ltr_c2] = pdist2(dic,rpm_c2','euclidean','Smallest',textonSize);
Ltr_c1 = Ltr_c1(1,:);
Ltr_c2 = Ltr_c2(1,:);
Stp = 1:1:textonSize*2;

for n2 = 1:trainingSet
    n2
    
    tic

    k1 = indexP_c1(n2);
    H_c1(n2,:) = hist(Ltr_c1(k:k+k1-1),Stp)/k1;
    tr_rpm_c1 = rpm_c1(:,k:k+k1-1);
    D_c1 = pdist2(ctrs_c1,tr_rpm_c1','euclidean','Smallest',textonSize);
    D_c1 = D_c1(1,:);
    D_c2 = pdist2(ctrs_c2,tr_rpm_c1','euclidean','Smallest',textonSize);
    D_c2 = D_c2(1,:);
    
%     if length(find(H_c1(n2,1:textonSize))) > length(find(H_c1(n2,textonSize+1:textonSize*2)))
    switch type
        case 1
            if sum(H_c1(n2,1:textonSize)) > sum(H_c1(n2,textonSize+1:textonSize*2))
                trainingLabel = [trainingLabel 1];
            else
                trainingLabel = [trainingLabel 2];
            end
        case 4
            if sum(D_c1) < sum(D_c2)
                trainingLabel = [trainingLabel 1];
            else
                trainingLabel = [trainingLabel 2];
            end
    end
    k = k + k1;
    
    toc
end    

k = 1;
for n2 = 1:trainingSet
    n2
    
    tic

    k1 = indexP_c2(n2);
    H_c2(n2,:) = hist(Ltr_c2(k:k+k1-1),Stp)/k1;
    tr_rpm_c2 = rpm_c2(:,k:k+k1-1);
    D_c1 = pdist2(ctrs_c1,tr_rpm_c2','euclidean','Smallest',textonSize);
    D_c1 = D_c1(1,:);
    D_c2 = pdist2(ctrs_c2,tr_rpm_c2','euclidean','Smallest',textonSize);
    D_c2 = D_c2(1,:);
    
%     if length(find(H_c1(n2,1:textonSize))) > length(find(H_c1(n2,textonSize+1:textonSize*2)))
    switch type
        case 1
            if sum(H_c1(n2,1:textonSize)) > sum(H_c1(n2,textonSize+1:textonSize*2))
                trainingLabel = [trainingLabel 1];
            else
                trainingLabel = [trainingLabel 2];
            end
        case 4
            if sum(D_c1) < sum(D_c2)
                trainingLabel = [trainingLabel 1];
            else
                trainingLabel = [trainingLabel 2];
            end
    end
    k = k + k1;
    
    toc
end   

%%%%%%%%%%%%% Testing Class #1

for n1 = trainingSet+1:trainingSet+testingSet  %m1
    n1-trainingSet
    
    tic
    testingSample = sampleSet_c1(n1);
    s1_2 = list1(testingSample).name;
    
    % s2_2 = 'SP_SS_H_037.bmp';
    s1 = [s1_1 s1_2];

    T = patchv(s1,PatchSize,PatchType_ts);
    indexP_test = size(T,2);

    c = rm';

    testmtr = rm' * T;

    [D_test,L_test] = pdist2(dic,testmtr','euclidean','Smallest',100);
    L_test = L_test(1,:);
    H_test = hist(L_test,Stp)/indexP_test;
    
    D_c1 = pdist2(ctrs_c1,testmtr','euclidean','Smallest',textonSize);
    D_c1 = D_c1(1,:);
    D_c2 = pdist2(ctrs_c2,testmtr','euclidean','Smallest',textonSize);
    D_c2 = D_c2(1,:);
%     plot(H_test)
    
%     if length(find(H_test(1:textonSize))) > length(find(H_test(textonSize+1:textonSize*2)))
    
    switch ClassifyType 
        case 1
            D_test_c1 = min(pdist2(H_c1,H_test));
            D_test_c2 = min(pdist2(H_c2,H_test));

            if D_test_c1 < D_test_c2
                testingLabel_1 = [testingLabel_1 1];
            else
                testingLabel_1 = [testingLabel_1 2];
            end
            
        case 2
            if sum(H_test(1:textonSize)) > sum(H_test(textonSize+1:textonSize*2))
                testingLabel_1 = [testingLabel_1 1];
            else
                testingLabel_1 = [testingLabel_1 2];
            end
            
        case 3
            if sum(D_c1) < sum(D_c2)
                testingLabel_1 = [testingLabel_1 1];
            else
                testingLabel_1 = [testingLabel_1 2];
            end   
    end
    
end

%%%%%%%%%%%%% Testing Class #2

for n2 = trainingSet+1:trainingSet+testingSet  %m2
    n2-trainingSet
    
    tic
    testingSample = sampleSet_c2(n2);
    s2_2 = list2(testingSample).name;
    
    % s2_2 = 'SP_SS_H_037.bmp';
    s2 = [s2_1 s2_2];

    T = patchv(s2,PatchSize,PatchType_ts);
    indexP_test = size(T,2);

    c = rm';

    testmtr = rm' * T;

    [D_test,L_test] = pdist2(dic,testmtr','euclidean','Smallest',100);
    L_test = L_test(1,:);
    H_test = hist(L_test,Stp)/indexP_test;
    
    D_c1 = pdist2(ctrs_c1,testmtr','euclidean','Smallest',textonSize);
    D_c1 = D_c1(1,:);
    D_c2 = pdist2(ctrs_c2,testmtr','euclidean','Smallest',textonSize);
    D_c2 = D_c2(1,:);    
%     plot(H_test)
    
%     if length(find(H_test(1:textonSize))) < length(find(H_test(textonSize+1:textonSize*2)))
    switch ClassifyType 
            case 1
                D_test_c1 = min(pdist2(H_c1,H_test));
                D_test_c2 = min(pdist2(H_c2,H_test));

                if D_test_c1 < D_test_c2
                    testingLabel_2 = [testingLabel_2 1];
                else
                    testingLabel_2 = [testingLabel_2 2];
                end

            case 2
                if sum(H_test(1:textonSize)) > sum(H_test(textonSize+1:textonSize*2))
                    testingLabel_2 = [testingLabel_2 1];
                else
                    testingLabel_2 = [testingLabel_2 2];
                end
                
            case 3
                if sum(D_c1) < sum(D_c2)
                    testingLabel_2 = [testingLabel_2 1];
                else
                    testingLabel_2 = [testingLabel_2 2];
                end  
    end
    
end

%Training_Class1_Acc = (length(find(trainingLabel(1:trainingSet) == 1)) + length(find(trainingLabel(trainingSet+1:trainingSet*2) == 2))) / (trainingSet*2)

Training_Class1_Acc = length(find(trainingLabel(1:trainingSet) == 1))/ (trainingSet)
Training_Class2_Acc = length(find(trainingLabel(trainingSet+1:trainingSet*2) == 2)) / (trainingSet)
Testing_Class1_Acc = length(find(testingLabel_1 == 1)) / (testingSet)  %(m1 - trainingSet)
Testing_Class2_Acc = length(find(testingLabel_2 == 2)) / (testingSet)  %(m2 - trainingSet)

filename = ['textonSize' num2str(textonSize) '_' 'PatchSize' num2str(PatchSize) '_' 'DimDeduction' num2str(DimDeduction)];

save(filename,'Training_Class1_Acc','Training_Class2_Acc','Testing_Class1_Acc','Testing_Class2_Acc',... 
     'rpm_c1', 'rpm_c2', 'dic', 'rm');

Training_Acc=(Testing_Class1_Acc+Testing_Class2_Acc)/2;

% clear 'Training_Class1_Acc','Training_Class2_Acc','Testing_Class1_Acc','Testing_Class2_Acc',...
%     'rpm_c1', 'rpm_c2', 'dic'
        
%%%%%%%%%%%%%%%%%%%%%%%%%%%% End

%         end
%     end
% end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function [patchmtr] = patchv(X,PatchSize,Type)
A = imread(X);
% A = double(rgb2gray(A));
A = double(A);
% A = imgestimateS(A);

% BlkSize = 5;

[M N] = size(A);
M1 = floor(M/PatchSize);
N1 = floor(N/PatchSize);
x = 1;

switch Type
    case 1    %Non-Overlapping
        for i = 1:M1
            for j = 1:N1
                B = A(1+(i-1)*PatchSize:i*PatchSize,1+(j-1)*PatchSize:j*PatchSize);
                C(:,x) = B(:);
                x = x+1;
            end
        end
        
    case 2    %Overlapping
        for i = 1:M-PatchSize+1
            for j = 1:N-PatchSize+1
                B = A(i:i+PatchSize-1,j:j+PatchSize-1);
                C(:,x) = B(:);
                x = x+1;
            end
        end
end

patchmtr = C;


function [rm] = randmtr(n,m,type)
% Create n*m Random matrix of type 1-3.
% 
% Inputs
%    N: Row number of random matrix.
%    M: Column number of random matrix.
%    Type: Random type of matrix.
% Output
%    RM: n*m random matrix of type 1, 2, or 3.
% Example: Create a 1000*800 random matrix of type 1.
% rm = randmtr(1000,800,1);
switch type
    case 1   %%%%% Gaussian, Zero Mean, Unit Variance
        for i = 1:n
            rm(i,:) = randn(1,m);
        end
    case 2
        A = ( rand(n,m)>0.5 );
        rm = A - ~A;
    case 3
        A = rand(n,m);
        rm = sqrt(3)*((A>1/6)-(A<1/6));        
end


function errimg=imgestimateS(ori)

[xs,ys]=size(ori);

ori=double(ori);
estimg=zeros(xs,ys);
errimg=zeros(xs,ys);
for i=1:xs-1
    for j=1:ys-1
        if ori(i+1,j+1)<=min(ori(i+1,j),ori(i,j+1))
            estimg(i,j)=max(ori(i+1,j),ori(i,j+1));
        else
            if ori(i+1,j+1)>=max(ori(i+1,j),ori(i,j+1))
                estimg(i,j)=min(ori(i+1,j),ori(i,j+1));
            else
                estimg(i,j)=ori(i+1,j)+ori(i,j+1)-ori(i+1,j+1);
            end
        end
    end
end

errimg(1:xs-1,1:ys-1)=estimg(1:xs-1,1:ys-1)-ori(1:xs-1,1:ys-1);
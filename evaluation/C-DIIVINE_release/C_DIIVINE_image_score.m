function score = C_DIIVINE_image_score(im)

%========================================================================
%C-DIIVINE Index
%Copyright(c) 2014  Yi Zhang, Anush K. Moorthy, Damon M. Chandler and Alan C. Bovik
%All Rights Reserved.
%----------------------------------------------------------------------
%Permission to use, copy, or modify this software and its documentation
%for educational and research purposes only and without fee is hereby
%granted, provided that this copyright notice and the original authors'
%names appear on all copies and supporting documentation. This program
%shall not be used, rewritten, or adapted as the basis of a commercial
%software or hardware product without first obtaining permission of the
%authors. The authors make no representations about the suitability of
%this software for any purpose. It is provided "as is" without express
%or implied warranty.
%----------------------------------------------------------------------
%
%This is an implementation of the algorithm for blind/referenceless image 
%quality assessment. Please refer to the following paper:
%
% Yi Zhang, Anush K. Moorthy, Damon M. Chandler and Alan C. Bovik, 
% "C-DIIVINE: No-reference image quality assessment based on local magnitude
% and phase statistics of natural scenes", Signal Processing: Image
% Communication 29(2014), 725-747. 

tic
f = Complex_DIIVINE_feature(im);
f = f';
load ModelLive_1s.mat

f_s = scale2svm(f, cdivine_range);
[score_1, accuracy] = svmpredict(1, f_s, cdivine_model);
load ModelLive_2s.mat

f_clsscale = scale2svm(f, ClassifyRange);
[Label, Accuracy, Prob] = svmpredict(1, f_clsscale, ClassifyModel, '-b 1');
ImgClassPreDmos = [];
for reg_mod_idx = 1:size(RegressModel, 2)
    f_regscale = scale2svm(f, RegressRange{1,reg_mod_idx});
    [pre_dmos, accuracy] =  svmpredict(1, f_regscale, RegressModel(1,reg_mod_idx));
    ImgClassPreDmos = [ImgClassPreDmos pre_dmos];
end

score_2 = sum(ImgClassPreDmos .* Prob);

score_min = min(score_1, score_2);
score_aver = (score_1 + score_2)/2;

score = (score_min + score_aver)/2;
toc








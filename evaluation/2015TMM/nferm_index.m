function score = nferm_index(img2)
%==========================================================================
% 1) Please cite the paper (K. Gu, G. Zhai, X. Yang, and W. Zhang, "Using 
% free energy principle for blind image quality assessment," IEEE Trans. 
% Multimedia, vol. 17, no. 1, pp. 50-63, Jan. 2015.)
% 2) If any question, please contact me through guke.doctor@gmail.com; 
% gukesjtuee@gmail.com. 
% 3) Welcome to cooperation, and I am very willing to share my experience.
%==========================================================================
if size(img2,3) ~= 1
img2 = rgb2gray(img2);
end
img2 = double(img2);
% Free Energy
[fedm,img1] = appre_index(img2);
% Feature Group One
sal = ones(size(img2,1),size(img2,2));
[i_11,e_11] = sdm_index(img2,11,2,sal);
[i_07,e_07] = sdm_index(img2,07,2,sal);
[i_03,e_03] = sdm_index(img2,03,2,sal);
[lztmp_i_11,cztmp_i_11,sztmp_i_11] = find_index(i_11,fedm);
[lztmp_i_07,cztmp_i_07,sztmp_i_07] = find_index(i_07,fedm);
[lztmp_i_03,cztmp_i_03,sztmp_i_03] = find_index(i_03,fedm);
[lztmp_e_11,cztmp_e_11,sztmp_e_11] = find_index(e_11,fedm);
[lztmp_e_07,cztmp_e_07,sztmp_e_07] = find_index(e_07,fedm);
[lztmp_e_03,cztmp_e_03,sztmp_e_03] = find_index(e_03,fedm);
load coff_nferm
in = [2 3 5 6 8 9];
xi = [coff_02(1)*lztmp_i_11+coff_02(2) ...
coff_03(1)*cztmp_i_11+coff_03(2) coff_04(1)*sztmp_i_11+coff_04(2) ...
coff_05(1)*lztmp_i_07+coff_05(2) coff_06(1)*cztmp_i_07+coff_06(2) ...
coff_07(1)*sztmp_i_07+coff_07(2) coff_08(1)*lztmp_i_03+coff_08(2) ...
coff_09(1)*cztmp_i_03+coff_09(2) coff_10(1)*sztmp_i_03+coff_10(2)];
xe = [coff_38(1)*lztmp_e_11+coff_38(2) ...
coff_39(1)*cztmp_e_11+coff_39(2) coff_40(1)*sztmp_e_11+coff_40(2) ...
coff_41(1)*lztmp_e_07+coff_41(2) coff_42(1)*cztmp_e_07+coff_42(2) ...
coff_43(1)*sztmp_e_07+coff_43(2) coff_44(1)*lztmp_e_03+coff_44(2) ...
coff_45(1)*cztmp_e_03+coff_45(2) coff_46(1)*sztmp_e_03+coff_46(2)];
f1 = [fedm,xi(in)-repmat(fedm,1,6),xe(in)-repmat(fedm,1,6)];
% Feature Group Two
psnr = psnr_new_index(img1,img2);
ssim = ssim_new_index(img1,img2);
pcgm = pcgm_index(img1,img2);
f2 = [psnr pcgm ssim];
% Feature Group Three
f3 = brisque_feature(img2);
% Feature Integration
feat = [f3 f2 f1];
fid = fopen('test_ind.txt','w');
for jj = 1:size(feat,1)
fprintf(fid,'1 ');
for kk = 1:size(feat,2)
fprintf(fid,'%d:%f ',kk,feat(jj,kk));
end
fprintf(fid,'\n');
end
fclose(fid);
warning off all
delete output.txt test_ind_scaled dump
system('svm-scale  -r range test_ind.txt >> test_ind_scaled');
system('svm-predict  -b 1  test_ind_scaled model output.txt>dump');
load output.txt;
score = output;
%=======================================================
function [Es,imgrec] = appre_index(imgin)
sr=3;
mr=1;
imgt=padarray(imgin,[sr+mr sr+mr],'symmetric');
imgrec=zeros(size(imgin));
[m,n]=size(imgt);
N=(2*sr+1)^2-1;
K=(2*mr+1)^2-1;
A=zeros(N,K+1);
for ii=mr+sr+1:m-sr-mr
for jj=mr+sr+1:n-sr-mr
con=1;
patch0=imgt(ii-mr:ii+mr,jj-mr:jj+mr);
for iii=-sr:+sr
for jjj=-sr:+sr
if iii==0&&jjj==0
continue;
end
patch=imgt(ii+iii-mr:ii+iii+mr,jj+jjj-mr:jj+jjj+mr);
vec=patch(:);
A(con,:)=vec';
con=con+1;
end
end
b=A(:,mr*(2*mr+2)+1);
A2=A;
A2(:,mr*(2*mr+2)+1)=[];
if rcond(A2'*A2)<1e-7
a = ones(K,1)/K;
else
a = A2\b;
end
vec0=patch0(:);
vec0(mr*(2*mr+2)+1)=[];
rec=vec0'*a;
imgrec(ii-sr-mr,jj-sr-mr)=rec;
end
end
err=imgin-imgrec;
xx=-255:255;
y=round(err);
[nn,~]=hist(y(:),xx);
p=(1+2*nn)/sum(1+2*nn);
Es = -sum(p.*log2(p));
%=======================================================
function [mssim2,mssim3] = sdm_index(img2,aaa,f,sal)
bbb = floor(aaa/2);
[M,N] = size(img2);
window = fspecial('gaussian', aaa, 1.5);
C = (0.03*255)^2;
window = window/sum(sum(window));
map2 = zeros(M,N);
map2(1:8:end,:) = 1;map2(:,1:8:end) = 1;
map2(8:8:end,:) = 1;map2(:,8:8:end) = 1;
map2(7:8:end,:) = 1;map2(:,7:8:end) = 1;
if(f>1)
lpf = ones(f,f);
lpf = lpf/sum(lpf(:));
img2 = imfilter(img2,lpf,'symmetric','same');
img2 = img2(1:f:end,1:f:end);
map2 = map2(1:f:end,1:f:end);
sal = imfilter(sal,lpf,'symmetric','same');
sal = sal(1:f:end,1:f:end);
sal = sal(bbb+1:end-bbb,bbb+1:end-bbb);
end
map2 = map2(bbb+1:end-bbb,bbb+1:end-bbb);
map3 = ones(size(map2,1),size(map2,2));
map3 = map3-map2;
%% Remapping
mu2 = filter2(window, img2, 'valid');
mu2_sq = mu2.*mu2;
sigma2_sq = filter2(window, img2.*img2, 'valid') - mu2_sq;
imgY = img2(bbb+1:end-bbb,bbb+1:end-bbb);
map2 = map2(bbb+1:end-bbb,bbb+1:end-bbb);
map3 = map3(bbb+1:end-bbb,bbb+1:end-bbb);
sal  = sal(bbb+1:end-bbb,bbb+1:end-bbb);
%% Distorted(l)
dmu1 = filter2(window, imgY, 'valid');
dmu2 = filter2(window, mu2,  'valid');
dmu1_sq  = dmu1.*dmu1;
dmu2_sq  = dmu2.*dmu2;
dmu1_mu2 = dmu1.*dmu2;
dsigma1_sq = filter2(window, imgY.*imgY, 'valid') - dmu1_sq;
dsigma2_sq = filter2(window, mu2.*mu2, 'valid') - dmu2_sq;
dsigma12   = filter2(window, imgY.*mu2, 'valid') - dmu1_mu2;
dc_map  = real((2*sqrt(dsigma1_sq).*sqrt(dsigma2_sq)+ C)./(dsigma1_sq + dsigma2_sq + C));
ds_map  = real((2*dsigma12 + C)./(2*sqrt(dsigma1_sq).*sqrt(dsigma2_sq)+ C));
dcssim  = mean2(dc_map.*map2.*sal)/mean2(map2.*sal);
dsssim  = mean2(ds_map.*map2.*sal)/mean2(map2.*sal);
dcssimZ = mean2(dc_map.*map3.*sal)/mean2(map3.*sal);
dsssimZ = mean2(ds_map.*map3.*sal)/mean2(map3.*sal);
%% Distorted(c)
X = real(sqrt(sigma2_sq));
Y = real(sqrt(imgY.*imgY-mu2_sq));
dmu1 = filter2(window, X, 'valid');
dmu2 = filter2(window, Y, 'valid');
dmu1_sq  = dmu1.*dmu1;
dmu2_sq  = dmu2.*dmu2;
dmu1_mu2 = dmu1.*dmu2;
dsigma1_sq = filter2(window, X.*X, 'valid') - dmu1_sq;
dsigma2_sq = filter2(window, Y.*Y, 'valid') - dmu2_sq;
dsigma12   = filter2(window, X.*Y, 'valid') - dmu1_mu2;
ds_map   = real((2*dsigma12 + C)./(2*sqrt(dsigma1_sq).*sqrt(dsigma2_sq)+ C));
dsssim2  = mean2(ds_map.*map2.*sal)/mean2(map2.*sal);
dsssimZ2 = mean2(ds_map.*map3.*sal)/mean2(map3.*sal);
%% Integration
mssim2 = [dcssim  dsssim  dsssim2];
mssim3 = [dcssimZ dsssimZ dsssimZ2];
%=======================================================
function [lztmp,cztmp,sztmp] = find_index(ssim,fedm)
lztmp = real(ssim(:,1));lztmp(fedm>5) = -lztmp(fedm>5);
cztmp = real(ssim(:,2));cztmp(fedm>5) = -cztmp(fedm>5);
sztmp = real(ssim(:,3));sztmp(fedm>5) = -sztmp(fedm>5);
%=======================================================
function psnr = psnr_new_index(img1,img2)
[M,N] = size(img1);
f = max(1,round(min(M,N)/256));
if(f>1)
lpf = ones(f,f);
lpf = lpf/sum(lpf(:));
img1 = imfilter(img1,lpf,'symmetric','same');
img2 = imfilter(img2,lpf,'symmetric','same');
img1 = img1(1:f:end,1:f:end);
img2 = img2(1:f:end,1:f:end);
end
mse = mean2((img1-img2).^2);
psnr = 10*log10(255^2/mse);
psnr = min(1000,psnr);
%=======================================================
function ssim = ssim_new_index(img1,img2)
win = fspecial('gaussian',11,1.5);	
C = (0.03*255)^2;
mu1 = filter2(win,img1,'valid');
mu2 = filter2(win,img2,'valid');
mu1_sq  = mu1.*mu1;
mu2_sq  = mu2.*mu2;
mu1_mu2 = mu1.*mu2;
sigma1_sq = filter2(win,img1.*img1,'valid')-mu1_sq;
sigma2_sq = filter2(win,img2.*img2,'valid')-mu2_sq;
sigma12   = filter2(win,img1.*img2,'valid')-mu1_mu2;
c_map = real((2*sqrt(sigma1_sq).*sqrt(sigma2_sq)+C)./(sigma1_sq+sigma2_sq+C));
s_map = real((2*sigma12+C)./(2*sqrt(sigma1_sq).*sqrt(sigma2_sq)+C));
ssim = [mean2(c_map) mean2(s_map)];
%=======================================================
function pcgm = pcgm_index(Y1,Y2)
[rows, cols] = size(Y1(:,:,1));
minDimension = min(rows,cols);
F = max(1,round(minDimension / 256));
aveKernel = fspecial('average',F);
aveY1 = conv2(Y1, aveKernel,'same');
aveY2 = conv2(Y2, aveKernel,'same');
Y1 = aveY1(1:F:rows,1:F:cols);
Y2 = aveY2(1:F:rows,1:F:cols);
PC1 = phasecong2(Y1);
PC2 = phasecong2(Y2);
dx = [3  0 -3; 10  0 -10;  3  0  -3]/16;
dy = [3 10  3;  0  0   0; -3 -10 -3]/16;
IxY1 = conv2(Y1, dx, 'same');     
IyY1 = conv2(Y1, dy, 'same');    
gradientMap1 = sqrt(IxY1.^2 + IyY1.^2);
IxY2 = conv2(Y2, dx, 'same');     
IyY2 = conv2(Y2, dy, 'same');    
gradientMap2 = sqrt(IxY2.^2 + IyY2.^2);
T1 = 0.85;  %fixed
T2 = 160; %fixed
PCSimMatrix = (2 * PC1 .* PC2 + T1) ./ (PC1.^2 + PC2.^2 + T1);
gradientSimMatrix = (2*gradientMap1.*gradientMap2 + T2) ./(gradientMap1.^2 + gradientMap2.^2 + T2);
PCm = max(PC1, PC2);
SimMatrix = gradientSimMatrix .* PCSimMatrix .* PCm;
FSIM = sum(sum(SimMatrix)) / sum(sum(PCm));
pcgm = [mean2(PCm) mean2(gradientSimMatrix) FSIM];
%=======================================================
function [ResultPC]=phasecong2(im)
nscale          = 4;     % Number of wavelet scales.    
norient         = 4;     % Number of filter orientations.
minWaveLength   = 6;     % Wavelength of smallest scale filter.    
mult            = 2;   % Scaling factor between successive filters.    
sigmaOnf        = 0.55;  % Ratio of the standard deviation of the
dThetaOnSigma   = 1.2;   % Ratio of angular interval between filter orientations    
k               = 2.0;   % No of standard deviations of the noise
epsilon         = .0001;                % Used to prevent division by zero.
thetaSigma = pi/norient/dThetaOnSigma;  % Calculate the standard deviation of the
[rows,cols] = size(im);
imagefft = fft2(im);              % Fourier transform of image
zero = zeros(rows,cols);
EO = cell(nscale, norient);       % Array of convolution results.                                 
estMeanE2n = [];
ifftFilterArray = cell(1,nscale); % Array of inverse FFTs of filters
if mod(cols,2)
xrange = [-(cols-1)/2:(cols-1)/2]/(cols-1);
else
xrange = [-cols/2:(cols/2-1)]/cols;	
end
if mod(rows,2)
yrange = [-(rows-1)/2:(rows-1)/2]/(rows-1);
else
yrange = [-rows/2:(rows/2-1)]/rows;	
end
[x,y] = meshgrid(xrange, yrange);
radius = sqrt(x.^2 + y.^2);       % Matrix values contain *normalised* radius from centre.
theta = atan2(-y,x);              % Matrix values contain polar angle.
radius = ifftshift(radius);       % Quadrant shift radius and theta so that filters
theta  = ifftshift(theta);        % are constructed with 0 frequency at the corners.
radius(1,1) = 1;                  % Get rid of the 0 radius value at the 0
sintheta = sin(theta);
costheta = cos(theta);
clear x; clear y; clear theta;    % save a little memory
lp = lowpassfilter([rows,cols],.45,15);   % Radius .45, 'sharpness' 15
logGabor = cell(1,nscale);
for s = 1:nscale
wavelength = minWaveLength*mult^(s-1);
fo = 1.0/wavelength;                  % Centre frequency of filter.
logGabor{s} = exp((-(log(radius/fo)).^2) / (2 * log(sigmaOnf)^2));  
logGabor{s} = logGabor{s}.*lp;        % Apply low-pass filter
logGabor{s}(1,1) = 0;                 % Set the value at the 0 frequency point of the filter
end
spread = cell(1,norient);
for o = 1:norient
angl = (o-1)*pi/norient;           % Filter angle.
ds = sintheta * cos(angl) - costheta * sin(angl);    % Difference in sine.
dc = costheta * cos(angl) + sintheta * sin(angl);    % Difference in cosine.
dtheta = abs(atan2(ds,dc));                          % Absolute angular distance.
spread{o} = exp((-dtheta.^2) / (2 * thetaSigma^2));  % Calculate the
end
EnergyAll(rows,cols) = 0;
AnAll(rows,cols) = 0;
for o = 1:norient                    % For each orientation.
sumE_ThisOrient   = zero;          % Initialize accumulator matrices.
sumO_ThisOrient   = zero;       
sumAn_ThisOrient  = zero;      
Energy            = zero;      
for s = 1:nscale,                  % For each scale.
filter = logGabor{s} .* spread{o};   % Multiply radial and angular
ifftFilt = real(ifft2(filter))*sqrt(rows*cols);  % Note rescaling to match power
ifftFilterArray{s} = ifftFilt;                   % record ifft2 of filter
EO{s,o} = ifft2(imagefft .* filter);      
An = abs(EO{s,o});                         % Amplitude of even & odd filter response.
sumAn_ThisOrient = sumAn_ThisOrient + An;  % Sum of amplitude responses.
sumE_ThisOrient = sumE_ThisOrient + real(EO{s,o}); % Sum of even filter convolution results.
sumO_ThisOrient = sumO_ThisOrient + imag(EO{s,o}); % Sum of odd filter convolution results.
if s==1                                 % Record mean squared filter value at smallest
EM_n = sum(sum(filter.^2));           % scale. This is used for noise estimation.
maxAn = An;                           % Record the maximum An over all scales.
else
maxAn = max(maxAn, An);
end
end                                       % ... and process the next scale
XEnergy = sqrt(sumE_ThisOrient.^2 + sumO_ThisOrient.^2) + epsilon;   
MeanE = sumE_ThisOrient ./ XEnergy; 
MeanO = sumO_ThisOrient ./ XEnergy; 
for s = 1:nscale,       
E = real(EO{s,o}); O = imag(EO{s,o});    % Extract even and odd
Energy = Energy + E.*MeanE + O.*MeanO - abs(E.*MeanO - O.*MeanE);
end
medianE2n = median(reshape(abs(EO{1,o}).^2,1,rows*cols));
meanE2n = -medianE2n/log(0.5);
noisePower = meanE2n/EM_n;                       % Estimate of noise power.
EstSumAn2 = zero;
for s = 1:nscale
EstSumAn2 = EstSumAn2 + ifftFilterArray{s}.^2;
end
EstSumAiAj = zero;
for si = 1:(nscale-1)
for sj = (si+1):nscale
EstSumAiAj = EstSumAiAj + ifftFilterArray{si}.*ifftFilterArray{sj};
end
end
sumEstSumAn2 = sum(sum(EstSumAn2));
sumEstSumAiAj = sum(sum(EstSumAiAj));
EstNoiseEnergy2 = 2*noisePower*sumEstSumAn2 + 4*noisePower*sumEstSumAiAj;
tau = sqrt(EstNoiseEnergy2/2);                     % Rayleigh parameter
EstNoiseEnergy = tau*sqrt(pi/2);                   % Expected value of noise energy
EstNoiseEnergySigma = sqrt( (2-pi/2)*tau^2 );
T =  EstNoiseEnergy + k*EstNoiseEnergySigma;       % Noise threshold
T = T/1.7;        % Empirical rescaling of the estimated noise effect to 
Energy = max(Energy - T, zero);          % Apply noise threshold
EnergyAll = EnergyAll + Energy;
AnAll = AnAll + sumAn_ThisOrient;
end  % For each orientation
ResultPC = EnergyAll ./ AnAll;
%=======================================================
function f = lowpassfilter(sze, cutoff, n)
if cutoff < 0 || cutoff > 0.5
error('cutoff frequency must be between 0 and 0.5');
end
if rem(n,1) ~= 0 || n < 1
error('n must be an integer >= 1');
end
if length(sze) == 1
rows = sze; cols = sze;
else
rows = sze(1); cols = sze(2);
end
if mod(cols,2)
xrange = [-(cols-1)/2:(cols-1)/2]/(cols-1);
else
xrange = [-cols/2:(cols/2-1)]/cols;	
end
if mod(rows,2)
yrange = [-(rows-1)/2:(rows-1)/2]/(rows-1);
else
yrange = [-rows/2:(rows/2-1)]/rows;	
end
[x,y] = meshgrid(xrange, yrange);
radius = sqrt(x.^2 + y.^2);        % A matrix with every pixel = radius relative to centre.
f = ifftshift( 1 ./ (1.0 + (radius ./ cutoff).^(2*n)) );   % The filter
%=======================================================
function feat = brisque_feature(imdist)
scalenum = 2;
window = fspecial('gaussian',7,7/6);
window = window/sum(sum(window));
feat = [];
for itr_scale = 1:scalenum
mu            = filter2(window, imdist, 'same');
mu_sq         = mu.*mu;
sigma         = sqrt(abs(filter2(window, imdist.*imdist, 'same') - mu_sq));
structdis     = (imdist-mu)./(sigma+1);
[alpha,overallstd] = estimateggdparam(structdis(:));
feat               = [feat alpha overallstd^2]; 
imdist             = imresize(imdist,0.5);
end
%=======================================================
function [gamparam,sigma] = estimateggdparam(vec)
gam                = 0.2:0.001:10;
r_gam              = (gamma(1./gam).*gamma(3./gam))./((gamma(2./gam)).^2);
sigma_sq           = mean((vec).^2);
sigma              = sqrt(sigma_sq);
E                  = mean(abs(vec));
rho                = sigma_sq/E^2;
[~,array_position] = min(abs(rho - r_gam));
gamparam           = gam(array_position);
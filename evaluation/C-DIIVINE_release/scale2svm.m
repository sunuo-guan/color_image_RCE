function varargout = scale2svm(varargin) 
% scale2svm.m 
% Usage: 
%    1. [dataScaled,Range] = scale2svm(dataset); 
%    2. [dataScaled,Range] = scale2svm(dataset,low,up); 
%    3. dataScaled = scale2svm(dataset,Range); 
% Input: 
%    dataset ---- ?scale???? 
%    low      ---- scale????default = -1 
%    up       ---- scale???, default = 1; 
% Output: 
%     dataScaled - scale????? 
%     Range    ---- scale????cell?? 

% Author: Genial@ustc 
% Date: 2005.4.2 

if nargin<1 
    help scale2svm; 
    error('Parameter Errors!'); 
elseif nargin==1 
    dataset = varargin{1}; 
    low = -1; 
    up = 1; 
elseif nargin==2 
    if ~iscell(varargin{2}) 
        help scale2svm; 
        erorr('Parameter Errors!'); 
    else 
        % scale testing data with range
        dataset = varargin{1}; 
        varargout{1} = scaleWithRange(dataset,varargin{2}); 
        return; 
    end 
elseif nargin==3 
    dataset = varargin{1}; 
    low = varargin{2}; 
    up =varargin{3}; 
    if up <= low 
        help scale2svm 
        error('Parameter Errors!'); 
    end 
else 
    help scale2svm 
    error('Parameter Errors!'); 
end 

len = up - low; 
cmin = min(dataset,[],1); 
cmax = max(dataset,[],1); 
rowsNum = size(dataset,1); 
maxSubMin = cmax - cmin; 
DataMin = repmat(cmin,rowsNum,1); 
DmaxSubMin = repmat(maxSubMin,rowsNum,1); 
dataSetScaled = low+(dataset-DataMin)./DmaxSubMin * len; 
clear DataMin DmaxSubMin 
varargout{1} = dataSetScaled; 
varargout{2} = {[low,up];cmin;cmax}; 



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%&&&&&&&&&&& 
function dataScaled = scaleWithRange(dataset,range) 
lowAndUp = range{1}; 
low = lowAndUp(1); 
up = lowAndUp(2); 
cmin = range{2}; 
cmax = range{3}; 
if size(dataset,2)~=size(cmin,2) 
    error('Length is not the same!'); 
end 
rowsNum = size(dataset,1); 
len = up-low; 
maxSubMin = cmax-cmin; 
kMatrix = len./maxSubMin; 
DataMin = repmat(cmin,rowsNum,1); 
DkMatrix = repmat(kMatrix,rowsNum,1); 
dataScaled = DkMatrix.*(dataset-DataMin)+low; 

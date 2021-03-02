function rss = RSS( X,Y )
%RSS Summary of this function goes here
%   Detailed explanation goes here
[M,N]=size(X);
histX=imhist(X);
[L,K]=size(histX);
X=double(X);Y=double(Y);
MSE=0;
for i=1:M,
    for j=1:N,
      MSE=MSE+(X(i,j)-Y(i,j))^2/(M*N);  
    end
end
RMSE=abs(sqrt(double(MSE)));
rss=1-RMSE/(L-1);

end


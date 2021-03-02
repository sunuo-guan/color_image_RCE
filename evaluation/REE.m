function ree = REE( pdfX,pdfY )
%REE Summary of this function goes here
%   Detailed explanation goes here
[L,K]=size(pdfX);
E1=0;E2=0;
for i=1:L,
    if pdfX(i,1)~=0,
    E1=E1-pdfX(i,1)*log(pdfX(i,1))/log(2);
    end
    if pdfY(i,1)~=0,
    E2=E2-pdfY(i,1)*log(pdfY(i,1))/log(2);
    end
end
ree=(E2-E1)/(2*log(L)/log(2))+0.5;


end


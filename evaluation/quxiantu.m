A = zeros(64,25);
A = xlsread('E:\experiment\task\result\color-3-brisque.xlsx');

B = zeros(64,25);
B = xlsread('E:\experiment\task\result\color-max-brisque.xlsx');

a = zeros(1,64);
b = zeros(1,64);
c = zeros(1,64);

for n1 = 1: 64
    a(n1) = A(n1, 16);
    b(n1) = B(n1, 16);
    c(n1) = n1;
end

plot(c,a,'g+-',c,b,'ro-');
xlabel('Number of histogram bin pairs expanded (i.e., S)');
ylabel('BRISQUE');

% Directly applying scheme [12]
% Proposed scheme
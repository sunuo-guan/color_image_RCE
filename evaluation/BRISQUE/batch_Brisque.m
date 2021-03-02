scoreBRISQUE = zeros(64,1);

s1_1 = 'E:\experiment\kodim²ÊÉ«bmp\kodim';
s1_3 = '.bmp';
for n1 = 1: 24
    s1_2 = int2str(n1);
    s11 = [s1_1 s1_2]; 
    s12 = [s11 s1_3];
    img = imread(s12);
scoreBRISQUE(n1,1) = brisquescore(img);
end

s=1:64;
plot(s,scoreBRISQUE,'r-+');
xlabel('S');
ylabel('Brisque');

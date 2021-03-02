scoreBRISQUEboat = zeros(52,1);

s1_1 = 'D:\Automatic contrast enhancement using reversible data hiding\boat\';
s1_3 = '.bmp';
for n1 = 1: 52
    s1_2 = int2str(n1);
    s11 = [s1_1 s1_2];
    s12 = [s11 s1_3];
    img = imread(s12);
scoreBRISQUEboat(n1,1) = brisquescore(img);
end
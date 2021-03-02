scoreBRISQUE = zeros(1000,1);

s1_1 = 'E:\experiment\bows2-1c\';
s1_3 = '.bmp';
for n1 = 1: 1000
    s1_2 = int2str(n1);
    s11 = [s1_1 s1_2]; 
    s12 = [s11 s1_3];
    img = imread(s12);
scoreBRISQUE(n1,1) =  C_DIIVINE_image_score(img);
end
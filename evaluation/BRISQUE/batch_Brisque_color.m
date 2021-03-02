scoreBRISQUE = zeros(64,24);


for n1 = 1: 24
    for n2 = 20:40
        address = ['E:\experiment\task\result-reversible\uce-newPre\kodak\kodim',num2str(n1),'_',num2str(n2),'.bmp'];
        if exist(address,'file') == 0 
            continue
        end
        I = imread(address);
        score = brisquescore(I);
        if numel(score) == 0
            score = -1;
        end
        scoreBRISQUE(n2,n1) = score;
        disp(n2)
    end
end
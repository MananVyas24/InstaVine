close all;
clear all;
clc;

Height = 480;
Width = 720;
filename = 'C:\Users\Shruti\Documents\MATLAB\EE 586\test1.raw';
G = readraw_color(filename,Height,Width);

imshow(G/256);

%Convert to grayscale
G = 0.299.*G(:,:,1) + 0.587.*G(:,:,2) + 0.114.*G(:,:,3);
% figure;
% imshow(G/256);

%Gaussian filtering

hsize = 3;
sigma = 0.5;
h = fspecial('gaussian', 5, sigma);

Gauss = imfilter(G,h);
% figure;
% imshow(Gauss/256);

%sobel filter:
h = fspecial('sobel');

%gradient map:
window = zeros(hsize,hsize);
gradientmap = zeros(size(Gauss));
grad_direction = zeros(size(Gauss));
% gradmap_x = zeros(size(Gauss));
% gradmap_y = zeros(size(Gauss));
rad_90 = 90*pi/180;
for i = 1:Height-hsize+1
    for j = 1:Width-hsize+1
        window = Gauss(i:i+hsize-1,j:j+hsize-1);
        grad = window.*h;
        grad_x = sum(sum(grad,1));
        grad_y = sum(sum(grad,2));
        grad_f = sqrt(grad_x.^2+grad_y.^2);
        %         gradmap_x(i,j) = grad_x;
        %         gradmap_y(i,j) = grad_y;
        if(grad_x~=0)
            grad_direction(i,j) = atan(grad_y/grad_x);
        else
            if(grad_y==0)
                grad_direction(i,j) = 0;
            else
                grad_direction(i,j) = rad_90;   %90 degrees
            end
        end
        gradientmap(i,j) = grad_f;
    end
end

% figure;
% imshow(gradientmap/256);

%finding edge direction
rad_10 = 10*pi/180;
% rad_45 = 45*pi/180;
rad_60 = 60*pi/180;
rad_110 = 110*pi/180;
% rad_135 = 135*pi/180;
edge_direc = zeros(size(grad_direction));
for i = 2:Height-1
    for j = 2:Width-1
        if (grad_direction(i,j) <= rad_10)
            edge_direc(i,j) = 0;    %0 degrees
        else if(grad_direction(i,j) <= rad_60)
                edge_direc(i,j) = 45;
            else if (grad_direction(i,j) <= rad_110)
                    edge_direc(i,j) = 90;
                else
                    edge_direc(i,j) = 135;
                end
            end
        end
    end
end

%find the possible edge pixels:

poss_edge = zeros(size(edge_direc));
dif_total =0;
for i = 2:Height-1
    for j = 2:Width-1
        switch(edge_direc(i,j))
            case 0
                if(gradientmap(i,j)>gradientmap(i,j-1) && gradientmap(i,j)>gradientmap(i,j+1))
                    poss_edge(i,j) = 1;
                    dif_total = dif_total+1;
                end
            case 45
                if(gradientmap(i,j)>gradientmap(i-1,j+1) && gradientmap(i,j)>gradientmap(i+1,j-1))
                    poss_edge(i,j) = 1;
                    dif_total = dif_total+1;
                end
            case 90
                if(gradientmap(i,j)>gradientmap(i-1,j) && gradientmap(i,j)>gradientmap(i+1,j))
                    poss_edge(i,j) = 1;
                    dif_total = dif_total+1;
                end
            case 135
                if(gradientmap(i,j)>gradientmap(i-1,j-1) && gradientmap(i,j)>gradientmap(i+1,j+1))
                    poss_edge(i,j) = 1;
                    dif_total = dif_total+1;
                end
        end
    end
end

% figure;
% imshow(poss_edge);

%histogram of the gradient map
[counts,x] = imhist(gradientmap);

%finding the differential values
diff = zeros(1,256);
diff(1) = counts(1);
for i = 2:256
    diff(i) = counts(i) - counts(i-1);
end

%gradient histogram difference diagram
% plot(x*256,diff);
Th_max = 255;
for i =256:1
    if(diff(i) == 0)
        Th_max = i-1;
    end
end

%classifying the image based on amount of edge information
% Th_max = 256
amt_edgeinfo = 0;
for i = 1:Height
    for j = 1:Width
        if(gradientmap(i,j)==0)
            amt_edgeinfo = amt_edgeinfo+1;
        end
    end
end
perimeter = 2*(Height+Width);

%Calculating the high and low thresholds
Th_h = 0;
Th_l = 0;
formula = zeros(1,256);
for i = 2:256
    formula(i) = (diff(i-1)+diff(i)>0 || diff(i) + diff(i+1)>0);
end

if(amt_edgeinfo < 5*perimeter)      %little edge information image
    
    j = 1;
    for i = 2:Th_max
        if(formula(i) == 1)
            high_thresh_poss(j) = i;
            j = j+1;
        end
    end
    
    Th_h = max(high_thresh_poss);
    Th_l = Th_h/2;
end

if(amt_edgeinfo >= 5*perimeter || amt_edgeinfo < 20*perimeter)  %rich edge information image
    j = 1;
    dif_5C = 5*perimeter;
    
    high_thresh_poss = [];
    for i = 3:Th_max
        if(formula(i) == 1)
            high_thresh_poss = [high_thresh_poss,i];
        end
    end
    low_thresh_poss = high_thresh_poss;
    Th_h = max(high_thresh_poss);
    Th_l = max(low_thresh_poss);
    high_thresh_poss = sort(high_thresh_poss);
    low_thresh_poss = sort(low_thresh_poss);
    exit_cond1 =0;
    exit_cond2=0;
    
    while(exit_cond1==0 || exit_cond2==0)
        %     while(cnt~=10)
        dif_th = 0;
        for i = 1:Height
            for j = 1:Width
                if((gradientmap(i,j) > Th_h) || (gradientmap(i,j) >Th_l))
                    dif_th = dif_th + Gauss(i,j)/256;
                end
            end
        end
        if dif_th>dif_5C
            exit_cond1 =1;
        else
            s = size(high_thresh_poss,2);
            high_thresh_poss = high_thresh_poss(1:s-1);
        end
        if dif_th>dif_total/2
            exit_cond2 = 1;
        else
            s = size(low_thresh_poss,2);
            low_thresh_poss = low_thresh_poss(1:s-1);
        end
        Th_h = max(high_thresh_poss);
        Th_l = max(low_thresh_poss);
    end
end

if(amt_edgeinfo >= 20*perimeter)  %numerous edge information image
    
    j=1;
    for i = 3:Th_max
        if(formula(i) == 1)
            high_thresh_poss(j) = i;
            j = j+1;
        end
    end
    low_thresh_poss = high_thresh_poss;
    Th_h = max(high_thresh_poss);
    Th_l = max(low_thresh_poss);
    high_thresh_poss = sort(high_thresh_poss);
    low_thresh_poss = sort(low_thresh_poss);
    exit_cond1 =0;
    exit_cond2=0; 
    while(exit_cond1==0 || exit_cond2==0)
        dif_th = 0;
        for i = 1:Height
            for j = 1:Width
                if(gradientmap(i,j) > Th_h || gradientmap(i,j) >Th_l)
                    dif_th = dif_th + Gauss(i,j)/256;
                end
            end
        end
        if dif_th>(dif_total/12)
            exit_cond1 =1;
        else
            s = size(high_thresh_poss,2);
            high_thresh_poss = high_thresh_poss(1:s-1);
        end
        if dif_th>(dif_total/4)
            exit_cond2 = 1;
        else
            s = size(low_thresh_poss,2);
            low_thresh_poss = low_thresh_poss(1:s-1);
        end
        Th_h = max(high_thresh_poss);
        Th_l = max(low_thresh_poss);
        
    end
end
Th_h
Th_l
%forming the edge map
edge_map = zeros(size(gradientmap));
for i = 2:Height-1
    for j = 2:Width-1
        if(gradientmap(i,j) > Th_h && poss_edge(i,j)==1)
            edge_map(i,j) = 1;
            switch(edge_direc(i,j))
                case 0
                    if(gradientmap(i,j-1)>Th_l &&poss_edge(i,j-1)==1)
                        edge_map(i,j-1)=1;
                    end
                    if(gradientmap(i,j+1)>Th_l&&poss_edge(i,j+1)==1)
                        edge_map(i,j+1)=1;
                    end
                case 45
                    if(gradientmap(i-1,j+1)>Th_l&&poss_edge(i-1,j+1)==1)
                        edge_map(i-1,j+1)=1;
                    end
                    if(gradientmap(i+1,j-1)>Th_l&&poss_edge(i+1,j-1)==1)
                        edge_map(i+1,j-1)=1;
                    end
                case 90
                    if(gradientmap(i,j+1)>Th_l&&poss_edge(i,j+1)==1)
                        edge_map(i,j+1)=1;
                    end
                    if(gradientmap(i,j-1)>Th_l&&poss_edge(i,j-1)==1)
                        edge_map(i,j-1)=1;
                    end
                case 135
                    if(gradientmap(i-1,j-1)>Th_l&&poss_edge(i-1,j-1)==1)
                        edge_map(i-1,j-1)=1;
                    end
                    if(gradientmap(i+1,j+1)>Th_l&&poss_edge(i+1,j+1)==1)
                        edge_map(i+1,j+1)=1;
                    end
            end
        end
    end
end
% 
% figure;
% imshow(edge_map);

sketch = zeros(size(edge_map));
%put original info back into the edge map
for i =1:Height
    for j =1:Width
        if(edge_map(i,j) == 1 && poss_edge(i,j) ==1)
            sketch(i,j) = G(i,j);
        end
    end
end
figure;
imshow(sketch/256);

%possible addition:
% h = fspecial('gaussian', 5, sigma);
% Gauss_sketch = imfilter(sketch,h);
% figure;
% imshow(Gauss_sketch/256);
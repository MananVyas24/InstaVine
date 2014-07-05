clear all;
close all;
clc;
%% Set Parameters
radius = 2;
grade = 2;
scale = 1;
%% File Select
[FileName,PathName] = uigetfile({'*.jpg';'*.jpeg';'*.JPG'},'SelectImage');
file = [PathName,FileName];
img = cast(imresize(imread(file),scale),'double');
%% Focal point and processing parameters
[r c d] = size(img);
IMG = zeros(r,c,d);
h = figure; imshow(cast(img,'uint8'));
X = 130;
Y = 135;
c0 = floor(X(1));
r0 = floor(Y(1));
c_ = max(c0,(c-c0));
r_ = max(r0,(r-r0));
L = sqrt(r_^2 + c_^2);
close(h);
%% F plot
x = 0:0.00001:1;
y = radius*x.^grade;
p = find(y>1);
y(p) = 1;
% h = figure; plot(x,y);
% pause;
% close(h);
%% Processing
for i = 1:r
  for j = 1:c
    Ln = sqrt((r0-i)^2 + (c0-j)^2);
    f = (L-Ln)./L;
    f = (radius*f^grade);
    if(f>1)
      f=1;
    end
    IMG(i,j,:) = f*img(i,j,:);
  end
end
%% Display
imG = cast(IMG,'uint8');
imshow(imG);
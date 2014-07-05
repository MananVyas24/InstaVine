function segment=skin(I)

I=double(I);
[hue,s,v]=rgb2hsv(I);

cb =  0.148* I(:,:,1) - 0.291* I(:,:,2) + 0.439 * I(:,:,3) + 128;
cr =  0.439 * I(:,:,1) - 0.368 * I(:,:,2) -0.071 * I(:,:,3) + 128;
[w h]=size(I(:,:,1));

for i=1:w
    for j=1:h            
        if  140<=cr(i,j) && cr(i,j)<=165 && 140<=cb(i,j) && cb(i,j)<=195 && 0.01<=hue(i,j) && hue(i,j)<=0.1     
            segment(i,j)=1;            
        else       
            segment(i,j)=0;    
        end    
    end
end

[height,width,BytesPerPixel] = size(I);


% imshow(segment);
im(:,:,1)=I(:,:,1).*segment;   
im(:,:,2)=I(:,:,2).*segment; 
im(:,:,3)=I(:,:,3).*segment; 
figure;imshow(uint8(im));

% Binarize
for i=1:1:height
    for j=1:1:width
        
        if ( im(i,j) ~=0 )
            Bin_img(i,j)= 1;
            
        else
            Bin_img(i,j)= 0;
        end
    end
end


% avegrage of the non 0 vectors from the center
average_vector_hor = 0.0;
average_vector_vert = 0.0;

[height, width, depth] = size (Bin_img);

for i=1:1:height
    for j=1:1:width
        
        if ((Bin_img(i,j) ~= 0))
           
            average_vector_hor = (average_vector_hor) + (j-(width/2));
            average_vector_vert = (average_vector_vert) + (i-(height/2));
            
        end
        
    end
end

a = average_vector_hor
b = average_vector_vert 
average_vector_hor = a/(width*height)
average_vector_vert = b/(height*width)

%% Set Parameters
radius = 0.2;
grade = 0.5;
% scale = 0.5;
% %% File Select
% [FileName,PathName] = uigetfile({'*.jpg';'*.jpeg';'*.JPG'},'SelectImage');
% file = [PathName,FileName];
% img = cast(imresize(imread(file),scale),'double');
%% Focal point and processing parameters
% [r c d] = size(img);
IMG = zeros(height,width,depth);
% h = figure; imshow(cast(img,'uint8'));
% [X Y] = getpts(h);
% c0 = floor(X(1));
% r0 = floor(Y(1));
% c_ = max(c0,(c-c0));
% r_ = max(r0,(r-r0));
L = sqrt(((height/2)-average_vector_vert)^2 + ((width/2)-average_vector_hor)^2);
% close(h);
%% F plot
x = 0:0.00001:1;
y = radius*x.^grade;
p = find(y>1);
y(p) = 1;
h = figure; plot(x,y);
% pause;
% close(h);
%% Processing
for i = 1:height
  for j = 1:width
    Ln = sqrt((average_vector_vert-i)^2 + (average_vector_hor-j)^2);
    f = (L-Ln)./L
    f = (radius*f^grade)
    if(f>1)
      f=1;
    end
    IMG(i,j,1) = (f)*I(i,j,1);
    IMG(i,j,2) = (f)*I(i,j,2);
    IMG(i,j,3) = (f)*I(i,j,3);
  end
end
%% Display
imG = cast(IMG,'uint8');
imshow(imG);



  

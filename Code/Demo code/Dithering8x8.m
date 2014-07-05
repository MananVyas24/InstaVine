function Output = Dithering8x8(X,mode)
%   GENERATES a COLOR HALFTONED IMAGE using 8x8 DITHERING MATRIX FOR
%   A GIVEN INPUT IMAGE 'X' and Mode selects the dithering Matriz

% define a dithering Matrix and its corresponding threshold matrix
% 8x8
if (mode == 8)
    D = [   0, 32, 8, 40, 2, 34, 10, 42; 
            48, 16, 56, 24, 50, 18, 58, 26; 
            12, 44, 4, 36, 14, 46, 6, 38; 
            60, 28, 52, 20, 62, 30, 54, 22;
            3, 35, 11, 43, 1, 33, 9, 41;
            51, 19, 59, 27, 49, 17, 57, 25;
            15, 47, 7, 39, 13, 45, 5, 37;
            63, 31, 55, 23, 61, 29, 53, 21    
          ];
     T = zeros(size(D));
else % Default Dithering by 4x4 Matrix
    D =  [ 5,9,6,10;
           13,1,14,2;
           7,11,4,8;
           15,3,12,0
         ];
    T = zeros(size(D));
    mode = 4;
end

% extract image size
[height,width] = size(X);
% Define output and threshold Matrices
Output = zeros(size(X));
switch size(X,3)

    case 1
    % Processing
    X = double(X);
    T = double(T);
    for i=1:mode:(height-mode)
        for j =1:mode:(width-mode)
            for h=1:1:mode
                for k=1:1:mode
                    T(h,k) = (D(h,k)+0.5)*255/(mode*mode);
                    if X(i+h,j+k)>=T(h,k)
                        Output(i+h,j+k) = 255;
                    else
                        Output(i+h,j+k) = 0;                    
                    end
                end
            end
        end
    end
    otherwise
        for i = 1:size(X,3)
            tD = ColorHalftoning(X(:,:,i),mode);
            Output(:,:,i) = tD;
        end      
end
Output = uint8(Output);
end


function [ C ] = Cartoonize(I)

% I = original Image
% This function calls the Bilateral_Filter function , so dont give the
% Bilateral Filtered images as its input
% -------------------------------------------------------------------------
% Creates a cartoon Effect on I to give OutImg
% by applying a Bilteral Filter and Egde map on the input image
% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
% References : 
% [1] : (online) page on Bilateral Filter as on 27th Februaury 2014
% <http://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/MANDUCHI1/Bilateral_Filtering.html>
% [2] : (online) http://stackoverflow.com/questions/8772999/an-algorithm-for-edge-detection-without-color-losing
% [3] : K.N. Chaudhury, D. Sage, and M. Unser, "Fast O(1) bilateral  filtering using
%   trigonometric range kernels," IEEE Transactions on Image Processing, vol. 20,
%   no. 11, 2011.
% *************************************************************************

B = Bilateral_Filter(I,20,30);
B= double(B);
% Initalize thresholds for edge map
max_grad      = 0.05;    % maximum gradient 
min_strength = 0.3;    % minimum gradient 

% Gradient Magnitudes 
[GX,GY] = gradient(B(:,:,1)/100);
G = sqrt(GX.^2+GY.^2);
G(G>max_grad) = max_grad;
G = G/max_grad;

% Edge map using Gradient Magnitudes.
E = G; E(E<min_strength) = 0;

% Determine per-pixel egde weighting 
% Gives Better result than just using G !
S = 7*G; % Determined by Trial and Error 

qB = B; dq = 100/(8-1);% Edges quantized into 8 different strengths
qB(:,:,1) = (1/dq)*qB(:,:,1);
qB(:,:,1) = dq*round(qB(:,:,1));
qB(:,:,1) = qB(:,:,1)+(dq/2)*tanh(S.*(B(:,:,1)-qB(:,:,1))); % Quantized edge directions

% Add gradient edges to quantized bilaterally-filtered image.
C = repmat(1-E,[1 1 3]).*qB;

C = uint8(C);

end


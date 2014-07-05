function [ outImg ]  =  Bilateral_Filter(inImg, sigmaS, sigmaR)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   filter parameters for Best Output
%   sigmaS = 20;
%   sigmaR = 30;
%   Reference :  K.N. Chaudhury, D. Sage, and M. Unser, "Fast O(1) 
%                bilateral  filtering using
%                trigonometric range kernels,
%                " IEEE Transactions on Image Processing, vol. 20,
%                no. 11, 2011.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

tol = 0.1;

% make odd kernel for the sake of window extension
if (mod(sigmaS,2) == 0)
  w  = sigmaS + 1;
else
  w  = sigmaS;
end

switch size(inImg,3)
    case 1
        inImg = double(inImg);
        % create spatial filter with Gaussian Kernel
        filt     = fspecial('gaussian', [w w], sigmaS);

        % set range interval and the order of raised cosine
        T = -1;
        sym    = (w - 1)/2;
        [m, n] = size(inImg);        
        % Image Extension by mirroring the boundaries
        pad1 =  w*ceil(m/w) - m;
        pad2 =  w*ceil(n/w) - n;

        inImg = padarray(inImg, [pad1 pad2], 'symmetric', 'post');

        template = inImg;

        m = m + pad1;
        n = n + pad2;

        % scan along row
        for i = 1 : m
                L     = zeros(n, 1);
                R     = zeros(n, 1);
                L(1)  = template(i, 1);
                R(n)  = template(i, n);
    
                for k = 2 : n
                    if  mod(k - 1, w) == 0
                        L(k)          = template(i ,  k);
                        R(n - k + 1)  = template(i ,  n - k + 1);
                    else
                        L(k)          = max( L(k-1) , template(i, k) );
                        R(n - k + 1)  = max( R(n - k + 2), template(i, n - k + 1) );
                    end
                end
    
                for k = 1 : n
                    p = k - sym;
                    q = k + sym;
                        if p < 1
                            r = -1;
                        else
                            r = R(p);
                        end
                        if q > n
                            l = -1;
                        else
                            l = L(q);
                        end
                            template(i, k) = max(r,l);
                end
    
        end

        % scan along column
        for j = 1 : n
    
            L    = zeros(m, 1);
            R    = zeros(m, 1);
            L(1) = template(1, j);
            R(m) = template(m, j);
    
            for k = 2 : m 
                if  mod(k - 1, w) == 0
                    L(k)          = template(k, j);
                    R(m - k + 1)  = template(m - k + 1, j);
                else
                    L(k)          = max( L(k - 1), template(k, j) );
                    R(m - k + 1)  = max( R(m - k + 2), template(m - k + 1, j));
                end
            end
    
            for k = 1 : m
                p = k - sym;
                q = k + sym;
                    if p < 1
                        r = -1;
                    else
                        r = R(p);
                    end
                    if q > m
                        l = -1;
                    else
                        l = L(q);
                    end
                temp = max(r,l) - inImg(k, j);
                    if temp > T
                        T = temp;
                    end
            end 
        end
        
    N  =  ceil( 0.405 * (T / sigmaR)^2 );

    gamma    =  1 / (sqrt(N) * sigmaR);
    % twoN     =  2^N;

    % compute truncation for raised cosines
    if tol == 0
        M = 0;
    else
        if sigmaR > 40
            M = 0;
        elseif sigmaR > 10
            sumCoeffs = 0;
                for k = 0 : round(N/2)
                    sumCoeffs = sumCoeffs + nchoosek(N,k)/(2^N);
                    if sumCoeffs > tol/2
                        M = k;
                        break;
                    end
                end
        else
            M = ceil( 0.5 * ( N - sqrt(4*N*log(2/tol)) ) );
        end
    end


    % main filter
    [m, n]   =  size(inImg);
    outImg1  =  zeros(m, n);
    outImg2  =  zeros(m, n);
    outImg   =  zeros(m, n);

    for k = M :1:N - M
    
        coeff = nchoosek(N,k) / (2^N);
    
        temp1  = cos( (2*k - N) * gamma * inImg);
        temp2  = sin( (2*k - N) * gamma * inImg);
    
        phi1 =  imfilter(inImg .* temp1, filt);
        phi2 =  imfilter(inImg .* temp2, filt);
        phi3 =  imfilter(temp1, filt);
        phi4 =  imfilter(temp2, filt);
    
        outImg1 = outImg1 + coeff * ( temp1 .* phi1 +  temp2 .* phi2 );
        outImg2 = outImg2 + coeff * ( temp1 .* phi3 +  temp2 .* phi4 );
    
    end

    % !division by zero
    idx1 = find( outImg2 < 0.0001);
    idx2 = find( outImg2 > 0.0001);

    outImg( idx1 ) = inImg( idx1 );
    outImg( idx2 ) = outImg1( idx2 ) ./ outImg2 (idx2);

    otherwise %RGB image
      for i = 1:size(inImg,3)
         tD = Bilateral_Filter(inImg(:,:,i),sigmaS,sigmaR);
         outImg(:,:,i) = tD;
      end
end
outImg = uint8(outImg);



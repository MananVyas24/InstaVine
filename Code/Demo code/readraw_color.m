% EE569 Homework Assignment #2
% Date: Oct 13, 2013
% Name: Shruti Sharma
% ID: 8846-4947-98
% email: shrutis@usc.edu
% 
% Compiled on MATLAB R2011b
% Question 1 part d


function G = readraw(filename,Height,Width)
%readraw - read RAW format grey scale image of square size into matrix G
% Usage:	G = readraw(filename)

	% Write pixels into matrix
disp(['	Retrieving Image ' filename ' ...']);

	% Get file ID for file
	fid=fopen(filename,'rb');

	% Check if file exists
	if (fid == -1)
	  	error('can not open input image file press CTRL-C to exit \n');
	  	pause
	end

	% Get all the pixels from the image
	pixel = fread(fid, inf, 'uchar');

	% Close file
	fclose(fid);

% 	% Calculate length/width, assuming image is square
% 	[Y X]=size(pixel);
% 	Size=(Y*X);
% 	N=sqrt(Size/3);

	% Construct matrix
	G = zeros(Height,Width,3);

	% Write pixels into matrix
    m = 1;
    for i=1:Height
        for j=1:Width
            G(i,j,1) = pixel(m,1);
            m = m + 3;
        end
    end
    
    m = 2;
    for i=1:Height
        for j=1:Width
            G(i,j,2) = pixel(m,1);
            m = m + 3;
        end
    end
        m = 3;
    for i=1:Height
        for j=1:Width
            G(i,j,3) = pixel(m,1);
            m = m + 3;
        end
    end

end %function

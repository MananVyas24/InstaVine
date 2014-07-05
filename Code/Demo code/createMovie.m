% UTILITY FUNCTION IN MATLAB TO CREATE A '.avi' MOVIE
% REFERENCES:
% [1]:
% http://stackoverflow.com/questions/11051307/approaches-to-create-a-video-in-matlab
% dated 30th Sept 2013
% [2]:
% DISCUSSION PPT SLIDE #23
% [3]:http://stackoverflow.com/questions/2408112/loading-multiple-images-in-matlab
% dated SEPT 30,2013
% NOTE : PLEASE OPEN THE '.AVI' FILE IN WINDOWN MEDIA PLAYER ONLY AND NOT IN ANY
% OTHER MEDIA PLAYER LIKE VLC, MEDIA PLAYER CLASSIC ETC ... 

function createMovie
%# figure
%figure, set(gcf, 'Color','white')
%Z = peaks; surf(Z);  axis tight
%set(gca, 'nextplot','replacechildren', 'Visible','off');
%set(gca, 'nextplot','replacechildren', 'Visible','off');
N = 255;
IMAGES = cell(1,N);
%FNAMEFMT = '%d.raw';
nFrames = 255;
%a(1:nFrames) = struct('cdata',[], 'colormap',[]);
% Load images
for i=1:255
    s = [int2str(i) '.raw'];
    fid = fopen(s,'rb');
    if(fid==1)
        error ('Cannot Open image. Press Ctrl+C to exit\n');
        pause
    end;
    x = fread(fid,inf,'uint8')'; 
    x=x/255;
    fclose(fid);
    
    width=256; height=256; N=width*height;
    
    ind1=1:3:3*N; 
    ind2=2:3:3*N; 
    ind3=3:3:3*N;

    Ir=x(ind1); Ig=x(ind2); Ib=x(ind3);
    Ir=reshape(Ir,width,height);
    Ig=reshape(Ig,width,height);
    Ib=reshape(Ib,width,height);

    figure(1); clf;
    C=zeros(height,width,3);
    C(:,:,1)=Ir';
    C(:,:,2)=Ig';
    C(:,:,3)=Ib';

    
   IMAGES{i} = C;
   axis(gca,'off')
   imagesc(C);
   axis(gca,'off')
   a(i)=getframe(gcf);
   axis(gca,'off')
end


%mov(1:nFrames) = IMAGES{1,:};

%movieObj = VideoWriter('ab.avi');
%mov(1:nFrames) = IMAGES(1:i);
%open (movieObj);

axis(gca,'off')
close(gcf);
%close movieobj;
movie2avi(a,'abc.avi','compression','None','fps',23);

end





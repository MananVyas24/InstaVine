function Cartoon_Filter()

vid = VideoReader('MyVideo.avi');

nFrames = vid.NumberOfFrames;
vidHeight = vid.Height;
vidWidth = vid.Height;

%Movie Structure
mov(1:nFrames) = struct ('cdata',zeros(vidHeight,vidWidth,3,'uint8'),...
                        'colormap',[]);
                    
% Read all the Frames

for i=1:1:nFrames
    mov(i).cdata = read(vid,i);
    
    % Processing on mov(k).cdata
    % Call Dithering Function
    %C = ColorHalftoning((mov(i).cdata),8);
    C = Cartoonize(mov(i).cdata);
    %[C map] = rgb2ind((mov(i).cdata),32);
    % DISPLAY THE VIDEO FRAMES
    %colormap(map);
    IMAGES{i} = C;
    axis(gca,'off')
    imagesc(C);
    axis(gca,'off')
    a(i)=getframe(gcf);
    axis(gca,'off')
    
end    

% SAVE THE MOVIE

axis(gca,'off')
close(gcf);
%close movieobj;
movie2avi(a,'Cartoon_Filtering.avi','compression','None','fps',3);

end


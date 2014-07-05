function Comicbook()
% ComicBook Function : Generates a Comic Book filter on the input video
% Uses the 4x4 and 8x8 Dithering matrices on each video frames
% 

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
    C = Floyd_Steinberg_Error_Diffusion(mov(i).cdata);
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
movie2avi(a,'ComicBookEffect_FloydSteinberg_ErrorDiffusion.avi','compression','None','fps',3);

end


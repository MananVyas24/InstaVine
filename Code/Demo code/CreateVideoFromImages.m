% Create Video from set of Images in jpeg Formats at 23 FPS
% Some of the images belong to
% <http://www.stat.psu.edu/~jiali/index.download.html>
% Make sure that all the images are of same size
% Order of the frames determined by date modified parameter !

ImagesFolder=uigetdir;
jpegFiles = dir(strcat(ImagesFolder,'\*.jpg'));
S = [jpegFiles(:).datenum]; 
[S,S] = sort(S);
jpegFilesS = jpegFiles(S);
VideoFile=strcat(ImagesFolder,'\MyVideo'); % Save as MyVideo.avi
writerObj = VideoWriter(VideoFile);
fps= 3; 
writerObj.FrameRate = fps;
open(writerObj);
for t= 1:length(jpegFilesS)
     Frame=imread(strcat(ImagesFolder,'\',jpegFilesS(t).name));
     writeVideo(writerObj,im2frame(Frame));
end

close(writerObj);
% END OF FILE
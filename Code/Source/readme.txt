focus_gray ---- focuses on the face and grays out everything else
vignette_quarter -- divides screen into 4 and implements vignette effect around the face
Emboss_half -- emboss effect on half the image
ComicBook_half -- comic book effect on half the image
edge_detection_half -- penci sketch on half the image


In Headers folder:
header_file.h - has all the variables and functions required for all the above files



        if ( dip0 == DIP_DOWN && dip1 == DIP_DOWN)
			White_balance();
		else if( dip0 == DIP_DOWN && dip2 == DIP_DOWN)
			sepia();
		else if (dip0 == DIP_DOWN && dip3 == DIP_DOWN)
			color_temp();
		else if (dip1 == DIP_DOWN && dip2 == DIP_DOWN)
			vignette_gray();
		else if (dip1 == DIP_DOWN && dip3 == DIP_DOWN)
			vignette_effect();
		else if (dip2 == DIP_DOWN && dip3 == DIP_DOWN)	
			emboss_effect();
		else if( dip2 == DIP_DOWN)
			comic_book();
		else if(dip3 == DIP_DOWN)
			pencil_sketch();


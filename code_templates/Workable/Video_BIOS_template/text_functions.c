#include "text_functions.h"


extern struct abc_font_struct tahomabold_20[];

//Prints a single character
void print_char(char num, int x_offset, int y_offset, struct abc_font_struct font[], struct display disp){
	//Get char information
	int i,j;
	unsigned char pixel,right_pixel,background;
	unsigned int value = 0;
	
	//print character
	unsigned char * char_data;
	char_data = font[num-33].char_alpha_map;
	
	for(i = 0; i < font[num-33].extents_height; i++)
	{
		if((y_offset+i) < disp.height)
		{//check for boundries
	    	for (j = 0; j < font[num-33].extents_width; j+=2)
	    	{	    		
	    		pixel = (unsigned int)(*char_data);
	    		value = (0x007F007F);//set to black
	    		// Only foreground
	    		if(pixel == 0xFF && ((x_offset+j) < disp.width/2))
	    		{
	    			value = value | ((unsigned int)((pixel)<<8));
	    			disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] = value;
	    		}
	    		// Only background
	    		else if(pixel == 0x00 && ((x_offset+j) < disp.width/2))
	    		{
					value = disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] ;
	    			disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] = value;
	 	    	}
	 	    	// Blend foreground and background
	 	    	else if(pixel > 0x00 && pixel < 0xFF && (x_offset+j) < disp.width/2)
	 	    	{
	 	    		background = (disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] & 0x0000FF00) >> 8;
	 	    		value = ((((0xff - pixel)*background)/(0xff) + (pixel*0xff)/0xff) << 8) | 0x7F;	 	    		
	 	    		disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] = value;
	 	    	}
	 	    	
	    		//printf("0x%x ",character);
	    		char_data++;
	    		
	    		if((j+1) < font[num-33].extents_width)
	    		{
	    			pixel = (unsigned int)(*char_data);
	    			right_pixel = (unsigned int)(*(char_data-1));
	    			background = (disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] & 0xFF000000) >> 24;
	    			
//	    			if(pixel != 0 && ((x_offset+j) < disp.width/2)){
//	    				value = value | ((unsigned int)((pixel)<<24));
//	    				disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] = value;
//	    			}// else {
//	    			//	buffer_out[(y_offset+i)*vWidth/2 + x_offset+j+1] = 0x007F007F;
//	    			//}
//	    			
	    			
				    if(pixel == 0xFF && ((x_offset+j) < disp.width/2))
				    {
	    				value = (value & 0x0000FF00) | 0x007F007F;
	    				value = value | ((unsigned int)((pixel)<<24));
	    				disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] = value;
	    			}
	    			// Added the following 2 cases to blend background and character edges for anti-aliasing (Ady)
	    			else if(pixel == 0x00 && ((x_offset+j) < disp.width/2))
	    			{
	    				if(right_pixel > 0x00)
	    				{
	    					value = (value & 0x0000FF00) | 0x007F007F;
	    					value = value | (background << 24);
	    				}
	    				else
	    				{
	    					value = disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2];
	    				}
	    				disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] = value;
	 	    		}
	 	    		else if(pixel > 0x00 && pixel < 0xFF && (x_offset+j) < disp.width/2)
	 	    		{
	 	    			value = (value & 0x0000FF00) | 0x007F007F;
	    				value = value | ((((0xff - pixel)*background)/(0xff) + (pixel*0xff)/0xff) << 24);
	    				disp.buffer[(y_offset+i)*disp.width/2 + x_offset+j/2] = value;
	 	    			
	 	    		}
	    			
		    		char_data++;
	    			//printf("0x%x ",character);
	    		}
	    	}//end for j
		}//end if(boundries)
	}//end for i
}

//Prints a string of characters
//void print_string(char str[], int x_offset, int y_offset, struct abc_font_struct font[], struct display disp){
void print_string(char str[], int x_offset, int y_offset, struct display disp){	
	struct abc_font_struct *font = tahomabold_20;
	
	int i = 0;
	int start = x_offset;
	while(str[i]){
		if(str[i] == '\n'){
			x_offset = start;
			y_offset += font['|' - 33].extents_height;
			i++;
			continue;
		}
		else if(str[i] == 32){//space character
     		x_offset += font[((int)'-')-33].extents_width/2;
     		if((font[((int)'-')-33].extents_width/2)%2 == 1)
     			x_offset++;
		}else if(str[i] < 32 || str[i] > 126) {
     		print_char(str[i],x_offset,y_offset,font,disp);
     		x_offset += font[((int)str[42])-33].extents_width/2;
     		if((font[((int)str[42])-33].extents_width/2)%2 == 1)
     			x_offset++;
     	}
		else {
     		print_char(str[i],x_offset,y_offset,font,disp);
     		x_offset += font[((int)str[i])-33].extents_width/2;
     		if((font[((int)str[i])-33].extents_width/2)%2 == 1)
     			x_offset++;
     	}
     	i++;
	}
}

//Prints supported text characters
void print_support_text( struct display disp ){
	char str_test[108] = "Supported Letters\nabcdefghijklmnopqrstuvwxyz\nABCDEFGHIJKLMNOPQRSTUVWXYZ\n0123456789\n!\"# $%&\'()*+`-./:;<=>\?@\0";
	//print text that is supported
//	print_string(str_test,20,20,tahomabold_32,disp);//print big font
    print_string(str_test,20,20,disp);//print small font
}

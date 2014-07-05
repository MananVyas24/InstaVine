#include "header_file.h"

void pencil_sketch(void)
{
	//int buf,buf1;
	//Uint32 buf2,buf3;
	int i,j,m;//,ext=WINDOW_SIZE/2;
	//int diff[256];
	//int count_hist[256];
	//int pixel_val[256];
	int amt_edgeinfo,perimeter,Th_max;
	//unsigned char window[WINDOW_SIZE][WINDOW_SIZE];
	//int *inp= (int*)buffer_out;

	//convert image to gray scale by extracting only the y components
			grayscale();
	//gaussian_filter();
			gradient();
			//find possible edges and dif_total
			    calc_poss_edges();

			    //find the histogram

			    for(i=0;i<255;i++)
			    {
			    	pixel_val[i] = i;
			    }
			    histogram();

			    //find differential value

			    diff[0] = count_hist[0];
			    for(i=1;i<256;i++)
			    	diff[i] = count_hist[i] - count_hist[i-1];

			    //gradient histogram difference diagram
			    Th_max = 255;
			    for(i=255;i>=0;i--)
			    {
			    	if(diff[i] == 0)
			    		Th_max = i-1;
			    }

			    //classifying the image based on amount of edge information
			    amt_edgeinfo = 0;
			    for(i=0;i<vHeight;i++)
			    {
			    	for(j=HalfWidth;j<vWidth;j++)
			    	{
			    		if((int)gradient_mag[i*vWidth+j]!=0)
			    			amt_edgeinfo++;

			    	}
			    }
			    perimeter = 2*(vHeight+HalfWidth);

			    //calculating high and low thresholds
			    dif_total = 0;
			    	calc_thresholds(amt_edgeinfo,perimeter,Th_max);




			    edge_mapping();

			    sketch();

	/*			for(i=0;i<vHeight;i++)
				{
					for(j=0;j<vWidth;j++)
					{
						poss_edge[i*vWidth+j] *=255;
					}
				}*/
				//cout<<Th_h<<" "<<Th_l<<endl;


			    m=0;
		    //display the sketch
			for(i = 0; i<vHeight; i++)
			{
				for(j = 0; j<vWidth/2; j++)
				{
					//buf = (int)buffer_gray[i*vWidth/2+(2*j+1)];
					//buf1 = (int)buffer_gray[i*vWidth/2+(2*j+0)];

					if(j<HalfWidth/2)
					{
						buffer_proc2[i*vWidth/2 +j] = buffer_proc1[i*vWidth/2+j];
					}
					else
					{
						if(i>=332 && i<408 && j>=274 && j<=337 )
						{
							buffer_proc2[i*vWidth/2 +j] = logo[m];
							m++;
						}
						else
						{
							buffer_proc2[i*vWidth/2 +j] = (Uint32) ((((int)edge_map[i*vWidth+(2*j+1)]) & 0x000000FF)<<24)|((((int)edge_map[i*vWidth+(2*j+0)]) & 0x000000FF)<<8)| 0x00800080;
						}

					}
					//buf2 = buffer_proc1[i*vWidth/2+j];
					//buf3 = buffer_proc2[i*vWidth/2+j];

				}
			}
			//printf("%d",m);
}

void grayscale(void)
{
	Uint32 i,j;//,buf1,buf2;
	//int buf_temp1,buf_temp2;
	for(i = 0; i < vHeight; i++)
	{

		for(j = HalfWidth/2; j < vWidth/2; j++)
		{

			//buffer_proc2[i*vWidth/2+j] = (buffer_proc1[i*vWidth/2+j] & 0xFF00FF00)| 0x00800080;
			//buf2 = buffer_proc2[i*vWidth/2];

			buffer_gray[i*vWidth+(2*j+1)] = (unsigned char)((buffer_proc1[i*vWidth/2+j] & 0xFF000000)>>24);
			//buf_temp1 = (int)buffer_gray[i*vWidth+(2*j+1)];
			buffer_gray[i*vWidth+(2*j+0)] = (unsigned char)((buffer_proc1[i*vWidth/2+j] & 0x0000FF00)>>8);
			//buf_temp2 = (int)buffer_gray[i*vWidth+(2*j+0)];
		}
	}
}


void  gaussian(int sigma)
 {
     int i,j;
     int ext = WINDOW_SIZE/2;
     int val = -ext;
     double sum = 0;
     int G_x[WINDOW_SIZE][WINDOW_SIZE], G_y[WINDOW_SIZE][WINDOW_SIZE];
     double Gauss[WINDOW_SIZE][WINDOW_SIZE];


     for(i =0;i<WINDOW_SIZE;i++)
     {
         for(j = 0;j<WINDOW_SIZE;j++)
         {
            G_x[i][j]=val;
            G_y[j][i]=val;
         }
         val++;
     }
     for(i=0;i<WINDOW_SIZE;i++)
     {
         for(j=0;j<WINDOW_SIZE;j++)
         {
            double ex = (pow(G_x[i][j],2)/(2*(pow(sigma,2)))) + (pow(G_y[i][j],2)/(2*(pow(sigma,2))));
            Gauss[i][j] = exp(-ex);
            sum += Gauss[i][j];
         }
     }
     //cout<<sum<<endl;
     ////normalization
     for(i=0;i<WINDOW_SIZE;i++)
     {
         for(j=0;j<WINDOW_SIZE;j++)
         {
            Gauss_norm[i][j] = Gauss[i][j]/sum;
            //cout<<Gauss_norm[i][j]<<" ";
         }
         //cout<<endl;
     }
 }

void gaussian_filter()
{
    int i,j,m,n, avg;
    double temp;
    int ext = WINDOW_SIZE/2;
    unsigned char window[WINDOW_SIZE][WINDOW_SIZE];
    double mul[WINDOW_SIZE][WINDOW_SIZE];
    double mul_sum = 0;
    double sum =0.0;

    for(i = ext;i< (vHeight-ext);i++)
    {
          for(j = HalfWidth+ext; j< (vWidth-ext);j++)
          {
            //form a window
            for(m=0;m<WINDOW_SIZE;m++)
            {
                for(n=0;n<WINDOW_SIZE;n++)
                {
	                 window[m][n] = buffer_gray[(i-ext+m)*vWidth+(j-ext+n)];
                }
            }
            temp = 0.0;
            //multiply the window with Gaussian:
            //cout<<"mul"<<endl;

            for(m =0; m<WINDOW_SIZE;m++)
            {
                for(n = 0; n<WINDOW_SIZE; n++)
                {
                    temp = Gauss_norm[m][n];
                    mul[m][n] = window[m][n]*temp;
                    sum+= Gauss_norm[m][n];
                    //cout<<mul[m][n]<<" ";
                }
               // cout<<endl;
            }
            //mul[m/2][n/2] = 0;
            //find the average value with which to replace :
            mul_sum = 0;
            for(m=0; m<WINDOW_SIZE; m++)
            {
                for(n = 0; n<WINDOW_SIZE; n++)
                {
                        mul_sum += mul[m][n];
                }
            }
            //cout<<"mul_sum"<<mul_sum<<endl;
            avg = mul_sum/sum;
            buffer_gray[i*vWidth+j] = mul_sum;
           }
    }


}

void gradient()
{
	int i,j,m,n,ext = WINDOW_SIZE/2;
	double grad_x=0,grad_y=0;
	unsigned char window[WINDOW_SIZE][WINDOW_SIZE];

	for(i = ext;i< (vHeight-ext);i++)
	{
	       for(j = HalfWidth+ext; j< (vWidth-ext);j++)
	       {
	            //form a window
	            for(m=0;m<WINDOW_SIZE;m++)
	            {
	                for(n=0;n<WINDOW_SIZE;n++)
	                {
	                 window[m][n] = buffer_gray[(i-ext+m)*vWidth+(j-ext+n)];
	                }
	            }
	            //sobel filter the window:
	            grad_x = ((window[0][2]+(2*window[1][2])+window[2][2])-(window[0][0]+(2*window[1][0])+window[2][0]));
	            grad_y = ((window[0][0]+(2*window[0][1])+window[0][2])-(window[2][0]+(2*window[2][1])+window[2][2]));
	            //cout<<sqrt(pow(grad_x,2)+pow(grad_y,2))<<endl;
	            gradient_mag[i*vWidth+j] = (unsigned char) sqrt(pow(grad_x,2)+pow(grad_y,2));
	            //cout<<(int)gradient_mag[i*vWidth+j]<<endl;

	            if(grad_x!=0)
	            	gradient_direc[i*vWidth+j] = (atan(grad_y/grad_x))*180*7/22; // in degree

	            else
	            {
	            	if(grad_y==0)
	            		gradient_direc[i*vWidth+j] = (unsigned char)0;
	            	else
	            		gradient_direc[i*vWidth+j] = (unsigned char)90;
	            }
	            if(gradient_direc[i*vWidth+j]<=(unsigned char)10)
	            	gradient_direc[i*vWidth+j] = (unsigned char)0;
	            else if(gradient_direc[i*vWidth+j]<=(unsigned char)60)
	            	gradient_direc[i*vWidth+j] = (unsigned char)45;
	            else if(gradient_direc[i*vWidth+j]<=(unsigned char)110)
	            	gradient_direc[i*vWidth+j] = (unsigned char)90;
	            else
	            	gradient_direc[i*vWidth+j] = (unsigned char)135;

	            //cout<<gradient_direc[i*vWidth+j]<<endl;
	       }
	}
}

void calc_poss_edges()
{
	int i,j;
	for(i=0;i<vHeight;i++)
	{
		for(j=HalfWidth;j<vWidth;j++)
		{
			//cout<<(int)gradient_direc[i*vWidth+j]<<endl;
			switch((int)gradient_direc[i*vWidth+j])
			{
				case 0:
					if((gradient_mag[i*vWidth+j]>gradient_mag[i*vWidth+(j-1)]) && (gradient_mag[i*vWidth+j]>gradient_mag[i*vWidth+(j+1)]))
					{
						poss_edge[i*vWidth+j] = 1;
						dif_total++;

					}
					break;
				case 45:
					if((gradient_mag[i*vWidth+j]>gradient_mag[(i-1)*vWidth+(j+1)]) && (gradient_mag[i*vWidth+j]>gradient_mag[(i+1)*vWidth+(j-1)]))
					{
						poss_edge[i*vWidth+j] = 1;
						dif_total++;

					}
					break;
				case 90:
					if((gradient_mag[i*vWidth+j]>gradient_mag[(i-1)*vWidth+j]) && (gradient_mag[i*vWidth+j]>gradient_mag[(i+1)*vWidth+j]))
					{
						poss_edge[i*vWidth+j] = 1;
						dif_total ++;

					}
					break;
				case 135:
					if((gradient_mag[i*vWidth+j]>gradient_mag[(i-1)*vWidth+(j-1)]) && (gradient_mag[i*vWidth+j]>gradient_mag[(i+1)*vWidth+(j+1)]))
					{
						poss_edge[i*vWidth+j] = 1;
						dif_total++;

					}
					break;

			}
		}
	}

}

int histogram()
{
	int i,j,val;

    //FILE *file;
    //initialize histogram to all zeros.
    for (i = 0; i < 256; i++)
    {
        count_hist[i] = 0;
    }
    for(i = 0;i<vHeight;i++)
    {
        for(j = HalfWidth;j<vWidth;j++)
        {
                    val = gradient_mag[i*vWidth+j];
                    count_hist[val] +=1;
        }
    }

    return 0;
}

void calc_thresholds(int amt_edgeinfo,int perimeter,int Th_max)
{

	int x;

	//int formula_array[256];
	Th_h = 0;
	Th_l = 0;

	//printf("Hey\n");
	//printf("Hey\n");
	//printf("%d\n",xtra);
	formula_array[0] = 0;
	for(x=1;x<256-1;x++)
	{
		//printf("%d\n",x);
		if(((diff[x-1]+diff[x])>0) || ((diff[x] + diff[x+1])>0))
		{
			formula_array[x] = 1;
		}
		else
		{
			formula_array[x] = 0;
		}
		//formula_array[i] = ((((diff[i-1]+diff[i])>0) || ((diff[i] + diff[i+1])>0))?1:0);
		//printf("%d\n",formula_array[x]);*/
	}
	//printf("k");
	formula_array[x] = (((diff[x-1]+diff[x])>0)?1:0);
	//printf("k");

	//printf("%d %d\n",amt_edgeinfo,perimeter);
	if(amt_edgeinfo < 5*perimeter) // little edge information
	{
		little_edge_info_th(Th_max);
	}

	else if(amt_edgeinfo >= 5*perimeter || amt_edgeinfo < 200*perimeter)  //rich edge information image
	{
		rich_edge_info_th(perimeter,Th_max);
	}

	else if(amt_edgeinfo >= 200*perimeter)
	{
		numerous_edge_info_th(Th_max);
	}

}

void little_edge_info_th(int Th_max)
{
	int i,j = 0;
	//int high_thresh_poss[256] ={0};
	for(i=0;i<=Th_max;i++)
	{
		if(formula_array[i] == 1)
		{
			 high_thresh_poss[j] = i;
			 j++;
		}
	}

//max of high_thres_poss
	Th_h = 0;
	for(i=0;i<256;i++)
	{
		if(high_thresh_poss[i]>Th_h)
			Th_h = high_thresh_poss[i];
	}

	    Th_l = Th_h/2;
	    //printf("%d\n%d",Th_h,Th_l);
}

void rich_edge_info_th(int perimeter,int Th_max)
{
	int i,j = 0;
	int dif_5C = 5*perimeter;
	int s_high,s_low,exit_cond1,exit_cond2;
	int dif_th;
	//int high_thresh_poss[256-3]={0};//,low_thresh_poss[256-3]={0};
	//int htp[256-3]={0},ltp[256-3]={0};

	//printf("in\n");
	for(i = 3;i<=Th_max;i++)
	{
	    if((int)formula_array[i] == 1)
	    {

	        high_thresh_poss[j] = i;
			j++;
	    }
	    else
		{
	    	high_thresh_poss[j] = 0;
			j++;
		}
	    //printf("%d\n",i);
	}

	s_high = j-1;
	s_low = j-1;

	j=0;
	for(i=0;i<s_high;i++)
	{
		if((int)high_thresh_poss[i]!=0)
		{
			htp[j] = high_thresh_poss[i];
			ltp[j] = high_thresh_poss[i];
			j++;
		}
	}

	s_high = j-1;
	s_low = j-1;

	//qsort(high_thresh_poss, s_high, sizeof(int), cmpfunc);
	//qsort(low_thresh_poss, s_low, sizeof(int), cmpfunc);

	Th_h = (int)htp[s_high];
	Th_l = (int)ltp[s_low];
	exit_cond1 = 0;
	exit_cond2 = 0;

	while(exit_cond1==0 || exit_cond2==0)
	{        //    while(cnt~=10)
	        dif_th = 0;
			//cout<<Th_h<<" "<<Th_l<<endl;
	        for(i=0;i<vHeight;i++)
	        {
	            for (j=HalfWidth;j<vWidth;j++)
	            {
	                if(((int)gradient_mag[i*vWidth+j] > Th_h) || ((int)gradient_mag[i*vWidth+j] >Th_l))
					{
	                    dif_th += (((int)buffer_gray[i*vWidth+j]+255)/255);
						//dif_th++;
		//				cout<<dif_th<<endl;

					}
	            }
	        }
			//cout<<dif_th<<" "<<dif_5C<<" "<<dif_total/2<<endl;
	        //printf("%d %d %d\n",dif_th,dif_5C,(dif_total/2));

			//cout<<s_high<<" "<<s_low<<endl;
	        if (dif_th>dif_5C)
	        {
	        	exit_cond1 =1;
				//cout<<"in"<<endl;
	        }
	        else
	        {
	            s_high= s_high-1;
	        }

	        if (dif_th>dif_total/2)
	        {
	            exit_cond2 = 1;
				//cout<<"out"<<endl;
	        }
	        else
	        {
	            s_low = s_low-1;
	        }
			//if(s_high <0)
				//s_high = 256-3;
			//if(s_low <0)
				//s_low = 256-3;
	        //qsort(htp, s_high, sizeof(int), cmpfunc);
	        //qsort(ltp, s_low, sizeof(int), cmpfunc);
	        Th_h = (int)htp[s_high];

	        Th_l = (int)ltp[s_low];
	    	//printf("%d %d\n",Th_h,Th_l);

	}
}

void numerous_edge_info_th(int Th_max)
{
	int i,j = 0;

	int s_high,s_low,exit_cond1,exit_cond2;
	int dif_th;
	//int high_thresh_poss[256-3]={0};//,low_thresh_poss[256-3]={0};
	//int htp[256-3]={0},ltp[256-3]={0};

	//printf("in\n");
	for(i = 3;i<=Th_max;i++)
	{
	    if((int)formula_array[i] == 1)
	    {

	        high_thresh_poss[j] = i;
			j++;
	    }
	    else
		{
	    	high_thresh_poss[j] = 0;
			j++;
		}
	    //printf("%d\n",i);
	}

	s_high = j-1;
	s_low = j-1;

	j=0;
	for(i=0;i<s_high;i++)
	{
		if((int)high_thresh_poss[i]!=0)
		{
			htp[j] = high_thresh_poss[i];
			ltp[j] = high_thresh_poss[i];
			j++;
		}
	}

	s_high = j-1;
	s_low = j-1;

	//qsort(high_thresh_poss, s_high, sizeof(int), cmpfunc);
	//qsort(low_thresh_poss, s_low, sizeof(int), cmpfunc);

	Th_h = (int)htp[s_high];
	Th_l = (int)ltp[s_low];
	exit_cond1 = 0;
	exit_cond2 = 0;

	while(exit_cond1==0 || exit_cond2==0)
	{        //    while(cnt~=10)
	        dif_th = 0;
			//cout<<Th_h<<" "<<Th_l<<endl;
	        for(i=0;i<vHeight;i++)
	        {
	            for (j=HalfWidth;j<vWidth;j++)
	            {
	                if(((int)gradient_mag[i*vWidth+j] > Th_h) || ((int)gradient_mag[i*vWidth+j] >Th_l))
					{
	                    dif_th += (((int)buffer_gray[i*vWidth+j]+255)/255);
						//dif_th++;
		//				cout<<dif_th<<endl;

					}
	            }
	        }
			//cout<<dif_th<<" "<<dif_5C<<" "<<dif_total/2<<endl;
	        //printf("%d %d %d\n",dif_th,dif_5C,(dif_total/2));

			//cout<<s_high<<" "<<s_low<<endl;
	        if (dif_th>dif_total/12)
	        {
	        	exit_cond1 =1;
				//cout<<"in"<<endl;
	        }
	        else
	        {
	            s_high= s_high-1;
	        }

	        if (dif_th>dif_total/4)
	        {
	            exit_cond2 = 1;
				//cout<<"out"<<endl;
	        }
	        else
	        {
	            s_low = s_low-1;
	        }
			//if(s_high <0)
				//s_high = 256-3;
			//if(s_low <0)
				//s_low = 256-3;
	        //qsort(htp, s_high, sizeof(int), cmpfunc);
	        //qsort(ltp, s_low, sizeof(int), cmpfunc);
	        Th_h = (int)htp[s_high];

	        Th_l = (int)ltp[s_low];
	    	//printf("%d %d\n",Th_h,Th_l);

	}
}

void edge_mapping()
{
	int i,j;
	//printf("in\n");
	for (i=1;i<vHeight-1;i++)
	{
	    for (j=HalfWidth+1;j<vWidth-1;j++)
	    {
	        if(gradient_mag[i*vWidth+j] > Th_h && poss_edge[i*vWidth+j]==1)
	        {
	            edge_map[i*vWidth+j] = 1;
	            switch(gradient_direc[i*vWidth+j])
	            {
	                case 0:
	                    if(gradient_mag[i*vWidth+(j-1)]>Th_l &&poss_edge[i*vWidth+(j-1)]==1)
	                        edge_map[i*vWidth+(j-1)]=1;

	                    if(gradient_mag[i*vWidth+(j+1)]>Th_l&&poss_edge[i*vWidth+(j+1)]==1)
	                        edge_map[i*vWidth+(j+1)]=1;
	                    break;
	                case 45:
	                    if(gradient_mag[(i-1)*vWidth+(j+1)]>Th_l&&poss_edge[(i-1)*vWidth+(j+1)]==1)
	                        edge_map[(i-1)*vWidth+(j+1)]=1;

	                    if(gradient_mag[(i+1)*vWidth+(j-1)]>Th_l&&poss_edge[(i+1)*vWidth+(j-1)]==1)
	                        edge_map[(i+1)*vWidth+(j-1)]=1;
	                    break;
	                case 90:
	                    if(gradient_mag[i*vWidth+(j+1)]>Th_l&&poss_edge[i*vWidth+(j+1)]==1)
	                        edge_map[i*vWidth+(j+1)]=1;

	                    if(gradient_mag[i*vWidth+(j-1)]>Th_l&&poss_edge[i*vWidth+(j-1)]==1)
	                        edge_map[i*vWidth+(j-1)]=1;
	                    break;
	                case 135:
	                    if(gradient_mag[(i-1)*vWidth+(j-1)]>Th_l&&poss_edge[(i-1)*vWidth+(j-1)]==1)
	                        edge_map[(i-1)*vWidth+(j-1)]=1;

	                    if(gradient_mag[(i+1)*vWidth+(j+1)]>Th_l&&poss_edge[(i+1)*vWidth+(j+1)]==1)
	                        edge_map[(i+1)*vWidth+(j+1)]=1;
	                    break;
	            }

	        }
	    }
	}

}

void sketch()
{
	int i,j;
	for(i=0;i<vHeight;i++)
	{
		for(j=HalfWidth;j<vWidth;j++)
		{
			if(edge_map[i*vWidth+j] == 1 && poss_edge[i*vWidth+j] ==1)
				edge_map[i*vWidth+j] = buffer_gray[i*vWidth+j];
			else
				edge_map[i*vWidth+j] = 255;
		}
	}
}


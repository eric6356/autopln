#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define NR_END 1
#define FREE_ARG char*
/*above is for fft*/     

int NHD;
int fft[8][8192];
int pelfft1k[150000];  
double AFETHD,AFEHD2,AFEHD3,AFESIGNAL,AFESNR,AFESFDR,AFESNDR,AFEENOB; 
       
double *dvector(long nl, long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
	   double *v;

	   v=(double *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(double)));
	   if (!v) printf("allocation failure in dvector()");
	   return v-nl+NR_END;
}

void hanning(double data[], unsigned long nn)
{
 

	unsigned long i;

	for (i=1;i<=nn;i++) {
		data[2*i-1]*= 0.5*(1-cos(6.28318530717959*(i-1)/nn)); /*(nn-1)--> nn  modify by gzq 20091224*/
	}

}

void free_dvector(double *v, long nl, long nh)
/* free a double vector allocated with dvector() */
{
	   free((FREE_ARG) (v+nl-NR_END));
}

void modecalc(double data[], double  mode_data[], long length)
{
	   long i;
	
	   for (i=1;i<=length;i++) 
	   {
		  mode_data[i]= 10*log10(pow(data[2*(i-1)+1],2) + pow(data[2*i],2));
	   }
	

}      
      
void dfour1(double data[], unsigned long nn, int isign)
{
	   unsigned long n,mmax,m,j,istep,i;
	   double wtemp,wr,wpr,wpi,wi,theta;
	   double tempr,tempi;

	   n=nn << 1;
	   j=1;
	   for (i=1;i<n;i+=2)
	   {
		  if (j > i)
		  {
			   SWAP(data[j],data[i]);
			   SWAP(data[j+1],data[i+1]);
		  }
		  m=n >> 1;
		  while (m >= 2 && j > m)
		  {
			   j -= m;
			   m >>= 1;
	   }
	   	j += m;
	  }
	  mmax=2;
	  while (n > mmax)
	  {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) 
		{
			for (i=m;i<=n;i+=istep)
			{
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

void thdcalc(double data[],double * signal, double * thd, double * hd2, double * hd3,int feqbin)
{
	/*2nd~10st*/
	  unsigned long i,j;
	  double thdsum;
	  double hd2sum;
	  double hd3sum;
	  double signsum;
 
  
	  thdsum=0.0;
  	signsum=0.0;
	
	  for (i=(feqbin-1);i<=(feqbin+1);i++) 
	  {
		  signsum += pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
	  }
	  
	  for(j=2;j<=NHD;j++)//
	  {
	  	for(i=(j*feqbin-1);i<=(j*feqbin+1);i++)
	  	{
	  	thdsum +=pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
	    }  	
	    
	    if(j==2)
	    {
	      for(i=(j*feqbin-1);i<=(j*feqbin+1);i++)
	  	  {
	  	    hd2sum +=pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
	      }  
	      
	    }  
	    if(j==3)
	    {
	      for(i=(j*feqbin-1);i<=(j*feqbin+1);i++)
	  	  {
	  	    hd3sum +=pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
	      }  	    
	  	
	    }
	  
    }

	*signal = 10*log10(signsum/10.0);

	*thd = 10*log10(signsum/thdsum);
    *hd2 = 10*log10(signsum/hd2sum);
	*hd3 = 10*log10(signsum/hd3sum);


}


fft_tran(int NN,int feqbin,int socket,double * THD,double * HD2,double * HD3,double * SIGNAL,double * SNR,double * SFDR, double * SNDR,double * ENOB, int NUMTH,int mainslob,int sideslob)
{


	       long i,j,k;
	       int isign;

         double *data, thd,hd2,hd3,signal,snr,sndr,sfdr,enob,*datapower;
         double thdsum,signsum,hd2sum,hd3sum,snrsum,sndrsum,sfdrsum,signone;
	       long NN2;
         NHD=NUMTH;

	       
         NN2=2*NN;
         feqbin=feqbin+1;// new
       
  	     data=dvector(1,NN2);
         datapower=dvector(1,NN2);
       
       
       	 for(i=1;i<=NN;i++)
	       {
		       data[i*2-1]=fft[socket][i-1];//SOCKET
		       data[i*2]=0.0;
       
		   
	      	}
       
       
	       hanning(data, NN);
	     
       
	       isign=1;
	       dfour1(data,NN,isign);
	     
		     for (i=1;i<=NN/2;i++)
	       {
	        	datapower[i] = pow(data[2*(i-1)+1],2) + pow(data[2*i],2);	
            if(strcmp(show_flag,"yes")==0)  
               printf("%f\n",10*log10(datapower[i]));
         }
       
///THD HD2,HD3 START
 
  
	  thdsum=0.0;
  	signsum=0.0;
    hd2sum=0.0;	
    hd3sum=0.0;	
	  for (i=(feqbin-mainslob);i<=(feqbin+mainslob);i++) 
	  {
		  signsum += pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
	  }
	  
	  for(j=2;j<=NHD;j++)//
	  {
       if(j*feqbin>=(NN/2))
         break;
	  	
	  	for(i=(j*feqbin-sideslob);i<=(j*feqbin+sideslob);i++)//bin
	  	{
	  	thdsum +=pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
	    }  	
	    
	    if(j==2)
	    {
	      for(i=(j*feqbin-sideslob);i<=(j*feqbin+sideslob);i++)
	  	  {
	  	    hd2sum +=pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
	      }  
	      
	    }  
	    if(j==3)
	    {
	      for(i=(j*feqbin-sideslob);i<=(j*feqbin+sideslob);i++)
	  	  {
	  	    hd3sum +=pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
	      }  	    
	  	
	    }
	  
    }

	  *SIGNAL = 10*log10(signsum/10.0);

	  *THD = 10*log10(thdsum/signsum);
	  if(2*feqbin<=(NN/2))
      *HD2 = 10*log10(hd2sum/signsum);
    else 
      *HD2=-1;   
   	if(3*feqbin<=(NN/2)) 
	    *HD3 = 10*log10(hd3sum/signsum);   	
    else 
      *HD3=-1;    	


// printf("-1-\n");
 
///THD HD2,HD3 END


///SNDR START
    sndrsum=0;

	  for(i=3;i<=NN/2;i++)//
	  {
	   if((i<(feqbin-1))||(i>(feqbin+1)))
	   sndrsum += pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
    }
 
    *SNDR=10*log10(signsum/sndrsum);
    
 
///SNDR END

///SNR START
    snrsum=sndrsum-thdsum;
 
    *SNR=10*log10(signsum/snrsum);
     
///SNR END     
     
///SFDR START     
 
    sfdrsum=0;  
    signone=0; 
    
    
    
     signone=pow(data[2*(feqbin-1)+1],2) + pow(data[2*feqbin],2);
      
	  for(i=3;i<=NN/2;i++)//
	  {
	  	if(i<(feqbin-2)||i>(feqbin+2))
	  	{ 
	  		
	    if(sfdrsum<pow(data[2*(i-1)+1],2) + pow(data[2*i],2))
	       {
	       sfdrsum= pow(data[2*(i-1)+1],2) + pow(data[2*i],2);
 
	       }
	    }   
    }       
    
    *SFDR=10*log10(signone/sfdrsum);
    
///SFDR END     
//ENOB START 
 
     *ENOB=(*SNR-1.76)/6.02;   
 
//ENOB END 
	       free_dvector(data,1,NN2);
	       free_dvector(datapower,1,NN2);
	       

}         

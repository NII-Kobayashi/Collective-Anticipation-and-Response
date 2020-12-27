//  Read  statictiscs of parameters (Log mean and variance) and
//    calculate hyper-parameters
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 

void  calc_hyp( int  num,  const double  par[], const double  cum[], double hyp[] );


int main(int argc, char *argv[])
{   
  if( argc != 8 )  
    {   printf("Usage:./a.out  *f_dist  $a_n  $b_n  $t_n  $a_p  $b_p  $t_p\n");  exit(1);   }
  
  FILE   *f_in, *f_out; 
  f_in  = fopen( argv[1], "r"); 
  if ( f_in == NULL)    {  printf("Cannot open $f_in\n");  exit(1);   }
  f_out = fopen( "prior.txt", "w");
  if ( f_out == NULL)   {  printf("Cannot open $f_out\n");  exit(1);  }        

  int      i, num;
  double   cum_a[5], cum_b[5], cum_t[5];
   // Read: Statictiscs of parameters
  fscanf( f_in, "%d", &num);
  for ( i=0;  i<5;  i++)     fscanf( f_in, "%lf  ", &cum_a[i]);
  for ( i=0;  i<5;  i++)     fscanf( f_in, "%lf  ", &cum_b[i]);
  for ( i=0;  i<5;  i++)     fscanf( f_in, "%lf  ", &cum_t[i]); 
  
  double   a[2], b[2], tau[2], hyp[3];
  a[0]= atof(argv[2]);     b[0]= atof(argv[3]);     tau[0]= atof(argv[4]);
  a[1]= atof(argv[5]);     b[1]= atof(argv[6]);     tau[1]= atof(argv[7]);
  
  //  Calculate  (c_q, d_q, ¥sigma_q^2) and write "prior.txt"  
  calc_hyp( num, a, cum_a, hyp);     //  q: a_{-,+}  
  fprintf( f_out, "%f  %f  %f\n", hyp[0], hyp[1], hyp[2]);

  calc_hyp( num, b, cum_b, hyp);     //  q: b_{-,+}
  fprintf( f_out, "%f  %f  %f\n", hyp[0], hyp[1], hyp[2]);

  calc_hyp( num, tau, cum_t, hyp);   //  q: tau_{-,+}
  fprintf( f_out, "%f  %f  %f\n", hyp[0], hyp[1], hyp[2]);
  
  fclose(f_in);        fclose(f_out);     return 0;        
}


// Calculate Hyper-parameters: hyp[].  
void  calc_hyp( int  num,  const double  x[], const double  cum[], double hyp[] )
{  
  int   i;    double   ln_x[2], mu[2], cov[3];
  for (i=0; i<2; i++)
    {
      ln_x[i]= 0;
      if (  x[i]> 1  )   {   ln_x[i]= log(x[i]);   }
    }
  
  //  Mean & Covariance: Without Cross-Validation. 
  // mu[0] = cum[0];     mu[1]= cum[1];
  // cov[0]= cum[2];     cov[1]= cum[3];    cov[2]= cum[4];
  
  //  Mean  
  mu[0]= cum[0];     mu[1]=  (num* cum[1]- ln_x[1])/ (num-1);
  //  Covariance  
  cov[0]= cum[2];
  cov[1]= (num* cum[3]- (ln_x[0]-cum[0])* (ln_x[1]-cum[1]) )/ (num-1);
  cov[2]= (num* cum[4]- pow( ln_x[1]-cum[1], 2) )/ (num-1);
  
  // Calculate Hyper-parameters
  hyp[0]= cov[1]/cov[0];     hyp[1]= mu[1]- hyp[0]* mu[0];
  hyp[2]= cov[2]- cov[1]* cov[1]/cov[0];  
}

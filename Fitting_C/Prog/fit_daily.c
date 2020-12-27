// Fiting Exponential + Constant function from a peak.
//    Print: ?? (15//)
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 

#define     N    168
#define   DIM      3

// Solving Linear Equation (Lapack)
int dgesv_(long *, long *, double *, long * , long *, double *, long *, long *);

// Fitting exponential function.
double  fit_exp( int num, const double y[], double par[] );

// Initialize parameters
void  init_par( int num, const double y[], double p_0[]);
// Levenberg–Marquardt algorithm
double  solv_LM( int num, const double y[], double *c_LM, double par[]);  
// Calculate Matrix & Vector for Levenberg–Marquardt algorithm.
double calc_FH( int num, const double y[], const double par[], double z[], double H[] );  

//  Bound for parameters. 
void bounds_par( double par[] );


int main(int argc, char *argv[])
{ 
  if( argc != 2 )
    {   printf("Usage:./a.out  *f_in \n");  exit(1);   }
  
  FILE *f_in, *f_out; 
  f_in  = fopen( argv[1], "r");
  if ( f_in == NULL)    {  printf("Cannot open $f_in\n");  exit(1);   }
  f_out = fopen( "tmp_daily.txt", "w");
  if ( f_out == NULL)   {  printf("Cannot open $f_out\n");  exit(1);  }
  
  int      i, j, num= 7, win= 24;
  double   x_t[2*N+1], x_n[num], x_p[num];

  for (i= 0; i< 2*N+1; i++)   // Read Data.
    {  fscanf( f_in, "%lf", &x_t[i]);  }     
  
  for (i= 0; i< num; i++)     // Calculate Daily Average.
    {   
      x_n[i]= 0;     x_p[i]= 0;
      for ( j=1; j<= win; j++)
	{
	  x_n[i] += x_t[ N- win*i-j ];
	  x_p[i] += x_t[ N+ win*i+j ];
	}
      x_n[i]= x_n[i]/win;     x_p[i]= x_p[i]/win;
    }  
    
  double   err[2], par_n[DIM], par_p[DIM];  
  err[0]= fit_exp( num, x_n, par_n);  
  err[1]= fit_exp( num, x_p, par_p);

  fprintf ( f_out, "%f %f %f    %f %f %f  \n",  par_n[0], par_n[1], par_n[2], par_p[0], par_p[1], par_p[2] );    

  fclose(f_in);        fclose(f_out);     return 0;        
}


// Fitting Exponential function
double  fit_exp( int num, const double y[], double par[] )
{
  int     n_r= 0;
  double  c_LM= 0.001, eps= 1.0;     
  init_par( num, y, par);
  while ( eps> pow(0.1, 4)  )   // Minimizing error.
    {
      // printf("%f  %f\n", par[0], par[1]);
      eps= solv_LM( num, y, &c_LM, par);     n_r++;
      if( n_r> pow(10, 4)  )
        {  printf("# Error (SLow) %f, %f, %f\n", eps, par[0], par[1]);   exit(1);  } 
    }    
  
  // Error
  double  F_x[DIM], J_x[DIM*DIM];
  return    calc_FH( num, y, par, F_x, J_x);  
}


// Initialize parameters via least square method
void  init_par( int num, const double y[], double p_0[] )
{  
  int     i;         long    n= 2, inc=1, info=1;
  long    piv[n];    double  F_x[n], J_x[n*n];  
 
  for (i=0; i< n; i++)  {   F_x[i]= 0;  J_x[n*i]= 0;  J_x[n*i+1]= 0;   }
  p_0[2]= ( y[num-2]+ y[num-1])/2.0;
  p_0[0]= y[0]- p_0[2];          p_0[1]= 1.0;  
  
  for (i=0; i< num; i++)
    {
      if ( y[i]> p_0[2]+ 0.1 )
	{  F_x[0] += log(y[i]-p_0[2]);     F_x[1] += i* log(y[i]-p_0[2]);  }
      else
	{  F_x[0] += log(0.1);             F_x[1] += i* log(0.1);          }
      J_x[1] +=  i;              J_x[3] +=  i* i;
    }
  J_x[0]= num;     J_x[2]= J_x[1];
  
  // Solve least square eqaution:
  dgesv_( &n, &inc, J_x, &n, piv, F_x, &n, &info);
  p_0[0]= exp(F_x[0]);     p_0[1]= (-1.0)/F_x[1];      
  bounds_par( p_0 );       
}


// Fitting exponential function via Levenberg-Marquardt method: 
double  solv_LM( int num, const double y[], double *c_LM, double par[])
{ 
  int     i, j;      long    n= DIM, inc=1, info=1; 
  long    piv[n];    double  err_o, err_n, F_x[n], J_x[n*n]; 
  
  err_o= calc_FH( num, y, par, F_x, J_x);  

  for (i= 0; i<n; i++)  // Modifying Hessian: LM-method
    {
      J_x[ (n+1)*i ]= (1+ (*c_LM) )* J_x[ (n+1)*i ];          //  Diagonal component
      for (j= i+1; j<n; j++)     J_x[ i+n*j ]= J_x[ j+n*i ];  //  Non-Diagonal component
    }   
  
  // Solve Linear Equation: Gauss-Newton method
  dgesv_( &n, &inc, J_x, &n, piv, F_x, &n, &info); 
  
  double  p_o[n], eps= 1;
  if ( info==0 )  // Update parameters
    { 
      for(i=0; i<n; i++)   {  p_o[i]= par[i];   par[i]+= F_x[i];  }      
      // Bounds for a, tau, b
      bounds_par( par );
      
      // Calculate the error using the new parameter set
      err_n= calc_FH( num, y, par, F_x, J_x);

      // eps: Degree of improvement 
      eps= 1.0- err_n/err_o;      
      
      if ( err_n > err_o )   // No improvement:
	{  
	  *c_LM= 10* (*c_LM);   eps= 1;
	  for (i=0; i<n; i++)  {  par[i]= p_o[i];  }
	}
      else   {  *c_LM= (*c_LM)/10;    }
    }
  else       {  *c_LM= 10* (*c_LM);   eps= 1;   }
  
  // Check for the exact singularity. 
  if( info > 0 ) {
    printf( "The diagonal element of the triangular factor of A,\n" );
    printf( "U(%ld,%ld) is zero, so that A is singular;\n", info, info );
    printf( "the solution could not be computed.\n" );
      
    printf( "Par: %f  %f  %f\n", p_o[0], p_o[1], p_o[2] );
    exit(1);
  }  
    
  return eps;
}


// Calculate  Gradient J, Hessian H= J* J^T. 
double calc_FH( int num, const double y[], const double par[], double z[], double H[] )	
{
  int     i, j, k, n= DIM;  
  // Initialization: z= -grad E, H= Hess(E)
  for (i=0; i<n; i++)     {   z[i]= 0;   }
  for (i=0; i<n*n; i++)   {   H[i]= 0;   }
  
  // Calc. Gradient: F_x and Hessian: J_x. 
  double  err, f_x, sum= 0, w[DIM];
  for (i= 0; i<num; i++)
    {     
      f_x= par[0]* exp( (-1)/par[1]*i )+ par[2];     err= y[i]- f_x;
      w[0]= (f_x- par[2])/ par[0];
      w[1]= i* (f_x- par[2])/ (par[1]*par[1]);       w[2]= 1;
      
      // z_k    = ¥sum err* df/d¥theta_k       
      for (j=0; j<n; j++)   {   z[j] += err* w[j];   }      
      // H_{jk} = df/d¥theta_j * df/d¥theta_k 
      for (j=0; j<n; j++)  
	{
	  for (k=0; k<= j; k++)   {  H[j+k*n] += w[j]* w[k];   }	    
	}
      
      sum += err* err;     // sum: Squared error 
    } 
  
  return   sum;
}


void  bounds_par( double par[] )     
{
  int  i;   
  // Lower and Upper bounds for par[0]: a, par[2]: b
  for (i=0; i<3; i++)
    {
      if (  par[i]< 0.001  )       {   par[i]= 0.001;  }
    }

  // Lower and Upper bounds for par[1]: tau    
  if ( par[1]< 0.5   )     {  par[1]= 0.5;  }
  if ( 7<  par[1]  )       {  par[1]= 7;    }
} 

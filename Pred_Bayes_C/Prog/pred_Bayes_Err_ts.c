//  Predicting future response by Bayesian method (ICWSM 2021)
//    Version 201217 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 

#define     N    168
#define   DIM      3     //  Num. Parameters for Fitting
#define     M      6     //  Num. Parameters for Prior Distribution

// Solving Linear Equation (Lapack)
int dgesv_(long *, long *, double *, long * , long *, double *, long *, long *);

// Fitting cyclic function with decay.
double  pred_CycDec( int num, int win, int k_p, const double y_p[], const double p_cyc[], const double p_pri[], double p_res[]);

// Levenberg–Marquardt algorithm
double  solv_LM( int num, int win, int k_p, const double y_p[], double *c_LM, const double p_cyc[], const double p_pri[], double p_res[]);  

// Set Initial condition: Revised version.  (v2012)
void  init_par( int win, int k_p, const double y_p[], const double p_cyc[], double p_res[] );

// Calculate Matrix & Vector for Levenberg–Marquardt algorithm.
double calc_FH_pos( int num, int k_p, const double y[], const double p_cyc[], const double p_res[],
		    double F_x[], double H_x[] );   

// Calculate Mean Relative Error
double  calc_err_MR( int num, int win, int k_p, const double y[], const double p_cyc[], const double p_res[]);

// Initilization
void  init_vect( int num, double v[] );
// Bounds for parameters
void  bounds_par( double par[] );


int main(int argc, char *argv[])
{ 
  if( argc != 10 )
    {   printf("Usage:./a.out  *f_in  *f_out  $k_p  $t_obs  $alpha_c  $t_c  $a_n  $b_n  $t_n\n");  exit(1);   }
  
  FILE   *f_in, *f_out, *f_par; 
  f_in  = fopen( argv[1], "r");
  if ( f_in == NULL)    {  printf("Cannot open $f_in\n");  exit(1);   }
  f_out = fopen( argv[2], "a");
  if ( f_out == NULL)   {  printf("Cannot open $f_out\n");  exit(1);  }
  f_par = fopen( "prior.txt", "r");
  if ( f_par == NULL)   {  printf("Cannot open $f_par\n");  exit(1);  }
 
  int      i, k_p= atoi(argv[3]), win= atoi(argv[4]);
  double   x_t, x_p[N];
  
  for (i=0; i<2*N+1; i++)   // Read  Peak data.
    {      
      fscanf( f_in, "%lf", &x_t);            
      if ( N< i )     x_p[i-N-1]= x_t; 
    }      
  
  double   err, b, r, p_cyc[2], p_pri[M], p_res[DIM], tmp[3];
  p_cyc[0]= atof(argv[5]);     p_cyc[1]= atof(argv[6]);
  tmp[0]=   atof(argv[7]);     tmp[1]=   atof(argv[8]);     tmp[2]= atof(argv[9]);
  if ( tmp[2] <  1  )   {  tmp[2]= 1;   }
  
  for (i=0; i< 3; i++)   // Set  Prior parameters
    {
      fscanf( f_par, "%lf  %lf  %lf", &r, &b, &p_pri[2*i+1]);
      p_pri[2*i]= r* log(tmp[i]) + b;            
    }  
  err=  pred_CycDec( N, win, k_p, x_p, p_cyc, p_pri, p_res);   

  //  Print parameters: Negative & Positive
  fprintf ( f_out, "%s    ",  argv[1] );
  for (i=0; i<DIM; i++)  fprintf ( f_out, "%f  ", p_res[i] );
  fprintf ( f_out, "%f\n",  err );  
  
  fclose(f_in);        fclose(f_out);     fclose(f_par);    return 0;        
}


// Fitting Cyclic function with Decay
double  pred_CycDec( int num, int win, int k_p, const double y_p[], const double p_cyc[], const double p_pri[], double p_res[])	   
{
  int     n_r= 0;
  double  c_LM= 0.001, eps= 1.0;
  
  init_par( win, k_p, y_p, p_cyc, p_res);   // Initilization: v2012        
  
  while ( eps> pow(0.1, 3)  )        // Minimizing error.
    {      
      eps= solv_LM( num, win, k_p, y_p, &c_LM, p_cyc, p_pri, p_res);     n_r++;

      if( n_r> pow(10, 4)  )   //  if( n_r> 1  )      
        {  printf("# Error (SLow) %f:  %f  %f  %f  %f\n", eps, p_res[0], p_res[1], p_res[2], p_res[3]);   exit(1);  }
    }    
  
  // Error: Mean Relative Error
  return    calc_err_MR( num, win, k_p, y_p, p_cyc, p_res );  
}


// Fitting exponential function via Levenberg-Marquardt method: 
double  solv_LM( int num, int win, int k_p, const double y_p[], double *c_LM, const double p_cyc[], const double p_pri[], double p_res[])   
{ 
  int     i, j;       long    n= DIM, inc=1, info=1; 
  long    piv[n];     double  err, var, dz, dF, dH, l_o, l_n, F_x[n], H_x[n*n]; 
  
  err= calc_FH_pos( win, k_p, y_p, p_cyc, p_res, F_x, H_x);    
  var= err/ win;   // Noise Variance

  // Calc. Log Posterior Prob.
  l_o= (-0.5)* win* (1+ log(var) );  
  for (i= 0; i<3; i++)
    {   l_o -= pow( log(p_res[i])- p_pri[2*i], 2 )/ ( 2*p_pri[2*i+1] )+ log(p_res[i]);   }
  
  for (i= 0; i<n; i++)  // Modifying Hessian: Bayes+ LM-method
    {
      //  Diagonal Component: Error     
      F_x[i]= F_x[i]/ var;                   // Gradient
      H_x[(n+1)*i ]= H_x[(n+1)*i ]/ var;     // (-1) * Hessian

      //   Diagonal Component: Prior
      dz= log( p_res[i] )- p_pri[2*i]+ p_pri[2*i+1];
      dF= (-1)* dz/ (p_pri[2*i+1]* p_res[i]);             F_x[i] += dF;      
      dH= (dz-1)/ (p_pri[2*i+1]* p_res[i]* p_res[i]);     H_x[ (n+1)*i ] += (-1)*dH; 
      
      //   LM method
      H_x[ (n+1)*i ]= (1+ (*c_LM) )* H_x[ (n+1)*i ];

      //  Non-Diagonal Component
      for (j= i+1; j<n; j++)    
	{   H_x[ j+n*i ]= H_x[ j+n*i ]/ var;     H_x[ i+n*j ]= H_x[ j+n*i ];   }
    }    
  
  // Solve Linear Equation: Gauss-Newton method
  dgesv_( &n, &inc, H_x, &n, piv, F_x, &n, &info); 
  
  double  p_o[n], eps= 1;
  if ( info==0 )  // Update parameters
    {
      for(i=0; i<n; i++)   {  p_o[i]= p_res[i];   p_res[i]+= F_x[i];     }      
      
      // Lower and Upper bounds for r, phi_0, b, tau     
      bounds_par( p_res );
      
      // Calc. Error by using the new parameter set                  
      err= calc_FH_pos( win, k_p, y_p, p_cyc, p_res, F_x, H_x);          
      var= err/ win;

      // Calc. Log Posterior Prob.
      l_n= (-0.5)* win* (1+ log(var) );   
      for (i= 0; i<3; i++)
	{   l_n -= pow( log(p_res[i])- p_pri[2*i], 2 )/ ( 2*p_pri[2*i+1] )+ log(p_res[i]);   }
       
      eps= fabs( l_o- l_n);     
      
      if ( l_n < l_o )   // No improvement.
	{  
	  *c_LM= 10* (*c_LM);   
	  for (i=0; i<n; i++)  {  p_res[i]= p_o[i];  }
	}
      else   {  *c_LM= (*c_LM)/10;    }
    }
  else       {  *c_LM= 10* (*c_LM);   eps= 1;   }
  
  // Check for the exact singularity. 
  if( info > 0 ) {
    printf( "The diagonal element of the triangular factor of A,\n" );
    printf( "U(%ld,%ld) is zero, so that A is singular;\n", info, info );
    printf( "the solution could not be computed.\n" );          
    exit(1);
  }
  // printf( "Par: %f  %f  %f  (err: %f  %f,  %f) \n", p_res[0], p_res[1], p_res[2], l_n, l_o, *c_LM );  
  return eps;
}


//  Calculate the Gradient and Hessian (t> 0)
//     1) Calculate Gradient: F_x, Hessian: H_x= J* J^T 
//     2) Return Squared Error. 
double calc_FH_pos( int num, int k_p, const double y[], const double p_cyc[], const double p_res[],
		    double F_x[], double H_x[] )
{
  int     i, j, k, n= DIM;    
  double  cyc, dec, exp_x, err, f, ome= M_PI/12.0, sum= 0, J_x[DIM];
  // Initialization
  init_vect( n, F_x);       init_vect( n, J_x);     init_vect( n*n, H_x);
  
  for (i= 0; i< num; i++)
    {      
      cyc= 1+ p_cyc[0]* cos( ome*(k_p+ (i+1)- p_cyc[1]) );
      exp_x= exp( (-1)*(i+1)/p_res[2] );      //  Decay
      dec= p_res[0]* exp_x+ p_res[1];                
      
      //  Fitted function
      f= cyc* dec;      err= y[i]- f;
      
      //  Gradient: J_x[i]= df/d p_res[i]      
      J_x[0]= cyc* exp_x;     J_x[1]= cyc;
      J_x[2]= cyc* p_res[0]* (i+1)/ (p_res[2]*p_res[2])* exp_x;
      
      // F_x[j] = ¥sum err* df/d¥theta_j 
      for (j=0; j<n; j++)   {   F_x[j] += err* J_x[j];   }      
      // H_{jk} = df/d¥theta_j * df/d¥theta_k
      for (j=0; j<n; j++)  
	{
	  for (k=0; k<= j; k++)   {  H_x[j+k*n] += J_x[j]* J_x[k];   }	    
	}
      
      sum += err* err;     // sum: Least Square Error 
    } 
  
  return   sum;
}


// Initialize parameters via least square method:  v2012
void  init_par( int win, int k_p, const double y_p[], const double p_cyc[], double p_res[] )  
{  
  int     i;         long    n= 2, inc=1, info=1;
  long    piv[n];    double  F_x[n], J_x[n*n], *f_p, cyc, y_t, ome= M_PI/12.0;    
  
  f_p= (double*)calloc( win, sizeof(double) );   // Assign  Memory  
  
  for (i=0; i< win; i++)
    {   //  Set baselines for Positive
      cyc    = 1+ p_cyc[0]* cos( ome*(k_p+ (i+1)- p_cyc[1]) );
      f_p[i] = y_p[i]/ cyc;
      
      if      ( i== 0 )            {   p_res[1]= f_p[i];     }
      else    {
	if ( f_p[i] < p_res[1] )   {   p_res[1]= f_p[i];     }
      }
    }
  if ( p_res[1] < 1  )   p_res[1]= 1;
  
  //  Initialization:  F_x, J_x
  init_vect( n, F_x);     init_vect( n*n, J_x);  
  for (i=0; i< n; i++)    {    piv[i]= 0;    }
    
  for (i=0; i< win; i++)
    {
      y_t= f_p[i]- p_res[1];
      if ( y_t < 0.1 )    y_t= 0.1;
      
      F_x[0] += log(y_t);    F_x[1] += (i+1)* log(y_t);      
      J_x[1] += i+1;         J_x[3] += (i+1)* (i+1);
    }
  J_x[0]= win;     J_x[2]= J_x[1];
  
  // Solve least square eqaution:
  dgesv_( &n, &inc, J_x, &n, piv, F_x, &n, &info);
  
  p_res[0]= exp(F_x[0]);     p_res[2]= (-1)/F_x[1];
  if ( p_res[2] < 10  )   p_res[2]= 10;
  
  free(f_p);    
}

// Evaluate Prediction Error
double  calc_err_MR( int num, int win, int k_p, const double y[], const double p_cyc[], const double p_res[])
{
  int     i, n_tot= 0;     
  double  cyc, dec, exp_x, x, err= 0, ome= M_PI/12.0;  
  
  for (i= win;  i< num;  i++)  // MAPE
    {  n_tot += y[i];   }     
  
  for (i= win;  i< num;  i++)   // for (i= 0;  i< num;  i++)
    {      
      cyc= 1+ p_cyc[0]* cos( ome*(k_p+ (i+1)- p_cyc[1]) );
      exp_x= exp( (-1)*(i+1)/p_res[2] );       //  Decay
      dec= p_res[0]* exp_x+ p_res[1];                      
      x= cyc* dec;           
      
      err += fabs(x-y[i])/n_tot;     //  Time seris
    }    
  return    err;
}



void  init_vect( int num, double v[] )
{
  int  i;
  for (i=0; i<num; i++)  {   v[i]= 0;   }
}


void  bounds_par( double p_res[] )     
{
  int  i;    
  for (i=0; i<2;  i++)
    {   //  a_+: p_res[0], b_+: p_res[1]
      if (  p_res[i]< 0.1  )       {   p_res[i]= 0.1;  }
    }
  
  //  tau_+: p_res[2]
  if ( p_res[2]< 0.5   )     {  p_res[2]= 0.5;  }
  if ( 336<  p_res[2]  )     {  p_res[2]= 336;  }
}       

// Fiting Exponential + Constant function from a peak.
//    With peak removal:  Football and Holiday data
//    Print OK: 20/12/23
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 

#define     N    168
#define   DIM      8

//  Detect peaks
void  calc_zt( const double y[],  int z[] );

// Solving Linear Equation (Lapack)
int dgesv_(long *, long *, double *, long * , long *, double *, long *, long *);

// Fitting cyclic function with decay.
double  fit_TiDeH_C( int k_p, const double y_n[], const double y_p[], const int z_n[], const int z_p[], double par[] );
// Levenberg–Marquardt algorithm
double  solv_LM( int k_p, const double y_n[], const double y_p[], const int z_n[], const int z_p[], double *c_LM, double par[]);

// Calculate Matrix & Vector for Levenberg–Marquardt algorithm.
double calc_FH_neg( int k_p, const double y[], const int z[], const double par[], double F_x[], double H_x[] );
double calc_FH_pos( int k_p, const double y[], const int z[], const double par[], double F_x[], double H_x[] );

// Initilization
void  init_vect( int num, double v[] );
// Bounds for parameters
void  bounds_par( double par[] );


int main(int argc, char *argv[])
{ 
  if( argc != 4 )
    {   printf("Usage:./a.out  *f_in  *f_out  $k_p\n");  exit(1);   }
  
  FILE *f_in, *f_out, *f_par; 
  f_in  = fopen( argv[1], "r");
  if ( f_in == NULL)    {  printf("Cannot open $f_in\n");  exit(1);   }
  f_out = fopen( argv[2], "a");
  if ( f_out == NULL)   {  printf("Cannot open $f_out\n");  exit(1);  }
  f_par = fopen( "tmp_daily.txt", "r");
  if ( f_par == NULL)   {  printf("Cannot open $f_par\n");  exit(1);  }

  int      i, k_p= atoi(argv[3]), max= 0;
  double   x_t, x_n[N], x_p[N];  

  for (i=0; i<2*N+1; i++)   // Read  Peak data.
    {      
      fscanf( f_in, "%lf", &x_t);
      if ( i< N )     x_n[N-i-1 ]= x_t; 
      if ( N< i )     x_p[i-N-1 ]= x_t;
      if ( i==N )     max= x_t; 
    } 
  
  //  Detect Peaks in Data.
  int  z_n[N], z_p[N];
  calc_zt( x_n, z_n );     calc_zt( x_p, z_p );
  
  double   err, par[DIM], tmp_p[6];
  for (i=0; i< 6; i++)           // Read  Daily parameters.
    {  fscanf( f_par, "%lf ", &tmp_p[i]);    }
 
  par[0]= 0.2;     par[1]= 18;
  par[2]= tmp_p[0]* exp( 0.45/tmp_p[1] );    par[3]= tmp_p[2];    par[4]= 24* tmp_p[1];  
  par[5]= tmp_p[3]* exp( 0.45/tmp_p[4] );    par[6]= tmp_p[5];    par[7]= 24* tmp_p[4];
  bounds_par( par );  

  //  Fitting paraneters
  err= fit_TiDeH_C( k_p, x_n, x_p, z_n, z_p, par);    

  //  Print parameters: Negative & Positive
  fprintf ( f_out, "%s    ",  argv[1] );
  for (i=0; i<DIM; i++)  fprintf ( f_out, "%f  ",  par[i] );
  fprintf ( f_out, "%f\n",  err );  
  
  fclose(f_in);        fclose(f_out);     fclose(f_par);     return 0;        
}



// Fitting Cyclic function with Decay
double  fit_TiDeH_C( int k_p, const double y_n[], const double y_p[],
		     const int z_n[], const int z_p[], double par[] )		     
{
  int     i, cnt= 0, n_d= DIM, n_r= 0;
  double  c_LM= 0.001, eps= 1.0;       
  while ( eps> pow(0.1, 4)  )   // Minimizing error.
    {
      // printf("%f  %f  %f  %f\n", par[0], par[1], par[2], c_LM);
      eps= solv_LM( k_p, y_n, y_p, z_n, z_p, &c_LM, par);     n_r++;
      if( n_r> pow(10, 4)  )
        {  printf("# Error (SLow) %f:  %f  %f  %f  %f\n", eps, par[0], par[1], par[2], par[3]);   exit(1);  }
    }    
  
  // Error
  double  err= 0, m_y= 0, var= 0, F_x[n_d], H_x[n_d*n_d];
  err= 0;     init_vect( n_d, F_x);     init_vect( n_d*n_d, H_x);  
  err += calc_FH_neg( k_p, y_n, z_n, par, F_x, H_x);
  err += calc_FH_pos( k_p, y_p, z_p, par, F_x, H_x);

  for (i= 0; i<N; i++)
    {  m_y += z_n[i]*y_n[i]+ z_p[i]*y_p[i];    cnt +=  z_n[i]+ z_p[i];    }
  m_y= m_y/ cnt;
  for (i= 0; i<N; i++)
    {  var +=  z_n[i]*pow(y_n[i]-m_y, 2)+ z_p[i]*pow(y_p[i]-m_y, 2);    }
  return    err/var;
}


// Fitting exponential function via Levenberg-Marquardt method: 
double  solv_LM( int k_p, const double y_n[], const double y_p[],
		 const int z_n[], const int z_p[], double *c_LM, double par[])
{ 
  int     i, j;         long    n_d= DIM, inc=1, info=1; 
  long    piv[n_d];     double  err_o, err_n, F_x[n_d], H_x[n_d*n_d ]; 
  
  err_o= 0;     init_vect( n_d, F_x);     init_vect( n_d*n_d, H_x);  
  err_o += calc_FH_neg( k_p, y_n, z_n, par, F_x, H_x);
  err_o += calc_FH_pos( k_p, y_p, z_p, par, F_x, H_x);    
  
  for (i= 0; i<n_d; i++)  // Modifying Hessian: LM-method
    {
      H_x[ (n_d+1)*i ]= (1+ (*c_LM) )* H_x[ (n_d+1)*i ];            // Diagonal
      for (j= i+1; j<n_d; j++)     H_x[ i+n_d*j ]= H_x[ j+n_d*i ];  // Non-Diagonal
    } 
  
  // Solve Linear Equation: Gauss-Newton method
  dgesv_( &n_d, &inc, H_x, &n_d, piv, F_x, &n_d, &info); 
  
  double  p_o[n_d], eps= 1;
  if ( info==0 )  //  Update parameters
    {
      for(i=0; i<n_d; i++)   {  p_o[i]= par[i];   par[i]+= F_x[i];     }      
      // Check bounds for r, phi_0, b, tau     
      bounds_par( par );
      
      // Calculate the error using the new parameter set.
      err_n= 0;     init_vect( n_d, F_x);     init_vect( n_d*n_d, H_x);
      err_n += calc_FH_neg( k_p, y_n, z_n, par, F_x, H_x);
      err_n += calc_FH_pos( k_p, y_p, z_p, par, F_x, H_x);          

      // eps: degree of improvement
      eps= 1.0- err_n/err_o;      
      
      if ( err_n > err_o )   // No improvement
	{  
	  *c_LM= 10* (*c_LM);   eps= 1;
	  for (i=0; i<n_d; i++)  {  par[i]= p_o[i];  }
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
  // printf( "Par: %f  %f  %f  %f %f\n", par[0], par[1], par[2], par[3], par[4] );
  return eps;
}


//  Calculate the Gradient and Hessian (t< 0)
//     1) Calculate Gradient: F_x, Hessian: H_x= J* J^T, 2) Return Squared error.
double calc_FH_neg( int k_p, const double y[], const int z[], const double par[], double F_x[], double H_x[] )	
{
  int     i, j, k, n_d= DIM;     
  double  cyc, dec, exp_x, err, f, phi, sum= 0, J_x[DIM];

  init_vect( n_d, J_x);   
  for (i= 0; i< N; i++) 
    {
      phi= M_PI* (k_p- (i+1)- par[1])/12.0;     //  Phase
      cyc= 1+ par[0]* cos(phi);                  
      exp_x= exp( (-1)*(i+1)/par[4] );          //  Decay
      dec= par[2]* exp_x+ par[3];            
      
      //  Fitted function
      f= cyc* dec;      err= y[i]- f;
      
      //  Gradient: J_x[i]= df/d par[i]
      J_x[0]= cos(phi)* dec;     J_x[1]= M_PI/ 12.0* par[0]* sin(phi)* dec;      
      J_x[2]= cyc* exp_x;        J_x[3]= cyc;
      J_x[4]= cyc* par[2] *(i+1)/ (par[4]*par[4])* exp_x;
      
      // F_x[j] = ¥sum err* df/d¥theta_j 
      for (j=0; j<n_d; j++)         {   F_x[j] += z[i]* err* J_x[j];   }  
      // H_{jk}   = df/d¥theta_j * df/d¥theta_k 
      for (j=0; j<n_d; j++)  
	{
	  for (k=0; k<= j; k++)     {  H_x[j+k*n_d] += z[i]* J_x[j]* J_x[k];   }
	}
      
      sum += z[i]* err* err;     // sum: Squared error 
    } 
  
  return   sum;
}


//  Calculate the Gradient and Hessian (t> 0)
//     1) Calculate Gradient: F_x, Hessian: H_x= J* J^T,   2) Return Squared error.
double calc_FH_pos( int k_p, const double y[], const int z[], const double par[], double F_x[], double H_x[] )	
{
  int     i, j, k, n_d= DIM;    
  double  cyc, dec, exp_x, err, f, phi, sum= 0, J_x[DIM];
  
  init_vect( n_d, J_x);   //  Initialization
  
  for (i= 0; i< N; i++)
    {
      phi= M_PI* (k_p+ (i+1)- par[1])/12.0;     //  Phase
      cyc= 1+ par[0]* cos(phi);                
      exp_x= exp( (-1)*(i+1)/par[7] );          //  Decay
      dec= par[5]* exp_x+ par[6];                
      
      //  Fitted function
      f= cyc* dec;      err= y[i]- f;
      
      //  Gradient: J_x[i]= df/d par[i]
      J_x[0]= cos(phi)* dec;     J_x[1]= M_PI/ 12.0* par[0]* sin(phi)* dec; 
      J_x[5]= cyc* exp_x;        J_x[6]= cyc;
      J_x[7]= cyc* par[5]* (i+1)/ (par[7]*par[7])* exp_x;
      
      // F_x[j] = ¥sum err* df/d¥theta_j 
      for (j= 0; j< n_d; j++)     {   F_x[j] += z[i]* err* J_x[j];   }      
      // H_{jk} = df/d¥theta_j * df/d¥theta_k
      for (j= 0; j< n_d; j++)  
	{
	  for (k=0; k<= j; k++)   {  H_x[j+k*n_d] += z[i]* J_x[j]* J_x[k];   }	    
	}
      
      sum += z[i]* err* err;     // sum: Squared error
    } 
  
  return   sum;
}


void  init_vect( int num, double v[] )
{
  int  i;
  for (i=0; i<num; i++)  {   v[i]= 0;   }
}


void  bounds_par( double par[] )     
{
  int  i;  
  //  r: par[0],  phi_0: par[1].
  if ( par[0]< 0  )    {  par[0]= (-1)* par[0];    par[1] += 12;  }
  if    ( 0.95 < par[0]  )     {   par[0] = 0.95;    } 
  while ( par[1]< 0      )     {   par[1] += 24;     } 
  while ( 24< par[1]     )     {   par[1] -= 24;     }
  
  //  a_-, a_+: par[2], par[5],  b_-, b_+: par[3], par[6]
  for (i=2; i<8;  i++)
    {
      if (  par[i]< 0.001  )       {   par[i]= 0.001;  }
      if (  i==4 || i==7   )
	{  //  tau_-, tau_+: par[4], par[7]
	  if ( par[i]< 0.5   )     {  par[i]= 0.5;  }
	  if ( 336<  par[i]  )     {  par[i]= 336;  }
	}      
    }   
} 


//  Detect Peaks: 
void  calc_zt( const double y[],  int z[] )	
{
  int     i, min= 24;  
  double  mu, sig, theta= 3, cum[2]= {0};
  
  for (i= min; i< N; i++)    {   cum[0] += y[i];    cum[1] += y[i]*y[i];    }
  // 1) Calculate  mean and SD  
  mu = cum[0]/ N;
  sig= sqrt( (cum[1]- N* mu* mu)/ (N-1)  );
  // printf(  "# Mean  %f,  SD  %f\n", mu, sig);
  
  // 2) If y(t) > mean + 3* SD -> Peak (z(t)= 0). 
  for (i= 0; i< N; i++)
    {
      z[i]= 1;
      if ( y[i] > mu+ theta* sig && i >= min  )    {  z[i]= 0;  }      
    }
}

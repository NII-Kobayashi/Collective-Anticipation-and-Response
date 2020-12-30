## C code for predicting response activity by a Bayesian method	
## (Kobayashi, Gildersleve, Uno, Lambiotte, ICWSM 2021)


## A. Requirements

### 1) clapack 

### 2) gnuplot


## B. Scripts for reproducing the results. 

### 1) Pred_Bayes_CV.pl

	Evaluate prediction error of the proposed method for the data set (Table 3 and 10). 
In the default setting, the code evaluates the prediction error of the proposed method for time series (Table 3). It can also evaluate the cumulative error (Table 10) by uncommenting Line 5. In addition, it can also evaluate the error of the Bayesian method utilizing only the anticipation activity by uncommenting Line 49 and commenting out Line 52.

### 2) gen_Fig3.pl
	Plotting the prediction results of the proposed method (Figure 3). 


## C. C codes in the "Prog/" directory

### 1) calc_prior.c

   This code calculates the prior parameters by the leave-one-out cross-validation procedure. 
   
   Type to compile:
   
      make  Prior
   
   Type to execute:
   
     ./a.out  *f_dist  $a_n  $b_n  $t_n  $a_p  $b_p  $t_p  
  
   where
   
     *f_dist:          Mean and variance of the  anticipation and response parameters (Stat/**.txt) 
     
     $a_n, $b_n, $t_n: a_-, b_-, ¥tau_-  ( D_{peak}(t) in Eq. (1) ) fitted from whole the time series. 
     
     $a_p, $b_p, $t_p: a_+, b_+, ¥tau_+  ( D_{peak}(t) in Eq. (1) ) fitted from whole the time series. 


### 2) pred_Bayes_Err_ts.c 
   
   This code estimates the response parameters based on the anticipation and category (prior.txt) and evaluate the time series error (Table 3 and 4). It requires the prior parameters (prior.txt). 
 
   Type to compile:
   
      make  Pred_Bayes_ts

   Type to execute:
   
     ./Pred_Prop  *f_in  *f_out  $k_p  $t_obs  $alpha_c  $t_c  $a_n  $b_n  $t_n
   
   where 
   
     *f_in:            time series file
     
     *f_out:           output file (*f_in, response parameters, and prediction error)
     
     $k_p:             time at the peak (UTF time)
     
     $t_obs:           observation period
     
     $alpha_c, $t_c:   ¥alpha_c, t_c     ( C(t) in Eq. (1) ) fitted before the peak.
     
     $a_n, $b_n, $t_n: a_-, b_-, ¥tau_-  ( D_{peak}(t) in Eq. (1) )  fitted before the peak.


### 3) pred_Bayes_Err_cum.c 
   
   This code estimates the response parameters based on the anticipation and category (prior.txt) and evaluate the cumulative error (Table 10 and 11). It requires the prior parameters (prior.txt). 

   Type to compile:
   
      make  Pred_Bayes_cum

   Type to execute:
   
     ./Pred_Prop  *f_in  *f_out  $k_p  $t_obs  $alpha_c  $t_c  $a_n  $b_n  $t_n
   
   where 
   
     *f_in:            time series file
     
     *f_out:           output file (*f_in, response parameters, and prediction error)
     
     $k_p:             time at the peak (UTF time)
     
     $t_obs:           observation period
     
     $alpha_c, $t_c:   ¥alpha_c, t_c     ( C(t) in Eq. (1) )  fitted before the peak.
     
     $a_n, $b_n, $t_n: a_-, b_-, ¥tau_-  ( D_{peak}(t) in Eq. (1) )  fitted before the peak.


### 4) pred_Bayes_example.c

   This code estimates the response parameters based on the anticipation and category (prior.txt) and predict the response time series (Figure 3). It requires the prior parameters (prior.txt). 

   Type to compile:
   
     make Pred_Bayes_ex
     
   Type to execute:
   
     ./Pred_Prop  *f_in  *f_out  $k_p  $t_obs  $alpha_c  $t_c  $a_n  $b_n  $t_n
   
   where 
   
     *f_in:            time series file
     
     *f_out:           prediction result (time and predicted value)
     
     $k_p:             time at the peak (UTF time)
     
     $t_obs:           observation period
     
     $alpha_c, $t_c:   ¥alpha_c, t_c     ( C(t) in Eq. (1) )  fitted before the peak.
     
     $a_n, $b_n, $t_n: a_-, b_-, ¥tau_-  ( D_{peak}(t) in Eq. (1) )  fitted before the peak.
     

## D.  Parameter files in the "Par/" directory

### 1) Par/tp_${cat}.txt

   The time at the peak (UTF time) for the ${cat} category (${cat}: Election, Film, Foot (which means Football), Holiday, or Sports).  

   Each file consists of two columns: *file_name  peak_time.


### 2) Par/neg_${cat}.txt
   
   The anticipation parameters fitted from the time series before the peak. 
   ${cat} represents either Election, Film, Foot (which means Football), Holiday, or Sports. 
   
   Each file consists of 6 columns: 
   
   *file_name  alpha_c  t_c  a_-  b_-  tau_-. 


### 3) Par/${cat}.txt

   The anticipation and response parameters fitted from the whole time series. 
   ${cat} represents either ALL (which means all the categories), Election, Film, Foot (which means Football), Holiday, or Sports. 

   Each file consists of 10 columns: 
   
   *file_name  alpha_c  t_c  a_-  b_-  tau_-  a_+  b_+  tau_+  fitting_error. 


### 4) Par/Stat/${cat}.txt

   The mean and covariance of the logarithm of the anticipation and response parameters fitted from the whole time series. ${cat} represents either ALL (which means all the categories), Election, Film, Foot (which means Football), Holiday, or Sports. 

   Each file consists of 4 rows. 
   
The first row shows the number of events. 

The 2nd, 3rd, and 4th row shows the mean and covariance: 

   E[ log(a_+) ]     E[ log(a_-) ]      Var[ log(a_-) ]     Cov[ log(a_-), log(a_+) ]  Var[ log(a_+) ]
   
   E[ log(b_+) ]     E[ log(b_-) ]      Var[ log(b_-) ]     Cov[ log(b_-), log(b_+) ]  Var[ log(b_+) ]
   
   E[ log(¥tau_+) ]  E[ log(¥tau_-) ]   Var[ log(¥tau_-) ]  Cov[ log(¥tau_-), log(¥tau_+) ]  Var[ log(¥tau_+) ]


## E.  Wikipedia time series in the "Data/" directory

### Data/${cat}/peak_${ID}.txt

   Page view time series of the Wikipedia page related to the ${ID}-th event in the ${cat} category. 

   Each file consists of 337 rows (before and after 7 days), and each row shows the number of page views in each hour.  


## Licence
MIT

Please contact [Ryota Kobayashi](http://www.hk.k.u-tokyo.ac.jp/r-koba/en/contact.html) if you want to use the code for commercial purposes.


## The program was developed by
Ryota Kobayashi

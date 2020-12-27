# C code for fitting Wikipedia time series	(Kobayashi, Gildersleve, Uno, Lambiotte, ICWSM 2021)


## A. Requirements

### 1) clapack 

### 2) gnuplot


## B. Scripts for reproducing the results. 

### 1) Fit_par.pl   
   Fitting parameters of the proposed model to the Wikipedia time series. 	 
   
   Type to execute:
   
      perl   Fit_par.pl  ${cat}
   
   where ${cat} represents the event category, i.e., Election, Sports, Foot, Film, or, Holiday. 
   
   This scripts generate a parameter file "par_${cat}.txt".

   Each file consists of 10 columns: 
   
   *file_name  alpha_c  t_c  a_-  b_-  tau_-  a_+  b_+  tau_+  fitting_error. 
   

### 2) gen_Fig1.pl
  Plotting the fitting results of the proposed model (Figure 1). 
  
  Type to execute:
   
      perl   gen_Fig1.pl
  
   This scripts generate an eps file "Figure1.eps", which reproduces Figure 1 in the paper.


## E.  Wikipedia time series in the "Data/" directory

### Data/${cat}/peak_${ID}.txt

   Page view time series of the Wikipedia page related to the ${ID}-th event in the ${cat} category. 

   Each file consists of 337 rows (before and after 7 days), and each row shows the number of page views in each hour.  


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


## C. C codes in the "Prog/" directory

### 1) fit_daily.c

   This code fits daily parameters (a_-, b_-, tau_-, a_+, a_- , and tau_+) from time series to obtain initial parameters.
   
   Type to compile:
   
      make  Fit_daily
   
   Type to execute:
   
     ./fit_day   *f_in  
  
   where
   
     *f_in:    Wikipedia time series (Data/${cat}/peak_${ID}.txt) 

### 2) fit_hourly.c 

   This code fits all the parameters (alpha_c, t_c, a_-, b_-, tau_-, a_+, a_- , and tau_+) from time series.
   
   Type to compile:
   
      make  Fit_hourly
   
   Type to execute:
   
     ./fit_hour   *f_in  *f_out  $k_p
  
   where
   
     *f_in:    Wikipedia time series (Data/${cat}/peak_${ID}.txt) 
     
     *f_out:   Output file (*f_in, fittied parameters, and error)
     
     $k_p:     Peak time of an event (UTF)

### 3) fit_hourly_rmp.c 

   This code fits all the parameters (alpha_c, t_c, a_-, b_-, tau_-, a_+, a_- , and tau_+) from time series with removing (sub-)peaks. (Used for Football and Holiday data)
   
   Type to compile:
   
      make  Fit_hourly
   
   Type to execute:
   
     ./fit_hour   *f_in  *f_out  $k_p
  
   where
   
     *f_in:    Wikipedia time series (Data/${cat}/peak_${ID}.txt) 
     
     *f_out:   Output file (*f_in, fittied parameters, and error)
     
     $k_p:     Peak time of an event (UTF)

### 4) fit_example.c 

   This code fits all the parameters (alpha_c, t_c, a_-, b_-, tau_-, a_+, a_- , and tau_+) from time series and compare the fitted time series with the data. 
   
   Type to compile:
   
      make  Fit_example
   
   Type to execute:
   
     ./fit_ex    *f_in  *f_fit  *f_peak  $k_p
  
   where
   
     *f_in:    Wikipedia time series (Data/${cat}/peak_${ID}.txt) 
     
     *f_fit:   Fit result (time from 0:00 (AM) of the event date, Wikipedia time series, and fitted value). For example, see "Example/fit_Election.txt".
     
     *f_peak:  Peak file  ($k_p  0;$k_p  $peak_value). For example, see "Example/peak_Election.txt".
     
     $k_p:     Peak time of an event (UTF)
     
### 5) fit_example_rmp.c 

   This code fits all the parameters (alpha_c, t_c, a_-, b_-, tau_-, a_+, a_- , and tau_+) from time series with removing (sub-)peaks and compare the fitted time series with the data. (Used for Football and Holiday data)
   
   Type to compile:
   
      make  Fit_example_rmp
   
   Type to execute:
   
     ./fit_ex_rmp    *f_in  *f_fit  *f_peak  $k_p
  
   where
   
     *f_in:    Wikipedia time series (Data/${cat}/peak_${ID}.txt) 
     
     *f_fit:   Fit result (time from 0:00 (AM) of the event date, Wikipedia time series, and fitted value). For example, see "Example/fit_Holiday.txt".
     
     *f_peak:  Peak file  ($k_p  0;$k_p  $peak_value). For example, see "Example/peak_Holiday.txt".
     
     $k_p:     Peak time of an event (UTF)     


## D.  Event information in the "Event_Info/" directory

### 1) Election.csv

   Information of 92 election events. This file consists of 4 columns: 
   
      $ID   $Date   $Peak_time   $Title
   
   where
   
      $ID:  ID of the peak corresponding to the file name in the "Data/Election" directory.       
      
      $Date: date of the election. 
      
      $Peak_time: peak time of the Wikipedia time series (UTF).


### 2) Sports.csv

   Information of 213 sports events. This file consists of 5 columns: 
   
      $ID   $Date   $Peak_time   $Event   $Title
   
   where
   
      $ID:  ID of the peak corresponding to the file name in the "Data/Sports" directory. 
      
      $Date: date of the sports event.             
      
      $Event: sports event (e.g., Basketball and American football).            


### 3) Foot.csv

   Information of 125 association football matches (2017-18 UEFA Champions League). This file consists of 6 columns: 
   
      $ID   $Date   $Peak_time   $Team   $Type   $Win
   
   where
   
      $ID:  ID of the peak corresponding to the file name in the "Data/Foot" directory. 
      
      $Date: date of the football match.             
      
      $Team: football club team. Note that each match corresponds to two timeseries (a football team and its opponent team).
      
      $Type: type of the match (4: Group stage, 3: Knockout stage, 1: Final match).
      
      $Win: result of the match (1: $Team won, 0: Draw, -1: $Team lost the match).


### 4) Film.csv

   Information of 229 film release events. This file consists of 4 columns: 
   
      $ID   $Date   $Peak_time   $Title
   
   where
   
      $ID:  ID of the peak corresponding to the file name in the "Data/Film" directory. 
      
      $Date: date of the film release.             


### 5) Holiday.csv

   Information of 58 national Holidays (1-43: USA, 44-58: UK and AUS). This file consists of 4 columns: 
   
      $ID   $Date   $Peak_time   $Title
   
   where
   
      $ID:  ID of the peak corresponding to the file name in the "Data/Holiday" directory. 
      
      $Date: date of the holiday.    
     


## E.  Wikipedia time series in the "Data/" directory

### Data/${cat}/peak_${ID}.txt

   Page view time series of the Wikipedia page related to the ${ID}-th event in the ${cat} category. 

   Each file consists of 337 rows (before and after 7 days), and each row shows the number of page views in each hour.  


## F.  Fitting results in the "Par/" directory

### Par/par_${cat}.txt

   Fitted parameters in the Wikipedia time series of ${cat} event (e.g., Election and Sports). 

   Each file consists of 10 columns:
   
     *File   alpha_c   t_c   a_-   b_-   tau_-   a_+   b_+   tau_+   $error
   
   where
   
      *File: file name of the event
      
      $error: fitting error evaluated by 1- R^2, where R^2 is the coefficient of determination (Equation 2 in the paper). 
   


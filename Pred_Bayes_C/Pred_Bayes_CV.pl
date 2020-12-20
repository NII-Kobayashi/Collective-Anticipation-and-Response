#  Evaluate Prediction Error: Bayesian method
#  Version 201217: Print OK 

system ("make  Prior  Pred_Bayes_ts");   #  Error: Time Series
# system ("make  Prior  Pred_Bayes_cum");   #  Error: Cummulative Count

$t_obs=  24;     #  Observation Period: 1 (day)

#  @cat: 5 Categories.
$cat[0]= "Election";     $cat[1]= "Sports";
$cat[2]= "Foot";         $cat[3]= "Film";      $cat[4]= "Holiday";

for ($i= 0; $i<5;  $i++)
{
    @file = glob "Data/$cat[$i]/*";
    $num= @file+1;
    
    printf("##  $cat[$i]  $num\n");
    
    open(TP, "Par/tp_$cat[$i].txt" );   $n= 1;
    #  Read $k_p: Peak Time
    while(<TP>){   
        chomp;
        my @d = split(/\s+/, $_);
        $k_p[$n]= $d[1];    $n++;
    }   close(TP);
    
    open(PAR, "Par/neg_$cat[$i].txt" );   $n= 1;
    #  Read $a_n, $b_n, $t_n: Fitted parameters before the peak
    while(<PAR>){   
        chomp;
        my @d = split(/\s+/, $_);
        $a_c[$n]= $d[1];    $t_c[$n]= $d[2];
	$a_n[$n]= $d[3];    $b_n[$n]= $d[4];	$t_n[$n]= $d[5];    $n++;
    }   close(PAR);

    open(PAR, "Par/$cat[$i].txt" );   $n= 1;
    #  Read $p_an, $p_ap, ...: Fitted parameters from the whole time series. 
    while(<PAR>){
        chomp;
        my @d = split(/\s+/, $_);
        $p_an[$n]= $d[3];    $p_bn[$n]= $d[4];    $p_tn[$n]= $d[5];
	$p_ap[$n]= $d[6];    $p_bp[$n]= $d[7];    $p_tp[$n]= $d[8];     $n++;
    }   close(PAR);        
    
    for($j= 1; $j< $num; $j++)   # for($j= 1; $j< 10; $j++)    
    {
	#  Prior: Anticipation only
	# system ("./a.out   Par/Stat/ALL.txt  $p_an[$j]  $p_bn[$j]  $p_tn[$j]  $p_ap[$j]  $p_bp[$j]  $p_tp[$j]");
	
	#  Prior: Anticipation + Category
	system ("./a.out  Par/Stat/$cat[$i].txt  $p_an[$j]  $p_bn[$j]  $p_tn[$j]  $p_ap[$j]  $p_bp[$j]  $p_tp[$j]");

	#  Predict response activity by Bayes method
	system ("./Pred_Prop  Data/$cat[$i]/peak_${j}.txt  err.txt  $k_p[$j]  $t_obs  $a_c[$j]  $t_c[$j]  $a_n[$j]  $b_n[$j]  $t_n[$j]");
    }
}

$n= 0;    open(ERR, "err.txt" );
while(<ERR>){
    chomp; 
    my @d = split(/\s+/, $_);
    $err_o[$n]= $d[4];    $n++;
}   close(ERR);

@err_s= sort {$a <=> $b} @err_o;

print "### Num: $n \n";   
print "#  $err_s[$n*0.5]  [ $err_s[$n*0.25]: $err_s[$n*0.75] ] \n";

system ("rm  a.out  Pred_Prop");

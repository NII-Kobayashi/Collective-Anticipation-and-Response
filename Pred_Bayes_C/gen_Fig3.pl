#  Example of predictions:  Figure 3

#  Compile
system ("make  Pred_Bayes_ex  Prior");

$t_obs=  24;     #  Observation Period: 1 (day)

open(SAMP, "Example/Example.txt" );   $n= 1;
while(<SAMP>){   #  Read Category & Peak ID.
    chomp;
    my @d = split(/\s+/, $_);
    #  @cat: 5 Categories,  @samp: ID.
    $cat[$n]= $d[0];     $samp[$n]= $d[1];     $n++;
}   close(SAMP);

for ($i= 1; $i< 6;  $i++)
{
    @file = glob "Data/$cat[$i]/*";
    $num= @file+1;
    
    printf("##  $cat[$i]  $num\n");
    
    open(TP, "Par/tp_$cat[$i].txt" );   $n= 1;    
    while(<TP>){   #  Read $k_p: Peak Time
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
    
    $k= $samp[$i];
    $f_in = "Data/$cat[$i]/peak_${k}.txt";
    $f_out= "Example/$cat[$i]/Bayes.txt";
    
    system ("./a.out  Par/Stat/$cat[$i].txt  $p_an[$k]  $p_bn[$k]  $p_tn[$k]  $p_ap[$k]  $p_bp[$k]  $p_tp[$k]");
    system ("./Pred_Prop  $f_in  $f_out  $k_p[$k]   $t_obs  $a_c[$k]  $t_c[$k]  $a_n[$k]  $b_n[$k]  $t_n[$k]");	
}

#  Generate Figure 3
#  Models:
$mod[0]= "Orig";       $col[0]= "black";       $leg[0]= "Data";
$mod[1]= "Bayes";      $col[1]= "magenta";     $leg[1]= "Proposed";
$mod[2]= "SpikeM";     $col[2]= "cyan";        $leg[2]= "SpikeM";
$mod[3]= "PowLaw";     $col[3]= "green";       $leg[3]= "Power Law";
$mod[4]= "LR";         $col[4]= "blue";        $leg[4]= "LR";

#  Tics
$ytic[1]= 4000;     $ytic[2]= 200;     $ytic[3]= 200;
$ytic[4]= 2000;     $ytic[5]= 10000;

#  Open Gnuplot 
open (GP, "| gnuplot -persist" )    or die "no gnuplot";

#  Set size, file name, and #panels.
print GP "set ter post eps color enhanced 24 dashlength 3 size 10in, 10in \n";
print GP "set out \"Figure3.eps\" \n";
print GP "set multiplot layout 3, 2 \n";
print GP "set lmargin 10 \n  set rmargin 2 \n";

print GP "set size ratio  0.5 \n";
print GP "set xtics  2 \n  set xtics scale  2,1  \n  set mxtics 2 \n";
print GP "set ytics scale  2,1  \n  set mytics 2 \n";

print GP "set xl  \"Time from Peak (days)\"  \n  set yl \"#Page Views\"  \n";

#  Title of peanel
print GP "set label 1 center at screen 0.3, 0.98 \"Election\" font \"Helvetica, 32\" \n";
print GP "set label 2 center at screen 0.3, 0.96 \"Germany\" font \"Helvetica, 28\" \n";
print GP "set label 3 center at screen 0.8, 0.98 \"Sports\" font \"Helvetica, 32\" \n";
print GP "set label 4 center at screen 0.8, 0.96 \"2017 British Grand Prix\" font \"Helvetica, 28\" \n";
print GP "set label 5 center at screen 0.3, 0.65 \"Football\" font \"Helvetica, 32\" \n";
print GP "set label 6 center at screen 0.3, 0.63 \"Juventus vs Sporting CP\" font \"Helvetica, 28\" \n";
print GP "set label 7 center at screen 0.8, 0.65 \"Film\" font \"Helvetica, 32\" \n";
print GP "set label 8 center at screen 0.8, 0.63 \"Red Sparrow\" font \"Helvetica, 28\" \n";
print GP "set label 9 center at screen 0.3, 0.33  \"Holiday\" font \"Helvetica, 32\" \n";
print GP "set label 10 center at screen 0.3, 0.31  \"Halloween\" font \"Helvetica, 28\" \n";

for ($i= 1; $i< 6; $i++)
{    
    print GP "set ytics  $ytic[$i]  \n";
    if ( $i < 3 )
    {     print GP "plot  [0:7][0:]  \"Example/$cat[$i]/$mod[0].txt\"  u (\$1/24):2  with linespoints pt 7  lw 2  dt (1,1)  lc rgb \"$col[0]\"  tit \"$leg[0]\",  ";    }
    else
    {     print GP "plot  [0:7][0:]  \"Example/$cat[$i]/$mod[0].txt\"  u (\$1/24):2  with linespoints pt 7  lw 2  dt (1,1)  lc rgb \"$col[0]\"  tit \"\",  ";    }
   
    
    for ($j=1; $j<5; $j++)
    {
        if ( $i < 3 )
        {     print GP "\"Example/$cat[$i]/$mod[$j].txt\"  u (\$1/24):2  w l lw 6  lc rgb \"$col[$j]\"  tit \"$leg[$j]\" ";     }
        else
        {     print GP "\"Example/$cat[$i]/$mod[$j].txt\"  u (\$1/24):2  w l lw 6  lc rgb \"$col[$j]\"  tit \"\" ";     }
        
        if ( $j<4 )    {   print GP ",  ";    }
        else           {   print GP " \n";    }
    }
}

close(GP);
system ("rm  a.out  Pred_Prop  prior.txt");

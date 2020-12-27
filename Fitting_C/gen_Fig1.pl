#  Example of Fitting:  Figure 1

#  Compile
system ("make  Fit_daily   Fit_example   Fit_example_rmp");

open(SAMP, "Example/Example.txt" );   $n= 1;
while(<SAMP>){   #  Read Category & Peak ID.
    chomp;
    my @d = split(/\t/, $_);
    #  @cat: Category,  @f_in: peak file,  @ev: Event info
    $cat[$n]= $d[0];     $f_in[$n]= $d[1];
    $ev[$n]= $d[2];      $k_p[$n]=  $d[3];     $n++;    
}   close(SAMP);

for ($i= 1; $i< 6;  $i++)
{    
    $f_dat = "Data/$cat[$i]/$f_in[$i]";     $f_fit[$i]= "Example/fit_$cat[$i].txt";
    $f_peak[$i]= "Example/peak_$cat[$i].txt";
    
    system ("./fit_day  $f_dat  ");
    
    if      (  $cat[$i] eq "Election" ||  $cat[$i] eq "Film"  ||  $cat[$i] eq "Sports" )
    {    system ("./fit_ex      $f_dat  $f_fit[$i]  $f_peak[$i]   $k_p[$i]  ");   }    
    elsif   (  $cat[$i] eq "Foot"     ||  $cat[$i] eq "Holiday"  )
    {    system ("./fit_ex_rmp  $f_dat  $f_fit[$i]  $f_peak[$i]   $k_p[$i]  ");	  }
}

#######
###   Generate Figure 1
#######
#  Tics
$ytic[1] = 1000;     $ytic[2]=  5000;     $ytic[3]= 2000;     $ytic[4]=  500;     $ytic[5]= 10000;
$ymax[1] = 2000;     $ymax[2]= 10000;     $ymax[3]= 6000;     $ymax[4]= 1500;     $ymax[5]= 35000;
#  Open Gnuplot 
open (GP, "| gnuplot -persist" )    or die "no gnuplot";

#  Set size, file name, and #panels.
print GP "set ter post eps color enhanced 20 dashlength 3 size 10in, 4in \n";
print GP "set out \"Figure1.eps\" \n";
print GP "set multiplot layout 2, 3 \n";
print GP "set tmargin 5 \n set lmargin 10 \n  set rmargin 2 \n";

print GP "set style fill solid \n";
print GP "set size ratio  0.45 \n";
print GP "set xtics  2 \n  set xtics scale  2,1  \n  set mxtics 2 \n";
print GP "set ytics scale  2,1  \n  set mytics 2 \n";

print GP "set xl  \"t (day)\"  \n  set yl \"#Page View (/h)\"  \n";

#  Title of peanel
print GP "set label 1 center at screen  0.2,    0.94   \"$cat[1]\"   font \"Helvetica, 32\" \n";
print GP "set label 2 center at screen  0.2,    0.88   \"$ev[1]\"    font \"Helvetica, 28\" \n";
print GP "set label 3 center at screen  0.535,  0.94   \"$cat[2]\"   font \"Helvetica, 32\" \n";
print GP "set label 4 center at screen  0.535,  0.88   \"$ev[2]\"    font \"Helvetica, 28\" \n";
print GP "set label 5 center at screen  0.87,   0.94   \"Football\"  font \"Helvetica, 32\" \n";
print GP "set label 6 center at screen  0.87,   0.88   \"$ev[3]\"    font \"Helvetica, 28\" \n";
print GP "set label 7 center at screen  0.2,    0.43   \"$cat[4]\"   font \"Helvetica, 32\" \n";
print GP "set label 8 center at screen  0.2,    0.38    \"$ev[4]\"    font \"Helvetica, 28\" \n";
print GP "set label 9 center at screen  0.535,  0.42   \"$cat[5]\"   font \"Helvetica, 32\" \n";
print GP "set label 10 center at screen 0.535,  0.37  \"$ev[5]\"    font \"Helvetica, 28\" \n";

for ($i= 1; $i< 6; $i++)
{    
    print GP "set ytics  $ytic[$i]  \n";    
    print GP "plot  [-5:6][0:$ymax[$i] ] \"$f_fit[$i]\" u  (\$1/24):2  ps 0.5 pt 6 lw 3 lc rgb \"black\" tit \"\",  \"$f_fit[$i]\" u  (\$1/24):3 w l lw 5 lc rgb \"magenta\" tit \"\",   \"$f_peak[$i]\" u  (\$1/24):2 pt 2  ps 1.5 lw 3 lc rgb \"blue\" tit \"\", \"$f_peak[$i]\" u  (\$1/24):2 w l  lw 4 lc rgb \"blue\" tit \"\"   \n ";       
}

close(GP);
system ("rm  tmp_daily.txt  fit_day  fit_ex  fit_ex_rmp");

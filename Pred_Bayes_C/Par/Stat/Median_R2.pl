#  Calculate Median of Fitting error

#  Set file name
$n= 0;   $f_in= "../$ARGV[0].txt";

open(PAR, $f_in );
while(<PAR>){
    chomp;
    my @d = split(/\s+/, $_);
    
    $s_n= $d[3]* $d[5]* (1- exp(-168/$d[5]) )+ 168* $d[4];
    $s_p= $d[6]* $d[8]* (1- exp(-168/$d[8]) )+ 168* $d[7];
    $tau[$n]= $s_n/$s_p;
    
    # $tau[$n]= $d[5];
    # $tau[$n]= $d[8];
    # $err[$n]= $d[9];
    $n++;
}   close(PAR);


@tau= sort {$a <=> $b} @tau;
# @err= sort {$a <=> $b} @err;

print "### Num: $n \n";
print "#  $tau[$n*0.5]  [ $tau[$n*0.25]: $tau[$n*0.75] ] \n";
# print "#  $err[$n*0.5]  [ $err[$n*0.25]: $err[$n*0.75] ] \n";

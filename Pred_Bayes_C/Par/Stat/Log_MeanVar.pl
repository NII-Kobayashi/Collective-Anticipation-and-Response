#  Calc. Histogram

#  Set file name
$n= 0;   $f_in= "../$ARGV[0].txt";

open(PAR, $f_in );
while(<PAR>){
    chomp;
    my @d = split(/\s+/, $_);
    for ( $i= 3;  $i< 9;  $i++)
    {   if ( $d[$i] < 1 )   {   $d[$i]= 1;  }     }
    
    $a_n[$n]= log($d[3]);   $b_n[$n]= log($d[4]);   $t_n[$n]= log($d[5]);
    $a_p[$n]= log($d[6]);   $b_p[$n]= log($d[7]);   $t_p[$n]= log($d[8]);     $n++;
}   close(PAR);

#  Write Statistics of Parameters.
$f_out= "$ARGV[0].txt";
open (OUT, '>>', $f_out );

calc_mu_sig($n, \@a_n, \@a_p, \@mu, \@sig);
print "## N: $n\n";
print "## Mean: $mu[0]  $mu[1]\n";
print "## Variance: $sig[0]  $sig[1];  $sig[2]  $sig[3]\n\n";

#  Stat of (a_-, a_+)
print OUT  "$n\n";
print OUT  "$mu[0]  $mu[1]  $sig[0]  $sig[1]  $sig[3]\n";

calc_mu_sig($n, \@b_n, \@b_p, \@mu, \@sig);
#  Stat of (b_-, b_+)
print OUT  "$mu[0]  $mu[1]  $sig[0]  $sig[1]  $sig[3]\n";

calc_mu_sig($n, \@t_n, \@t_p, \@mu, \@sig);
#  Stat of (\tau_-, \tau_+)
print OUT  "$mu[0]  $mu[1]  $sig[0]  $sig[1]  $sig[3]\n";
close(OUT);


# Subrootin: Mean and Vaiance 2D
sub calc_mu_sig {
    my  ($num, $r_x, $r_y, $r_mu, $r_sig) = @_;
    
    for ($i= 0; $i< 2; $i++)   #  Initilization: mu, sigma
    {   $r_mu->[$i]= 0;   $r_sig->[2*$i]= 0;   $r_sig->[2*$i+1]= 0;   }
    
    for ($i= 0; $i< $num; $i++)   #  Calc. Mean (1)
    {   $r_mu->[0] += $r_x->[$i];      $r_mu->[1] += $r_y->[$i];   }
    for ($i= 0; $i< 2; $i++)      #  Calc. Mean (2)
    {   $r_mu->[$i] /= $num;   }
    
    for ($i= 0; $i< $num; $i++)   #  Calc. Covariance (1)
    {
        $r_sig->[0] += ( $r_x->[$i]- $r_mu->[0] )** 2;
        $r_sig->[1] += ( $r_x->[$i]- $r_mu->[0] )* ( $r_y->[$i]- $r_mu->[1] );
        $r_sig->[3] += ( $r_y->[$i]- $r_mu->[1] )** 2;
    }
    $r_sig->[2]= $r_sig->[1];
    for ($i= 0; $i< 4; $i++)      #  Calc. Covariance (2)
    {   $r_sig->[$i] /= $num;   }
    
    ## print "###  $r_mu->[0]  $r_mu->[1]\n";
}

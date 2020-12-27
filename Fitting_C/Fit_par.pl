#  Fit all the peaks!
#    f(t)= Oscillation(t) * Decay(t)

if ( $#ARGV == 0 )     {   $cat= $ARGV[0];    }
else                   {   printf("# Error: perl  Fit_par_v1.pl  category \n");    exit(1);     }

#  Read Peak time ($k_p) and Event name ($ev).
$f_list= "Event_Info/${cat}.csv";    $num= 1;
open(LIST, $f_list );
while(<LIST>){
    chomp;
    my @d = split(/\t/, $_);
    $k_p[$num]= $d[2];     $num++;
}   close(LIST);

#  Compile
#     Category: Election , Film, Sports
if  (  $cat  eq "Election" ||  $cat  eq "Film"  ||  $cat  eq "Sports" )
{     system("make  Fit_daily  Fit_hourly");         }
#     Category: Football, Holiday
elsif  (  $cat  eq "Foot"     ||  $cat  eq "Holiday"  )   
{     system("make  Fit_daily  Fit_hourly_rmp");     }
else
{     printf("# Error: Category\n");    exit(1);     }

for ($i=1; $i< $num; $i++) #  for ($i=1; $i< 16; $i++)
{       
    $f_in=  "Data/${cat}/peak_${i}.txt";
    $f_par= "par_${cat}.txt";        

    system("./fit_day   $f_in ");
    system("./fit_hour  $f_in  $f_par  $k_p[$i] ");   
}   

system("rm  fit_day   fit_hour  tmp_daily.txt");

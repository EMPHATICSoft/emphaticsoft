#!/usr/bin/perl

# Linyan 2022.01.12

# Structure of this program is taken from generate_gdml.pl that 
# generates MicroBooNE / Nova fragment files (Thank you.)

# Each subroutine generates a fragment GDML file, and the last subroutine
# creates an XML file that make_gdml.pl will use to appropriately arrange
# the fragment GDML files to create the final desired GDML file, 
# to be named by make_gdml output command

# If you are playing with different geometries, you can use the
# suffix command to help organize your work.

# Manuals can be found at DocDB 1242
# Visualization of SSDs can be found at DocDB 1260

use Getopt::Long;
use Math::BigFloat;
use GDMLUtil;
Math::BigFloat->precision(-10);

GetOptions( "help|h" => \$help,
	"suffix|s:s" => \$suffix,
	"output|o:s" => \$output,
	"target|t:i" => \$target,
	"magnet|m:i" => \$magnet);

if ( defined $help )
{
	# If the user requested help, print the usage notes and exit.
	usage();
	exit;
}

if ( ! defined $suffix )
{
	# The user didn't supply a suffix, so append nothing to the file
	# names.
	$suffix = "";
}
else
{
	# Otherwise, stick a "-" before the suffix, so that a suffix of
	# "test" applied to filename.gdml becomes "filename-test.gdml".
	$suffix = "-" . $suffix;
}


# constants for T0
$T0_switch = 1;
$n_acrylic = 20;

# constants for TARGET
$target_switch = 1;
@target_matt = ("Graphite", "CH2");
$target_v = 0;

# constants for MAGNET
$magnet_switch = 1;
$magnet_layer = 3;

# constants for SSD
# Check DocDB 1260 for details.
$SSD_switch = 1;
$nstation_type = 3; # types of station
$nSSD_station = 6; # numbers of station
@station_type = ("single", "rotate", "double"); # yx, wyx, xxyyww 
@bkpln_size = (1.0, 1.3, 2.6); # bkpln size scale to SSD sensor
@SSD_lay = (2, 3, 3); # num. of SSD layer in a station
@SSD_par = (1, 1, 2); # num. of SSD in a layer
@SSD_angle = (0, 270, 0, 270, 315, 0, 270, 315, 0, 270, 270, 90, 0, 180, 225, 45, 270, 90, 0, 180, 225, 45); # angle from measuring Y
#Due to the coordination system, this is equivalent to @SSD_angle = (0, 90, 0, 90, 45, 0, 90, 45, 0, 90, 270, 90, 0, 180, 135, 315, 90, 270, 0, 180, 10, 315); # angle from measuring Y
# Visualization of SSDs can be found at DocDB 1260
@SSD_station_shift = (0, 120.5, 360.0, 481.0, 985.6, 1211.8);
@SSD_station_rotation = ([0,0], [0,0], [0,0], [0,0], [0,0], [0,0]);
@SSD_bkpln= (1, 2, 2); # num. of bkpln in a station
@SSD_mod = ("D0", "D0", "D0"); # SSD type in a station
$nD0chan = 640; # number of channels per sensor
@SSD_station = (2, 2, 2); # num. of stations

# constants for ARICH
$arich_switch = 1;
$n_aerogel = 2;
@aerogel_mat = ("AERO_1026", "AERO_1030");
$n_mPMT1d = 3;
$n_anode1d = 8;

# constants for RPC
$RPC_switch = 1;
$n_glass = 6;
$n_gas = 2;
$n_cover=2;

# constants for LG
$LG_switch = 1;
$n_LG = 3; # horizontal
$m_LG = 3; # vertical

if ( defined $magnet )
{
	# If the user requested help, print the usage notes and exit.
	if($magnet == 0){
		$magnet_switch = 0;
	}
	elsif($magnet == 1){
		$magnet_switch = 1;
	}
	else{
		print "wrong magnet parameter\n";
	}
}

if ( defined $target)
{
	# If the user requested help, print the usage notes and exit.
	if($target == 0){
		$target_switch = 0;
	}
	elsif($target < 0 || $target > 2){
		print "wrong target parameter\n";
	}
	else{
		$target_v = $target - 1;
	}
}

# run the sub routines that generate the fragments

gen_Define(); 	 # generates definitions at beginning of GDML
gen_Materials(); # generates materials to be used
gen_PMTs(); # generates PMTs to be used
gen_Solids();    # generates solids
gen_Modules();   # generate modules
gen_DetEnclosure();   # generate DetEnclosure
gen_World();	 # places the enclosure among DUSEL Rock


write_fragments(); # writes the XML input for make_gdml.pl
# which zips together the final GDML
exit;


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++ usage +++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub usage()
{
	print "Usage: $0 [-h|--help] [-o|--output <fragments-file>] [-t|--target <target number>] [-m|--magnet <0 or 1>] [-s|--suffix <string>]\n";
	print "       if -o is omitted, output goes to STDOUT; <fragments-file> is input to make_gdml.pl\n";
	print "       -t 0 is no target, 1 is graphite, 2 is CH2; Default is graphite\n";
	print "       -m 0 is no magnet, 1 is the 100 mrad magnet; Default is 1\n";
	print "       -s <string> appends the string to the file names; useful for multiple detector versions\n";
	print "       -h prints this message, then quits\n";
}



#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++ gen_Define +++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_Define()
{

	# Create the <define> fragment file name, 
	# add file to list of fragments,
	# and open it
	$DEF = "phase1_Def" . $suffix . ".gdml";
	push (@gdmlFiles, $DEF);
	$DEF = ">" . $DEF;
	open(DEF) or die("Could not open file $DEF for writing");

	print DEF <<EOF;
<?xml version='1.0'?>
	 <gdml>

  <define>

    <constant name="DEG2RAD" value="pi/180." />
	 <quantity name="world_size" value="7000." unit="mm"/>
	 <quantity name="Tolerance_space" value="1." unit="mm" />
	 <position name="center" x="0" y="0" z="0" unit="mm"/>

EOF
	if($T0_switch){
		print DEF <<EOF;
	 <!-- BELOW IS FOR T0 -->

	 <quantity name="T0_length" value="280.0" unit="mm"/>
	 <quantity name="T0_width" value="210.0" unit="mm"/>
	 <quantity name="T0_height" value="300.0" unit="mm"/>
	 <quantity name="T0_shift" value="-852.9" unit="mm"/>
	 <quantity name="T0_acrylic_shift" value="40.0" unit="mm"/>
	 <position name="T0_pos" z="T0_shift+T0_acrylic_shift+T0_length*0.5"/>

	 <quantity name="T0_acrylic_length" value="150.0" unit="mm"/>
	 <quantity name="T0_acrylic_width" value="3.0" unit="mm"/>
	 <quantity name="T0_acrylic_height" value="3.0" unit="mm"/>

EOF

		$j=0;
		for($i = 0; $i < $n_acrylic; ++$i){
			$j=$i%2;
			print DEF <<EOF;
	 <position name="T0_acrylic@{[ $i ]}_pos" x="T0_acrylic_width*($i-($n_acrylic-1)*0.5)" z="-T0_acrylic_shift+T0_acrylic_width*$j"/>
EOF
		}
		print DEF <<EOF;

	 <rotation name="T0_union1_rot" x="90" unit="deg"/>
	 <rotation name="T0_acrylic_rot" x="-45" unit="deg"/>

	 <!-- ABOVE IS FOR T0 -->
EOF
	}

	if($target_switch){
		print DEF <<EOF;
	 <!-- BELOW IS FOR TARGET -->

	 <quantity name="target_thick" value="20.0" unit="mm"/>
	 <quantity name="target_width" value="100.0" unit="mm"/>
	 <quantity name="target_height" value="50.0" unit="mm"/>

	 <position name="target_pos" x="0" y="0" z="200.5"/>

	 <!-- ABOVE IS FOR TARGET -->

EOF
	}

	if($magnet_switch){
		print DEF <<EOF;
	 <!-- BELOW IS FOR MAGNET -->

    <quantity name="magnetShift" value="757.7" unit="mm"/>
    <quantity name="magnetSideWidth" value="50" unit="mm"/>
	 <quantity name="magnetSideZLength" value="160" unit="mm"/>
	 <quantity name="magnetSideOHeight" value="240" unit="mm"/>
	 <quantity name="magnetSideIHeight0" value="48" unit="mm"/>
	 <quantity name="magnetSideIHeight1" value="62" unit="mm"/>
	 <quantity name="magnetSideIHeight2" value="80" unit="mm"/>

	 <position name="magnet_pos" x="0" y="0" z="magnetShift"/>

EOF
		for($i = 0; $i < $magnet_layer; ++$i){
			print DEF <<EOF;
    <position name="magnetSide_pos@{[ $i ]}" x="0" y="0" z="(-1+$i)*magnetSideWidth"/>
EOF
		}
		print DEF <<EOF;

	 <!-- ABOVE IS FOR MAGNET -->

EOF
	}

	if($SSD_switch){
		print DEF <<EOF;
	 <!-- BELOW IS FOR SSD -->

	 <quantity name="ssdD0_thick" value=".300" unit="mm"/>
	 <quantity name="ssdD0_height" value="38.46" unit="mm"/>
	 <quantity name="ssdD0_width" value="98.33" unit="mm"/>

    <quantity name="ssdD0_chanwidth" value="0.059999" unit="mm"/>
	 <quantity name="ssdD0_changap" value="0.000001" unit="mm"/>
	 
EOF
      for($i = 0; $i < $nSSD_station; ++$i){
         print DEF <<EOF;
    <quantity name="ssdStation@{[ $i ]}_shift" value="@{[ $SSD_station_shift[$i] ]}" unit="mm"/>
EOF
      }

		print DEF <<EOF;

	 <quantity name="bkpln_thick" value="6.35" unit="mm" />
	 <quantity name="bkpln_width" value="115.98" unit="mm" />
	 <quantity name="bkpln_hole" value="80.00" unit="mm" />

	 <quantity name="Mylar_Window_thick" value="0.500" unit="mm" />
	 <quantity name="Mylar_shift" value="20" unit="mm"/>

	 <quantity name="ssdStationsingleLength" value="50" unit="mm" />
	 <quantity name="ssdStationsingleWidth" value="300" unit="mm" />
	 <quantity name="ssdStationsingleHeight" value="300" unit="mm" />

EOF

		for($i = 0; $i < $nSSD_station; ++$i){
         print DEF <<EOF;
      <position name="ssdStation@{[ $i ]}_pos" x="0" y="0" z="ssdStation@{[ $i ]}_shift+ssdD0_thick-0.5*bkpln_thick"/>
EOF
      }

      print DEF <<EOF;

      <position name="ssd_bkpln_u1_pos" x="70" y="0" z="0"/>
      <rotation name="ssd_bkpln_u1_rot" y="-90" unit="deg"/>
      <position name="ssd_bkpln_u2_pos" x="0" y="-70" z="0"/>
      <rotation name="ssd_bkpln_u2_rot" y="-90" z="-90" unit="deg"/>
      <rotation name="ssd_bkpln_u3_rot" z="90" unit="deg"/>
      <position name="ssd_bkpln_u4_pos" x="80.1" y="-80.4"/>
      <rotation name="ssd_bkpln_u4_rot" y="-90" z="135" unit="deg"/>

	 <position name="ssdsingle00_pos" x="0" y="0" z="0"/>
	 <position name="ssdbkplnsingle0_pos" x="0" y="0" z="0.5*ssdD0_thick+0.5*bkpln_thick"/>
	 <position name="ssdsingle10_pos" x="0" y="0" z="ssdD0_thick+bkpln_thick"/>
	 <position name="ssdsingle_USMylarWindow_pos" x="0" y="0" z="Mylar_shift"/>
	 <position name="ssdsingle_DSMylarWindow_pos" x="0" y="0" z="-1.*Mylar_shift"/>

	 <quantity name="ssdStationrotateLength" value="50" unit="mm" />
	 <quantity name="ssdStationrotateWidth" value="300" unit="mm" />
	 <quantity name="ssdStationrotateHeight" value="300" unit="mm" />
	 <quantity name="ssd3plane_shift" value="10" unit="mm" />

	 <position name="ssdrotate00_pos" x="0" y="0" z="0"/>
	 <position name="ssdbkplnrotate0_pos" x="0" y="0" z="0.5*ssdD0_thick+0.5*bkpln_thick"/>
	 <position name="ssdrotate10_pos" x="0" y="0" z="ssdD0_thick+bkpln_thick"/>
	 <position name="ssdrotate20_pos" x="0" y="0" z="ssd3plane_shift"/>
	 <position name="ssdbkplnrotate1_pos" x="0" y="0" z="ssd3plane_shift+0.5*ssdD0_thick+0.5*bkpln_thick" />
	 <position name="ssdrotate_USMylarWindow_pos" x="0" y="0" z="Mylar_shift"/>
	 <position name="ssdrotate_DSMylarWindow_pos" x="0" y="0" z="-1.*Mylar_shift"/>

	 <quantity name="ssdStationdoubleLength" value="100" unit="mm" />
	 <quantity name="ssdStationdoubleWidth" value="450" unit="mm" />
	 <quantity name="ssdStationdoubleHeight" value="450" unit="mm" />

	 <position name="ssddouble00_pos" x="0.5*ssdD0_height" y="0" z="0" />
	 <position name="ssddouble01_pos" x="-0.5*ssdD0_height" y="0" z="0" />
	 <position name="ssddouble10_pos" y="-0.5*ssdD0_height" x="0" z="ssdD0_thick+bkpln_thick" />
	 <position name="ssddouble11_pos" y="0.5*ssdD0_height" x="0" z="ssdD0_thick+bkpln_thick" />
	 <position name="ssddouble20_pos" x="0.354*ssdD0_height" y="0.354*ssdD0_height" z="ssd3plane_shift" />
	 <position name="ssddouble21_pos" x="-0.354*ssdD0_height" y="-0.354*ssdD0_height" z="ssd3plane_shift" />
	 <position name="ssddouble_USMylarWindow_pos" x="0" y="0" z="Mylar_shift"/>
	 <position name="ssddouble_DSMylarWindow_pos" x="0" y="0" z="-1.*Mylar_shift"/>
	 <position name="ssdbkplndouble0_pos" x="0" y="0" z="0.5*ssdD0_thick+0.5*bkpln_thick" />
	 <position name="ssdbkplndouble1_pos" x="0" y="0" z="ssd3plane_shift+0.5*ssdD0_thick+0.5*bkpln_thick" />

EOF
		$isensor = 0;
		for($i = 0; $i < $nstation_type; ++$i){
			for($l = 0; $l < $SSD_station[$i]; ++$l){
				for($j = 0; $j < $SSD_lay[$i]; ++$j){
					for($k = 0; $k < $SSD_par[$i]; ++$k){

						print DEF <<EOF;
	 <rotation name="ssd@{[ $station_type[$i] ]}@{[ $l ]}_@{[ $j ]}_@{[ $k ]}_rot" z="@{[ $SSD_angle[$isensor] ]}" unit="deg"/>
EOF
						$isensor ++;
					}
				}
			}
		}

	}

	for($i = 0; $i < $nD0chan; ++$i){
		print DEF <<EOF;
		<position name="ssd_chan_@{[ $i ]}_pos" x="0" y="(@{[ -($nD0chan-1)/2 ]}+@{[ $i ]})*(ssdD0_chanwidth+ssdD0_changap)" z="0"/>
EOF
	}
		print DEF <<EOF;

	 <!-- ABOVE IS FOR SSD -->
EOF

	if($arich_switch){
		print DEF <<EOF;
	 <!-- BELOW IS FOR ARICH -->

	 <quantity name="arich_shift" value="1377.9" unit="mm"/>
	 <quantity name="arich_thick" value="280.0" unit="mm"/>
	 <quantity name="arich_width" value="365.0" unit="mm"/>
	 <quantity name="arich_height" value="365.0" unit="mm"/>

	 <position name="arich_pos" x="0" y="0" z="arich_shift+0.5*arich_thick" />

	 <quantity name="aerogel_thick0" value="18.9" unit="mm"/>
	 <quantity name="aerogel_thick1" value="20.4" unit="mm"/>
	 <quantity name="aerogel_size" value="93.0" unit="mm"/>
	 <quantity name="aerogel_shift" value="43.0" unit="mm"/>
	 
	 <position name="aerogel_pos0" x="0" y="0" z="aerogel_shift-0.5*arich_thick+0.5*aerogel_thick0" />
	 <position name="aerogel_pos1" x="0" y="0" z="aerogel_shift-0.5*arich_thick+aerogel_thick0+0.5*aerogel_thick1" />

	 <quantity name="mPMT_thick" value="16.4" unit="mm"/>
	 <quantity name="mPMT_size" value="49.3" unit="mm"/>
	 <quantity name="mPMT_gap" value="5.4" unit="mm"/>
	 <quantity name="manode_size" value="6.0" unit="mm"/>
	 <quantity name="mPMT_shift" value="103.0" unit="mm"/>

EOF
		for($i = 0; $i < $n_mPMT1d; ++$i){
			for($j = 0; $j < $n_mPMT1d; ++$j){
				for($k = 0; $k < $n_anode1d; ++$k){
					for($l = 0; $l < $n_anode1d; ++$l){
						print DEF <<EOF;
	 <position name="mPMT@{[ $i ]}_@{[ $j ]}_anode@{[ $k ]}_@{[$l]}_pos" x="(@{[ -($n_mPMT1d-1)/2 ]}+@{[ $j ]})*(mPMT_size+mPMT_gap)+(@{[ -($n_anode1d-1)/2 ]}+@{[ $l ]})*manode_size" y="(@{[ -($n_mPMT1d-1)/2 ]}+@{[ $i ]})*(mPMT_size+mPMT_gap)+(@{[ -($n_anode1d-1)/2 ]}+@{[ $k ]})*manode_size" z="mPMT_shift+0.5*mPMT_thick" />
EOF
					}
				}
			}
		}
		print DEF <<EOF;
	 
	 <quantity name="PMTplate_thick" value="5.8" unit="mm"/>
	 <quantity name="PMTplate_size" value="195.7" unit="mm"/>

	 <position name="PMTplate_pos" x="0" y="0" z="mPMT_shift+mPMT_thick+0.5*PMTplate_thick" />

	 <!-- ABOVE IS FOR ARICH -->

EOF
	}


	if($RPC_switch){
		print DEF <<EOF;

	 <!-- BELOW IS FOR RPC -->

	 <quantity name="RPC_thick" value="54" unit="mm" />
	 <quantity name="RPC_width" value="1066" unit="mm" />
	 <quantity name="RPC_height" value="252" unit="mm" />
	 <quantity name="RPC_shift" value="2699.7" unit="mm" />

	 <position name="RPC_pos" x="0" y="0" z="RPC_shift+0.5*RPC_thick"/>

	 <quantity name="RPC_Al_thick" value="1" unit="mm" />
	 <quantity name="RPC_comb_thick" value="17" unit="mm" />

	 <quantity name="RPC_PCB_thick" value="1" unit="mm" />
	 <quantity name="RPC_acrylic_thick" value="1" unit="mm" />
	 <quantity name="RPC_gas_width" value="960.5" unit="mm" />
	 <quantity name="RPC_gas_height" value="250.5" unit="mm" />
	 <quantity name="RPC_gas_thick" value="6" unit="mm" />

EOF
		for($i = 0; $i < $n_cover; ++$i){
			print DEF <<EOF;
	 <position name="RPC_Al@{[ $i ]}_pos" z="RPC_thick*($i-($n_cover-1)*0.5)"/>
EOF
		}
#		
#		for($i = 0; $i < $n_gas+1; ++$i){
#			print DEF <<EOF;
#	 <position name="RPC_PCB@{[ $i ]}_pos" z="(RPC_PCB_thick+RPC_gas_thick+RPC_acrylic_thick*2)*($i-$n_gas*0.5)" unit="mm"/>
#EOF
#		}
#		for($i = 0; $i < $n_gas; ++$i){
#			print DEF <<EOF;
#	 <position name="RPC_gas@{[ $i ]}_pos" z="(RPC_PCB_thick+RPC_gas_thick+RPC_acrylic_thick*2)*($i-($n_gas-1)*0.5)" unit="mm"/>
#EOF
#			for($j = 0; $j < $n_cover; ++$j){
#				print DEF <<EOF;
#	 <position name="RPC_acrylic@{[ $i ]}@{[ $j ]}_pos" z="(RPC_PCB_thick+RPC_gas_thick+RPC_acrylic_thick*2)*($i-($n_gas-1)*0.5)+(RPC_acrylic_thick+RPC_gas_thick)*($j-($n_cover-1)*0.5)" unit="mm"/>
#EOF
#			}
#		}
#
		print DEF <<EOF;

	 <!-- ABOVE IS FOR RPC -->
EOF
	}

	if($LG_switch){
		print DEF <<EOF;

	 <!-- BELOW IS FOR LG -->

	 <quantity name="LG_length" value="340" unit="mm" />
	 <quantity name="LG_height" value="122" unit="mm" />
	 <quantity name="LG_width0" value="113" unit="mm" />
	 <quantity name="LG_width1" value="135" unit="mm" />
	 <quantity name="LG_width1T" value="145.4824" unit="mm" />
	 <quantity name="LG_angle" value="3.7074" unit="degree"/>
	 <quantity name="LG_TransHorOff_shift" value="2." unit="mm" />

	 <quantity name="LG_protrusion_thick" value="40" unit="mm" />
	 <quantity name="LG_PMTr" value="38" unit="mm" />
	 <quantity name="LG_PMTl" value="120" unit="mm" />

	 <position name="LG_trap_pos1" x="0" y="0" z="-0.5*(LG_TransHorOff_shift+LG_length+LG_protrusion_thick+LG_PMTl-LG_length)"/>
	 <position name="LG_protrusion_pos" x="0" y="0" z="LG_length-0.5*(LG_TransHorOff_shift+LG_length+LG_protrusion_thick+LG_PMTl)+0.5*LG_protrusion_thick"/>
	 <position name="LG_PMT_pos" x="0" y="0" z="LG_length-0.5*(LG_TransHorOff_shift+LG_length+LG_protrusion_thick+LG_PMTl)+LG_protrusion_thick+0.5*LG_PMTl"/>


EOF
		for($i = 0; $i < $n_LG; ++$i){
			print DEF <<EOF;
	 <position name="LG_block@{[ $i ]}0_pos" x="(0.5*(LG_width0+LG_width1T)+LG_TransHorOff_shift+Tolerance_space)" y="LG_height*($i-1)+2.5*($i-1)" z="0."/>
	 <position name="LG_block@{[ $i ]}1_pos" x="0" y="LG_height*($i-1)+2.5*($i-1)" z="0"/>
	 <position name="LG_block@{[ $i ]}2_pos" x="-(0.5*(LG_width0+LG_width1T)+LG_TransHorOff_shift+Tolerance_space)" y="LG_height*($i-1)+2.5*($i-1)" z="0."/>
	 <rotation name="LG_block@{[ $i ]}0_rot" x="0" y="-1.0*LG_angle" z="0" />
	 <rotation name="LG_block@{[ $i ]}1_rot" x="0" y="0" z="0" />
	 <rotation name="LG_block@{[ $i ]}2_rot" x="0" y="LG_angle" z="0" />
EOF
		}

		print DEF <<EOF;
	 <quantity name="calor_length" value="520" unit="mm" />
	 <quantity name="calor_height" value="380" unit="mm" />
	 <quantity name="calor_width" value="900" unit="mm" />

	 <quantity name="calor_shift" value="2886.4" unit="mm" />
	 <position name="calor_pos" x="0" y="0" z="calor_shift+calor_length*0.5"/>

	 <!-- ABOVE IS FOR LG -->

EOF
	}

	print DEF <<EOF;
  </define>

</gdml>
EOF
	close (DEF);
}


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++ gen_Materials +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_Materials() 
{

	# Create the <materials> fragment file name,
	# add file to list of output GDML fragments,
	# and open it
	$MAT = "phase1_Materials" . $suffix . ".gdml";
	push (@gdmlFiles, $MAT);
	$MAT = ">" . $MAT;
	open(MAT) or die("Could not open file $MAT for writing");


	print MAT <<EOF;

  @{[ dumpfile('material.gdml')]}

EOF

	close(MAT);
}


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++ gen_PMTs +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_PMTs() 
{

	# Create the <PMT> fragment file name,
	# add file to list of output GDML fragments,
	# and open it
	$MAT = "phase1_PMTs" . $suffix . ".gdml";
	push (@gdmlFiles, $MAT);
	$MAT = ">" . $MAT;
	open(MAT) or die("Could not open file $MAT for writing");


	print MAT <<EOF;

  @{[ dumpfile('PMT.gdml')]}

EOF

	close(MAT);
}



#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++ gen_Solids +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_Solids() 
{

	# Create the <solids> fragment file name,
	# add file to list of output GDML fragments,
	# and open it
	$SOL = "phase1_Solids" . $suffix . ".gdml";
	push (@gdmlFiles, $SOL);
	$SOL = ">" . $SOL;
	open(SOL) or die("Could not open file $SOL for writing");


	print SOL <<EOF;

	<solids>

	 <box name="world_box" x="world_size" y="world_size" z="world_size" />

EOF
	if($T0_switch){
		print SOL <<EOF;

	 <!-- BELOW IS FOR T0 -->

	 <box name="T0_box" x="T0_width" y="T0_height" z="T0_length" />
	 <box name="T0_acrylic_box" x="T0_acrylic_width" y="T0_acrylic_height" z="T0_acrylic_length" />
	 <union name="T0_acrylic_union">
		<first ref="T0_acrylic_box"/>  <second ref="T0_acrylic_box"/>
		<positionref ref="center" />
		<rotationref ref="T0_union1_rot" />
	 </union>	 

	 <!-- ABOVE IS FOR T0 -->

EOF
	}

	if($target_switch){
		print SOL <<EOF;

	 <!-- BELOW IS FOR TARGET -->

	 <box name="target_box" x="target_width" y="target_height" z="target_thick" />

	 <!-- ABOVE IS FOR TARGET -->
EOF
	}

	if($magnet_switch){
		print SOL <<EOF;

	 <!-- BELOW IS FOR MAGNET -->

    <box name="magnet_box" x="magnetSideOHeight"
	    y="magnetSideOHeight" z="magnetSideZLength" />
EOF
	for($i = 0; $i< $magnet_layer; ++$i){
		print SOL <<EOF;
	 <tube name="magnet_tube@{[ $i ]}" rmin="0.5*magnetSideIHeight@{[ $i ]}" rmax="0.5*magnetSideOHeight" z="magnetSideWidth" deltaphi="360" aunit="deg"/>
EOF
	}
	print SOL <<EOF;

	 <!-- ABOVE IS FOR MAGNET -->

EOF
	}

	if($SSD_switch){
		print SOL <<EOF;

	 <!-- BELOW IS FOR SSD -->

	 <box name="ssd_chan_box" x="ssdD0_width" y="ssdD0_chanwidth" z="ssdD0_thick" />
	 <box name="ssd_box" x="ssdD0_width" y="ssdD0_height" z="ssdD0_thick" />
    <box name="ssd_bkpln_box1" x="bkpln_width" y="bkpln_width" z="bkpln_thick" />
    <box name="ssd_bkpln_box2" x="bkpln_hole" y="bkpln_hole" z="bkpln_thick" />
    <tube name="ssd_bkpln_tube" z="bkpln_thick" rmax="85.0" deltaphi="90" aunit="deg"/>
    <trd name="ssd_bkpln_trd" x1="bkpln_thick" x2="bkpln_thick" y1="115.98" y2="170.00" z="140"/>
    <trd name="ssd_bkpln_tri" x1="bkpln_thick" x2="bkpln_thick" y1="0" y2="115.98" z="57.99"/>

    <union name="ssd_bkpln_u1">
	   <first ref="ssd_bkpln_box1"/>
		<second ref="ssd_bkpln_trd"/>
		<positionref ref="ssd_bkpln_u1_pos"/>
	   <rotationref ref="ssd_bkpln_u1_rot"/>
	 </union>
	 <union name="ssd_bkpln_u2">
		<first ref="ssd_bkpln_u1"/>
		<second ref="ssd_bkpln_trd"/>
		<positionref ref="ssd_bkpln_u2_pos"/>
	   <rotationref ref="ssd_bkpln_u2_rot"/>
	 </union>
	 <union name="ssd_bkpln_u3">
		<first ref="ssd_bkpln_u2"/>
		<second ref="ssd_bkpln_tube"/>
	   <rotationref ref="ssd_bkpln_u3_rot"/>
	 </union>
	 <union name="ssd_bkpln_u4">
	   <first ref="ssd_bkpln_u3"/>
		<second ref="ssd_bkpln_tri"/>
		<positionref ref="ssd_bkpln_u4_pos"/>
	   <rotationref ref="ssd_bkpln_u4_rot"/>
	 </union>

	 <subtraction name="ssd_bkpln_box">
	   <first ref="ssd_bkpln_u4"/>
		<second ref="ssd_bkpln_box2"/>
	 </subtraction>

EOF
		for($i = 0; $i < $nstation_type; ++$i){
			print SOL <<EOF;
	  <box name="ssdStation@{[ $station_type[$i] ]}_box" x="ssdStation@{[ $station_type[$i] ]}Width" y="ssdStation@{[ $station_type[$i] ]}Height" z="ssdStation@{[ $station_type[$i] ]}Length" />
	  <box name="ssd@{[ $station_type[$i] ]}_MylarWindow_box" x="ssdStation@{[ $station_type[$i] ]}Width*0.8" y="ssdStation@{[ $station_type[$i] ]}Width*0.8" z="Mylar_Window_thick" />

EOF
		}
		print SOL <<EOF;
	 <!-- ABOVE IS FOR SSD -->

EOF
	}

	if($arich_switch){
		print SOL <<EOF;

	 <!-- BELOW IS FOR ARICH -->

	 <box name="arich_box" x="arich_width" y="arich_height" z="arich_thick" />

EOF
		for($i = 0; $i< $n_aerogel; ++$i){
			print SOL <<EOF;
	 <box name="aerogel_box@{[ $i ]}" x="aerogel_size" y="aerogel_size" z="aerogel_thick@{[ $i ]}" />
EOF
		}
		print SOL <<EOF;

EOF
		for($i = 0; $i < $n_mPMT1d; ++$i){
			for($j = 0; $j < $n_mPMT1d; ++$j){
				for($k = 0; $k < $n_anode1d; ++$k){
					for($l = 0; $l < $n_anode1d; ++$l){
						print SOL <<EOF;
	 <box name="mPMT_box@{[ $i ]}_@{[ $j ]}_anode@{[ $k ]}_@{[$l]}" x="manode_size" y="manode_size" z="mPMT_thick"/>
EOF
					}
				}
			}
		}
		print SOL <<EOF;

	 <box name="PMTplate_box" x="PMTplate_size" y="PMTplate_size" z="PMTplate_thick"/>

	 <!-- ABOVE IS FOR ARICH -->
EOF
	}

	if($RPC_switch){
		print SOL <<EOF;

	 <!-- BELOW IS FOR RPC -->

	 <box name="RPC_box" x="Tolerance_space+RPC_width" y="Tolerance_space+RPC_height" z="Tolerance_space+RPC_thick"/>

	 <box name="RPC_Al_box" x="RPC_width" y="RPC_height" z="RPC_Al_thick"/>
	 <box name="RPC_comb_box" x="RPC_width" y="RPC_height" z="RPC_comb_thick"/>

	 <box name="RPC_PCB_box" x="RPC_width" y="RPC_height" z="RPC_PCB_thick"/>
	 <box name="RPC_acrylic_box" x="RPC_gas_width" y="RPC_gas_height" z="RPC_acrylic_thick"/>
	 <box name="RPC_gas_box" x="RPC_gas_width" y="RPC_gas_height" z="RPC_gas_thick"/>

	 <!-- ABOVE IS FOR RPC -->
EOF
	}
	if($LG_switch){
		#DEBUG
		print SOL <<EOF;

	 <!-- BELOW IS FOR LG -->
	 
	 <trd name="LG_box1" x1="(Tolerance_space+LG_width0)" x2="(Tolerance_space+LG_width1T)" y1="(2*Tolerance_space+LG_height)" y2="(2*Tolerance_space+LG_height)" z="2.0+LG_TransHorOff_shift+LG_length+LG_protrusion_thick+LG_PMTl" />
	 <trd name="LG_trap" x1="LG_width0" x2="LG_width1" y1="LG_height" y2="LG_height" z="LG_length" />
	 <tube name="LG_protrusion_tube" rmin="0" rmax="LG_PMTr" z="LG_protrusion_thick" deltaphi="360" aunit="deg"/>
	 <tube name="LG_PMT_tube" rmax="LG_PMTr" z="LG_PMTl" deltaphi="360" aunit="deg"/>

	 <box name="calor_box" x="calor_width" y="calor_height" z="calor_length"/>

	 <!-- ABOVE IS FOR LG -->

EOF
	}

	print SOL <<EOF;
	</solids>	

EOF
	close(SOL);
}


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++ gen_Modules +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_Modules() 
{

	# Create the Modules
	# add file to list of output GDML fragments,
	# and open it
	$MOD = "phase1_Modules" . $suffix . ".gdml";
	push (@gdmlFiles, $MOD);
	$MOD = ">" . $MOD;
	open(MOD) or die("Could not open file $MOD for writing");


	print MOD <<EOF;
  <structure>    
EOF
	if($T0_switch){
		print MOD <<EOF;

  <!-- BELOW IS FOR T0 -->

  <volume name="T0_acrylic_vol">
	 <materialref ref="Acrylic"/>
	 <solidref ref="T0_acrylic_union"/>
  </volume>

  <!-- ABOVE IS FOR T0 -->

EOF
	}

	if($target_switch){
		print MOD <<EOF;

  <!-- BELOW IS FOR TARGET -->

  <volume name="target_vol">
	 <materialref ref="@{[ $target_matt[$target_v] ]}"/>
	 <solidref ref="target_box"/>
  </volume>

  <!-- ABOVE IS FOR TARGET -->

EOF
	}

	if($magnet_switch){
		print MOD <<EOF;

  <!-- BELOW IS FOR MAGNET -->
EOF

		for($i = 0; $i< $magnet_layer; ++$i){
			print MOD <<EOF;
  <volume name="magnetSide_vol@{[ $i ]}">
	 <materialref ref="NeodymiumAlloy"/>
	 <solidref ref="magnet_tube@{[ $i ]}"/>
  </volume>
EOF
		}

		print MOD <<EOF
  <!-- ABOVE IS FOR MAGNET -->

EOF
	}

	if($SSD_switch){
		print MOD <<EOF;

  <!-- BELOW IS FOR SSD -->

       <volume name="ssd_chan_vol">
			<materialref ref="SiliconWafer"/>
			<solidref ref="ssd_chan_box"/>
		</volume>

EOF
		$station=0;
		$lay=0;
		$sen=0;
		for($i = 0; $i < $nstation_type; ++$i){
			for($l = 0; $l < $SSD_station[$i]; ++$l){
				for($j = 0; $j < $SSD_lay[$i]; ++$j){
					for($k = 0; $k < $SSD_par[$i]; ++$k){

						print MOD <<EOF;
		 <volume name="ssd@{[ $station_type[$i] ]}@{[ $station ]}@{[ $j ]}@{[ $k ]}_vol">
			<materialref ref="SiliconWafer"/>
			<solidref ref="ssd_box"/>
EOF
						for($m = 0; $m < $nD0chan; ++$m){
							print MOD <<EOF;
		 <physvol name="ssd_chan_@{[ $station ]}_@{[ $lay ]}_@{[ $sen ]}_@{[ $m ]}_vol">
			<volumeref ref="ssd_chan_vol"/>
			<positionref ref="ssd_chan_@{[ $m ]}_pos"/>
		 </physvol>
EOF
						}
						print MOD <<EOF;
		 </volume>
EOF
						++$sen;
					}
					++$lay;
				}
				++$station;
			}
		}

		for($i = 0; $i < $nstation_type; ++$i){
			print MOD <<EOF;
		 <volume name="ssd@{[ $station_type[$i] ]}_bkpln_vol">
			<materialref ref="CarbonFiber"/>
			<solidref ref="ssd_bkpln_box"/>
		 </volume>

	  <volume name="ssd@{[ $station_type[$i] ]}_MylarWindow_vol">
		 <materialref ref="Mylar"/>
		 <solidref ref="ssd@{[ $station_type[$i] ]}_MylarWindow_box"/>
	  </volume>

EOF
		}
		print MOD <<EOF;

  <!-- ABOVE IS FOR SSD -->

EOF
	}

	if($arich_switch){
		print MOD <<EOF;

  <!-- BELOW IS FOR ARICH -->

EOF
		for($i = 0; $i< $n_aerogel; ++$i){
			print MOD <<EOF;
		<volume name="aerogel_vol@{[ $i ]}">
			<materialref ref="@{[ $aerogel_mat[$i] ]}"/>
			<solidref ref="aerogel_box@{[ $i ]}"/>
		</volume>
EOF
		}
		for($i = 0; $i < $n_mPMT1d; ++$i){
			for($j = 0; $j < $n_mPMT1d; ++$j){
				for($k = 0; $k < $n_anode1d; ++$k){
					for($l = 0; $l < $n_anode1d; ++$l){
						print MOD <<EOF;
	 <volume name="mPMT_vol@{[ $i ]}_@{[ $j ]}_anode@{[ $k ]}_@{[$l]}">
	 	<materialref ref="SiliconDioxide"/>
	 	<solidref ref="mPMT_box@{[ $i ]}_@{[ $j ]}_anode@{[ $k ]}_@{[$l]}"/>
	</volume>
EOF
					}
				}
			}
		}

		print MOD <<EOF;

		<volume name="PMTplate_vol">
			<materialref ref="FiberGlass"/>
			<solidref ref="PMTplate_box"/>
		</volume>

  <!-- ABOVE IS FOR ARICH -->

EOF
	}


	if($RPC_switch){
		print MOD <<EOF;

  <!-- BELOW IS FOR RPC -->

  <volume name="RPC_Al_vol">
	 <materialref ref="AlShell"/>
	 <solidref ref="RPC_Al_box"/>
  </volume>

  <volume name="RPC_PCB_vol">
	 <materialref ref="CarbonFiber"/>
	 <solidref ref="RPC_PCB_box"/>
  </volume>

  <volume name="RPC_comb_vol">
	 <materialref ref="Air"/>
	 <solidref ref="RPC_comb_box"/>
  </volume>

  <volume name="RPC_acrylic_vol">
	 <materialref ref="Acrylic"/>
	 <solidref ref="RPC_acrylic_box"/>
  </volume>

  <volume name="RPC_gas_vol">
	 <materialref ref="RPC_Gas"/>
	 <solidref ref="RPC_gas_box"/>
  </volume>

  <!-- ABOVE IS FOR RPC -->

EOF
	}

	if($LG_switch){
		print MOD <<EOF;


  <!-- BELOW IS FOR LG -->

  <volume name="LG_glass_vol">
	 <materialref ref="LeadGlass"/>
	 <solidref ref="LG_trap"/>
  </volume>
  <volume name="LG_protrusion_vol">
	 <materialref ref="LeadGlass"/>
	 <solidref ref="LG_protrusion_tube"/>
  </volume>
  <volume name="LG_PMT_vol">
	 <materialref ref="Air"/>
	 <solidref ref="LG_PMT_tube"/>
  </volume>
EOF
	for($i = 0; $i< $n_LG; ++$i){
		for($j = 0; $j< $m_LG; ++$j){
		
				print MOD <<EOF;
           <volume name="LG_block@{[ $i ]}@{[ $j ]}_vol">
	   <materialref ref="Air"/>
	   <solidref ref="LG_box1"/>
	   <physvol name="LG_glass@{[ $i ]}@{[ $j ]}_phys">
		<volumeref ref="LG_glass_vol"/>
		<positionref ref="LG_trap_pos1"/>
	   </physvol>
	   <physvol name="LG_protrusion@{[ $i ]}@{[ $j ]}_phys">
		<volumeref ref="LG_protrusion_vol"/>
		<positionref ref="LG_protrusion_pos"/>
	   </physvol>
	   <physvol name="LG_PMT@{[ $i ]}@{[ $j ]}_phys">
		<volumeref ref="LG_PMT_vol"/>
		<positionref ref="LG_PMT_pos"/>
	   </physvol>
        </volume>
EOF
			}
		}
		print MOD <<EOF;
  
  <!-- ABOVE IS FOR LG -->

EOF
	}
	print MOD <<EOF;

  </structure>    

EOF

	close(MOD);
}

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++ gen_DetEnclosure +++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_DetEnclosure() 
{

	# Create the DetEnclosure
	# add file to list of output GDML fragments,
	# and open it
	$DET = "phase1_DetEnclosure" . $suffix . ".gdml";
	push (@gdmlFiles, $DET);
	$DET = ">" . $DET;
	open(DET) or die("Could not open file $DET for writing");

	print DET <<EOF;
  <structure>
EOF
	if($T0_switch){
		print DET <<EOF;

  <!-- BELOW IS FOR T0 -->

  <volume name="T0_vol">
	 <materialref ref="Air"/>
	 <solidref ref="T0_box"/>
EOF
		for($i = 0; $i < $n_acrylic; ++$i){
			print DET <<EOF;
	 <physvol name="T0_acrylic@{[ $i ]}_phys">
		<volumeref ref="T0_acrylic_vol"/>
		<positionref ref="T0_acrylic@{[ $i ]}_pos"/>
		<rotationref ref="T0_acrylic_rot"/>
	 </physvol>
EOF
		}
		print DET <<EOF;
  </volume>

  <!-- BELOW IS FOR T0 -->

EOF
	}
	if($magnet_switch){
		print DET <<EOF;

  <!-- BELOW IS FOR MAGNET -->

  <volume name="magnet_vol">
	 <materialref ref="Air"/>
	 <solidref ref="magnet_box"/>  
EOF

      for($i = 0; $i < $magnet_layer; ++$i){
         print DET <<EOF;
	 <physvol name="magnetSide@{[ $i ]}_phys">
		 <volumeref ref="magnetSide_vol@{[ $i ]}"/>
		 <positionref ref="magnetSide_pos@{[ $i ]}"/>
	 </physvol>
EOF
		}

		print DET <<EOF;
  </volume>

  <!-- ABOVE IS FOR MAGNET -->

EOF
	}
	if($SSD_switch){
		print DET <<EOF;

  <!-- BELOW IS FOR SSD -->

EOF
		$station=0;
		for($i = 0; $i < $nstation_type; ++$i){
			for($l = 0; $l < $SSD_station[$i]; ++$l){
				print DET <<EOF;
	  <volume name="ssdStation@{[ $station_type[$i] ]}@{[ $l ]}_vol">
		 <materialref ref="Air"/>
		 <solidref ref="ssdStation@{[ $station_type[$i] ]}_box"/>
		 <physvol name="ssd@{[ $station_type[$i] ]}@{[ $l ]}_USMylarWindow_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}_MylarWindow_vol"/>
			<positionref ref="ssd@{[ $station_type[$i] ]}_USMylarWindow_pos"/>
		 </physvol>
		 <physvol name="ssd@{[ $station_type[$i] ]}@{[ $l ]}_DSMylarWindow_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}_MylarWindow_vol"/>
			<positionref ref="ssd@{[ $station_type[$i] ]}_DSMylarWindow_pos"/>
		 </physvol>

EOF
				for($j = 0; $j < $SSD_lay[$i]; ++$j){
					for($k = 0; $k < $SSD_par[$i]; ++$k){

						print DET <<EOF;
		 <physvol name="ssdsensor@{[ $station_type[$i] ]}@{[ $l ]}@{[ $j ]}@{[ $k ]}_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}@{[ $station ]}@{[ $j ]}@{[ $k ]}_vol"/>
			<positionref ref="ssd@{[ $station_type[$i] ]}@{[ $j ]}@{[ $k ]}_pos"/>
			<rotationref ref="ssd@{[ $station_type[$i] ]}@{[ $l ]}_@{[ $j ]}_@{[ $k ]}_rot"/>
		 </physvol>
EOF
					}
				}
				for($j = 0; $j < $SSD_bkpln[$i]; ++$j){
					print DET <<EOF;
		 <physvol name="ssdbkpln@{[ $station_type[$i] ]}@{[ $l ]}@{[ $j ]}_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}_bkpln_vol"/>
			<positionref ref="ssdbkpln@{[ $station_type[$i] ]}@{[ $j ]}_pos"/>
		 </physvol>
EOF

				}
				print DET <<EOF;
	  </volume>

EOF
				++$station;
			}
		}
		print DET <<EOF;

  <!-- ABOVE IS FOR SSD -->

EOF
	}
	if($arich_switch){
		print DET <<EOF;

  <!-- BELOW IS FOR ARICH -->

  <volume name="arich_vol">
	 <materialref ref="Air"/>
	 <solidref ref="arich_box"/>
EOF

      for($i = 0; $i < $n_aerogel; ++$i){
         print DET <<EOF;
	 <physvol name="aerogel@{[ $i ]}_phys">
		 <volumeref ref="aerogel_vol@{[ $i ]}"/>
		 <positionref ref="aerogel_pos@{[ $i ]}"/>
	 </physvol>
EOF
		}

		for($i = 0; $i < $n_mPMT1d; ++$i){
			for($j = 0; $j < $n_mPMT1d; ++$j){
				for($k = 0; $k < $n_anode1d; ++$k){
					for($l = 0; $l < $n_anode1d; ++$l){
						print DET <<EOF;
	 <physvol name="mPMT_phys@{[ $i ]}_@{[ $j ]}_anode@{[ $k ]}_@{[$l]}">
	 	<volumeref ref="mPMT_vol@{[ $i ]}_@{[ $j ]}_anode@{[ $k ]}_@{[$l]}"/>
	 	<positionref ref="mPMT@{[ $i ]}_@{[ $j ]}_anode@{[ $k ]}_@{[$l]}_pos"/>
	</physvol>
EOF
					}
				}
			}
		}

		print DET <<EOF;

	  <physvol name="PMTplate_phys">
		 <volumeref ref="PMTplate_vol"/>
		 <positionref ref="PMTplate_pos"/>
	 </physvol>

  </volume>

  <!-- ABOVE IS FOR ARICH -->

EOF
	}

	if($RPC_switch){
		print DET <<EOF;

  <!-- BELOW IS FOR RPC -->

  <volume name="RPC_vol">
	 <materialref ref="Air"/>
	 <solidref ref="RPC_box"/>
EOF
		for($i = 0; $i < $n_cover; ++$i){
			print DET <<EOF;
	 <physvol name="RPC_Al@{[ $i ]}_phys">
		<volumeref ref="RPC_Al_vol"/>
		<positionref ref="RPC_Al@{[ $i ]}_pos"/>
	 </physvol>
EOF
		}
		print DET <<EOF;
  </volume>

  <!-- ABOVE IS FOR RPC -->

EOF
	}

	if($LG_switch){
		print DET <<EOF;


  <!-- BELOW IS FOR LG -->
  
  <volume name="calor_vol">
	 <materialref ref="Air"/>
	 <solidref ref="calor_box"/>
EOF
		for($i = 0; $i< $n_LG; ++$i){
			for($j = 0; $j< $m_LG; ++$j){
				print DET <<EOF;
	 <physvol name="LG_block@{[ $i ]}@{[ $j ]}_phys">
		<volumeref ref="LG_block@{[ $i ]}@{[ $j ]}_vol"/>
		<positionref ref="LG_block@{[ $i ]}@{[ $j ]}_pos"/>
		<rotationref ref="LG_block@{[ $i ]}@{[ $j ]}_rot"/>
	 </physvol>
EOF
			}
		}
		print DET <<EOF;
  </volume>

  <!-- ABOVE IS FOR LG -->

EOF
	}
	print DET <<EOF;

  </structure> 

EOF

	close(DET);
}


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++ gen_World +++++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub gen_World()
{

	# Create the WORLD fragment file name,
	# add file to list of output GDML fragments,
	# and open it
	$WORLD = "phase1_World" . $suffix . ".gdml";
	push (@gdmlFiles, $WORLD);
	$WORLD = ">" . $WORLD;
	open(WORLD) or die("Could not open file $WORLD for writing");


	# The standard XML prefix and starting the gdml
	print WORLD <<EOF;
<?xml version='1.0'?>
<gdml>
EOF


	# All the World solids.
	print WORLD <<EOF;

  <structure>

  <volume name="world">
	 <materialref ref="Air"/>
	 <solidref ref="world_box"/>
EOF
	if($T0_switch){
		print WORLD <<EOF;

  <!-- BELOW IS FOR T0 -->

  <physvol name="T0_phys">
	 <volumeref ref="T0_vol"/>
	 <positionref ref="T0_pos"/>
  </physvol>

  <!-- ABOVE IS FOR T0 -->

EOF
	}

	if($target_switch){
		print WORLD <<EOF;

  <!-- BELOW IS FOR TARGET -->

  <physvol name="target_phys">
	 <volumeref ref="target_vol"/>
	 <positionref ref="target_pos"/>
  </physvol>

  <!-- ABOVE IS FOR TARGET -->

EOF
	}

	if($magnet_switch){
		print WORLD <<EOF;

  <!-- BELOW IS FOR MAGNET -->

  <physvol name="magnet_phys">
	 <volumeref ref="magnet_vol"/>
	 <positionref ref="magnet_pos"/>
  </physvol>

  <!-- ABOVE IS FOR MAGNET -->
EOF
	}
	if($SSD_switch){
		print WORLD <<EOF;

  <!-- BELOW IS FOR SSD -->

EOF

		$station = 0;
		for($i = 0; $i < $nstation_type; ++$i){
				for($j = 0; $j < $SSD_station[$i]; ++$j){

					print WORLD <<EOF;

		  <physvol name="ssdStation@{[ $station ]}_phys">
			 <volumeref ref="ssdStation@{[ $station_type[$i] ]}@{[ $j ]}_vol"/>
			 <positionref ref="ssdStation@{[ $station ]}_pos"/>
		  </physvol>
EOF

					$station++;
			}
		}  

		print WORLD <<EOF;
  <!-- ABOVE IS FOR SSD -->

EOF
	}

	if($arich_switch){
		print WORLD <<EOF;

  <!-- BELOW IS FOR ARICH -->

  <physvol name="ARICH_phys">
	 <volumeref ref="arich_vol"/>
	 <positionref ref="arich_pos"/>
  </physvol>

  <!-- ABOVE IS FOR ARICH -->

EOF
	}

	if($RPC_switch){
		print WORLD <<EOF;

  <!-- BELOW IS FOR RPC -->

  <physvol name="RPC_phys">
	 <volumeref ref="RPC_vol"/>
	 <positionref ref="RPC_pos"/>
  </physvol>

  <!-- ABOVE IS FOR RPC -->

EOF
	}
	if($LG_switch){
		print WORLD <<EOF;


  <!-- BELOW IS FOR LG -->

  <physvol name="LGCalo_phys">
	 <volumeref ref="calor_vol"/>
	 <positionref ref="calor_pos"/>
  </physvol>

  <!-- ABOVE IS FOR LG -->

EOF
	}
	print WORLD <<EOF;

  </volume>

  </structure>

</gdml>
EOF

	# make_gdml.pl will take care of <setup/>

	close(WORLD);
}



#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++ write_fragments ++++++++++++++++++++++++++++++++++++
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sub write_fragments()
{
	# This subroutine creates an XML file that summarizes the the subfiles output
	# by the other sub routines - it is the input file for make_gdml.pl which will
	# give the final desired GDML file. Specify its name with the output option.
	# (you can change the name when running make_gdml)

	# This code is taken straigh from the similar MicroBooNE generate script, Thank you.

	if ( ! defined $output )
	{
		$output = "-"; # write to STDOUT 
	}

	# Set up the output file.
	$OUTPUT = ">" . $output;
	open(OUTPUT) or die("Could not open file $OUTPUT");

	print OUTPUT <<EOF;
<?xml version='1.0'?>

<!-- Input to Geometry/gdml/make_gdml.pl; define the GDML fragments
	  that will be zipped together to create a detector description. 
	  -->

<config>

	<constantfiles>

		<!-- These files contain GDML <constant></constant>
			  blocks. They are read in separately, so they can be
			  interpreted into the remaining GDML. See make_gdml.pl for
			  more information. 
		-->

EOF

	foreach $filename (@defFiles)
	{
		print OUTPUT <<EOF;
		<filename> $filename </filename>
EOF
	}

	print OUTPUT <<EOF;

	</constantfiles>

	<gdmlfiles>

		<!-- The GDML file fragments to be zipped together. -->

EOF

	foreach $filename (@gdmlFiles)
	{
		print OUTPUT <<EOF;
		<filename> $filename </filename>
EOF
	}

	print OUTPUT <<EOF;

	</gdmlfiles>

</config>
EOF

	close(OUTPUT);
}

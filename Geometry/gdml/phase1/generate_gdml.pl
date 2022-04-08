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

use Getopt::Long;
use Math::BigFloat;
use GDMLUtil;
Math::BigFloat->precision(-10);

GetOptions( "help|h" => \$help,
	"suffix|s:s" => \$suffix,
	"output|o:s" => \$output,
	"helpcube|c" => \$helpcube);

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
$n_acrylic = 10;

# constants for target
$target_switch = 1;

# constants for magnet
$magnet_switch = 1;
$n_magseg = 16;

# constants for SSD
$SSD_switch = 1;
$nstation_type = 3; # types of station
@station_type = ("single", "rotate", "double"); # xy, uxy, yyxxuu 
@bkpln_size = (1.0, 1.3, 2.6); # bkpln size scale to SSD sensor
@SSD_lay = (2, 3, 3); # num. of SSD layer in a station
@SSD_par = (1, 1, 2); # num. of SSD in a layer
@SSD_bkpln= (1, 2, 2); # num. of bkpln in a station
@SSD_mod = ("D0", "D0", "D0"); # SSD type in a station
@SSD_station = (2, 2, 2); # num. of stations

# constants for RPC
$RPC_switch = 1;
$n_glass = 6;
$n_gas = 2;
$n_cover=2;

# constants for LG
$LG_switch = 1;
$n_LG = 3; # horizontal
$m_LG = 3; # vertical

# run the sub routines that generate the fragments

gen_Define(); 	 # generates definitions at beginning of GDML
gen_Materials(); # generates materials to be used
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
	print "Usage: $0 [-h|--help] [-o|--output <fragments-file>] [-s|--suffix <string>]\n";
	print "       if -o is omitted, output goes to STDOUT; <fragments-file> is input to make_gdml.pl\n";
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
	 <quantity name="world_size" value="3000." unit="mm"/>
	 <position name="center" x="0" y="0" z="0" unit="mm"/>

EOF
	if($T0_switch){
		print DEF <<EOF;
	 <!-- BELOW IS FOR T0 -->

	 <quantity name="T0_length" value="280.0" unit="mm"/>
	 <quantity name="T0_width" value="210.0" unit="mm"/>
	 <quantity name="T0_height" value="300.0" unit="mm"/>
	 <position name="T0_pos" z="-350" unit="mm"/>

	 <quantity name="T0_acrylic_length" value="150.0" unit="mm"/>
	 <quantity name="T0_acrylic_width" value="3.0" unit="mm"/>
	 <quantity name="T0_acrylic_height" value="3.0" unit="mm"/>

EOF

		for($i = 0; $i < $n_acrylic; ++$i){
			print DEF <<EOF;
	 <position name="T0_acrylic@{[ $i ]}_pos" x="T0_acrylic_width*($i-($n_acrylic-1)*0.5)" z="-40" unit="mm"/>
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

	 <position name="target_pos" x="0" y="0" z="0" unit="mm"/>

	 <!-- ABOVE IS FOR TARGET -->

EOF
	}

	if($magnet_switch){
		print DEF <<EOF;
	 <!-- BELOW IS FOR MAGNET -->

	 <quantity name="magnetSideYOffset" value="29" unit="mm" />
	 <quantity name="magnetSideUSBottomWidth" value="11.5" unit="mm"/>
	 <quantity name="magnetSideDSBottomWidth" value="33.3" unit="mm"/>
	 <quantity name="magnetSideTopWidth" value="71.6" unit="mm"/>
	 <quantity name="magnetSideZLength" value="150" unit="mm"/>
	 <quantity name="magnetSideUSHeight" value="151" unit="mm"/>
	 <quantity name="magnetSideDSHeight" value="96.2" unit="mm"/>

	 <position name="magnetSide_pos" x="0" y="0" z="0" unit="mm"/>
	 <position name="magnet_pos" x="0" y="0" z="370" unit="mm"/>

EOF
		for($i = 0; $i < $n_magseg; ++$i){
			print DEF <<EOF;
	 <rotation name="RotateZMagSeg@{[ $i ]}" z="(-157.5+22.5*$i)" unit="deg"/>
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
	 <quantity name="ssdD0_height" value="98.33" unit="mm"/>
	 <quantity name="ssdD0_width" value="38.34" unit="mm"/>

	 <quantity name="carbon_fiber_thick" value="0.300"
		 unit="mm" />
	 <quantity name="Mylar_Window_thick" value="0.500"
		 unit="mm" />

	 <quantity name="ssdStationsingleLength" value="50" unit="mm" />
	 <quantity name="ssdStationsingleWidth" value="150" unit="mm" />
	 <quantity name="ssdStationsingleHeight" value="150" unit="mm" />
	 <position name="ssdStation0_pos" x="0" y="0" z="-170" unit="mm" />
	 <position name="ssdStation1_pos" x="0" y="0" z="-120" unit="mm" />
	 <position name="ssdsingle00_pos" x="0" y="0" z="0" unit="mm"/>
	 <rotation name="ssdsingle0_rot" z="90" unit="deg"/>
	 <position name="ssdbkplnsingle0_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
	 <position name="ssdsingle10_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssdsingle1_rot" z="0" unit="deg"/>
	 <position name="ssdsingle_USMylarWindow_pos" x="0" y="0"
			z="-10." unit="mm" />
	 <position name="ssdsingle_DSMylarWindow_pos" x="0" y="0"
			z="10." unit="mm" />

	 <quantity name="ssdStationrotateLength" value="50" unit="mm" />
	 <quantity name="ssdStationrotateWidth" value="200" unit="mm" />
	 <quantity name="ssdStationrotateHeight" value="200" unit="mm" />
	 <position name="ssdStation2_pos" x="0" y="0" z="120" unit="mm" />
	 <position name="ssdStation3_pos" x="0" y="0" z="170" unit="mm" />
	 <position name="ssdrotate00_pos" x="0" y="0" z="0" unit="mm"/>
	 <rotation name="ssdrotate0_rot" z="45" unit="deg"/>
	 <position name="ssdbkplnrotate0_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
	 <position name="ssdrotate10_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssdrotate1_rot" z="90" unit="deg"/>
	 <position name="ssdrotate20_pos" x="0" y="0" z="3" unit="mm"/>
	 <rotation name="ssdrotate2_rot" z="0" unit="deg"/>	 
	 <position name="ssdbkplnrotate1_pos" x="0" y="0" z="3+ssdD0_thick" unit="mm"/>
	 <position name="ssdrotate_USMylarWindow_pos" x="0" y="0"
			z="-10." unit="mm" />
	 <position name="ssdrotate_DSMylarWindow_pos" x="0" y="0"
		 z="10." unit="mm" />

	 <quantity name="ssdStationdoubleLength" value="50" unit="mm" />
	 <quantity name="ssdStationdoubleWidth" value="300" unit="mm" />
	 <quantity name="ssdStationdoubleHeight" value="300" unit="mm" />
	 <position name="ssdStation4_pos" x="0" y="0" z="570" unit="mm" />
	 <position name="ssdStation5_pos" x="0" y="0" z="630" unit="mm" />
	 <position name="ssddouble00_pos" x="-0.5*ssdD0_width" y="0" z="0" unit="mm"/>
	 <position name="ssddouble01_pos" x="0.5*ssdD0_width" y="0" z="0" unit="mm"/>
	 <rotation name="ssddouble0_rot" z="0" unit="deg"/>
	 <position name="ssdbkplndouble0_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
	 <position name="ssddouble10_pos" y="-0.5*ssdD0_width" x="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <position name="ssddouble11_pos" y="0.5*ssdD0_width" x="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssddouble1_rot" z="90" unit="deg"/>
	 <position name="ssddouble20_pos" x="-0.354*ssdD0_width" y="-0.354*ssdD0_width" z="3" unit="mm"/>
	 <position name="ssddouble21_pos" x="0.354*ssdD0_width" y="0.354*ssdD0_width" z="3" unit="mm"/>
	 <rotation name="ssddouble2_rot" z="-45" unit="deg"/>
	 <position name="ssdbkplndouble1_pos" x="0" y="0" z="3+ssdD0_thick" unit="mm"/>
	 <position name="ssddouble_USMylarWindow_pos" x="0" y="0"
			z="-10." unit="mm" />
	 <position name="ssddouble_DSMylarWindow_pos" x="0" y="0"
		 z="10." unit="mm" />

	 <!-- ABOVE IS FOR SSD -->
EOF
	}

	if($RPC_switch){
		print DEF <<EOF;

	 <!-- BELOW IS FOR RPC -->

	 <quantity name="RPC_thick" value="54" unit="mm" />
	 <quantity name="RPC_width" value="1066" unit="mm" />
	 <quantity name="RPC_height" value="252" unit="mm" />
	 <position name="RPC_pos" x="0" y="0" z="730" unit="mm" />

	 <quantity name="RPC_Al_thick" value="1" unit="mm" />
	 <quantity name="RPC_comb_thick" value="17" unit="mm" />

	 <quantity name="RPC_PCB_thick" value="1" unit="mm" />
	 <quantity name="RPC_acrylic_thick" value="1" unit="mm" />
	 <quantity name="RPC_gas_width" value="960" unit="mm" />
	 <quantity name="RPC_gas_height" value="250" unit="mm" />
	 <quantity name="RPC_gas_thick" value="6" unit="mm" />

EOF
		for($i = 0; $i < $n_cover; ++$i){
			print DEF <<EOF;
	 <position name="RPC_Al@{[ $i ]}_pos" z="RPC_thick*($i-($n_cover-1)*0.5)" unit="mm"/>
	 <position name="RPC_comb@{[ $i ]}_pos" z="(RPC_PCB_thick*($n_gas+1)+RPC_gas_thick*$n_gas+RPC_acrylic_thick*$n_gas*$n_cover+RPC_comb_thick)*($i-($n_cover-1)*0.5)" unit="mm"/>
EOF
		}
		for($i = 0; $i < $n_gas+1; ++$i){
			print DEF <<EOF;
	 <position name="RPC_PCB@{[ $i ]}_pos" z="(RPC_PCB_thick+RPC_gas_thick+RPC_acrylic_thick*2)*($i-$n_gas*0.5)" unit="mm"/>
EOF
		}
		for($i = 0; $i < $n_gas; ++$i){
			print DEF <<EOF;
	 <position name="RPC_gas@{[ $i ]}_pos" z="(RPC_PCB_thick+RPC_gas_thick+RPC_acrylic_thick*2)*($i-($n_gas-1)*0.5)" unit="mm"/>
EOF
			for($j = 0; $j < $n_cover; ++$j){
				print DEF <<EOF;
	 <position name="RPC_acrylic@{[ $i ]}@{[ $j ]}_pos" z="(RPC_PCB_thick+RPC_gas_thick+RPC_acrylic_thick*2)*($i-($n_gas-1)*0.5)+(RPC_acrylic_thick+RPC_gas_thick)*($j-($n_cover-1)*0.5)" unit="mm"/>
EOF
			}
		}

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
tation_type <quantity name="LG_width1" value="135" unit="mm" />
	 <constant name="LG_angle_v" value="3.7022129"/>
	 <quantity name="LG_angle" value="LG_angle_v" unit="deg" />

	 <quantity name="LG_protrusion_thick" value="40" unit="mm" />
	 <quantity name="LG_PMTr" value="38" unit="mm" />
	 <quantity name="LG_PMTl" value="120" unit="mm" />

	 <position name="LG_para_pos1" x="(LG_width1-LG_width0)*0.5" y="0" z="0" unit="mm"/>
	 <position name="LG_para_pos" x="(LG_width1-LG_width0)*0.5" y="0" z="-0.5*(LG_protrusion_thick+LG_PMTl)" unit="mm"/>

	 <position name="LG_glass_pos" x="0" y="0" z="-0.5*(LG_protrusion_thick+LG_PMTl)" unit="mm"/>
	 <position name="LG_protrusion_pos" x="0" y="0" z="0.5*(LG_length-LG_PMTl)" unit="mm"/>
	 <position name="LG_PMT_pos" x="0" y="0" z="0.5*(1*LG_protrusion_thick+LG_length)" unit="mm"/>


EOF
		for($i = 0; $i < $n_LG; ++$i){
			print DEF <<EOF;
	 <position name="LG_block@{[ $i ]}0_pos" x="-LG_width0" y="LG_height*($i-1)" z="0." unit="mm"/>
	 <position name="LG_block@{[ $i ]}1_pos" x="0" y="LG_height*($i-1)" z="0." unit="mm"/>
	 <position name="LG_block@{[ $i ]}2_pos" x="LG_width0+(LG_width1-LG_width0)" y="LG_height*($i-1)" z="0." unit="mm"/>
	 <rotation name="LG_block@{[ $i ]}0_rot" x="0" y="0" z="180" unit="deg"/>
	 <rotation name="LG_block@{[ $i ]}1_rot" x="0" y="0" z="0" unit="deg"/>
	 <rotation name="LG_block@{[ $i ]}2_rot" x="0" y="-LG_angle" z="0" unit="deg"/>
EOF
		}

		print DEF <<EOF;
	 <quantity name="calor_length" value="520" unit="mm" />
	 <quantity name="calor_height" value="380" unit="mm" />
	 <quantity name="calor_width" value="450" unit="mm" />

	 <quantity name="calor_shift" value="800" unit="mm" />
	 <position name="calor_pos" x="0" y="0" z="calor_shift+calor_length*0.5" unit="mm"/>

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

	 <box name="magnet_box" x="magnetSideYOffset+magnetSideUSHeight"
		 y="magnetSideYOffset+magnetSideUSHeight" z="magnetSideZLength" />

	 <arb8 name="magnetSide_arb8"
	  v1x="-0.5*magnetSideTopWidth" v1y="magnetSideUSHeight+magnetSideYOffset"
	  v2x="0.5*magnetSideTopWidth" v2y="magnetSideUSHeight+magnetSideYOffset"
	  v3x="0.5*magnetSideUSBottomWidth" v3y="magnetSideYOffset"
	  v4x="-0.5*magnetSideUSBottomWidth" v4y="magnetSideYOffset"
	  v5x="-0.5*magnetSideDSBottomWidth" v5y="magnetSideUSHeight-magnetSideDSHeight+magnetSideYOffset"   
	  v6x="-0.5*magnetSideTopWidth" v6y="magnetSideUSHeight+magnetSideYOffset"
	  v7x="0.5*magnetSideTopWidth" v7y="magnetSideUSHeight+magnetSideYOffset"
	  v8x="0.5*magnetSideDSBottomWidth" v8y="magnetSideUSHeight-magnetSideDSHeight+magnetSideYOffset"
	  dz="magnetSideZLength" unit="mm"/>

	 <!-- ABOVE IS FOR MAGNET -->

EOF
	}

	if($SSD_switch){
		print SOL <<EOF;

	 <!-- BELOW IS FOR SSD -->

EOF
		for($i = 0; $i < $nstation_type; ++$i){
			print SOL <<EOF;
	  <box name="ssdStation@{[ $station_type[$i] ]}_box" x="ssdStation@{[ $station_type[$i] ]}Width" y="ssdStation@{[ $station_type[$i] ]}Height" z="ssdStation@{[ $station_type[$i] ]}Length" />
	  <box name="ssd@{[ $station_type[$i] ]}_MylarWindow_box" x="ssdStation@{[ $station_type[$i] ]}Width*0.8" y="ssdStation@{[ $station_type[$i] ]}Width*0.8" z="Mylar_Window_thick" />
	  <box name="ssd@{[ $station_type[$i] ]}_box" x="ssdD0_width" y="ssdD0_height" z="ssdD0_thick" />
	  <box name="ssd@{[ $station_type[$i] ]}_bkpln_box" x="@{[ $bkpln_size[$i] ]}*ssdD0_width" y="@{[ $bkpln_size[$i] ]}*ssdD0_width" z="carbon_fiber_thick" />

EOF
		}
		print SOL <<EOF;
	 <!-- ABOVE IS FOR SSD -->

EOF
	}
	if($RPC_switch){
		print SOL <<EOF;

	 <!-- BELOW IS FOR RPC -->

	 <box name="RPC_box" x="RPC_width" y="RPC_height" z="RPC_thick"/>

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

	 <para name="LG_para1" x="LG_width0" y="LG_height" z="LG_length" theta="LG_angle_v*DEG2RAD" unit="rad"/>
	 <box name="LG_box1" x="LG_width0" y="LG_height" z="LG_length"/>
	 <box name="LG_box2" x="LG_width0" y="LG_height" z="LG_length+LG_protrusion_thick+LG_PMTl"/>
	 <union name="LG_union">
		<first ref="LG_box1"/>  <second ref="LG_para1"/>
		<positionref ref="LG_para_pos1" />
	 </union>	 
	 <union name="LG_block_union">
		<first ref="LG_box2"/>  <second ref="LG_para1"/>
		<positionref ref="LG_para_pos" />
	 </union>	 

	 <tube name="LG_protrusion_tube" rmax="LG_PMTr" z="LG_protrusion_thick" deltaphi="360" unit="deg"/>
	 <tube name="LG_PMT_tube" rmax="LG_PMTr" z="LG_PMTl" deltaphi="360" unit="deg"/>

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
	 <materialref ref="Graphite"/>
	 <solidref ref="target_box"/>
  </volume>

  <!-- ABOVE IS FOR TARGET -->

EOF
	}

	if($magnet_switch){
		print MOD <<EOF;

  <!-- BELOW IS FOR MAGNET -->

  <volume name="magnetSide_vol">
	 <materialref ref="NeodymiumAlloy"/>
	 <solidref ref="magnetSide_arb8"/>
  </volume>

  <!-- ABOVE IS FOR MAGNET -->

EOF
	}

	if($SSD_switch){
		print MOD <<EOF;

  <!-- BELOW IS FOR SSD -->

EOF
		for($i = 0; $i < $nstation_type; ++$i){
			print MOD <<EOF;
		 <volume name="ssd@{[ $station_type[$i] ]}_vol">
			<materialref ref="SiliconWafer"/>
			<solidref ref="ssd@{[ $station_type[$i] ]}_box"/>
		 </volume>

		 <volume name="ssd@{[ $station_type[$i] ]}_bkpln_vol">
			<materialref ref="CarbonFiber"/>
			<solidref ref="ssd@{[ $station_type[$i] ]}_bkpln_box"/>
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
	 <solidref ref="LG_union"/>
  </volume>
  <volume name="LG_protrusion_vol">
	 <materialref ref="LeadGlass"/>
	 <solidref ref="LG_protrusion_tube"/>
  </volume>
  <volume name="LG_PMT_vol">
	 <materialref ref="LeadGlass"/>
	 <solidref ref="LG_PMT_tube"/>
  </volume>

  <volume name="LG_block_vol">
	 <materialref ref="Air"/>
	 <solidref ref="LG_block_union"/>
	 <physvol name="LG_glass_phys">
		<volumeref ref="LG_glass_vol"/>
		<positionref ref="LG_glass_pos"/>
	 </physvol>
	 <physvol name="LG_protrusion_phys">
		<volumeref ref="LG_protrusion_vol"/>
		<positionref ref="LG_protrusion_pos"/>
	 </physvol>
	 <physvol name="LG_PMT_phys">
		<volumeref ref="LG_PMT_vol"/>
		<positionref ref="LG_PMT_pos"/>
	 </physvol>
  </volume>

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

		for($i = 0; $i < $n_magseg; ++$i){
			print DET <<EOF;
	 <physvol name="magnetSide@{[ $i ]}_phys">
		<volumeref ref="magnetSide_vol"/>
		<positionref ref="magnetSide_pos"/>
		<rotationref ref="RotateZMagSeg@{[ $i ]}"/>
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
		for($i = 0; $i < $nstation_type; ++$i){
			print DET <<EOF;
	  <volume name="ssdStation@{[ $station_type[$i] ]}_vol">
		 <materialref ref="Air"/>
		 <solidref ref="ssdStation@{[ $station_type[$i] ]}_box"/>
		 <physvol name="ssd@{[ $station_type[$i] ]}_USMylarWindow_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}_MylarWindow_vol"/>
			<positionref ref="ssd@{[ $station_type[$i] ]}_USMylarWindow_pos"/>
		 </physvol>
		 <physvol name="ssd@{[ $station_type[$i] ]}_DSMylarWindow_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}_MylarWindow_vol"/>
			<positionref ref="ssd@{[ $station_type[$i] ]}_DSMylarWindow_pos"/>
		 </physvol>

EOF
			for($j = 0; $j < $SSD_lay[$i]; ++$j){
				for($k = 0; $k < $SSD_par[$i]; ++$k){

					print DET <<EOF;
		 <physvol name="ssdsensor@{[ $station_type[$i] ]}@{[ $j ]}@{[ $k ]}_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}_vol"/>
			<positionref ref="ssd@{[ $station_type[$i] ]}@{[ $j ]}@{[ $k ]}_pos"/>
			<rotationref ref="ssd@{[ $station_type[$i] ]}@{[ $j ]}_rot"/>
		 </physvol>
EOF
				}
			}
			for($j = 0; $j < $SSD_bkpln[$i]; ++$j){
				print DET <<EOF;
		 <physvol name="ssdbkpln@{[ $station_type[$i] ]}@{[ $j ]}_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}_bkpln_vol"/>
			<positionref ref="ssdbkpln@{[ $station_type[$i] ]}@{[ $j ]}_pos"/>
		 </physvol>
EOF

			}
			print DET <<EOF;
	  </volume>

EOF
		}
		print DET <<EOF;

  <!-- ABOVE IS FOR SSD -->

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
	 <physvol name="RPC_comb@{[ $i ]}_phys">
		<volumeref ref="RPC_comb_vol"/>
		<positionref ref="RPC_comb@{[ $i ]}_pos"/>
	 </physvol>
EOF
		}
		for($i = 0; $i < $n_gas+1; ++$i){
			print DET <<EOF;
	 <physvol name="RPC_PCB@{[ $i ]}_phys">
		<volumeref ref="RPC_PCB_vol"/>
		<positionref ref="RPC_PCB@{[ $i ]}_pos"/>
	 </physvol>
EOF
		}
		for($i = 0; $i < $n_gas; ++$i){
			print DET <<EOF;
	 <physvol name="RPC_gas@{[ $i ]}_phys">
		<volumeref ref="RPC_gas_vol"/>
		<positionref ref="RPC_gas@{[ $i ]}_pos"/>
	 </physvol>
EOF
			for($j = 0; $j < $n_cover; ++$j){
				print DET <<EOF;
	 <physvol name="RPC_acrylic@{[ $i ]}@{[ $j ]}_phys">
		<volumeref ref="RPC_acrylic_vol"/>
		<positionref ref="RPC_acrylic@{[ $i ]}@{[ $j ]}_pos"/>
	 </physvol>
EOF
			}
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
		<volumeref ref="LG_block_vol"/>
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
			 <volumeref ref="ssdStation@{[ $station_type[$i] ]}_vol"/>
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

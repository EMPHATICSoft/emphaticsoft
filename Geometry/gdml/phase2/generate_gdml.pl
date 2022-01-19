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

use Math::Trig;
use Getopt::Long;
use Math::BigFloat;
use File::Basename;
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

# constants for magnet
$n_magseg = 16; 

# constants for SSD
$nstation_type = 4; # types of station
@station_type = ("a", "b", "c", "d"); # xy, uxy, ...
@bkpln_size = (1.0, 1.3, 1.3, 1.3); # bkpln size scale to ssd
@SSD_lay = (2, 3, 3, 3); # num. of SSD in a station
@SSD_bkpln= (1, 2, 2, 2); # num. of bkpln in a station
@SSD_mod = ("D0", "D0", "D0", "CMS"); # SSD type in a station
@SSD_station = (2, 3, 2, 1); # num. of stations

# constants for ARICH
$n_layer = 2;
$n_aerogel = 3;
$m_aerogel = 3;
@aerogel_mod = ("1035", "1045");
$n_chan = 4;
$n_SiPM = 18;

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
	$DEF = "phase2_Def" . $suffix . ".gdml";
	push (@gdmlFiles, $DEF);
	$DEF = ">" . $DEF;
	open(DEF) or die("Could not open file $DEF for writing");

	print DEF <<EOF;
<?xml version='1.0'?>
	 <gdml>

  <define>

	 <quantity name="PI" value="3.1415927" />
	 <quantity name="DEG2RAD" value="0.017453293" />

	 <quantity name="world_size" value="3000." unit="mm"/>
	 <position name="center" x="0" y="0" z="0"/>

	 <!-- BELOW IS FOR TARGET -->

	 <quantity name="target_thick" value="20.0" unit="mm"/>
	 <quantity name="target_width" value="100.0" unit="mm"/>
	 <quantity name="target_height" value="50.0" unit="mm"/>

	 <position name="target_pos" x="0" y="0" z="0" unit="mm"/>

	 <!-- ABOVE IS FOR TARGET -->

	 <!-- BELOW IS FOR MAGNET -->

	 <quantity name="magnetSideYOffset" value="29" unit="mm" />
	 <quantity name="magnetSideUSBottomWidth" value="11.5" unit="mm"/>
	 <quantity name="magnetSideDSBottomWidth" value="33.3" unit="mm"/>
	 <quantity name="magnetSideTopWidth" value="71.6" unit="mm"/>
	 <quantity name="magnetSideZLength" value="150" unit="mm"/>
	 <quantity name="magnetSideUSHeight" value="151" unit="mm"/>
	 <quantity name="magnetSideDSHeight" value="96.2" unit="mm"/>

	 <position name="magnetSide_pos" x="0" y="0" z="0" unit="mm"/>
	 <position name="magnet_pos" x="0" y="0" z="160" unit="mm"/>

EOF
	for($i = 0; $i < $n_magseg; ++$i){
		print DEF <<EOF;
	 <rotation name="RotateZMagSeg@{[ $i ]}" z="(-157.5+22.5*$i)*DEG2RAD" aunit="rad"/>
EOF
	}
	print DEF <<EOF;

	 <!-- ABOVE IS FOR MAGNET -->

	 <!-- BELOW IS FOR SSD -->

	 <quantity name="ssdD0_thick" value=".300" unit="mm"/>
	 <quantity name="ssdD0_width" value="98.33" unit="mm"/>
	 <quantity name="ssdD0_height" value="38.34" unit="mm"/>

	 <quantity name="ssdCMS_thick" value=".500" unit="mm"/>
	 <quantity name="ssdCMS_width" value="100." unit="mm"/>
	 <quantity name="ssdCMS_height" value="100" unit="mm"/>

	 <quantity name="carbon_fiber_thick" value="0.300"
		 unit="mm" />
	 <quantity name="Mylar_Window_thick" value="0.500"
		 unit="mm" />

	 <quantity name="ssdStationaLength" value="50" unit="mm" />
	 <quantity name="ssdStationaWidth" value="150" unit="mm" />
	 <quantity name="ssdStationaHeight" value="150" unit="mm" />
	 <position name="ssdStation0_pos" x="0" y="0" z="-100" unit="mm" />
	 <position name="ssdStation1_pos" x="0" y="0" z="-20" unit="mm" />
	 <position name="ssda0_pos" x="0" y="0" z="0" unit="mm"/>
	 <rotation name="ssda0_rot" z="0*DEG2RAD" aunit="rad"/>
	 <position name="ssdbkplna0_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
	 <position name="ssda1_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssda1_rot" z="90*DEG2RAD" aunit="rad"/>
	 <position name="ssda_USMylarWindow_pos" x="0" y="0"
			z="-10." unit="mm" />
	 <position name="ssda_DSMylarWindow_pos" x="0" y="0"
			z="10." unit="mm" />

	 <quantity name="ssdStationbLength" value="50" unit="mm" />
	 <quantity name="ssdStationbWidth" value="200" unit="mm" />
	 <quantity name="ssdStationbHeight" value="200" unit="mm" />
	 <position name="ssdStation2_pos" x="0" y="0" z="30" unit="mm" />
	 <position name="ssdStation3_pos" x="0" y="0" z="80" unit="mm" />
	 <position name="ssdStation4_pos" x="0" y="0" z="130" unit="mm" />
	 <position name="ssdb0_pos" x="0" y="0" z="0" unit="mm"/>
	 <rotation name="ssdb0_rot" z="60*DEG2RAD" aunit="rad"/>
	 <position name="ssdbkplnb0_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
	 <position name="ssdb1_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssdb1_rot" z="-60*DEG2RAD" aunit="rad"/>
	 <position name="ssdb2_pos" x="0" y="0" z="3" unit="mm"/>
	 <rotation name="ssdb2_rot" z="0*DEG2RAD" aunit="rad"/>
	 <position name="ssdbkplnb1_pos" x="0" y="0" z="3+ssdD0_thick" unit="mm"/>
	 <position name="ssdb_USMylarWindow_pos" x="0" y="0"
			z="-10." unit="mm" />
	 <position name="ssdb_DSMylarWindow_pos" x="0" y="0"
		 z="10." unit="mm" />

	 <quantity name="ssdStationcLength" value="50" unit="mm" />
	 <quantity name="ssdStationcWidth" value="500" unit="mm" />
	 <quantity name="ssdStationcHeight" value="500" unit="mm" />
	 <position name="ssdStation5_pos" x="0" y="0" z="330" unit="mm" />
	 <position name="ssdStation6_pos" x="0" y="0" z="380" unit="mm" />
	 <position name="ssdc0_pos" x="0" y="0" z="0" unit="mm"/>
	 <rotation name="ssdc0_rot" z="60*DEG2RAD" aunit="rad"/>
	 <position name="ssdbkplnc0_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
	 <position name="ssdc1_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssdc1_rot" z="-60*DEG2RAD" aunit="rad"/>
	 <position name="ssdc2_pos" x="0" y="0" z="3" unit="mm"/>
	 <rotation name="ssdc2_rot" z="0*DEG2RAD" aunit="rad"/>
	 <position name="ssdbkplnc1_pos" x="0" y="0" z="3+ssdD0_thick" unit="mm"/>
	 <position name="ssdc_USMylarWindow_pos" x="0" y="0"
			z="-10." unit="mm" />
	 <position name="ssdc_DSMylarWindow_pos" x="0" y="0"
		 z="10." unit="mm" />

	 <quantity name="ssdStationdLength" value="50" unit="mm" />
	 <quantity name="ssdStationdWidth" value="500" unit="mm" />
	 <quantity name="ssdStationdHeight" value="500" unit="mm" />
	 <position name="ssdStation7_pos" x="0" y="0" z="430" unit="mm" />
	 <position name="ssdd0_pos" x="0" y="0" z="0" unit="mm"/>
	 <rotation name="ssdd0_rot" z="60*DEG2RAD" aunit="rad"/>
	 <position name="ssdbkplnd0_pos" x="0" y="0" z="ssdCMS_thick" unit="mm"/>
	 <position name="ssdd1_pos" x="0" y="0" z="ssdCMS_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssdd1_rot" z="-60*DEG2RAD" aunit="rad"/>
	 <position name="ssdd2_pos" x="0" y="0" z="3" unit="mm"/>
	 <rotation name="ssdd2_rot" z="0*DEG2RAD" aunit="rad"/>
	 <position name="ssdbkplnd1_pos" x="0" y="0" z="3+ssdCMS_thick" unit="mm"/>
	 <position name="ssdd_USMylarWindow_pos" x="0" y="0"
			z="-10." unit="mm" />
	 <position name="ssdd_DSMylarWindow_pos" x="0" y="0"
		 z="10." unit="mm" />

	 <!-- ABOVE IS FOR SSD -->

	 <!-- BELOW IS FOR ARICH -->

	 <quantity name="arich_shift" value="460" unit="mm" />

	 <quantity name="aerogel_size" value="100.0" unit="mm"/>
	 <quantity name="aerogel_thick" value="20" unit="mm" />

	 <quantity name="flight_length" value="210.0" unit="mm"/>

	 <quantity name="channel_size" value="6.0" unit="mm"/>
	 <quantity name="channel_gap" value="0.2" unit="mm"/>
	 <quantity name="SiPM_size" value="25.0" unit="mm"/>
	 <quantity name="SiPM_gap" value="0.5" unit="mm"/>
	 <quantity name="SiPM_thick" value="0.15" unit="mm"/>
	 <quantity name="panel_thick" value="1.35" unit="mm"/>

	 <quantity name="box_size" value="(SiPM_size+SiPM_gap)*$n_SiPM+SiPM_gap" unit="mm"/>
	 <quantity name="box_length" value="flight_length+aerogel_thick*$n_layer+SiPM_thick+panel_thick" unit="mm"/>

	 <position name="arich_pos" z="box_length*0.5+arich_shift" unit="mm"/>

EOF

	for($i = 0; $i < $n_layer; ++$i){
		for($j = 0; $j < $n_aerogel; ++$j){
			for($k = 0; $k < $m_aerogel; ++$k){
				print DEF <<EOF;
	 <position name="aerogel@{[ $i ]}_@{[ $j ]}@{[ $k ]}_pos" x="-102.0+101*$j" y="-102.0+101*$k" z="aerogel_thick*0.5+aerogel_thick*$i-box_length*0.5" unit="mm"/>
EOF
			}
		}
	}

	for($i = 0; $i < $n_SiPM; ++$i){
		for($j = 0; $j < $n_SiPM; ++$j){
			for($ci = 0; $ci < $n_chan; ++$ci){
				$fi = sprintf("%02d", $i);
				for($cj = 0; $cj < $n_chan; ++$cj){
					$fj = sprintf("%02d", $j);
					print DEF <<EOF;
	 <position name="channel@{[ $fi ]}@{[ $fj ]}_@{[ $ci ]}@{[ $cj ]}_pos" x="(channel_size+channel_gap)*($ci-($n_chan-1)/2.)+(SiPM_size+SiPM_gap)*($i-($n_SiPM-1)/2.)" y="(channel_size+channel_gap)*($cj-($n_chan-1)/2.)+(SiPM_size+SiPM_gap)*($j-($n_SiPM-1)/2.)" z="-SiPM_thick*0.5-panel_thick+box_length*0.5" unit="mm"/>
EOF
				}
			}
		}
	}

	print DEF <<EOF;
	 <!-- ABOVE IS FOR ARICH -->

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
	$MAT = "phase2_Materials" . $suffix . ".gdml";
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
	$SOL = "phase2_Solids" . $suffix . ".gdml";
	push (@gdmlFiles, $SOL);
	$SOL = ">" . $SOL;
	open(SOL) or die("Could not open file $SOL for writing");


	print SOL <<EOF;

	<solids>

	 <box name="world_box" x="world_size" y="world_size" z="world_size" />

	 <!-- BELOW IS FOR TARGET -->

	 <box name="target_box" x="target_width" y="target_height" z="target_thick" />

	 <!-- ABOVE IS FOR TARGET -->

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

	 <!-- BELOW IS FOR SSD -->

EOF
	for($i = 0; $i < $nstation_type; ++$i){
		print SOL <<EOF;
	  <box name="ssdStation@{[ $station_type[$i] ]}_box" x="ssdStation@{[ $station_type[$i] ]}Width" y="ssdStation@{[ $station_type[$i] ]}Height" z="ssdStation@{[ $station_type[$i] ]}Length" />
	  <box name="ssd@{[ $station_type[$i] ]}_MylarWindow_box" x="ssdStation@{[ $station_type[$i] ]}Width*0.8" y="ssdStation@{[ $station_type[$i] ]}Width*0.8" z="Mylar_Window_thick" />
	  <box name="ssd@{[ $station_type[$i] ]}_box" x="ssd@{[ $SSD_mod[$i] ]}_width" y="ssd@{[ $SSD_mod[$i] ]}_height" z="ssd@{[ $SSD_mod[$i] ]}_thick" />
	  <box name="ssd@{[ $station_type[$i] ]}_bkpln_box" x="@{[ $bkpln_size[$i] ]}*ssd@{[ $SSD_mod[$i] ]}_width" y="@{[ $bkpln_size[$i] ]}*ssd@{[ $SSD_mod[$i] ]}_width" z="carbon_fiber_thick" />

EOF
	}
	print SOL <<EOF;
	 <!-- ABOVE IS FOR SSD -->

	 <!-- BELOW IS FOR ARICH -->
		<box name="aerogel_box" x="aerogel_size" y="aerogel_size" z="aerogel_thick"/>
		<box name="arich_box" x="box_size" y="box_size" z="box_length"/>
		<box name="channel_box" x="channel_size" y="channel_size" z="SiPM_thick"/>
	 <!-- ABOVE IS FOR ARICH -->

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
	$MOD = "phase2_Modules" . $suffix . ".gdml";
	push (@gdmlFiles, $MOD);
	$MOD = ">" . $MOD;
	open(MOD) or die("Could not open file $MOD for writing");


	print MOD <<EOF;
  <structure>    

  <!-- BELOW IS FOR TARGET -->

  <volume name="target_vol">
	 <materialref ref="Graphite"/>
	 <solidref ref="target_box"/>
  </volume>

  <!-- ABOVE IS FOR TARGET -->

  <!-- BELOW IS FOR MAGNET -->

  <volume name="magnetSide_vol">
	 <materialref ref="NeodymiumAlloy"/>
	 <solidref ref="magnetSide_arb8"/>
  </volume>

  <!-- ABOVE IS FOR MAGNET -->

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

  <!-- BELOW IS FOR ARICH -->
EOF

	for($i = 0; $i < $n_layer; ++$i){
		print MOD <<EOF;
		<volume name="aerogel_@{[ $aerogel_mod[$i] ]}_vol">
			<materialref ref="AERO_@{[ $aerogel_mod[$i] ]}"/>
			<solidref ref="aerogel_box"/>
		</volume>
EOF
	}
	print MOD <<EOF;

		<volume name="channel_vol">
			<materialref ref="SiliconWafer"/>
			<solidref ref="channel_box"/>
		</volume>

  <!-- ABOVE IS FOR ARICH -->

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
	$DET = "phase2_DetEnclosure" . $suffix . ".gdml";
	push (@gdmlFiles, $DET);
	$DET = ">" . $DET;
	open(DET) or die("Could not open file $DET for writing");

	print DET <<EOF;
  <structure>

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
			print DET <<EOF;
		 <physvol name="ssd@{[ $station_type[$i] ]}@{[ $j ]}_phys">
			<volumeref ref="ssd@{[ $station_type[$i] ]}_vol"/>
			<positionref ref="ssd@{[ $station_type[$i] ]}@{[ $j ]}_pos"/>
			<rotationref ref="ssd@{[ $station_type[$i] ]}@{[ $j ]}_rot"/>
		 </physvol>
EOF
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

  <!-- BELOW IS FOR ARICH -->

  <volume name="arich_vol">
	 <materialref ref="Air"/>
	 <solidref ref="arich_box"/>
EOF

	for($i = 0; $i < $n_layer; ++$i){
		for($j = 0; $j < $n_aerogel; ++$j){
			for($k = 0; $k < $m_aerogel; ++$k){
				print DET <<EOF;
		 <physvol name="aerogel@{[ $i ]}_@{[ $j ]}@{[ $k ]}_phys">
			<volumeref ref="aerogel_@{[ $aerogel_mod[$i] ]}_vol"/>
			<positionref ref="aerogel@{[ $i ]}_@{[ $j ]}@{[ $k ]}_pos"/>
		 </physvol>
EOF
			}
		}
	}

	for($i = 0; $i < $n_SiPM; ++$i){
		for($j = 0; $j < $n_SiPM; ++$j){
			for($ci = 0; $ci < $n_chan; ++$ci){
				$fi = sprintf("%02d", $i);
				for($cj = 0; $cj < $n_chan; ++$cj){
					$fj = sprintf("%02d", $j);
					print DET <<EOF;
		 <physvol name="channel@{[ $fi ]}@{[ $fj ]}_@{[ $ci ]}@{[ $cj ]}_phys">
			<volumeref ref="channel_vol"/>
			<positionref ref="channel@{[ $fi ]}@{[ $fj ]}_@{[ $ci ]}@{[ $cj ]}_pos"/>
		 </physvol>
EOF
				}
			}
		}
	}

	print DET <<EOF;
  </volume>
  <!-- ABOVE IS FOR ARICH -->

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
	$WORLD = "phase2_World" . $suffix . ".gdml";
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

  <!-- BELOW IS FOR TARGET -->

  <physvol name="target_phys">
	 <volumeref ref="target_vol"/>
	 <positionref ref="target_pos"/>
  </physvol>

  <!-- ABOVE IS FOR TARGET -->

  <!-- BELOW IS FOR MAGNET -->

  <physvol name="magnet_phys">
	 <volumeref ref="magnet_vol"/>
	 <positionref ref="magnet_pos"/>
  </physvol>

  <!-- ABOVE IS FOR MAGNET -->

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

  <!-- BELOW IS FOR ARICH -->

  <physvol name="arich_phys">
	 <volumeref ref="arich_vol"/>
	 <positionref ref="arich_pos"/>
  </physvol>

  <!-- ABOVE IS FOR ARICH -->

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

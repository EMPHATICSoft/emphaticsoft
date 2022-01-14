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

# constants for T0
$n_acrylic = 10;

# constants for SSD
$nstation_type = 3; # types of station
$bkpln_size = (1.0, 1.3, 1.3); # bkpln size scale to ssd
@SSD_lay = (2, 3, 3); # num. of SSD in a station
@SSD_bkpln= (1, 2, 2); # num. of bkpln in a station
@SSD_mod = ("D0", "D0", "D0twin"); # SSD type in a station
@SSD_station = (2, 2, 2); # num. of stations

# constants for LG
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

    <quantity name="PI" value="3.1415927" />
    <quantity name="DEG2RAD" value="0.017453293" />

    <quantity name="world_size" value="3000." unit="mm"/>
	 <position name="center" x="0" y="0" z="0"/>
	 
	 <!-- BELOW IS FOR T0 -->
	 
	 <quantity name="T0_length" value="280.0" unit="mm"/>
	 <quantity name="T0_width" value="210.0" unit="mm"/>
	 <quantity name="T0_height" value="300.0" unit="mm"/>
	 <position name="T0_pos" z="-300" unit="mm"/>
	 
	 <quantity name="T0_acrylic_length" value="150.0" unit="mm"/>
	 <quantity name="T0_acrylic_width" value="3.0" unit="mm"/>
	 <quantity name="T0_acrylic_height" value="3.0" unit="mm"/>

EOF

for($i = 0; $i < $n_acrylic; ++$i){
	print DEF <<EOF;
	 <position name="T0_acrylic@{[ $i ]}_pos" x="T0_acrylic_width*($i-($n_acrylic-1)*0.5)" unit="mm"/>
EOF
}

print DEF <<EOF;

	 <rotation name="T0_union1_rot" x="45*DEG2RAD" aunit="rad"/>
	 <rotation name="T0_union2_rot" x="-45*DEG2RAD" aunit="rad"/>
	 
	 <!-- ABOVE IS FOR T0 -->

	 <!-- BELOW IS FOR TARGET -->

	 <quantity name="target_thick" value="20.0" unit="mm"/>
    <quantity name="target_width" value="100.0" unit="mm"/>
    <quantity name="target_height" value="50.0" unit="mm"/>
  
    <position name="target_pos" x="0" y="0" z="0" unit="mm"/>

	 <!-- ABOVE IS FOR TARGET -->

	 <!-- BELOW IS FOR SSD -->

    <quantity name="ssdD0_thick" value=".300" unit="mm"/>
    <quantity name="ssdD0_height" value="98.33" unit="mm"/>
    <quantity name="ssdD0_width" value="38.34" unit="mm"/>
    <quantity name="ssdD0twin_thick" value=".300" unit="mm"/>
    <quantity name="ssdD0twin_height" value="98.33" unit="mm"/>
    <quantity name="ssdD0twin_width" value="76.68" unit="mm"/>

    <quantity name="carbon_fiber_thick" value="0.300"
       unit="mm" />
    <quantity name="Mylar_Window_thick" value="0.500"
       unit="mm" />

    <quantity name="ssdStation0Length" value="50" unit="mm" />
    <quantity name="ssdStation0Width" value="150" unit="mm" />
    <quantity name="ssdStation0Height" value="150" unit="mm" />
    <position name="ssdStation0_pos" x="0" y="0" z="-110" unit="mm" />
	 <position name="ssdStation1_pos" x="0" y="0" z="-60" unit="mm" />
    <position name="ssd00_pos" x="0" y="0" z="0" unit="mm"/>
	 <rotation name="ssd00_rot" z="90*DEG2RAD" aunit="rad"/>
    <position name="ssdbkpln00_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
    <position name="ssd01_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssd01_rot" z="0" aunit="rad"/>
    <position name="ssd0_USMylarWindow_pos" x="0" y="0"
         z="-10." unit="mm" />
    <position name="ssd0_DSMylarWindow_pos" x="0" y="0"
         z="10." unit="mm" />

    <quantity name="ssdStation2Length" value="50" unit="mm" />
    <quantity name="ssdStation2Width" value="200" unit="mm" />
    <quantity name="ssdStation2Height" value="200" unit="mm" />
    <position name="ssdStation2_pos" x="0" y="0" z="60" unit="mm" />
	 <position name="ssdStation3_pos" x="0" y="0" z="110" unit="mm" />
    <position name="ssd20_pos" x="0" y="0" z="0" unit="mm"/>
    <rotation name="ssd20_rot" z="45*DEG2RAD" aunit="rad"/>
    <position name="ssdbkpln20_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
    <position name="ssd21_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
    <rotation name="ssd21_rot" z="90*DEG2RAD" aunit="rad"/>
    <position name="ssd22_pos" x="0" y="0" z="3" unit="mm"/>
    <rotation name="ssd22_rot" z="0" aunit="rad"/>	 
    <position name="ssdbkpln21_pos" x="0" y="0" z="3+ssdD0_thick" unit="mm"/>
    <position name="ssd2_USMylarWindow_pos" x="0" y="0"
         z="-10." unit="mm" />
    <position name="ssd2_DSMylarWindow_pos" x="0" y="0"
       z="10." unit="mm" />
	
	 <quantity name="ssdStation4Length" value="50" unit="mm" />
    <quantity name="ssdStation4Width" value="300" unit="mm" />
    <quantity name="ssdStation4Height" value="300" unit="mm" />
	 <position name="ssdStation4_pos" x="0" y="0" z="430" unit="mm" />
    <position name="ssdStation5_pos" x="0" y="0" z="490" unit="mm" />
	 <position name="ssd40_pos" x="0" y="0" z="0" unit="mm"/>
    <rotation name="ssd40_rot" z="0" aunit="rad"/>
    <position name="ssdbkpln40_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
    <position name="ssd41_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
    <rotation name="ssd41_rot" z="90*DEG2RAD" aunit="rad"/>
    <position name="ssd42_pos" x="0" y="0" z="3" unit="mm"/>
    <rotation name="ssd42_rot" z="-45*DEG2RAD" aunit="rad"/>
    <position name="ssdbkpln41_pos" x="0" y="0" z="3+ssdD0_thick" unit="mm"/>
    <position name="ssd4_USMylarWindow_pos" x="0" y="0"
         z="-10." unit="mm" />
    <position name="ssd4_DSMylarWindow_pos" x="0" y="0"
       z="10." unit="mm" />
 
	 <!-- ABOVE IS FOR SSD -->
	 
	 <!-- BELOW IS FOR LG -->

    <quantity name="LG_length" value="340" unit="mm" />
    <quantity name="LG_height" value="122" unit="mm" />
    <quantity name="LG_width0" value="113" unit="mm" />
    <quantity name="LG_width1" value="135" unit="mm" />
    <quantity name="LG_angle" value="0.064615804" aunit="rad" />

    <quantity name="LG_protrusion_thick" value="20" unit="mm" />
	 <position name="LG_protrusion_shift" x="0" y="0" z="LG_length*0.5" unit="mm"/>

    <quantity name="LG_PMTr" value="38" unit="mm" />
    <quantity name="LG_PMTl" value="120" unit="mm" />
	 <position name="LG_PMT_shift" x="0" y="0" z="LG_length*0.5+LG_protrusion_thick" unit="mm"/>
 
	 <position name="LG_para_pos" x="(LG_width1-LG_width0)*0.5" y="0" z="0" unit="mm"/>
	 <position name="LG_para2_pos" x="0" y="0" z="0.5*(LG_protrusion_thick+LG_PMTl)" unit="mm"/>

	 <position name="LG_glass_pos" x="0" y="0" z="0" unit="mm"/>
	 <position name="LG_protrusion_pos" x="0" y="0" z="0.5*(LG_length+LG_protrusion_thick)" unit="mm"/>
	 <position name="LG_PMT_pos" x="0" y="0" z="0.5*(2*LG_protrusion_thick+LG_length+LG_PMTl)" unit="mm"/>

  
EOF
		for($i = 0; $i < $n_LG; ++$i){
print DEF <<EOF;
	 <position name="LG_block@{[ $i ]}0_pos" x="-LG_width0" y="LG_height*($i-1)" z="-0.5*(LG_protrusion_thick+LG_PMTl)" unit="mm"/>
	 <position name="LG_block@{[ $i ]}1_pos" x="0" y="LG_height*($i-1)" z="-0.5*(LG_protrusion_thick+LG_PMTl)" unit="mm"/>
	 <position name="LG_block@{[ $i ]}2_pos" x="LG_width0+(LG_width1-LG_width0)*0.5" y="LG_height*($i-1)" z="-0.5*(LG_protrusion_thick+LG_PMTl)" unit="mm"/>
	 <rotation name="LG_block@{[ $i ]}0_rot" x="0" y="0" z="PI" aunit="rad"/>
	 <rotation name="LG_block@{[ $i ]}1_rot" x="0" y="0" z="0" />
	 <rotation name="LG_block@{[ $i ]}2_rot" x="0" y="-LG_angle" z="0" aunit="rad"/>
EOF
		}

print DEF <<EOF;
	 <quantity name="calor_length" value="500" unit="mm" />
	 <quantity name="calor_height" value="380" unit="mm" />
	 <quantity name="calor_width" value="450" unit="mm" />

	 <quantity name="calor_shift" value="1000" unit="mm" />
	 <position name="calor_pos" x="0" y="0" z="calor_shift+calor_length*0.5" unit="mm"/>

	 <!-- ABOVE IS FOR LG -->

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

    <!-- BELOW IS FOR T0 -->

	 <box name="T0_box" x="T0_width" y="T0_height" z="T0_length" />
	 <box name="T0_acrylic_box" x="T0_acrylic_width" y="T0_acrylic_height" z="T0_acrylic_length" />
    <union name="T0_acrylic_union">
      <first ref="T0_acrylic_box"/>  <second ref="T0_acrylic_box"/>
      <positionref ref= "center" />
      <rotationref ref= "T0_union1_rot" />
      <firstpositionref ref= "center"/>
      <firstrotationref ref= "T0_union2_rot"/>
    </union>	 

	 <!-- ABOVE IS FOR T0 -->

	 <!-- BELOW IS FOR TARGET -->

	 <box name="target_box" x="target_width" y="target_height" z="target_thick" />

	 <!-- ABOVE IS FOR TARGET -->
	 
	 <!-- BELOW IS FOR SSD -->

EOF
  $modi = 0;
  for($i = 0; $i < $nstation_type; ++$i){
	  print SOL <<EOF;
	  <box name="ssdStation@{[ $modi ]}_box" x="ssdStation@{[ $modi ]}Width" y="ssdStation@{[ $modi ]}Height" z="ssdStation@{[ $modi ]}Length" />
	  <box name="ssd@{[ $modi ]}_MylarWindow_box" x="ssdStation@{[ $modi ]}Width*0.8" y="ssdStation@{[ $modi ]}Width*0.8" z="Mylar_Window_thick" />
	  <box name="ssd@{[ $modi ]}_box" x="ssd@{[ $SSD_mod[$i] ]}_width" y="ssd@{[ $SSD_mod[$i] ]}_height" z="ssd@{[ $SSD_mod[$i] ]}_thick" />
	  <box name="ssd@{[ $modi ]}_bkpln_box" x="@{[ $bkpln_size ]}*ssd@{[ $SSD_mod[$i] ]}_width" y="@{[ $bkpln_size ]}*ssd@{[ $SSD_mod[$i] ]}_width" z="carbon_fiber_thick" />

EOF
	  $modi+=$SSD_station[$i];
  }
  print SOL <<EOF;
	 <!-- ABOVE IS FOR SSD -->

	 <!-- BELOW IS FOR LG -->

	 <para name="LG_para1" x="LG_width0" y="LG_height" z="LG_length" theta="LG_angle" aunit="rad"/>
	 <box name="LG_box1" x="LG_width0" y="LG_height" z="LG_length"/>
	 <box name="LG_box2" x="LG_width0" y="LG_height" z="LG_length+LG_protrusion_thick+LG_PMTl"/>
    <union name="LG_union">
      <first ref="LG_box1"/>  <second ref="LG_para1"/>
      <positionref ref="LG_para_pos" />
      <firstpositionref ref= "center"/>
    </union>	 
    <union name="LG_block_union">
      <first ref="LG_box2"/>  <second ref="LG_para1"/>
      <positionref ref="LG_para_pos" />
      <firstpositionref ref= "LG_para2_pos"/>
    </union>	 

	 <tube name="LG_protrusion_tube" rmax="LG_PMTr" z="LG_protrusion_thick" deltaphi="2*PI" aunit="rad"/>
	 <tube name="LG_PMT_tube" rmax="LG_PMTr" z="LG_PMTl" deltaphi="2*PI" aunit="rad"/>

	 <box name="calor_box" x="calor_width" y="calor_height" z="calor_length"/>

	 <!-- ABOVE IS FOR LG -->

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

  <!-- BELOW IS FOR T0 -->

  <volume name="T0_acrylic_vol">
    <materialref ref="Acrylic"/>
    <solidref ref="T0_acrylic_union"/>
  </volume>

  <!-- ABOVE IS FOR T0 -->

  <!-- BELOW IS FOR TARGET -->

  <volume name="target_vol">
    <materialref ref="Graphite"/>
    <solidref ref="target_box"/>
  </volume>

  <!-- ABOVE IS FOR TARGET -->

  <!-- BELOW IS FOR SSD -->

EOF
  $modi = 0;
  for($i = 0; $i < $nstation_type; ++$i){
	  print MOD <<EOF;
	    <volume name="ssd@{[ $modi ]}_vol">
         <materialref ref="SiliconWafer"/>
         <solidref ref="ssd@{[ $modi ]}_box"/>
       </volume>
     
       <volume name="ssd@{[ $modi ]}_bkpln_vol">
         <materialref ref="CarbonFiber"/>
         <solidref ref="ssd@{[ $modi ]}_bkpln_box"/>
       </volume>

  	  <volume name="ssd@{[ $modi ]}_MylarWindow_vol">
	    <materialref ref="Mylar"/>
	    <solidref ref="ssd@{[ $modi ]}_MylarWindow_box"/>
	  </volume>

EOF
	  $modi += $SSD_station[$i];
  }
  print MOD <<EOF;
  
  <!-- ABOVE IS FOR SSD -->

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
    </physvol>
EOF
  }
  print DET <<EOF;
  </volume>

  <!-- BELOW IS FOR T0 -->
 
  <!-- BELOW IS FOR SSD -->

EOF
  $modi = 0;
  for($i = 0; $i < $nstation_type; ++$i){
	  print DET <<EOF;
     <volume name="ssdStation@{[ $modi ]}_vol">
       <materialref ref="Air"/>
       <solidref ref="ssdStation@{[ $modi ]}_box"/>
       <physvol name="ssd@{[ $modi ]}_USMylarWindow_phys">
         <volumeref ref="ssd@{[ $modi ]}_MylarWindow_vol"/>
         <positionref ref="ssd@{[ $modi ]}_USMylarWindow_pos"/>
       </physvol>
       <physvol name="ssd@{[ $modi ]}_DSMylarWindow_phys">
         <volumeref ref="ssd@{[ $modi ]}_MylarWindow_vol"/>
         <positionref ref="ssd@{[ $modi ]}_DSMylarWindow_pos"/>
       </physvol>

EOF
		 for($j = 0; $j < $SSD_lay[$i]; ++$j){
	  		print DET <<EOF;
       <physvol name="ssd@{[ $modi ]}@{[ $j ]}_phys">
         <volumeref ref="ssd@{[ $modi ]}_vol"/>
         <positionref ref="ssd@{[ $modi ]}@{[ $j ]}_pos"/>
         <rotationref ref="ssd@{[ $modi ]}@{[ $j ]}_rot"/>
       </physvol>
EOF
		}
		 for($j = 0; $j < $SSD_bkpln[$i]; ++$j){
	  		print DET <<EOF;
       <physvol name="ssdbkpln@{[ $modi ]}@{[ $j ]}_phys">
         <volumeref ref="ssd@{[ $modi ]}_bkpln_vol"/>
         <positionref ref="ssdbkpln@{[ $modi ]}@{[ $j ]}_pos"/>
       </physvol>
EOF

		}
	print DET <<EOF;
     </volume>

EOF
	  $modi += $SSD_station[$i];
  }
  print DET <<EOF;
  
  <!-- ABOVE IS FOR SSD -->

  <!-- BELOW IS FOR LG -->

  <volume name = "calor_vol">
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
 
  <!-- BELOW IS FOR T0 -->

  <physvol name="T0_phys">
    <volumeref ref="T0_vol"/>
    <positionref ref="T0_pos"/>
  </physvol>

  <!-- ABOVE IS FOR T0 -->


  <!-- BELOW IS FOR TARGET -->

  <physvol name="target_phys">
    <volumeref ref="target_vol"/>
    <positionref ref="target_pos"/>
  </physvol>

  <!-- ABOVE IS FOR TARGET -->

 
  <!-- BELOW IS FOR SSD -->

EOF

  $modi = 0;
  $station = 0;
  for($i = 0; $i < $nstation_type; ++$i){
	  for($j = 0; $j < $SSD_station[$i]; ++$j){

		  print WORLD <<EOF;

        <physvol name="ssdStation@{[ $station ]}_phys">
          <volumeref ref="ssdStation@{[ $modi ]}_vol"/>
          <positionref ref="ssdStation@{[ $station ]}_pos"/>
        </physvol>
EOF

	 $station++;
	 }
	 $modi += $SSD_station[$i];
  }  

  print WORLD <<EOF;
  <!-- ABOVE IS FOR SSD -->

  <!-- BELOW IS FOR LG -->

  <physvol name="calor_phys">
    <volumeref ref="calor_vol"/>
    <positionref ref="calor_pos"/>
  </physvol>

  <!-- ABOVE IS FOR LG -->

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

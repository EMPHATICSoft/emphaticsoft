#!/usr/bin/perl

# Linyan 2022.01.12

# Structure of this program is taken straight from generate_gdml.pl that 
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

# constants for SSD
$nstation_type = 4; # types of station
$bkpln_size = (1.0, 1.3, 1.3, 1.3); # bkpln size scale to ssd
@SSD_lay = (2, 3, 3, 3); # num. of SSD in a station
@SSD_bkpln= (1, 2, 2, 2); # num. of bkpln in a station
@SSD_mod = ("D0", "D0", "D0", "CMS"); # SSD type in a station
@SSD_station = (2, 3, 2, 1); # num. of stations

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
    $DEF = "SSD_Def" . $suffix . ".gdml";
    push (@gdmlFiles, $DEF);
    $DEF = ">" . $DEF;
    open(DEF) or die("Could not open file $DEF for writing");

print DEF <<EOF;
<?xml version='1.0'?>
    <gdml>
    
  <define>

    <quantity name="world_size" value="3000." unit="mm"/>

	 <!-- BELOW IS FOR TARGET -->

	 <quantity name="target_thick" value="20.0" unit="mm"/>
    <quantity name="target_width" value="100.0" unit="mm"/>
    <quantity name="target_height" value="50.0" unit="mm"/>
  
    <position name="target_pos" x="0" y="0" z="0" unit="mm"/>

	 <!-- ABOVE IS FOR TARGET -->

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

    <quantity name="ssdStation0Length" value="50" unit="mm" />
    <quantity name="ssdStation0Width" value="150" unit="mm" />
    <quantity name="ssdStation0Height" value="150" unit="mm" />
    <position name="ssdStation0_pos" x="0" y="0" z="-100" unit="mm" />
	 <position name="ssdStation1_pos" x="0" y="0" z="-20" unit="mm" />
    <position name="ssd00_pos" x="0" y="0" z="0" unit="mm"/>
	 <rotation name="ssd00_rot" z="0" unit="deg"/>
    <position name="ssdbkpln00_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
    <position name="ssd01_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
	 <rotation name="ssd01_rot" z="90" unit="deg"/>
    <position name="ssd0_USMylarWindow_pos" x="0" y="0"
         z="-10." unit="mm" />
    <position name="ssd0_DSMylarWindow_pos" x="0" y="0"
         z="10." unit="mm" />

    <quantity name="ssdStation2Length" value="50" unit="mm" />
    <quantity name="ssdStation2Width" value="200" unit="mm" />
    <quantity name="ssdStation2Height" value="200" unit="mm" />
    <position name="ssdStation2_pos" x="0" y="0" z="30" unit="mm" />
	 <position name="ssdStation3_pos" x="0" y="0" z="80" unit="mm" />
    <position name="ssdStation4_pos" x="0" y="0" z="130" unit="mm" />
    <position name="ssd20_pos" x="0" y="0" z="0" unit="mm"/>
    <rotation name="ssd20_rot" z="60" unit="deg"/>
    <position name="ssdbkpln20_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
    <position name="ssd21_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
    <rotation name="ssd21_rot" z="-60" unit="deg"/>
    <position name="ssd22_pos" x="0" y="0" z="3" unit="mm"/>
    <rotation name="ssd22_rot" z="0" unit="deg"/>
    <position name="ssdbkpln21_pos" x="0" y="0" z="3+ssdD0_thick" unit="mm"/>
    <position name="ssd2_USMylarWindow_pos" x="0" y="0"
         z="-10." unit="mm" />
    <position name="ssd2_DSMylarWindow_pos" x="0" y="0"
       z="10." unit="mm" />

    <quantity name="ssdStation5Length" value="50" unit="mm" />
    <quantity name="ssdStation5Width" value="500" unit="mm" />
    <quantity name="ssdStation5Height" value="500" unit="mm" />
    <position name="ssdStation5_pos" x="0" y="0" z="190" unit="mm" />
    <position name="ssdStation6_pos" x="0" y="0" z="290" unit="mm" />
	 <position name="ssd50_pos" x="0" y="0" z="0" unit="mm"/>
    <rotation name="ssd50_rot" z="60" unit="deg"/>
    <position name="ssdbkpln50_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
    <position name="ssd51_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
    <rotation name="ssd51_rot" z="-60" unit="deg"/>
    <position name="ssd52_pos" x="0" y="0" z="3" unit="mm"/>
    <rotation name="ssd52_rot" z="0" unit="deg"/>
    <position name="ssdbkpln51_pos" x="0" y="0" z="3+ssdD0_thick" unit="mm"/>
    <position name="ssd5_USMylarWindow_pos" x="0" y="0"
         z="-10." unit="mm" />
    <position name="ssd5_DSMylarWindow_pos" x="0" y="0"
       z="10." unit="mm" />

    <quantity name="ssdStation7Length" value="50" unit="mm" />
    <quantity name="ssdStation7Width" value="500" unit="mm" />
    <quantity name="ssdStation7Height" value="500" unit="mm" />
    <position name="ssdStation7_pos" x="0" y="0" z="430" unit="mm" />
    <position name="ssd70_pos" x="0" y="0" z="0" unit="mm"/>
    <rotation name="ssd70_rot" z="60" unit="deg"/>
    <position name="ssdbkpln70_pos" x="0" y="0" z="ssdCMS_thick" unit="mm"/>
    <position name="ssd71_pos" x="0" y="0" z="ssdCMS_thick+carbon_fiber_thick" unit="mm"/>
    <rotation name="ssd71_rot" z="-60" unit="deg"/>
    <position name="ssd72_pos" x="0" y="0" z="3" unit="mm"/>
    <rotation name="ssd72_rot" z="0" unit="deg"/>
    <position name="ssdbkpln71_pos" x="0" y="0" z="3+ssdCMS_thick" unit="mm"/>
    <position name="ssd7_USMylarWindow_pos" x="0" y="0"
         z="-10." unit="mm" />
    <position name="ssd7_DSMylarWindow_pos" x="0" y="0"
       z="10." unit="mm" />
	 
	 <!-- ABOVE IS FOR SSD -->

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
    $MAT = "SSD_Materials" . $suffix . ".gdml";
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
    $SOL = "SSD_Solids" . $suffix . ".gdml";
    push (@gdmlFiles, $SOL);
    $SOL = ">" . $SOL;
    open(SOL) or die("Could not open file $SOL for writing");


  print SOL <<EOF;

	<solids>

    <box name="world_box" x="world_size" y="world_size" z="world_size" />

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
	  <box name="ssd@{[ $modi ]}_box" x="ssd@{[ $SSD_mod[$i] ]}_width" y="ssd@{[ $SSD_mod[$i] ]}_height" z="ssdD0_thick" />
	  <box name="ssd@{[ $modi ]}_bkpln_box" x="@{[ $bkpln_size ]}*ssd@{[ $SSD_mod[$i] ]}_width" y="@{[ $bkpln_size ]}*ssd@{[ $SSD_mod[$i] ]}_width" z="carbon_fiber_thick" />

EOF
	  $modi+=$SSD_station[$i];
  }
  print SOL <<EOF;
	 <!-- ABOVE IS FOR SSD -->

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
    $MOD = "SSD_Modules" . $suffix . ".gdml";
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
    $DET = "SSD_DetEnclosure" . $suffix . ".gdml";
    push (@gdmlFiles, $DET);
    $DET = ">" . $DET;
    open(DET) or die("Could not open file $DET for writing");

  print DET <<EOF;
  <structure>
  
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
    $WORLD = "SSD_World" . $suffix . ".gdml";
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

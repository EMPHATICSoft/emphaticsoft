#!/usr/bin/perl

# Much of this program is taken straight from generate_gdml.pl that 
# generates MicroBooNE fragment files (Thank you.)

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

    <quantity name="ssdD0_thick" value=".300" unit="mm"/>
    <quantity name="ssdD0_width" value="98.33" unit="mm"/>
    <quantity name="ssdD0_height" value="38.34" unit="mm"/>

    <quantity name="carbon_fiber_thick" value="0.300"
       unit="mm" />
    <quantity name="Mylar_Window_thick" value="0.500"
       unit="mm" />

    <quantity name="ssdStation0Length" value="50" unit="mm" />
    <quantity name="ssdStation0Width" value="150" unit="mm" />
    <quantity name="ssdStation0Height" value="150" unit="mm" />
    <position name="ssdStation0_pos" x="0" y="0" z="-100" unit="mm" />
  
    <position name="ssd0a_pos" x="0" y="0" z="0" unit="mm"/>
    <position name="ssdbkpln0_pos" x="0" y="0" z="ssdD0_thick" unit="mm"/>
    <position name="ssd0b_pos" x="0" y="0" z="ssdD0_thick+carbon_fiber_thick" unit="mm"/>
    <position name="ssd0_USMylarWindow_pos" x="0" y="0"
         z="-10." unit="mm" />
    <position name="ssd0_DSMylarWindow_pos" x="0" y="0"
         z="10." unit="mm" />

	 <rotation name="RotateZP45" z="45" unit="deg"/>
	 <rotation name="RotateZM45" z="-45" unit="deg"/>
	 <rotation name="RotateZP60" z="60" unit="deg"/>
	 <rotation name="RotateZM60" z="-60" unit="deg"/>
	 <rotation name="RotateZP90" z="90" unit="deg"/>
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
	  <box name="ssdStation0_box" x="ssdStation0Width" y="ssdStation0Height" z="ssdStation0Length" />
	  <box name="ssd0_MylarWindow_box" x="ssdStation0Width*0.8" y="ssdStation0Width*0.8" z="Mylar_Window_thick" />
	  <box name="ssdStation0_box" x="ssdStation0Width" y="ssdStation0Height" z="ssdStation0Length" />
	  <box name="ssd0_MylarWindow_box" x="ssdStation0Width*0.8" y="ssdStation0Width*0.8" z="Mylar_Window_thick" />
	  <box name="ssd_single_box" x="ssdD0_width" y="ssdD0_height" z="ssdD0_thick" />
	  <box name="ssd_single_bkpln_box" x="ssdD0_width" y="ssdD0_width"
       z="carbon_fiber_thick" />
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

  <volume name="ssd_single_vol">
    <materialref ref="SiliconWafer"/>
    <solidref ref="ssd_single_box"/>
  </volume>

  <volume name="ssd_single_bkpln_vol">
    <materialref ref="CarbonFiber"/>
    <solidref ref="ssd_single_bkpln_box"/>
  </volume>

  <volume name="ssd0_MylarWindow_vol">
    <materialref ref="Mylar"/>
    <solidref ref="ssd0_MylarWindow_box"/>
  </volume>

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

  <volume name="ssdStation0_vol">
    <materialref ref="Air"/>
    <solidref ref="ssdStation0_box"/>
    <physvol name="ssd0_USMylarWindow_phys">
      <volumeref ref="ssd0_MylarWindow_vol"/>
      <positionref ref="ssd0_USMylarWindow_pos"/>
    </physvol>
    <physvol name="ssd0a_phys">
      <volumeref ref="ssd_single_vol"/>
      <positionref ref="ssd0a_pos"/>
    </physvol>
    <physvol name="ssdbkpln0_phys">
      <volumeref ref="ssd_single_bkpln_vol"/>
      <positionref ref="ssdbkpln0_pos"/>
    </physvol>
    <physvol name="ssd0b_phys">
      <volumeref ref="ssd_single_vol"/>
      <positionref ref="ssd0b_pos"/>
      <rotationref ref="RotateZP90"/>
    </physvol>
    <physvol name="ssd0_DSMylarWindow_phys">
      <volumeref ref="ssd0_MylarWindow_vol"/>
      <positionref ref="ssd0_DSMylarWindow_pos"/>
    </physvol>
  </volume>

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

    <physvol name="ssdStation0_phys">
      <volumeref ref="ssdStation0_vol"/>
      <positionref ref="ssdStation0_pos"/>
    </physvol>
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

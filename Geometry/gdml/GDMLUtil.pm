package GDMLUtil;
use strict;
use warnings;
use Exporter;

our @ISA= qw( Exporter );

our @EXPORT = qw( dumpfile material $steeldensity );


sub dumpfile
{
  my $name = $_[0];
  open my $fh, '<', $name or die "$name: $!";
  my $file_content = do { local $/; <$fh> };
  return $file_content 
}

sub material
{
  my $name = 'compositions/' . $_[0] . '.gdml';
  dumpfile($name);
}

$GDMLUtil::steeldensity  = 7.85;

1;

package GDMLUtil;
use strict;
use warnings;
use Exporter;

our @ISA= qw( Exporter );

our @EXPORT = qw( dumpfile );


sub dumpfile
{
  my $name = $_[0];
  open my $fh, '<', $name or die "$name: $!";
  my $file_content = do { local $/; <$fh> };
  return $file_content 
}

1;

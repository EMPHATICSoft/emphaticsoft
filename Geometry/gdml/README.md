How to generate gdml files
- Need to install GDMLUtil.pm in perl path (/etc/perl)

Run by
./phase2/make_gdml.sh

You can modify material.gdml and phase\*/generate_gdml.pl.
Do not modify phase1b.gdml. It is a generated file.

A manual can be found at DocDB 1242.
Visualization of SSDs can be found at DocDB 1260

Phase-1
- T0 implemented
- Target implemented
- Magnet implemented (material only, EM field not by gdml)
- SSD implemented (rough position only, precise position in calibration module)
- RPC implemented (material needs more work)
- LG implemented (LG material and optical property needs more work)

Phase-2
- T0 implemented
- Target implemented
- Magnet implemented (material only, EM field not by gdml)
- SSD implemented (rough position only, precise position in calibration module)
- RPC implemented (material needs more work)
- ARICH implemented (aerogel and imaging plane, no mirror)
- LG implemented (LG material and optical property needs more work)

Validated by G4 visualization and ROOT visualization

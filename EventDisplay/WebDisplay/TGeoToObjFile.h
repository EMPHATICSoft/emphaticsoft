//File: TGeoToObjFile.h
//Brief: Converts a TGeoShape from ROOT into a Wavefront file suitable for three.js
//       and probably any other Wavefront loader.  All faces are triangulated which
//       sacrifices rendering efficiency for simplicity and three.js support.  Relies
//       on the Wavefront renderer to calculate normal vectors.  Does not support .mtl
//       files or any texture data.
//Author: Andrew Olivier andrew@djoinc.com
#include <iostream>

#ifndef TGEOTOOBJFILE_H
#define TGEOTOOBJFILE_H

class TGeoShape;

std::ostream& TGeoToObjFile(const TGeoShape& shape, std::ostream& dest, const double unitConversion = 1.);

#endif //TGEOTOOBJFILE_H

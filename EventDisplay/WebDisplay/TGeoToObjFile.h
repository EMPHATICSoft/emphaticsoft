//File: TGeoToObjFile.h
//Brief: Converts a TGeoShape from ROOT into a Wavefront file suitable for three.js
//       and probably any other Wavefront loader.  All faces are triangulated which
//       sacrifices rendering efficiency for simplicity and three.js support.  Relies
//       on the Wavefront renderer to calculate normal vectors.  Does not support .mtl
//       files or any texture data.
//Author: Andrew Olivier andrew@djoinc.com
#include <iostream>
#include <vector>
#include <string>

#ifndef TGEOTOOBJFILE_H
#define TGEOTOOBJFILE_H

class TGeoShape;
class TGeoNode;
class TGeoMatrix;
class TGeoManager;

std::ostream& TGeoToObjFile(const TGeoShape& shape, std::ostream& dest, const double unitConversion = 1.);

std::vector<std::pair<TGeoNode*, TGeoMatrix*>> searchGeometryTree(TGeoManager& man, const std::vector<std::string>& nodeNames);

#endif //TGEOTOOBJFILE_H

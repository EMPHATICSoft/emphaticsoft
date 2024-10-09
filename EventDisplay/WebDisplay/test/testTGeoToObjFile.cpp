#include "TGeoToObjFile.h"

#include "TGeoManager.h"

#include <iostream>

int main(const int argc, const char** argv)
{
  auto geo = TGeoManager::Import(argv[1]);
  if(!geo) return 1;

  auto volOfInterest = geo->GetVolume(argv[2]);
  auto shape = volOfInterest->GetShape();
  
  TGeoToObjFile(*shape, std::cout);

  return 0;
}

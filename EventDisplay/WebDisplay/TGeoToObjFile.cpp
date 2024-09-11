//File: TGeoToObjFile.cpp
//Brief: Converts a TGeoShape from ROOT into a Wavefront file suitable for three.js
//       and probably any other Wavefront loader.  All faces are triangulated which
//       sacrifices rendering efficiency for simplicity and three.js support.  Relies
//       on the Wavefront renderer to calculate normal vectors.  Does not support .mtl
//       files or any texture data.
//
//       It's easy to modify this function to return raw vertices and indices if you're
//       doing rendering in c++.  This means we could replace ROOT's OpenGL2 backend
//       with OpenGL3, Vulkan, or Metal!  Just return triangles and ptsVec instead of
//       dest at the end of the function.  Those indices will be suitable for
//       GL_TRIANGLES rendering mode.  If you comment out the last block, you get
//       indices for GL_TRIANGLE_STRIP which is more efficient.  If you comment out
//       the last block and uncomment the block before that, you get
//       GL_TRIANGLE_STRIP_ADJACENCY which lets you calculate normals and identify edges
//       of your mesh ON THE GPU.  See my old edepViewer project for an example!
//Author: Andrew Olivier andrew@djoinc.com

#include "TGeoToObjFile.h"

#include "TGeoShape.h"
#include "TBuffer3D.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TClass.h"

#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>
#include <iomanip>

#ifndef TGEOTOOBJFILE_CPP
#define TGEOTOOBJFILE_CPP

namespace
{
  struct hash
  {
    size_t operator ()(const std::pair<int, int>& pair) const
    {
      return size_t(pair.first) << 32 | pair.second;
    }
  };

  //Basic vector math helper that mimics the interface of glm
  struct vec3
  {
    vec3(const double myX, const double myY, const double myZ): x(myX), y(myY), z(myZ) {}
    vec3(): x(0), y(0), z(0) {}

    double x, y, z;

    vec3 operator+ (const vec3& other) const
    {
      return vec3{x+other.x, y+other.y, z+other.z};
    }

    vec3 operator- (const vec3& other) const
    {
      return vec3{x-other.x, y-other.y, z-other.z};
    }

    vec3 operator* (const double scale) const
    {
      return vec3{scale*x, scale*y, scale*z};
    }

    vec3 operator/ (const double scale) const
    {
      return vec3{x/scale, y/scale, z/scale};
    }

    vec3& operator += (const vec3& rhs)
    {
      *this = *this + rhs;
      return *this;
    }

    vec3& operator -= (const vec3& rhs)
    {
      *this = *this - rhs;
      return *this;
    }

    vec3& operator *= (const double scale)
    {
      *this = *this * scale;
      return *this;
    }

    vec3& operator /= (const double scale)
    {
      *this = *this / scale;
      return *this;
    }
  };

  double dot(const vec3& lhs, const vec3& rhs) { return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z; }
  double length(const vec3& vec) { return sqrt(dot(vec, vec)); }
  vec3 normalize(const vec3& vec) { return vec/length(vec); }
  vec3 cross(const vec3& lhs, const vec3& rhs) { return {lhs.y*rhs.z - lhs.z*rhs.y, lhs.z*rhs.x - lhs.x*rhs.z, lhs.x*rhs.y - lhs.y*rhs.x}; }
}

std::ostream& TGeoToObjFile(const TGeoShape& shape, std::ostream& dest, const double unitConversion)
{
  const auto& buf = shape.GetBuffer3D(TBuffer3D::kRaw | TBuffer3D::kRawSizes, true);
  auto points = buf.fPnts; //Points to draw?
  auto nPts = buf.NbPnts(); //Number of points to draw?
  auto segs = buf.fSegs;
  //auto nSegs = buf.NbSegs();
  auto pols = buf.fPols;
  auto nPols = buf.NbPols();

  std::vector<vec3> ptsVec;
  vec3 polCenter;
  for(size_t whichPoint = 0; whichPoint < nPts; ++whichPoint)
  {
    vec3 point{points[3*whichPoint], points[3*whichPoint+1], points[3*whichPoint+2]};
    ptsVec.push_back(point);
    polCenter += point;
  }
  polCenter *= 1./ptsVec.size();

  //Mapping from half-edge to third index of triangle
  std::unordered_map<std::pair<int, int>, int, ::hash> edgeToIndex;

  //Construct nested vector of indices.  Each vector corresponds to the indices needed by one polygon
  std::vector<std::vector<unsigned int>> indices;
  size_t polPos = 0; //Position in the array of polygon "components".  See https://root.cern.ch/doc/master/viewer3DLocal_8C_source.html
  for(size_t pol = 0; pol < nPols; ++pol)
  {
    size_t nVertices = pols[polPos+1]; //The second "component" of each polygon is the number of vertices it contains
    std::vector<unsigned int> thisPol; //Collect the unique vertices in this polygon in the order they were intended for drawing
    std::set<unsigned int> indicesFound; //Make sure that each index appears only once, but in eactly the order they appeared

    //Get the list of vertices for this polygon
    for(size_t vert = 0; vert < nVertices; ++vert)
    {
      const auto seg = pols[polPos+2+vert];
      //auto segToAdd = segs[1+seg*3];
      indicesFound.insert(segs[1+seg*3]);
      indicesFound.insert(segs[2+seg*3]);
    }

    //Add the vertices in counter-clockwise order
    //Find the center of the polygon as the average vertex position.
    vec3 center(0., 0., 0.);
    for(const size_t index: indicesFound) center += ptsVec[index];
    center *= 1./nVertices;
    const auto out = normalize(center-polCenter);

    //Sort vertices by angle from the first vertex
    //TODO: Can I take advantage of TGeoShape::ComputeNormal() to get normals at vertices?  Will ROOT behave reasonably at vertices?  
    //      How can I use normals to find neighbors of a triangle?
    vec3 prevDir = normalize(ptsVec[*(indicesFound.begin())]-center);
    std::vector<unsigned int> indicesSort(indicesFound.begin(), indicesFound.end());
    std::sort(indicesSort.begin(), indicesSort.end(), [&center, &ptsVec, &prevDir](const size_t first, const size_t second)
                                              {
                                                const auto firstDir = normalize(ptsVec[first]-center);
                                                const auto secondDir = normalize(ptsVec[second]-center);
                                                const float firstCos = dot(firstDir, prevDir);
                                                const float secondCos = dot(secondDir, prevDir);
                                                const float firstSin = length(cross(prevDir, firstDir));
                                                const float secondSin = length(cross(prevDir, secondDir));
                                                return atan2(firstSin, firstCos) < atan2(secondSin, secondCos);
                                              });

    //Next, make sure that pairs of vertices alternate sides of the polygon in the winding order 
    //TODO: This is the problem.  Something is wrong here.
    for(auto first = indicesSort.begin()+1; first < indicesSort.end()-1; first += 2)
    {
      auto second = first+1;
      const auto firstDir = normalize(ptsVec[*first]-center);
      //std::cout << "firstDir is " << firstDir << "\n";
      //const auto secondDir = normalize(ptsVec[*second]-center);
      //std::cout << "secondDir is " << secondDir << "\n";
      const float projFromCenter = dot(cross(prevDir, firstDir), out);
      //std::cout << "projFromCenter is " << projFromCenter << "\n";
      if(projFromCenter < 0)
      {
        //std::cout << "Swapping indices " << *first << " and " << *second << "\n";
        const int tmp = *first;
        *first = *second;
        *second = tmp;
        //std::cout << "After the swap, projFromCenter is " << dot(cross(prevDir, normalize(ptsVec[*first]-center)), out) << "\n";
      }
    }

    //Fill map with HalfEdges
    const auto begin = indicesSort.begin();
    const auto end = indicesSort.end()-2;

    //Special case for second edge of first triangle in each polygon
    edgeToIndex[std::make_pair(*(begin), *(begin+1))] = *(begin+2);
    //std::cout << "Entered index " << *(begin+2) << " for edge (" << *(begin) << ", " << *(begin+1) << ")\n";

    for(auto index = begin; index < end; ++index)
    {
      //Only include edges on the border
      edgeToIndex[std::make_pair(*(index+2), *index)] = *(index+1);
      //std::cout << "Entered index " << *(index+1) << " for edge (" << *(index+2) << ", " << *(index) << ")\n";
    }

    //Special case for second edge of last triangle in each polygon
    edgeToIndex[std::make_pair(*(end), *(end+1))] = *(end-1);
    //std::cout << "Entered index " << *(end-1) << " for edge (" << *(end) << ", " << *(end+1) << ")\n";

    thisPol.insert(thisPol.end(), indicesSort.begin(), indicesSort.end());

    polPos += nVertices+2;
    indices.push_back(thisPol);
  }

  //Convert from GL_TRIANGLE_STRIP (original format) to GL_TRIANGLES
  std::vector<std::vector<unsigned int>> triangles;
  for(const auto& pol: indices)
  {
    for(size_t whichIndex = 0; whichIndex < pol.size()-2; ++whichIndex)
    {
      //This if/else fixes the winding order when converting from GL_TRIANGLE_STRIP to GL_TRIANGLES
      //TODO: Fix winding order of triangles.  I only tested this with TGeoCylinder
      if(whichIndex%2 == 0) triangles.push_back({pol[whichIndex], pol[whichIndex+1], pol[whichIndex+2]});
      else triangles.push_back({pol[whichIndex], pol[whichIndex+2], pol[whichIndex+1]});
    }
  }

//Next block adds in adjacency information like in OpenGL's GL_TRIANGLE_STRIPS_ADJACENCY
//  for(auto& pol: indices)
//  {
//    const auto polCopy = pol;
//    /*std::cout << "Before I add any indices to pol, polCopy is:\n(";
//    for(const auto& index: polCopy)
//    {
//      std::cout << index << ", ";
//    }
//    std::cout << ")\n";*/
//
//    pol.clear();
//    pol.push_back(polCopy[0]);
//    /*if(edgeToIndex.find(std::make_pair(polCopy[1], polCopy[0])) == edgeToIndex.end()) std::cout << "Could not find index across from "
//                                                                                                << "edge (" << polCopy[0] << ", "
//                                                                                                << polCopy[1] << ")\n";*/
//    pol.push_back(edgeToIndex[std::make_pair(polCopy[1], polCopy[0])]);
//    //std::cout << "Looked up value " << edgeToIndex[std::make_pair(polCopy[1], polCopy[0])] << " for edge (" << polCopy[0] << ", "
//    //          << polCopy[1] << ")\n";
//    pol.push_back(polCopy[1]);
//
//    /*if(edgeToIndex.find(std::make_pair(polCopy[0], polCopy[2])) == edgeToIndex.end()) std::cout << "Could not find index across from "
//                                                                                                << "edge (" << polCopy[2] << ", "
//                                                                                                << polCopy[0] << ")\n";*/
//    pol.push_back(edgeToIndex[std::make_pair(polCopy[0], polCopy[2])]);
//    /*std::cout << "Looked up value " << edgeToIndex[std::make_pair(polCopy[0], polCopy[2])] << " for edge (" << polCopy[2] << ", "
//              << polCopy[0] << ")\n";*/
//    pol.push_back(polCopy[2]);
//
//    for(auto indexIt = polCopy.begin()+1; indexIt < polCopy.end()-2; ++indexIt)
//    {
//      /*if(edgeToIndex.find(std::make_pair(*(indexIt), *(indexIt+2))) == edgeToIndex.end()) std::cout << "Could not find index across from "
//                                                                                                      << "edge (" << *(indexIt+2) << ", "
//                                                                                                      << *(indexIt) << ")\n";*/
//      pol.push_back(edgeToIndex[std::make_pair(*(indexIt), *(indexIt+2))]);
//      /*std::cout << "Looked up value " << edgeToIndex[std::make_pair(*(indexIt), *(indexIt+2))] << " for edge (" << *(indexIt+2) << ", "
//                << *indexIt << ")\n";*/
//      pol.push_back(*(indexIt+2));
//    }
//
//    /*if(edgeToIndex.find(std::make_pair(*(polCopy.end()-1), *(polCopy.end()-2))) == edgeToIndex.end()) std::cout << "Could not find index across from "
//                                                                                                    << "edge (" << *(polCopy.end()-2) << ", "
//                                                                                                    << *(polCopy.end()-1) << ")\n";*/
//    pol.push_back(edgeToIndex[std::make_pair(*(polCopy.end()-1), *(polCopy.end()-2))]);
//    /*std::cout << "Looked up value " << edgeToIndex[std::make_pair(*(polCopy.end()-1), *(polCopy.end()-2))] << " for edge (" << *(polCopy.end()-2) << ", "
//              << *(polCopy.end()-1) << ")\n";*/
//  }

  //Print out ptsVec for debugging
  /*std::cout << "Printing " << ptsVec.size() << " points from shape " << shape->GetName() << ":\n";
  for(const auto& point: ptsVec) std::cout << "(" << point.x << ", " << point.y << ", " << point.z << ")\n";

  std::cout << "Printing " << nPols << " polygons from shape " << shape->GetName() << ":\n";
  for(size_t polInd = 0; polInd < indices.size(); ++polInd)
  {
    std::cout << "Polygon " << polInd << ":\n";
    for(auto index: indices[polInd])
    {
      auto point = ptsVec[index];
      std::cout << "(" << point.x << ", " << point.y << ", " << point.z << ")\n";
    }
    std::cout << "\n";
  }*/

  //Convert to .OBJ format
  dest << std::setprecision(7);
  for(const auto& point: ptsVec) dest << "v " << point.x/unitConversion << " " << point.y/unitConversion << " " << point.z/unitConversion << "\n";
  for(const auto& pol: triangles) //indices)
  {
    dest << "f ";
    for(const auto index: pol) dest << index+1 << " "; //Wavefront files expect indices to be 1-based
    dest << "\n";
  }

  return dest;
}

//Helper functions for searchGeometryTree()
void appendChildren(const std::vector<std::string>& searchNames, std::vector<std::pair<TGeoNode*, TGeoMatrix*>>& toDraw, TGeoNode* parentNode, TGeoMatrix* parentMatrix);

void appendNode(const std::vector<std::string>& searchNames, std::vector<std::pair<TGeoNode*, TGeoMatrix*>>& toDraw, TGeoNode* node, TGeoMatrix* parentMatrix)
{
  auto localMatrix = new TGeoHMatrix(*node->GetMatrix());

  const double* translation = localMatrix->GetTranslation();
  std::cout << "Got node " << node->GetName() << " at (" << translation[0] << ", " << translation[1] << ", " << translation[2] << ")\n";

  //TODO: Something is still wrong about how rotations and translations interact on this line.  The lead-glass calorimeter has this problem.
  localMatrix->MultiplyLeft(parentMatrix); //TODO: Do I need to left-multiply or right-multiply in ROOT?

  translation = localMatrix->GetTranslation();
  std::cout << "World position of " << node->GetName() << " is (" << translation[0] << ", " << translation[1] << ", " << translation[2] << ")\n";

  if(strcmp(node->GetVolume()->GetShape()->IsA()->GetName(), "TGeoCompositeShape")  //Skip TGeoCompositeShapes because asking ROOT to triangulate them causes an invalid memory access!
     && std::find(searchNames.begin(), searchNames.end(), node->GetName()) != searchNames.end())
  {
    toDraw.emplace_back(node, localMatrix);
  }

  appendChildren(searchNames, toDraw, node, localMatrix);
}

void appendChildren(const std::vector<std::string>& searchNames, std::vector<std::pair<TGeoNode*, TGeoMatrix*>>& toDraw, TGeoNode* parentNode, TGeoMatrix* parentMatrix)
{
  auto children = parentNode->GetNodes();
  if(children)
  {
    for(auto child: *children) appendNode(searchNames, toDraw, static_cast<TGeoNode*>(child), parentMatrix);
  }
}

std::vector<std::pair<TGeoNode*, TGeoMatrix*>> searchGeometryTree(TGeoManager& man, const std::vector<std::string>& nodeNames)
{
  std::vector<std::pair<TGeoNode*, TGeoMatrix*>> result;
  appendNode(nodeNames, result, man.GetTopNode(), gGeoIdentity);
  return result;
}

#endif //TGEOTOOBJFILE_CPP

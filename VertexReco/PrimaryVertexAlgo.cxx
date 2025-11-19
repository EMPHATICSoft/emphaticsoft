////////////////////////////////////////////////////////////////////////
/// \brief   Class for primary vertex finding/fitting algorithm
/// \author  Jon Paley
/// \date    9/22/25
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "VertexReco/PrimaryVertexAlgo.h"
#include "RecoBase/SpacePoint.h"
#include "RecoUtils/RecoUtils.h"

#include <vector>
#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "Math/SVector.h"
#include "Math/SMatrix.h"
#include "Math/Vector3D.h"

using namespace std;

typedef ROOT::Math::SVector<double,3> Vector3d;
typedef ROOT::Math::SMatrix<double,3,3> Matrix3d;

namespace emph {
       
   PVAlgo::PVAlgo() {}

  //----------------------------------------------------------------------

  bool PVAlgo::FindVertexDOCA(std::vector<rb::Track>& trks, 
				       rb::Vertex& vtx)
  {
    using namespace ROOT::Math;

    if (trks.size() <= 1) return false;

    if (trks.size() == 2) {
      vtx.pos = (trks[1].posTrgt+trks[0].posTrgt)/2.;
      //      vtx.pos.SetZ(-99999.);
      vtx.trkIdx.push_back(0);
      vtx.trkIdx.push_back(1);

      auto a = trks[0].momTrgt.Cross(trks[1].momTrgt);
      double dot = a.Dot(a);

      if (dot == 0) {
	std::cout<<"Uh oh, tracks are _exactly_ parallel!"<<std::endl;
	return false;
      }

      auto ab = trks[1].posTrgt - trks[0].posTrgt;
      auto b = ab.Cross(trks[1].momTrgt);

      double t = b.Dot(a) / dot;
      vtx.pos = trks[1].posTrgt + t*trks[1].momTrgt;
      std::cout << "vtx.pos = " << vtx.pos << std::endl;

    }
    else {
      Vector3d b(0.,0.,0.);
      Matrix3d A;
      
      for (size_t itrk=0;  itrk<trks.size(); ++itrk) {
	auto trk = trks[itrk];
	Vector3d dir(trk.momTrgt.X(),trk.momTrgt.Y(),trk.momTrgt.Z());
	dir = dir.Unit();
	
	Matrix3d dd;// = ROOT::Math::TensorProd(dir,dir);
	
	for (int i=0; i<3; ++i)
	  for (int j=0; j<3; ++j)
	    dd(i,j) = dir(i) * dir(j);     
	
	Matrix3d P = Matrix3d(SMatrixIdentity()) - dd;
	
	A += P;
	Vector3d pvec(trk.posTrgt.X(),trk.posTrgt.Y(),trk.posTrgt.Z());
	b += P*pvec;
	
	vtx.trkIdx.push_back(itrk);
      }
      
      std::cout << "A = " << A << std::endl;
      // Solve A * x = b
      int ok=0;
      Vector3d x = A.Inverse(ok) * b;
      if (!ok) {
	std::cerr << "Matrix inversion failed (lines may be parallel/degenerate)." << std::endl;
	vtx.pos.SetXYZ(99999.,99999.,99999.);
	return false;
      }
      else {
	std::cout << "vtx = " << x << std::endl;
	vtx.pos.SetXYZ(x(0), x(1), x(2));
      }
    }

    return true;

  }
  
  //----------------------------------------------------------------------
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// \brief   Class for primary vertex finding/fitting algorithm
/// \author  Jon Paley
/// \date    9/22/25
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "VertexReco/PrimaryVertexAlgo.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/Track.h"
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
 
  namespace math {
    struct Line {
      ROOT::Math::XYZVector p; // point on line
      ROOT::Math::XYZVector d; // direction
    };
  }
      
  //----------------------------------------------------------------------
  
  PVAlgo::PVAlgo() {}

  //----------------------------------------------------------------------

  bool PVAlgo::FindVertexDOCA(std::vector<rb::Track>& trks, 
				       rb::Vertex& vtx)
  {
    using namespace ROOT::Math;

    if (trks.size() <= 1) return false;

    std::cout << "In PVAlgo::FindVertexDOCA: " << std::endl;

    if (trks.size() == 2) {
      Vector3d a(trks[0].mom.X(),trks[0].mom.Y(),trks[0].mom.Z());
      Vector3d b(trks[1].mom.X(),trks[1].mom.Y(),trks[1].mom.Z());
      Vector3d p_a(trks[0].posTrgt.X(),trks[0].posTrgt.Y(),trks[0].posTrgt.Z());
      Vector3d p_b(trks[1].posTrgt.X(),trks[1].posTrgt.Y(),trks[1].posTrgt.Z());
      std::cout << "trk 0 p = (" << trks[0].mom << ")" << std::endl;
      std::cout << "trk 1 p = (" << trks[1].mom << ")" << std::endl;
      std::cout << "a = " << a << std::endl;
      std::cout << "b = " << b << std::endl;

      auto c = ROOT::Math::Cross(a,b);
      double dot = ROOT::Math::Dot(c,c); // inner product
      if (dot == 0)
	return false;

      Vector3d d = ROOT::Math::Cross((p_b-p_a),b);
      float t = ROOT::Math::Dot(d,c)/dot;
      Vector3d tvtx = p_a + (t * a);
      std::cout << "tvtx = " << tvtx << std::endl;
    }
    else {
      Vector3d b(0.,0.,0.);
      Matrix3d A;
      
      int itrk=0;
      for (const auto& trk : trks) {
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
	
	vtx.AddTrackUID(itrk++);
      }
      
      std::cout << "A = " << A << std::endl;
      // Solve A * x = b
      int ok=0;
      Vector3d x = A.Inverse(ok) * b;
      if (!ok) {
	std::cerr << "Matrix inversion failed (lines may be parallel/degenerate)." << std::endl;
	vtx.SetPosition(99999.,99999.,99999.);
	return false;
      }
      else {
	std::cout << "vtx = " << x << std::endl;
	vtx.SetPosition(x(0), x(1), x(2));          
      }
    }

    return true;

  }
  
  //----------------------------------------------------------------------
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////

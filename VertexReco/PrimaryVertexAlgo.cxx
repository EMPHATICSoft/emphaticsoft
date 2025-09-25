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

  bool PVAlgo::FindVertexDOCA(std::vector<const rb::Track*>trks, 
				       rb::Vertex& vtx)
  {
    using namespace ROOT::Math;

    if (trks.size() <= 1) return false;

    Vector3d b(0.,0.,0.);
    Matrix3d A;

    int itrk=0;
    for (const auto& trk : trks) {
      if (trk->NTrackSegments() > 0) {
	auto trkseg = trk->GetTrackSegment(0);
	Vector3d dir(trkseg->P()[0],trkseg->P()[1],trkseg->P()[2]);
	dir = dir.Unit();

        Matrix3d dd = ROOT::Math::TensorProd(dir,dir);
	/*			
        for (int i=0; i<3; ++i)
	  for (int j=0; j<3; ++j)
	    dd(i,j) = dir(i) * dir(j);
	*/

	Matrix3d P = Matrix3d(SMatrixIdentity()) - dd;

	A += P;
	Vector3d pvec(trkseg->A()[0],trkseg->A()[1],trkseg->A()[2]);
	b += P*pvec;
	
	vtx.AddTrackUID(itrk++);
      }
    }
    
    // Solve A * x = b
    int ok;
    Vector3d x = A.Inverse(ok) * b;
    if (!ok) {
      std::cerr << "Matrix inversion failed (lines may be parallel/degenerate)." << std::endl;
      vtx.SetPosition(99999.,99999.,99999.);
      return false;
    }
    else
      vtx.SetPosition(x(0), x(1), x(2));    
    return true;

  }
  
  //----------------------------------------------------------------------
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////

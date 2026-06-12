////////////////////////////////////////////////////////////////////////
/// \brief   Class for primary vertex finding/fitting algorithm
/// \author  Jon Paley and Aayush Bhattarai
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

    vtx.sectrkIdx.clear();
    vtx.pos.SetXYZ(99999., 99999., 99999.);

    if (trks.size() <= 1) return false;

    if (trks.size() == 2) {

      // Modeling tracks as an infinite 3D lines:
      //   track 0 (Beam track)     : C0(s) = P0 + s*d0
      //   track 1 (Secondary track): C1(t) = P1 + t*d1
      //
      // For two reconstructed tracks, the lines usually do not intersect
      // exactly. The DOCA vertex is taken to be the midpoint of the two
      // points of closest approach.

      auto P0 = trks[0].posTrgt; // trks[0] = beam track
      auto P1 = trks[1].posTrgt; // trks[1] = secondary track
      auto d0 = trks[0].momTrgt;
      auto d1 = trks[1].momTrgt;

      const double eps = 1.e-12;

      const double d0mag2 = d0.Dot(d0);
      const double d1mag2 = d1.Dot(d1);


      // Reject tracks with zero/near-zero direction (degenerate input)
      if (d0mag2 <= eps || d1mag2 <= eps) {
        std::cerr << "FindVertexDOCA: track has zero or near-zero direction."
                  << std::endl;
        return false;
      }

      auto n = d0.Cross(d1);
      const double denom = n.Dot(n);

      // denom = |d0 x d1|^2.
      // If this is very small, the two tracks are parallel or nearly parallel.
      if (denom <= eps*d0mag2*d1mag2) {
        std::cerr << "FindVertexDOCA: tracks are parallel or nearly parallel."
                  << std::endl;
        return false;
      }

      auto r = P1 - P0; // displacement vector

      // Parameters of closest approach on each track.
      const double s = r.Cross(d1).Dot(n) / denom; // parameter on track 0
      const double t = r.Cross(d0).Dot(n) / denom; // parameter on track 1

      auto poca0 = P0 + s*d0;
      auto poca1 = P1 + t*d1;

      // DOCA vertex: midpoint between the two closest points.
      vtx.pos = (poca0 + poca1)/2.;

      vtx.sectrkIdx.push_back(1);
    }
    else {
      Vector3d b(0.,0.,0.);
      Matrix3d A;

      for (size_t itrk = 0; itrk < trks.size(); ++itrk)
      {
        auto trk = trks[itrk];
        Vector3d dir(trk.momTrgt.X(), trk.momTrgt.Y(), trk.momTrgt.Z());
        dir = dir.Unit();

        Matrix3d dd; // = ROOT::Math::TensorProd(dir,dir);

        for (int i = 0; i < 3; ++i)
          for (int j = 0; j < 3; ++j)
            dd(i, j) = dir(i) * dir(j);

        Matrix3d P = Matrix3d(SMatrixIdentity()) - dd;

        A += P;
        Vector3d pvec(trk.posTrgt.X(), trk.posTrgt.Y(), trk.posTrgt.Z());
        b += P * pvec;

        if (itrk > 0)
          vtx.sectrkIdx.push_back(itrk);
      }

      std::cout << "A = " << A << std::endl;
      // Solve A * x = b
      int ok = 0;
      Vector3d x = A.Inverse(ok) * b;
      if (ok)
      {
        std::cerr << "Matrix inversion failed (lines may be parallel/degenerate)." << std::endl;
        vtx.pos.SetXYZ(99999., 99999., 99999.);
        return false;
      }
      else
      {
        std::cout << "vtx = " << x << std::endl;
        vtx.pos.SetXYZ(x(0), x(1), x(2));
      }
    }

    return true;

  }
  
  //----------------------------------------------------------------------
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////

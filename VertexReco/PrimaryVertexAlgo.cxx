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

      vtx.sectrkIdx.push_back(1);

      // Two-track DOCA midpoint vertex.
      //
      // Lines: L0(s) = P0 + s*d0,  L1(t) = P1 + t*d1
      // Common-perpendicular normal: n = d0 x d1
      // Closest-approach parameters:
      //   s* = (r x d1) . n / |n|^2,  where r = P1 - P0
      //   t* = (r x d0) . n / |n|^2
      // Closest-approach points: C0 = P0 + s*d0,  C1 = P1 + t*d1
      // Vertex = midpoint (C0 + C1) / 2
      const auto& P0 = trks[0].posTrgt;
      const auto& P1 = trks[1].posTrgt;
      const auto& d0 = trks[0].momTrgt;
      const auto& d1 = trks[1].momTrgt;

      auto n     = d0.Cross(d1);
      double denom = n.Dot(n);

      // Relative parallelism check: sin^2(theta) < 1e-10
      if (denom < 1e-10 * d0.Dot(d0) * d1.Dot(d1)) {
        std::cout << "FindVertexDOCA: tracks are parallel, returning midpoint of origin positions." << std::endl;
        vtx.pos = (P0 + P1) * 0.5;
        return true;
      }

      auto r  = P1 - P0;
      double s = r.Cross(d1).Dot(n) / denom;   // parameter along track 0
      double t = r.Cross(d0).Dot(n) / denom;   // parameter along track 1

      auto C0 = P0 + s * d0;
      auto C1 = P1 + t * d1;
      vtx.pos  = (C0 + C1) * 0.5;

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

      // Solve A * x = b
      int ok = 0;
      Vector3d x = A.Inverse(ok) * b;
      if (!ok)
      {
        std::cerr << "FindVertexDOCA: matrix inversion failed (tracks may be parallel/degenerate)." << std::endl;
        vtx.pos.SetXYZ(99999., 99999., 99999.);
        return false;
      }
      vtx.pos.SetXYZ(x(0), x(1), x(2));
    }

    return true;

  }
  
  //----------------------------------------------------------------------
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////

/// \file    GeometryDrawer.h
/// \brief   Class to aid in the rendering of Geometry objects
/// \author  messier@indiana.edu
/// \version $Id: GeometryDrawer.h,v 1.5 2012-12-05 02:24:46 bckhouse Exp $
#ifndef EVD_GEOMETRYDRAWER_H
#define EVD_GEOMETRYDRAWER_H

#include <vector>
#include <map>

//#include "GeometryObjects/PlaneGeo.h" // For View_t

class TH1F;
namespace art     { class Event;       }
namespace evdb    { class View2D;      }
namespace evdb    { class View3D;      }
namespace geo     { class Geometry;    }

namespace evd {
  /// Aid in the rendering of Geometry objects
  class GeometryDrawer {
  public:
    GeometryDrawer();
    ~GeometryDrawer();
    /*
    void GetBox(double* xlo, double* xhi, double* ylo,
		double* yhi, double* zlo, double* zhi);
    */

    void DetOutline2D(const art::Event&    evt,
		      evdb::View2D*        xzview,
		      evdb::View2D*        yzview);

    void DetOutline3D(const art::Event&    evt,
		      evdb::View3D*        view);

    /*
    void FiducialVolumeUser2D(const art::Event& evt,
			      evdb::View2D*     xzview,
			      evdb::View2D*     yzview);
    
    void DrawDCMBoxes(const art::Event&     evt,
		      evdb::View2D*         xzview,
		      evdb::View2D*         yzview);

    void DrawCells2D(const art::Event&     evt,
		     evdb::View2D*         xzview,
		     evdb::View2D*         yzview,
		     double xlo=-99E9, double xhi=99E9,
		     double ylo=-99E9, double yhi=99E9,
		     double zlo=-99E9, double zhi=99E9);
    
    void DrawHighlightCell(const art::Event& evt,
                           evdb::View2D*   xzview,
                           evdb::View2D*   yzview);

    void ShowCellAndPlaneNumbers(evdb::View2D* view,
				 int ip,
				 int ic,
				 double z,
				 double dz,
				 double t,
				 double dt);

    /// Draw the exact boxes LiveGeometry uses internally
    void DrawBadBoxesExact(const art::Event& evt,
                             evdb::View2D*     xzview,
                             evdb::View2D*     yzview);

    /// Draw the minimal outline of the bad regions
    void DrawBadBoxesPretty(const art::Event& evt,
                      evdb::View2D*     xzview,
                      evdb::View2D*     yzview);
    */

  protected:
    /*
    struct Box
    {
      double xylo, zlo, xyhi, zhi;
    };

    std::map<int, Box> GetDCMBoxes(geo::View_t view) const;
    */

  };
}

#endif
////////////////////////////////////////////////////////////////////////

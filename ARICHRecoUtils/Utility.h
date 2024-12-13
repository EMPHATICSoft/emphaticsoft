////////////////////////////////////////////////////////////////////////
/////// \brief   Definition of some functions
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////


#ifndef UTILITY_INCLUDE
#define UTILITY_INCLUDE

#include "TVector3.h"
#include "TMatrixD.h"
#include "TCutG.h"
#include "TEllipse.h"

    namespace ARICHRECO{


        inline TMatrixD makeRotationMatrix(TVector3 dir) {
          /*
          Rotation matrix to rotate (x,y,z)=(0,0,1) onto dir
          https://math.stackexchange.com/questions/180418
          https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula#Matrix_notation
          Could also use ROOT's TVector3::RotateUz() instead
          */

          TMatrixD rot(3, 3);
          // a = (0, 0, 1)
          // b = (dirx, diry, dirz)
          // v = a x b = (-diry, dirx, 0)
          TVector3 v(-dir.Y(), dir.X(), 0.);
          double c = dir.Z(); // cos(a,b) = a . b = (0,0,1) . (dirx, diry, dirz) = dirz

          // rot =   I   + v    + v^2                    * (1-c)/s^2
          TMatrixD V = TMatrixD(3, 3);

          double k = 1./(1.+c);
          rot(0,0) = 1.0        - (v[2]*v[2]+v[1]*v[1])*k;
          rot(0,1) =     - v[2] - (v[0]*v[1])          *k;
          rot(0,2) =     + v[1] + (v[0]*v[2])          *k;
          rot(1,0) =     + v[2] + (v[0]*v[1])          *k;
          rot(1,1) = 1.0        - (v[2]*v[2]+v[0]*v[0])*k;
          rot(1,2) =     - v[0] - (v[2]*v[1])          *k;
          rot(2,0) =     - v[1] - (v[0]*v[2])          *k;
          rot(2,1) =     + v[0] + (v[1]*v[2])          *k;
          rot(2,2) = 1.0        - (v[1]*v[1]+v[0]*v[0])*k;
          return rot;
        }

        inline TVector3 rotateVector(TMatrixD rot, double theta, double phi) {
          /*
          Apply a rotation matrix to a theta and phi
          */
          // Beam frame angle of photon
          TVector3 dir;
          dir.SetX(cos(phi)*sin(theta));
          dir.SetY(sin(phi)*sin(theta));
          dir.SetZ(cos(theta));
          // photon direction rotated onto particle direction
          return rot*dir;
        }

        inline TVector3 refractedDirection(TVector3 l, TVector3 n, double n1, double n2) {
          /*
          From: https://en.wikipedia.org/wiki/Snell%27s_law#Vector_form
          light w/ normalized direction l incident on surface w/ normal n, from indices n1 to n2
          */
          double c = -l*n; // cos(theta1) = - l dot n
          double r = n1 / n2;
          return r*l + (r*c - sqrt(1-r*r*(1-c*c)))*n;
        }

        inline TVector3 reflectedDirection(TVector3 l, TVector3 n) {
          /*
          From: https://en.wikipedia.org/wiki/Snell%27s_law#Vector_form
          light w/ normalized direction l incident on surface w/ normal n
          */
          double c = -l*n; // cos(theta1) = - l dot n
          return l + 2*c*n;
        }

        inline TCutG *createCutFromEllipse(TEllipse *ellipse) {
          /*
          Return an elliptical TCutG, given a TEllipse
          */
          int np = 200;
          double angle,dx,dy;
          double dphi = 2.*TMath::Pi()/np;
          double x1 = ellipse->GetX1();
          double y1 = ellipse->GetY1();
          double r1 = ellipse->GetR1();
          double r2 = ellipse->GetR2();
          double ct   = TMath::Cos(TMath::Pi()*ellipse->GetTheta()/180);
          double st   = TMath::Sin(TMath::Pi()*ellipse->GetTheta()/180);
          TCutG *cut = new TCutG();
          for (Int_t i=0;i<=np;i++) {
             angle = Double_t(i)*dphi;
             dx    = r1*TMath::Cos(angle);
             dy    = r2*TMath::Sin(angle);
             cut->SetPoint(i, x1 + dx*ct - dy*st, y1 + dx*st + dy*ct);
           }
          return cut;
        }
}

#endif

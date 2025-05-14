#include "Propagator.h"
#include <cmath>

double kalman::Propagator::fKappa = 2.997925e-4; // (GeV/c) x kG x mm

namespace kalman {

  Propagator::Propagator()
  {
  }

  // ------------------------------------------------------------

  double Propagator::Extrapolate(State& sstart, double st, double B[3])
  {
    
    double ds = 0.; // return the total pathlength.

    // Calculate needed parameters
    double x = sstart.GetPar(0);
    double y = sstart.GetPar(1);	
    double tx = sstart.GetPar(2);
    double ty = sstart.GetPar(3);		
    double tx2 = tx*tx;
    double ty2 = ty*ty;
    double txy = tx*ty;
    double qdp = sstart.GetPar(4);
    double h = fKappa*qdp*sqrt(1+tx2+ty2);
    
    double dC = fKappa*qdp*fKappa*qdp;
    
    double Bx = B[0];
    double By = B[1];
    double Bz = B[2];
    double dtx1, dtx2, dtx3;
    double dty1, dty2, dty3;
    dtx1 = dtx2 = dtx3 = 0.;
    dty1 = dty2 = dty3 = 0.;
    
    dtx1 = txy*Bx - (1+tx2)*By + ty*Bz;
    dtx2 = tx*(3*ty2+1)*Bx*Bx -2*ty*(3*tx2+1)*Bx*By + (3*ty2-tx2+1)*Bx*Bz + 3*tx*(tx2+1)*By*By - 4*txy*By*Bz - tx*Bz*Bz;
    dtx3 = 3*txy*(5*ty2+3)*Bx*Bx*Bx - 3*(3*tx2+3*ty2+15*tx2*ty2+1)*Bx*Bx*By + ty*(-10*tx2+15*ty2+9)*Bx*Bx*Bz +  9*tx*ty*(5*tx2+3)*Bx*By*By
      + tx*(10*tx2-40*ty2-2)*Bx*By*Bz - 11*tx*ty*Bx*Bz*Bz - 3*(tx2+1)*(5*tx2+1)*By*By*By + ty*(25*tx2+7)*By*By*Bz
      + (7*tx2-4*ty2+1)*By*Bz*Bz - ty*Bz*Bz*Bz;
      
    dty1 = (1+ty2)*Bx - txy*By - tx*Bz;
    dty2 = 3*ty*(ty2+1)*Bx*Bx - 2*tx*(3*ty2+1)*Bx*By - 4*txy*Bx*Bz + ty*(3*tx2+1)*By*By + (3*tx2-ty2+1)*By*Bz - ty*Bz*Bz;
    dty3 = 3*(ty2+1)*(5*ty2+1)*Bx*Bx*Bx -9*txy*(5*ty2+3)*Bx*Bx*By - tx*(25*ty2+7)*Bx*Bx*Bz + 3*(3*tx*tx+3*ty*ty+15*tx2*ty2+1)*Bx*By*By
      - ty*(-40*tx2+10*ty2-2)*Bx*By*Bz + (4*tx2-7*ty2-1)*Bx*Bz*Bz - 3*txy*(5*tx2+3)*By*By*By
      - tx*(15*tx2-10*ty2+9)*By*By*Bz + 11*txy*By*Bz*Bz + tx*Bz*Bz*Bz;

    x = x + tx*st + h*dtx1*st*st/2. + h*h*dtx2*st*st*st/6. + h*h*h*dtx3*st*st*st*st/24.;
    y = y + ty*st + h*dty1*st*st/2. + h*h*dty2*st*st*st/6. + h*h*h*dty3*st*st*st*st/24.;
    
    KPar par;
    par[0] = x;
    par[1] = y;
    par[2] = tx + h*dtx1*st + h*h*dtx2*st*st/2. + h*h*h*dtx3*st*st*st/6.;
    par[3] = ty + h*dty1*st + h*h*dty2*st*st/2. + h*h*h*dty3*st*st*st/6.;
    par[4] = qdp;
    sstart.SetPar(par);
    sstart.SetZ(sstart.GetZ()+st);
    double avgtx = (tx + par[2])/2.;
    double avgty = (ty + par[3])/2.;
    ds = st*sqrt(1. + avgtx*avgtx + avgty*avgty);

    double dtx1dtx = 0.;
    double dtx1dty = 0.;
    double dtx2dtx = 0.;
    double dtx2dty = 0.;
    double dtx3dtx = 0.;
    double dtx3dty = 0.;
    double dty1dtx = 0.;
    double dty1dty = 0.;
    double dty2dtx = 0.;
    double dty2dty = 0.;
    double dty3dtx = 0.;
    double dty3dty = 0.;

    K5x5 J;
    for (int i=0; i<5; ++i) {
      for (int j=0; j<5; ++j)
	J[i][j] = 0.;
      J[i][i] = 1.;
    }
    
    dtx1dtx = ty*Bx - 2*tx*By;
    dtx1dty = tx*Bx + Bz;
      
    dtx2dtx = (3*ty2+1)*Bx*Bx - 12*txy*Bx*By - 2*tx*Bx*Bz + 3*(3*tx2+1)*By*By - 4*ty*By*Bz - Bz*Bz;
    dtx2dty = 6*txy*Bx*Bx - 2*(3*tx2+1)*Bx*By + 6*ty*Bx*Bz - 4*tx*By*Bz;
      
    dtx3dtx = 3*ty*(5*ty2+3)*Bx*Bx*Bx - 18*tx*(5*ty2+1)*Bx*Bx*By - 20*txy*Bx*Bx*Bz + 27*ty*(5*tx2+1)*Bx*By*By + 2*(15*tx2-20*ty2-1)*Bx*By*Bz 
      - 11*ty*Bx*Bz*Bz - 12*tx*(5*tx2+3)*By*By*By + 40*txy*By*By*Bz + 14*tx*By*Bz*Bz;
    dtx3dty = 9*tx*(5*ty2+1)*Bx*Bx*Bx - 18*ty*(5*tx2+1)*Bx*Bx*By + (-10*tx2+45*ty2+9)*Bx*Bx*Bz + 9*tx*(5*tx2+3)*Bx*By*By -80*txy*Bx*By*Bz
      - 11*tx*Bx*Bz*Bz + (25*tx2+7)*By*By*Bz - 8*ty*By*Bz*Bz - Bz*Bz*Bz;
      
      
    dty1dtx = -ty*By - Bz;
    dty1dty = 2*ty*Bx + tx*By;
      
    dty2dtx = -2*(3*ty2+1)*Bx*By - 4*ty*Bx*Bz + 6*txy*By*By + 6*tx*By*Bz;
    dty2dty = 3*(3*ty2+1)*Bx*Bx - 12*txy*Bx*By - 4*tx*Bx*Bz + (3*tx2+1)*By*By - 2*ty*By*Bz - Bz*Bz;	
      
    dty3dtx = -9*ty*(5*ty2+3)*Bx*Bx*By - (25*ty2+7)*Bx*Bx*Bz + 18*tx*(5*ty2+1)*Bx*By*By + 80*txy*Bx*By*Bz + 8*tx*Bx*Bz*Bz
      - 9*ty*(5*tx2+1)*By*By*By - (45*tx2-10*ty2+9)*By*By*Bz + 11*ty*By*Bz*Bz + Bz*Bz*Bz;
    dty3dty = -12*ty*(5*ty2+3)*Bx*Bx*Bx - 27*tx*(5*ty2+1)*Bx*Bx*By - 40*txy*Bx*Bx*Bz + 18*ty*(5*tx2+1)*Bx*By*By + 2*(20*tx2-15*ty2+1)*Bx*By*Bz
      - 14*ty*Bx*Bz*Bz - 3*tx*(5*tx2+3)*By*By*By + 20*txy*By*By*Bz + 11*By*Bz*Bz;
    
    // calculate Jacobian to account for change coordinate system (change in tx and ty)
    
    J[0][0] = 1;
    J[0][2] = st + h*dtx1dtx*st*st/2. + dC*tx*dtx1*st*st/2./h + h*h*dtx2dtx*st*st*st/6. + dC*tx*dtx2*st*st*st/3.
      + h*h*h*dtx3dtx*st*st*st*st/24. + dC*tx*h*dtx3*st*st*st*st/12.;
    J[0][3] = h*dtx1dty*st*st/2. + dC*ty*dtx1*st*st/2./h + h*h*dtx2dty*st*st*st/6. + dC*ty*dtx2*st*st*st/3. + h*h*h*dtx3dty*st*st*st*st/24. + dC*ty*h*dtx3*st*st*st*st/12.;
    J[0][4] = h*dtx1*st*st/(2.*qdp) + h*h*dtx2*st*st*st/(3.*qdp) + h*h*h*dtx3*st*st*st*st/(8.*qdp);
    
    J[1][1] = 1;
    J[1][2] = h*dty1dtx*st*st/2. + dC*tx*dty1*st*st/2./h + h*h*dty2dtx*st*st*st/6. + dC*tx*dty2*st*st*st/3. + h*h*h*dty3dtx*st*st*st*st/24. + dC*tx*h*dty3*st*st*st*st/12.;
    J[1][3] = st + h*dty1dty*st*st/2. + dC*ty*dty1*st*st/2./h + h*h*dty2dty*st*st*st/6. + dC*ty*dty2*st*st*st/3. + h*h*h*dty3dty*st*st*st*st/24. + dC*ty*h*dty3*st*st*st*st/12.;
    J[1][4] = h*dty1*st*st/(2.*qdp) + h*h*dty2*st*st*st/(3.*qdp) + h*h*h*dty3*st*st*st*st/(8.*qdp);
    
    J[2][2] = 1 + h*dtx1dtx*st + dC*tx*dtx1*st/h + h*h*dtx2dtx*st*st/2. + dC*tx*dtx2*st*st + h*h*h*dtx3dtx*st*st*st/6. + dC*tx*h*dtx3*st*st*st/2.;
   J[2][3] = h*dtx1dty*st + dC*ty*dtx1*st/h + h*h*dtx2dty*st*st/2. + dC*ty*dtx2*st*st + h*h*h*dtx3dty*st*st*st/6. + dC*ty*h*dtx3*st*st*st/2.;
    J[2][4] = h*dtx1*st/qdp + h*h*dtx2*st*st/qdp + h*h*h*dtx3*st*st*st/(2.*qdp);	
    
    J[3][2] = h*dty1dtx*st + dC*tx*dty1*st/h + h*h*dty2dtx*st*st/2. + dC*tx*dty2*st*st + h*h*h*dty3dtx*st*st*st/6. + dC*tx*h*dty3*st*st*st/2.;
    J[3][3] = 1 + h*dty1dty*st + dC*ty*dty1*st/h + h*h*dty2dty*st*st/2. + dC*ty*dty2*st*st + h*h*h*dty3dty*st*st*st/6. + dC*ty*h*dty3*st*st*st/2.;
    J[3][4] = h*dty1*st/qdp + h*h*dty2*st*st/qdp + h*h*h*dty3*st*st*st/(2.*qdp);		    
    J[4][4] = 1;
     
    // update covariance
    K5x5 JT = Transpose(J);
    K5x5 C2 = J * sstart.GetCov() * JT;
    sstart.SetCov(C2);

    return ds;
  }

  // ------------------------------------------------------------

  void Propagator::AddNoise(State &tstate, double dz, double len)
  {
    
    double SigTheta = 0.0136*fabs(tstate.GetPar(4) * sqrt(len) * (1.+0.038*log(len)));
    if (fVerbosity)
      std::cout << "len = " << len << ", SigTheta x 1e6 = " << SigTheta*1.e6 << std::endl;
    
    double p3 = tstate.GetPar(2);
    double p4 = tstate.GetPar(3);	
  	
    double Vartx   = SigTheta*SigTheta * (1 + p3*p3) * (1 + p3*p3 + p4*p4);
    double Varty   = SigTheta*SigTheta * (1 + p4*p4) * (1 + p3*p3 + p4*p4);
    double Covtxty = SigTheta*SigTheta * p3*p4       * (1 + p3*p3 + p4*p4);

    K5x5 C2 = tstate.GetCov();
    C2[0][0] += dz*dz*Vartx;
    C2[0][1] += dz*dz*Covtxty;
    C2[0][2] -= -dz*Vartx;
    C2[0][3] -= dz*Covtxty;
    C2[1][0] += dz*dz*Vartx;
    C2[1][1] += dz*dz*Varty;
    C2[1][2] -= dz*Covtxty;
    C2[1][3] -= dz*Varty;
    C2[2][0] -= dz*Covtxty;
    C2[2][1] -= dz*Covtxty;
    C2[2][2] += Vartx;
    C2[2][3] += Covtxty;
    C2[3][0] -= dz*Covtxty; 
    C2[3][1] -= dz*Varty;
    C2[3][2] += Covtxty;
    C2[3][3] += Varty;

    tstate.SetCov(C2);
  		
  }

} // end namespace kalman

#include "Reconstruction.h"
#include "Utility.h"
#include "TMath.h"
#include <iomanip>
#include <algorithm>
//#include "omp.h"
#include "TFile.h"
#include "TrackTools.h"

using namespace utl;
using namespace std;

using namespace TrackTools;


void Reconstruction::CalculateClusterPosition(Cluster &cluster){

    #ifdef DEBUG
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "DEBUG OUTPUT" << endl;
    cout << __func__ << " in " << __FILE__ << endl;
    #endif
    int id = cluster.GetPlaneId();
    double size = fPlanes.at(id).GetSizeX();


    double xC = cluster.GetPosition(eX);
    double yC = cluster.GetPosition(eY);
    double zC = cluster.GetPosition(eZ);


    double t;
    double tErr;

    double a;

    t = (xC+0.5*size)/fPlanes.at(id).GetPitch();

    double min = TMath::Floor(t);
    double max = min + 1;

    if(t-min < fPlanes.at(id).GetChargeSharing()/2.) {

	double t1 = (min-0.5)*fPlanes.at(id).GetPitch();
	double t2 = (max-0.5)*fPlanes.at(id).GetPitch();
	t = t*fPlanes.at(id).GetPitch();

	double r2 = fPlanes.at(id).GetNumberOfADCChannels();
	double r1 = r2*(t2-t)/(t-t1);

	r1 = TMath::Ceil(r1+0.5);
	r2 = TMath::Ceil(r2+0.5);

	t = (r1*t1 + r2*t2)/(r1+r2);

	tErr = sqrt(r1*r1 + r2*r2)*fPlanes.at(id).GetPitch()/sqrt(12)/(r1+r2);

    }
    else if(max-t < fPlanes.at(id).GetChargeSharing()/2.) {

	double t1 = (min+0.5)*fPlanes.at(id).GetPitch();
	double t2 = (max+0.5)*fPlanes.at(id).GetPitch();
	t = t*fPlanes.at(id).GetPitch();
	//double r1 = fCharge->GetRandom();
	double r1 = fPlanes.at(id).GetNumberOfADCChannels();
	double r2 = r1*(t1-t)/(t-t2);
	//cerr << "TEST5" << endl;
	r1 = TMath::Ceil(r1+0.5);
	r2 = TMath::Ceil(r2+0.5);

	t = (r1*t1 + r2*t2)/(r1+r2);

	tErr = sqrt(r1*r1 + r2*r2)*fPlanes.at(id).GetPitch()/sqrt(12)/(r1+r2);
	//cerr << "TEST6" << endl;
    }
    else{
	t = (min+0.5)*fPlanes.at(id).GetPitch();
	tErr = fPlanes.at(id).GetPitch()/sqrt(12);

    }

    cluster.SetPosition(t-size/2., 0, zC);
    cluster.SetPositionError(tErr, fPlanes.at(id).GetSizeY()/sqrt(12));

    #ifdef DEBUG
    cout << "   Initial position: (" << xC << ", " << yC << ", " << zC << ")"  << endl;
    cout << "   Final position: (" << t-size/2. << ", " << 0 << ", " << zC << ")"  << endl;
    cout << "   Uncertainty: (" << tErr << ", " << fPlanes.at(id).GetSizeY()/sqrt(12) << ", " << 0 << ")" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    #endif

}


bool point_in_plane( const double x, const double y, const double z, /*const*/ Plane & plane ){
  double xp, yp, zp;
  plane.GetPosition( xp, yp, zp );
  double xsize, ysize, zsize;
  plane.GetSize( xsize, ysize, zsize );

  // transform point to local rotation of plane
  SVector3 V;
  V(0) = x;  V(1) = y;  V(2) = z;
  V(2) = V(2) - zp;
  V = plane.GetInverseRotationMatrix() * V;

  // now check if point is inside rectangle of plane
  if ( V(0) > xp-xsize/2 && V(0) < xp+xsize/2 &&
       V(1) > yp-ysize/2 && V(1) < yp+ysize/2 &&
       V(2) > -zsize/2 && V(2) < zsize/2 ) {
    return true;
  }
  return false;
}

void Reconstruction::DoMC(std::vector<double> hitX, std::vector<double> hitY, std::vector<double> hitZ, std::vector<double> hitpX, std::vector<double> hitpY, std::vector<double> hitpZ){

  //vector<Cluster> clusters;
  clusters.clear();

    #ifdef DEBUG
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "DEBUG OUTPUT" << endl;
    cout << __func__ << " in " << __FILE__ << endl;
    #endif

    /// Step 1 is to find the clusters
    for(int i = 0; i < fPlanes.size(); i++) {

	double a = fPlanes.at(i).GetPlaneNormVector()(0);
	double b = fPlanes.at(i).GetPlaneNormVector()(1);
	double c = fPlanes.at(i).GetPlaneNormVector()(2);
	double xp, yp, zp;
	fPlanes.at(i).GetPosition(xp, yp, zp);
	double d = -a*xp - b*yp - c*zp;

		#ifdef DEBUG
	cout << "Plane " << i << endl;
	cout << "   Parameters: " << a << ", " << b << ", " << c << ", " << d << endl;
		#endif

	for(int j = 0; j < hitX.size(); j++) {
	    double dist = TMath::Abs(0.1*a*hitX.at(j) + 0.1*b*hitY.at(j) + 0.1*c*hitZ.at(j) + d)/sqrt(a*a + b*b + c*c);
	    //cout << j << " " << dist << " " << hitX.at(j) << " " << hitY.at(j) << " " << hitZ.at(j) << endl;
	    if(dist > 0.05) continue;

	    if ( ! point_in_plane( 0.1*hitX.at(j), 0.1*hitY.at(j), 0.1*hitZ.at(j), fPlanes.at(i) ) ) continue;  

	    double tx = hitpX.at(j)/ hitpZ.at(j);
	    double ty = hitpY.at(j)/ hitpZ.at(j);
	    double bx = 0.1*(hitX.at(j) - tx*hitZ.at(j));
	    double by = 0.1*(hitY.at(j) - ty*hitZ.at(j));
	    double z = -(d + a*bx + b*by)/(a*tx + b*ty + c);

	    double x1 = tx*z + bx;
	    double y1 = ty*z + by;
	    double x = 0;
	    double dx = 0;
	    double y = 0;
	    double dy = 0;

	    Cluster temp(x1, y1, z, dx, dy, 1, i);
	    fPlanes.at(i).FromGlobalToLocal(temp);


	    CalculateClusterPosition(temp);


	    clusters.push_back(temp);
	}


    }


    /// Step 2 is to do the reconstruction
    double par[5] = {0, 0, 0.0, 0.0, 1/7.};
    double cov[15] = {10,  0,  0,  0,  0,
	              10,  0,  0,  0,
	              1,  0,  0,
	              1,  0,
	              1};

	#ifdef DEBUG
    cout << "Start z = " << fPlanes.at(0).GetZPosition()-fPlanes.at(0).GetSizeZ() << " cm" << endl;
	#endif
    TrackPar trpar(eKisel, fPlanes.at(0).GetZPosition()-fPlanes.at(0).GetSizeZ(), par, cov);

    fExtrap->SetTrackPar(trpar);


    int clId = 0;
    for( int i = 0; i < fPlanes.size(); i++) {


	fExtrap->SetRadLength(30390);
	double a = fPlanes.at(i).GetPlaneNormVector()(0);
	double b = fPlanes.at(i).GetPlaneNormVector()(1);
	double c = fPlanes.at(i).GetPlaneNormVector()(2);
	double d = fPlanes.at(i).GetDPars().at(0);


	    #ifdef DEBUG
	cout << fPlanes.at(i).GetDPars().size() << endl;
	cout << "Extrapolating to plane: a = " << a << "," << " b = " << b << "," << " c = " << c << "," << " d = " << d << " cm" << endl;
	    #endif
	fExtrap->ExtrapolateToPlane(a, b, c, d);

	int planeInd = 1;
	for(unsigned int j = 0; j < fPlanes.at(i).GetLayers().size(); j++) {
	    fExtrap->SetRadLength(fPlanes.at(i).GetRadiationLengths().at(j));
	    d = fPlanes.at(i).GetDPars().at(planeInd);
	    if(j == fPlanes.at(i).GetActiveLayer()) {

		    #ifdef DEBUG
		cout << "Extrapolating to plane: a = " << a << "," << " b = " << b << "," << " c = " << c << "," << " d = " << d << " cm" << endl;
		    #endif
		fExtrap->ExtrapolateToPlane(a, b, c, d);
		planeInd++;
		d = fPlanes.at(i).GetDPars().at(planeInd);
		if(clusters.at(clId).GetPlaneId()==i) {
		    fExtrap->DoKalmanStep(clusters.at(clId), fPlanes.at(i));
		    clId++;
		}

		if(clId == clusters.size()) break;

	    }

		#ifdef DEBUG
	    cout << "Extrapolating to plane: a = " << a << "," << " b = " << b << "," << " c = " << c << "," << " d = " << d << " cm" << endl;
		#endif
	    fExtrap->ExtrapolateToPlane(a, b, c, d);
	    planeInd++;

	}
	if(clId == clusters.size()) break;
    }


    /// Step 3 is to output result at last plane to TTree
    double a = fPlanes.at(fPlanes.size()-1).GetPlaneNormVector()(0);
    double b = fPlanes.at(fPlanes.size()-1).GetPlaneNormVector()(1);
    double c = fPlanes.at(fPlanes.size()-1).GetPlaneNormVector()(2);
    double xp, yp, zp;
    fPlanes.at(fPlanes.size()-1).GetPosition(xp, yp, zp);
    double d = -a*xp - b*yp - c*zp;

    int last = hitX.size()-1;
    double pTrue = 0.001*sqrt(hitpX.at(last)*hitpX.at(last) + hitpY.at(last)*hitpY.at(last) + hitpZ.at(last)*hitpZ.at(last));

    //double pTrue = 0.001*hitP.at(last);
    TrackPar &trackPar = fExtrap->GetStopTrackParam();
    //trackPar.Print();

    double p = TMath::Abs(1/trackPar.GetPar(4));

    double tx = hitpX.at(last)/hitpZ.at(last);
    double ty = hitpY.at(last)/hitpZ.at(last);

    //double tx = hitpX.at(j)/ hitpZ.at(j);
    //double ty = hitpY.at(j)/ hitpZ.at(j);
    double bx = 0.1*(hitX.at(last) - tx*hitZ.at(last));
    double by = 0.1*(hitY.at(last) - ty*hitZ.at(last));
    double z = -(d + a*bx + b*by)/(a*tx + b*ty + c);
    //cout << z << " " << 0.1*hitZ.at(last) << endl;
    double x1 = tx*z + bx;
    double y1 = ty*z + by;
    //double dz = fPlanes.at(last).GetSizeZ()/2.;

    //double x0 = tx*dz + 0.1*hitX.at(last);
    //double y0 = ty*dz + 0.1*hitY.at(last);
    double sigmaP = trackPar.GetStd(4)/p/p;
    hdp->Fill((p-pTrue)/pTrue);
    hdtx->Fill((trackPar.GetPar(2)-tx));
    hdty->Fill((trackPar.GetPar(3)-ty));
    hdx->Fill((trackPar.GetPar(0)-x1));
    hdy->Fill((trackPar.GetPar(1)-y1));

    hdpRel->Fill((trackPar.GetPar(4)-1./pTrue)/trackPar.GetStd(4));
    hdtxRel->Fill((trackPar.GetPar(2)-tx)/trackPar.GetStd(2));
    hdtyRel->Fill((trackPar.GetPar(3)-ty)/trackPar.GetStd(3));
    hdxRel->Fill((trackPar.GetPar(0)-x1)/trackPar.GetStd(0));
    hdyRel->Fill((trackPar.GetPar(1)-y1)/trackPar.GetStd(1));



}

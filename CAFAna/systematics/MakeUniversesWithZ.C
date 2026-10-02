#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include "TRandom3.h"
#include "TMatrixD.h"
#include "TDecompChol.h"

using namespace std;

void MakeUniversesWithZ(std::string shifts, std::string errs, std::string num, std::string corr, std::string zshifts = "0"){

  // Read in files
  ifstream shiftsfile(shifts);
  ifstream errsfile(errs);

  if (!shiftsfile.is_open()) {
    std::cerr << "Error opening file: " << shifts << std::endl;
  }

  if (!errsfile.is_open()) {
    std::cerr << "Error opening file: " << errs << std::endl;
  }

  // Get run number for later
  size_t start = shifts.rfind('_') + 1;
  size_t end = shifts.rfind('.');
  std::string r = shifts.substr(start, end - start);

  // Get corr
  int correlation = std::stoi(corr);
  if (correlation != 0) num = "1";
  else if (std::abs(correlation) > 1) { std::cout<<"corr should be 0,-1, or 1"<<std::endl; exit(1); }

  // Using z-shifts
  int zs = std::stoi(zshifts);
  // if z = 0, no z shifts
  // if z = 1, z shifts + other shifts
  // if z = 2, z shifts only

  std::string line;
  int nshifts = 0;
  int index = 0;

  // Get the number of shifts nshifts
  std::getline(shiftsfile, line); // Ignore the first two lines of the text file
  std::getline(shiftsfile, line); // Ignore the first two lines of the text file

  while (getline(shiftsfile, line)){
    std::stringstream ss(line);

    std::string station, plane, sensor;
    double dx, dy, dz, dphi, detheta, dpsi;

    ss >> station >> plane >> sensor >> dx >> dy >> dz >> dphi >> detheta >> dpsi ;

    nshifts += (dx != 0);
    nshifts += (dy != 0);
    nshifts += (dz != 0);
    nshifts += (dphi != 0);
    nshifts += (detheta != 0);
    nshifts += (dpsi != 0);
  }

  shiftsfile.clear();
  shiftsfile.seekg(0);

  // Construct mean vector meanVec
  TVectorD meanVec(nshifts);
  int c=0;

  std::getline(shiftsfile, line); // Ignore the first two lines of the text file
  std::getline(shiftsfile, line); // Ignore the first two lines of the text file

  while (getline(shiftsfile, line)){
    std::stringstream ss(line);

    std::string station, plane, sensor;
    double dx, dy, dz, dphi, detheta, dpsi;

    ss >> station >> plane >> sensor >> dx >> dy >> dz >> dphi >> detheta >> dpsi ;

    if (dx != 0) {meanVec(c) = dx; c++; }
    if (dy != 0) {meanVec(c) = dy; c++; }
    if (dz != 0) {meanVec(c) = dz; c++; }
    if (dphi != 0) {meanVec(c) = dphi; c++; }
    if (detheta != 0) {meanVec(c) = detheta; c++; }
    if (dpsi != 0) {meanVec(c) = dpsi; c++; }
  }

  shiftsfile.clear();
  shiftsfile.seekg(0);

  // Construct covariance matrix covMatrix
  TMatrixD covMatrix(nshifts,nshifts);
  int ce = 0;

  std::getline(errsfile, line); // Ignore the first two lines of the text file
  std::getline(errsfile, line); // Ignore the first two lines of the text file

  while (getline(errsfile, line)){
    std::stringstream ss(line);

    std::string station, plane, sensor;
    double edx, edy, edz, edphi, edetheta, edpsi;

    ss >> station >> plane >> sensor >> edx >> edy >> edz >> edphi >> edetheta >> edpsi ;

    if (edx != 0) {covMatrix(ce,ce) = edx*edx; ce++; }
    if (edy != 0) {covMatrix(ce,ce) = edy*edy; ce++; }
    if (edz != 0) {covMatrix(ce,ce) = edz*edz; ce++; }
    if (edphi != 0) {covMatrix(ce,ce) = edphi*edphi; ce++; }
    if (edetheta != 0) {covMatrix(ce,ce) = edetheta*edetheta; ce++; }
    if (edpsi != 0) {covMatrix(ce,ce) = edpsi*edpsi; ce++; }
  }
  for (size_t i=0; i<nshifts; i++){
    for (size_t j=0; j<nshifts; j++){
      if (i != j) covMatrix(i,j) = 0;
    }
  }

  errsfile.close();

  // GaussianND unavailable so do Cholesky decomposition 
  TDecompChol chol(covMatrix);
  chol.Decompose();

  TMatrixD U = chol.GetU();

  for (int univ=0; univ<std::stoi(num); univ++){

    TRandom3 rng(0); 

    TVectorD z(nshifts);
    for (int i=0; i<nshifts; i++){
      if (correlation == 0) z[i] = rng.Gaus(0,1);      // Universes created by random function
      else if (correlation == -1) z[i] = -1;           // fully correlated -1 sigma
      else if (correlation == 1) z[i] = 1;             // fully correlated +1 sigma
      else{ std::cout<<"corr should be 0,-1, or 1"<<std::endl; exit(1); }
    }  
  
    TVectorD x = meanVec + U * z;

    // Loop through nshifts
    int it=0;
    std::string filename;
    if (correlation == 0) filename = Form("SSDAlign_1c_%s_u%i.txt",r.c_str(),univ);
    else if (correlation == -1) filename = Form("SSDAlign_1c_%s_FullyCorrelatedDown.txt",r.c_str());
    else if (correlation == 1) filename = Form("SSDAlign_1c_%s_FullyCorrelatedUp.txt",r.c_str());
    std::ofstream file(filename);

    double prev_dz = 0.;
    std::string prev_pl = "";

    std::getline(shiftsfile, line);
    file << line << std::endl;
    std::getline(shiftsfile, line);
    file << line << std::endl;
    while (getline(shiftsfile, line)){
      std::stringstream ss(line);

      std::string station, plane, sensor;
      double dx, dy, dz, dphi, detheta, dpsi;

      ss >> station >> plane >> sensor >> dx >> dy >> dz >> dphi >> detheta >> dpsi ;

      if (zs == 0){
	if (correlation == 0){
          if (dx != 0) { dx = x[it]; it++; }
          if (dy != 0) { dy = x[it]; it++; }
          if (dz != 0) { dz = x[it]; it++; }
          if (dphi != 0) { dphi = x[it]; it++; }
          if (detheta != 0) { detheta = x[it]; it++; }
          if (dpsi != 0) { dpsi = x[it]; it++; }
	}
	else{
          int flip = correlation;
          if (std::stoi(station) % 2 == 0) flip = correlation;
          else flip = -1*correlation;

          if (dx != 0) { dx = flip*x[it]; it++; }
          if (dy != 0) { dy = flip*x[it]; it++; }
          if (dz != 0) { dz = flip*x[it]; it++; }
          if (dphi != 0) { dphi = flip*x[it]; it++; }
          if (detheta != 0) { detheta = flip*x[it]; it++; }
          if (dpsi != 0) { dpsi = flip*x[it]; it++; }

          std::cout<<"flip = "<<flip<<" and station is "<<station<<std::endl;
        }
      }
      if (zs == 1){
        if (dx != 0) { dx = x[it]; it++; }
        if (dy != 0) { dy = x[it]; it++; }
        if (dz != 0) { dz = x[it]; it++; }
        else { dz = rng.Gaus(0,0.2); }
        if (dphi != 0) { dphi = x[it]; it++; }
        if (detheta != 0) { detheta = x[it]; it++; }
        if (dpsi != 0) { dpsi = x[it]; it++; }
      }
      if (zs == 2){
	if (plane == "0" || plane == "2"){
          if (prev_pl == plane) dz = prev_dz;
          else dz = rng.Gaus(0,0.2);
          prev_dz = dz;
	  prev_pl = plane;
	}
        if (plane == "1"){
          dz = prev_dz;
	  prev_pl = plane;
	}
      }
/*
      if (correlation == 1 || correlation == -1){
          int flip = correlation;
          if (std::stoi(station) % 2 == 0) flip = correlation;
          else flip = -1*correlation;

          if (dx != 0) { dx = flip*x[it]; it++; }
          if (dy != 0) { dy = flip*x[it]; it++; }
          if (dz != 0) { dz = flip*x[it]; it++; }
          if (dphi != 0) { dphi = flip*x[it]; it++; }
          if (detheta != 0) { detheta = flip*x[it]; it++; }
          if (dpsi != 0) { dpsi = flip*x[it]; it++; }

          std::cout<<"flip = "<<flip<<" and station is "<<station<<std::endl;
      }
	//std::cout<<station<<","<<plane<<","<<sensor<<" = "<<dz<<std::endl;
*/
      file << std::left
           << std::setw(15) << station
           << std::setw(15) << plane
           << std::setw(15) << sensor
           << std::setprecision(8) << std::setw(15) << dx
           << std::setprecision(8) << std::setw(15) << dy
           << std::setprecision(8) << std::setw(15) << dz
           << std::setprecision(8) << std::setw(15) << dphi
           << std::setprecision(8) << std::setw(15) << detheta
           << std::setprecision(8) << std::setw(15) << dpsi
           << '\n';
    }
    file.close();
    shiftsfile.clear();
    shiftsfile.seekg(0); 
  }

  shiftsfile.close();
}
int main(int argc, const char* argv[]){
  if (argc < 5) return 1;

  if (argc >= 6) {
    MakeUniversesWithZ(argv[1], argv[2], argv[3], argv[4], argv[5]);
  }
  else {
    MakeUniversesWithZ(argv[1], argv[2], argv[3], argv[4]);
  }
//  MakeUniversesWithZ(argv[1],argv[2],argv[3],argv[4],argc >= 6 ? argv[5] : nullptr);
  return 0;
}

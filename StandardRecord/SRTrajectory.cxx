////////////////////////////////////////////////////////////////////////
/// \file  SRTrajectory.cxx
/// \brief Container of trajectory information for a particle
////////////////////////////////////////////////////////////////////////

//#include "cetlib_except/exception.h"
#include "StandardRecord/SRTrajectory.h"

#include <TLorentzVector.h>
#include <TVector3.h>
#include <cmath>
#include <deque>
#include <iterator>
#include <vector>
#include <set>
#include <map>

namespace caf {

  // Nothing special need be done for the default constructor or destructor.
  SRTrajectory::SRTrajectory() 
    : fPos(), fMom()
  {}

  //----------------------------------------------------------------------------
  SRTrajectory::SRTrajectory( const TLorentzVector& position, 
			      const TLorentzVector& momentum )
  {
    fPos.push_back(position);
    fMom.push_back(momentum);
  }

  //----------------------------------------------------------------------------
  const TLorentzVector& SRTrajectory::Position( const size_t index ) const
  {
    auto i = fPos.begin();
    std::advance(i,index);
    return (*i);
  }
  
  //----------------------------------------------------------------------------
  const TLorentzVector& SRTrajectory::Momentum( const size_t index ) const
  {
    auto i = fMom.begin();
    std::advance(i,index);
    return (*i);
  }

  //----------------------------------------------------------------------------
  double SRTrajectory::TotalLength() const
  {
    const int N = size();
    if(N < 2) return 0;

    // We take the sum of the straight lines between the trajectory points
    double dist = 0;
    for(int n = 0; n < N-1; ++n){
      dist += (fPos[n+1]-fPos[n]).Vect().Mag();
    }

    return dist;
  }

  //----------------------------------------------------------------------------
  std::ostream& operator<< ( std::ostream& output, const SRTrajectory& list )
  {
    // Determine a field width for the voxel number.
    size_t numberOfTrajectories = list.size();
    int numberOfDigits = (int) std::log10( (double) numberOfTrajectories ) + 1;

    // A simple header.
    output.width( numberOfDigits );
    output << "#" << ": < position (x,y,z,t), momentum (Px,Py,Pz,E) >" << std::endl; 
    
    // Write each trajectory point on a separate line.
    size_t nTrajectory = 0;
    for ( size_t i=0; i<list.fPos.size(); ++i,++nTrajectory) {      
      const TLorentzVector& pos = list.fPos[i];
      const TLorentzVector& mom = list.fMom[i];
      output.width( numberOfDigits );
      output << nTrajectory << ": "
	     << "< (" << pos.X() 
	     << "," << pos.Y() 
	     << "," << pos.Z() 
	     << "," << pos.T() 
	     << ") , (" << mom.Px() 
	     << "," << mom.Py() 
	     << "," << mom.Pz() 
	     << "," << mom.E() 
	     << ") >" << std::endl;
    }
    
    return output;
  }

  //----------------------------------------------------------------------------
  unsigned char SRTrajectory::ProcessToKey(std::string const& process) const
  {
    unsigned char key = 0;
    
    if     (process.compare("hadElastic")       == 0) key = 1;
    else if(process.compare("pi-Inelastic")     == 0) key = 2;
    else if(process.compare("pi+Inelastic")     == 0) key = 3;
    else if(process.compare("kaon-Inelastic")   == 0) key = 4;
    else if(process.compare("kaon+Inelastic")   == 0) key = 5;
    else if(process.compare("protonInelastic")  == 0) key = 6;
    else if(process.compare("neutronInelastic") == 0) key = 7;
    else if(process.compare("CoulombScat")      == 0) key = 8;
    else if(process.compare("nCapture")         == 0) key = 9;
    else if(process.compare("Transportation") == 0)
      key = 10;
    
    return key;
  }
  
  //----------------------------------------------------------------------------
  std::string SRTrajectory::KeyToProcess(unsigned char const& key) const
  {
    std::string process("Unknown");
    
    if     (key == 1) process = "hadElastic";
    else if(key == 2) process = "pi-Inelastic";
    else if(key == 3) process = "pi+Inelastic";
    else if(key == 4) process = "kaon-Inelastic";
    else if(key == 5) process = "kaon+Inelastic";
    else if(key == 6) process = "protonInelastic";
    else if(key == 7) process = "neutronInelastic";
    else if(key == 8) process = "CoulombScat";
    else if(key == 9) process = "nCapture";
    else if(key == 10) process = "Transportation";
    
    return process;
  }
  
  //----------------------------------------------------------------------------
  void SRTrajectory::Add(TLorentzVector const& p,
                         TLorentzVector const& m,
                         std::string    const& process,
                         bool keepTransportation)
  {
    // add the the momentum and position, then get the location of the added
    // bits to store the process
    fPos.push_back(p);
    fMom.push_back(m);
    
    size_t insertLoc = fPos.size() - 1;
    
    auto key = this->ProcessToKey(process);
    
    // only add a process to the list if it is defined, ie one of the values
    // allowed in the ProcessToKey() method
    //
    // Also, keep 10 (transportation) if the flag allows
    if(key > 0 && (key != 10 || keepTransportation))
      fTrajectoryProcess.push_back(std::make_pair(insertLoc, key));
    
    return;
  }


} // namespace sim

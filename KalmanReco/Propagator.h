#ifndef KPROPAGATOR_H
#define KPROPAGATOR_H

#include <iostream>
#include <string>

#include "KState.h"

using namespace std;

namespace kalman {

  class Propagator{
    
  public:
    Propagator();
    ~Propagator() {};

    State Extrapolate(State sstart, double z, double B[3]);

    static double GetKappa(){return fKappa;}
    void AddNoise(State &tstate, double ds, double X, double radLength);
    
  private:
    
    static double fKappa;    
    
  };
}

#endif

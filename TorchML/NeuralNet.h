////////////////////////////////////////////////////////////////////////
/////// \brief   Class that handles torch NN and relative functions
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////

#ifndef NN_INCLUDE
#define NN_INCLUDE

#include "stdlib.h"
#include <iostream>
#include <chrono>
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>
#include <string>
#include <ATen/ATen.h> 

namespace tml {

  class NeuralNet {
  public:
    NeuralNet();
    ~NeuralNet();

    void loadModel(const std::string& path);

    at::Tensor predict(std::vector<at::Tensor> inputs); 

  private:
    struct Impl; // forward-declare Torch implementation
    Impl* pimpl;
  };

}

#endif

#include "NeuralNet.h"


#ifndef __CINT__   // hide from ROOT's dictionary parser
#include <torch/torch.h>
#include <torch/script.h>
#endif

/*
#ifdef ClassDef
  #pragma push_macro("ClassDef")
  #undef ClassDef
#endif

#include <torch/torch.h>
#include <torch/script.h>

#ifdef ClassDef
  #pragma pop_macro("ClassDef")
#endif
*/
#include <iostream>


namespace torchml{

struct NeuralNet::Impl {
    torch::jit::script::Module module;
    torch::Device device{torch::kCPU};
};

NeuralNet::NeuralNet() : pimpl(new Impl()) {  
	
	pimpl->device = torch::kCPU;
	
}


NeuralNet::~NeuralNet(){ delete pimpl;}

void NeuralNet::loadModel(const std::string& path) {

    pimpl->module = torch::jit::load(path);

    pimpl->module.to(pimpl->device);
}

at::Tensor NeuralNet::predict(std::vector<at::Tensor> inputs) {

    torch::Tensor input1 = torch::Tensor(inputs[0]).to(pimpl->device);
    torch::Tensor input2 = torch::Tensor(inputs[1]).to(pimpl->device);
	
//   mf::LogError("input 1 shape ") <<"shape1  " <<  input1.sizes() << " dtype " << input1.dtype() << std::endl;
//   mf::LogError("input 2 shape ") <<"shape2  " <<  input2.sizes() << " dtype " << input2.dtype() << std::endl;


    return at::Tensor(pimpl->module.forward({input1,input2}).toTensor());

}


}

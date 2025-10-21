#[================================================================[.rst:
Findlibtorch
------------
  Find PyTorch C++ library (libtorch) using official TorchConfig.cmake

  This module is a wrapper around the official PyTorch CMake config.
  It uses TorchConfig.cmake provided by the libtorch package.

  Environment variables used:
    TORCH_ROOT - Root directory of PyTorch installation
    TORCH_INSTALL_PREFIX - Alternative to TORCH_ROOT

#]================================================================]

include_guard()

# Add TORCH_ROOT to CMAKE_PREFIX_PATH if available
if(NOT DEFINED TORCH_ROOT)
  if(DEFINED ENV{TORCH_ROOT})
    set(TORCH_ROOT "$ENV{TORCH_ROOT}")
  elseif(DEFINED ENV{TORCH_INSTALL_PREFIX})
    set(TORCH_ROOT "$ENV{TORCH_INSTALL_PREFIX}")
  endif()
endif()

if(TORCH_ROOT)
  list(APPEND CMAKE_PREFIX_PATH "${TORCH_ROOT}")
endif()

# Use the official TorchConfig.cmake
find_package(Torch REQUIRED)

# Set libtorch variables for compatibility
set(libtorch_FOUND ${Torch_FOUND})
set(libtorch_INCLUDE_DIRS ${TORCH_INCLUDE_DIRS})
set(libtorch_LIBRARIES ${TORCH_LIBRARIES})
set(libtorch_CXX_FLAGS ${TORCH_CXX_FLAGS})
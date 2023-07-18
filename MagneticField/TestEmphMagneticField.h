///////////////////////////////////////////////////////////////////////////////
// File: TestEmphMagneticField.h
// Description: A class for Testing the access to the magnetic field. 
// Could be invoked from the art service  
///////////////////////////////////////////////////////////////////////////////
#ifndef TestEmphMagneticField_H
#define TestEmphMagneticField_H

#include <iostream>
#include <sstream>
#include <vector>
#include "MagneticField/MagneticField.h"

namespace emph {
 
    class TestEmphMagneticField {
      
      public:
      
        explicit TestEmphMagneticField(EMPHATICMagneticField* theField);
	
        void test1(); // Check that divB ~ 0.;  
        void test2(); // test integration, study expected deflections.  
        void test3(); // test calculation of preliminary acceptance sensitivity of beam axis and/or SSD Yaw uncertainty.  
	
      private:
        EMPHATICMagneticField* fBField;
	
    };

} // namespace emph 

#endif //TestEmphMagneticField_H

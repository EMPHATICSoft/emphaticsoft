/*  L++ header file - put it at the top of main.cpp
 *  Created by Leo Bellantoni on 12/7/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  All these #includes increase the output of the preprocessor but not the
 *  size of the executable.
 *
 */
 // The Apple Accelerate framework is a lovely thing.
 #include <Accelerate/Accelerate.h>
 
// libxml2 is... a thing.
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

// Les Houches Accord parton distribution functions, ya. A thing.
#include "LHAPDF/LHAPDF.h"
using namespace LHAPDF;



// The individual L++ classes have the includes that they need and those are
// indirectly included in main.cpp via the L++ class header files.
using std::vector;

 
 
// There are a few holes in C++
#include "concatVectors.h"
#include "concatDeques.h"
#include "unorderedUnique.h"
#include "Ldelete.h"
#include "singleton.h"

#include "LppGlobalDefs.h"

// N.B.  <complex> is not known to avoid over/underflows in fabs(),
// pow() or division.  Use the L++ quadd if need be.  See also the macro
// 'cmplx' defined in ra.h, which will become obsolete in a future C++
// standard.  In principle, C++ should handle 'abs' for all of the cases
// formerly handled by abs(integer types), fabs(real types) and
// cabs(complex types) in C.  There are reports of that not actually working
// in all implementations though?
// Comments here only show first appearance of each included library
#include "globbing.h"
#include "filehamna.h"
#include "talk2me.h"
#include "cr2continue.h"
#include "stringify.h"
#include "plusORminus.h"
#include "inputtextfile.h"

#include "date.h"
#include "thyme.h"
#include "seiko.h"

#include "ra.h"				// includes <limits>, <valarray> and <complex>
#include "Lvec.h"
#include "Lmat.h"

#include "intsNabs.h"
#include "minmax.h"
#include "xp.h"
#include "unReal.h"
#include "quadd.h"
#include "permuter.h"
#include "polyQint.h"
#include "polyMint.h"
#include "Bessel.h"

#include "plotter1d.h"
#include "plotter2d.h"
#include "integrator.h"
#include "solver.h"
#include "fitter.h"
#include "SFT.h"
#include "VEGAS.h"

#include "randoms.h"

#include "stats.h"
#include "fullim.h"
#include "poiprob.h"
#include "meansig.h"
#include "meancorrel.h"
#include "BLUE.h"

#include "Lvec4.h"
#include "RAMBO.h"
#include "Lpdf.h"

//#include "Ltuple.h"
#include "Lcount.h"
#include "Lhistbin.h"
#include "Lprofbin.h"
#include "Lbins.h"
#include "Lhist1.h"
#include "Lhist2.h"
#include "Lprof1.h"
#include "Lprof2.h"
#include "Ldata.h"

#include "plotbase.h"
#include "plotfit.h"
#include "Lfit1.h"
#include "Lzcol.h"
#include "Lscatter.h"
#include "Lfield.h"

#include "xmlSteppers.h"

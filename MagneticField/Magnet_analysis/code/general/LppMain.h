/*  L++ header file - put it at the top of main.cpp
 *  Created by Leo Bellantoni on 12/7/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  All these #includes increase the output of the preprocessor but not the
 *  size of the executable.
 *
 */




 
// There are a few holes in C++
#include "../C--/concatVectors.h"
#include "../C--/concatDeques.h"
#include "../C--/unorderedUnique.h"
#include "../C--/Ldelete.h"
#include "../C--/singleton.h"
#include "../C--/ThreadHandler.h"

#include "LppGlobalDefs.h"

// N.B.  <complex> is not known to avoid over/underflows in fabs(),
// pow() or division.  Use the L++ quadd if need be.  See also the macro
// 'cmplx' defined in ra.h, which will become obsolete in a future C++
// standard.  In principle, C++ should handle 'abs' for all of the cases
// formerly handled by abs(integer types), fabs(real types) and
// cabs(complex types) in C.  There are reports of that not actually working
// in all implementations though?
// Comments here only show first appearance of each included library
#include "../io/globbing.h"
#include "../io/filehamna.h"
#include "../io/talk2me.h"
#include "../io/cr2continue.h"
#include "../io/stringify.h"
#include "../io/plusORminus.h"
#include "../io/inputtextfile.h"

#include "../when/date.h"
#include "../when/thyme.h"
#include "../when/seiko.h"

#include "../ra/ra.h"
#include "../ra/Lvec.h"
#include "../ra/Lmat.h"

#include "../functions/intsNabs.h"
#include "../functions/minmax.h"
#include "../functions/xp.h"
#include "../functions/unReal.h"
#include "../functions/quadd.h"
#include "../functions/permuter.h"
#include "../functions/polyQint.h"
#include "../functions/polyMint.h"
#include "../functions/Bessel.h"

#include "../functionals/plotter1d.h"
#include "../functionals/plotter2d.h"
#include "../functionals/integrator.h"
#include "../functionals/solver.h"
#include "../functionals/fitter.h"
#include "../functionals/SFT.h"
#include "../functionals/VEGAS.h"

#include "../randoms/randoms.h"

#include "../lies/stats.h"
#include "../lies/fullim.h"
#include "../lies/poiprob.h"
#include "../lies/meansig.h"
#include "../lies/meancorrel.h"
#include "../lies/BLUE.h"

#include "../particles/Lvec4.h"
#include "../particles/RAMBO.h"

#include "../histograms/Lcount.h"
#include "../histograms/Lhistbin.h"
#include "../histograms/Lprofbin.h"
#include "../histograms/Lbins.h"
#include "../histograms/Lhist1.h"
#include "../histograms/Lhist2.h"
#include "../histograms/Lprof1.h"
#include "../histograms/Lprof2.h"
#include "../histograms/Ldata.h"

#include "../GUI/plotbase.h"
#include "../GUI/plotfit.h"
#include "../GUI/Lfit1.h"
#include "../GUI/Lzcol.h"
#include "../GUI/Lscatter.h"
#include "../GUI/Lfield.h"

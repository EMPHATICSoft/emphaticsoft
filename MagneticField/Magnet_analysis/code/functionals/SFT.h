/*  L++ SFT
 *  Created by Leo Bellantoni on 29 Dec 2012.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  A slow Fourier transform; to check that your fast one is right, eh?
 *  And also to transform series of length not 2^(some power).  But the
 *  length of the series must be even!
 *  The transform is defined as sum from k=0 to N-1
 *  of data(k) exp(2 pi i k n/N), where n indexes the transformed series.
 *  Obviously, n=0 corresponds to the DC term and negative (postive) n
 *  to negative (positive) frequency components.  The transformed values
 *  for n = -N/2 or f = -1/(2dt) and n = +N/2 or f = +1/(2dt) are equal.
 *  So the natural layout of the data is
 *
 *    Structure of INPUT array:		Structure of OUTPUT array:
 *	  Time domain @ t = 0           Freq. domain @ f = +-1/(2dt)
 *	  Time domain @ t = dt          Freq. domain @ f = -(N/2 - 1)/(Ndt)
 *	  Time domain @ t = 2dt         Freq. domain @ f = -(N/2 - 2)/(Ndt)
 *	  .                             .
 *	  .                             .
 *	  .                             Freq. domain @ f = -1/(Ndt)
 *    .                             Freq. domain @ f = 0
 *	  .                             Freq. domain @ f = +1/(Ndt)
 *	  .                             .
 *	  .                             .
 *	  Time domain @ t = (N-1)dt     Freq. domain @ f = +(N/2 - 1)/(Ndt)
 *
 *  with the output indexed from n = -N/2 to N/2 - 1; this code forces
 *  that indexing, even if you input an ra<cmplx> with some other offset.
 *
 *  For the inverse transform, the sum is from k = -N/2 to N/2-1 of
 *  (1/N) data(n) exp(-2 pi i k n /N).
 *
 *  The easiest way to determine the scaling of the output is to use the
 *  aliasing criteria Fmax = 1/(2*dt); so if there is one sample per
 *  second, the output will go from -0.5 to +0.5 Hz.  Then df = Fmax/(Nin/2)
 *  since there are both positive and negative frequencies and therefore
 *  only half the Nin points in frequency space are positive.
 
 *  Magnitude of transform is N times the coefficient at that point; i.e.
 *  a transform of 32 points that lie in fact along Acos(wt) +B will have
 *  32B for the DC component, 16A for the positive frequency corresponding
 *  to w and another 16A at the negative frequency - exactly, if the frequency
 *  is a multiple of df, approximately otherwise.
 *
 *  For the one-sided power distribution, the power method will sum the
 *  magnitude of the negative frequency number and the magnitude of the
 *  positive frequency number,i.e. there is no interference cross-term.
 *  To get then the power, it divides by N (magnitude of transform is N times
 *  the coefficient) and then squares it.
 *
 */
#pragma once



#include "ra.h"



class SFT{
public:
    // inSign +1 for transform, -1 for reverse transform
    explicit SFT(int inSign);
    // Default, copy and assignment only - no heap storage allocated
    SFT();
    SFT(SFT const& inSFT);
    SFT& operator=(SFT const& rhs);

    // Can't do this transform in-place, may as well return another ra.
    ra<cmplx> transform(ra<cmplx> data);
    // Call power with the transformed data.  Returned value will
    // have N/2 + 1 entries, indexed from 0 to N/2.
    ra<double> power(ra<cmplx> atad);

private:
    int Sign;
};

//
//  ErrorCodeList.h
//  L++
//
//  Created by Leo Bellantoni on 10/20/23.
//
#pragma once



    // Numbering convention is: < 100, syntactic jibberish; 100-199, mathematical
    // jibberish; 200-299, physically jibberish, 300-399 histogram/GUI related,
    // 400-499 runtime machine faults & other things  More or less.
    LppErrTbl[  0] = (table_entry){0,Fatal,  "Invalid input arguments"};
    LppErrTbl[  1] = (table_entry){0,Fatal,  "Divide by zero"};
    LppErrTbl[  2] = (table_entry){0,Warning,"Overflow!"};
    LppErrTbl[  3] = (table_entry){0,Warning,"Underflow!"};
    LppErrTbl[  4] = (table_entry){0,Fatal,  "ra<T> incorrectly indexed"};
    LppErrTbl[  5] = (table_entry){0,Fatal,  "ra<T> index out of bounds"};
    LppErrTbl[  6] = (table_entry){0,Fatal,  "Size of covariances and measurements differs"};
    LppErrTbl[  7] = (table_entry){0,Fatal,  "Wrong number of indices in ra"};
    LppErrTbl[  8] = (table_entry){0,Fatal,  "Lmat must be square"};
    LppErrTbl[  9] = (table_entry){0,Fatal,  "Binary op with wrong-sized matrices or vectors"};
    LppErrTbl[ 10] = (table_entry){0,Fatal,  "Invalid date or time construction or math"};
    LppErrTbl[ 11] = (table_entry){0,Fatal,  "Bin boundaries not in increasing order"};
    LppErrTbl[ 12] = (table_entry){0,Fatal,  "Invalid bin accessed"};
    LppErrTbl[ 13] = (table_entry){0,Fatal,  "Attempt to overwrite existing file"};
    LppErrTbl[ 14] = (table_entry){0,Fatal,  "Problem writing file"};
    LppErrTbl[ 15] = (table_entry){0,Fatal,  "Problem opening file"};
    LppErrTbl[ 16] = (table_entry){0,Fatal,  "Invalid dimension index"};
    LppErrTbl[ 17] = (table_entry){0,Fatal,  "File does not exist"};
    LppErrTbl[ 18] = (table_entry){0,Fatal,  "Failure in sscanf"};
    LppErrTbl[ 19] = (table_entry){0,Inform, "Not a regular file"};
    LppErrTbl[ 20] = (table_entry){0,Inform, "Failbit in opening file; retrying"};
    LppErrTbl[ 21] = (table_entry){0,Fatal,  "Problem reading file"};
    LppErrTbl[ 22] = (table_entry){0,Warning,"operator= shallow-copies a pointer to a file or stream"};
    
    LppErrTbl[100] = (table_entry){0,Fatal,  "Degenerate matrix"};
    LppErrTbl[101] = (table_entry){0,Fatal,  "LAPACK routine ZGEEVX barfed"};
    LppErrTbl[102] = (table_entry){0,Warning,"Increase NiterMax for better results"};
    LppErrTbl[103] = (table_entry){0,Fatal,  "Overflow in ra size"};
    LppErrTbl[104] = (table_entry){0,Warning,"Unwise parameters, N*p or N*(1-p) < 1%"};
    LppErrTbl[105] = (table_entry){0,Fatal,  "Covariance requires prior minimization"};
    LppErrTbl[106] = (table_entry){0,Fatal,  "Improvement requires prior covariance"};
    LppErrTbl[107] = (table_entry){0,Fatal,  "Operation on differently binned histograms"};
    LppErrTbl[108] = (table_entry){0,Warning,"Divide by zero in histogram - result set to 0"};
    LppErrTbl[109] = (table_entry){0,Warning,"Nent too small in profile - result set to 0"};
    LppErrTbl[110] = (table_entry){0,Warning,"SDev undefined in profile - result set to 0"};
    LppErrTbl[111] = (table_entry){0,Fatal,  "SFT needs even number of data points"};
    LppErrTbl[112] = (table_entry){0,Fatal,  "Not enough data pushed yet onto meansig"};
    LppErrTbl[113] = (table_entry){0,Fatal,  "Wrongly dimensioned initial volume"};
    LppErrTbl[114] = (table_entry){0,Fatal,  "Can't permute just one index"};
    LppErrTbl[115] = (table_entry){0,Warning,"Tried to permute too many times - permuter reset"};
    LppErrTbl[116] = (table_entry){0,Fatal,  "Wrongly dimensioned polynomials"};
    LppErrTbl[117] = (table_entry){0,Fatal,  "Duplicate X points in polynomial input"};
    LppErrTbl[118] = (table_entry){0,Fatal,  "Wrong stuff to make an unReal"};
    LppErrTbl[119] = (table_entry){0,Fatal,  "Abs(correlation coefficent) > 1"};
    LppErrTbl[120] = (table_entry){0,Fatal,  "Wrongly dimensioned argument"};
    LppErrTbl[121] = (table_entry){0,Fatal,  "Define grid 1st, function values 2nd"};
    LppErrTbl[122] = (table_entry){0,Fatal,  "Grid dimensions != function values dimension"};
    LppErrTbl[123] = (table_entry){0,Warning,"Improvement went out of bounds"};
    LppErrTbl[124] = (table_entry){0,Warning,"Improvement failed - probably nonquadratic chi^2"};
    LppErrTbl[125] = (table_entry){0,Fatal,  "Negative number of degrees of freedom"};

    LppErrTbl[200] = (table_entry){0,Fatal,  "Excessive weights generated"};
    LppErrTbl[201] = (table_entry){0,Warning,"Many iterations, no convergence"};
    LppErrTbl[202] = (table_entry){0,Fatal,  "Q-squared not set"};
    LppErrTbl[203] = (table_entry){0,Warning,"Illegitimate kinematics"};
    LppErrTbl[204] = (table_entry){0,Fatal,  "Illegitimate kinematics"};
    LppErrTbl[205] = (table_entry){0,Fatal,  "Chi2 is flat!"};
    LppErrTbl[206] = (table_entry){0,Fatal,  "Function constant with respect to a parameter"};
    LppErrTbl[207] = (table_entry){0,Inform, "Small step size in gradient calculation"};
    LppErrTbl[208] = (table_entry){0,Warning,"Many scans in gradient search; minimization stopped"};
    LppErrTbl[209] = (table_entry){0,Warning,"Many steps in scan; consider increasing stepFactor"};
    LppErrTbl[210] = (table_entry){0,Fatal,  "Can not find good step size; might be at parameter bounds"};
    LppErrTbl[211] = (table_entry){0,Fatal,  "hesse & improve called >= 10 times"};
    LppErrTbl[212] = (table_entry){0,Warning,"Cannot combine only 1 value"};
    LppErrTbl[213] = (table_entry){0,Warning,"Pull plot invalid with correlated measurements"};
    LppErrTbl[214] = (table_entry){0,Warning,"PDG scale factor invalid with correlated measurements"};
    LppErrTbl[215] = (table_entry){0,Warning,"PDG scale factor undefined with 1 useable measurement"};
    LppErrTbl[216] = (table_entry){0,Warning,"Peelle's Problem found - negative weights in average"};
    LppErrTbl[217] = (table_entry){0,Fatal,  "Covariance matrix not symmetric"};
    LppErrTbl[218] = (table_entry){0,Fatal,  "Stepsize large enough to go out of bounds"};
    LppErrTbl[219] = (table_entry){0,Warning,"1 sigma in fit parameters large enough to be out of bounds"};

    LppErrTbl[300] = (table_entry){0,Fatal,  "Nbins does not match data/M.C."};
    LppErrTbl[301] = (table_entry){0,Fatal,  "fitter not correctly initialized"};
    LppErrTbl[302] = (table_entry){0,Fatal,  "Can have fit or M.C. histo - but not both!"};
    LppErrTbl[303] = (table_entry){0,Warning,"Autoscaling gives negative range for log plot"};
    LppErrTbl[304] = (table_entry){0,Fatal,  "Could not open pipe to gnuplot"};
    LppErrTbl[305] = (table_entry){0,Fatal,  "Nonsense exclusion bound"};
    LppErrTbl[306] = (table_entry){0,Fatal,  "Profile / Histo confusion"};
    LppErrTbl[307] = (table_entry){0,Warning,"Data points with zero uncertainty"};
    LppErrTbl[308] = (table_entry){0,Fatal,  "Fit to Black with Curve, or Red/Blue on y2"};
    LppErrTbl[309] = (table_entry){0,Fatal,  "Wrong tag in Lcount"};
    LppErrTbl[310] = (table_entry){0,Fatal,  "Attempt to reset nonexistent variable"};
    LppErrTbl[311] = (table_entry){0,Warning,"fitter can't improve fit; too close to a boundary"};
    LppErrTbl[312] = (table_entry){0,Fatal,  "blankEmpties only works with Lhists not Lprofs"};
    LppErrTbl[400] = (table_entry){0,Fatal,  "malloc failed"};

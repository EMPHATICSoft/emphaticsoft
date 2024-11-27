/*  L++ RA3
 *  Created by Leo Bellantoni on 29 Sep 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  A simplified version of ra with only arrays of size 3, 3x3 or 3x3x3 but
 *  but without the dynamic memory allocation / freeing that slows the
 *  full-functionality template down.  Maintain indexing from 1 by ignoring
 *  index values of 0.
 *
 */
#ifndef Lpp_RAfast_H
#define Lpp_RAfast_H

#include "../ra/ra.h"
#include "../general/LppGlobals.h"





// =============================================================================
// 1-D version here ============================================================
// =============================================================================

template <class T> class RA_1 {
public:
    RA_1() {};      // Compiler wants default constructor written out here i dunno why

	// Copy, assignment constructors
	RA_1(RA_1<T> const& rhs) {comcopy(rhs);};
	RA_1<T>& operator= (RA_1<T> const& rhs) {
		if (this != &rhs) comcopy(rhs);
		return *this;
	}
	RA_1<T>& operator= (T rhs) {
        for (addr i=0; i<=3; ++i) val[i] = rhs;
		return *this;
	}
	RA_1<T>& operator= (ra<T> rhs) {
        // Not a lot of index checking here, be careful!
        for (addr i=1; i<=3; ++i) val[i] = rhs(i);
		return *this;
	}



    // Indexing operator
    inline T& operator() (int i) {
		return val[i];
	}



    // Simple math operators.
    RA_1<T>& operator+= (T rhs) {
        for (addr i=0; i<=3; ++i) val[i] += rhs;
        return *this;
    }
    RA_1<T>& operator*= (T rhs) {
        for (addr i=0; i<=3; ++i) val[i] *= rhs;
        return *this;
    }
    RA_1<T>& operator+= (RA_1<T> rhs) {
        for (addr i=0; i<=3; ++i) val[i] += rhs.val[i];
        return *this;
    }



    // Dot and cross products
    double operator*(RA_1<T> const& rhs) const {
        // Not a lot of index checking here, be careful!
        double retval = 0;
        for (int i=1; i<=3; ++i) {
            retval += val[i] * rhs.val[i];
        }
        return retval;
    }
    RA_1<T> operator^(RA_1<T> const& rhs) const {
        // Not a lot of index checking here, be careful!
        RA_1<T> retval;
        retval.val[1] = val[2] * rhs.val[3] - val[3] * rhs.val[2];
        retval.val[2] = val[3] * rhs.val[1] - val[1] * rhs.val[3];
        retval.val[3] = val[1] * rhs.val[2] - val[2] * rhs.val[1];
        return retval;
    }



private:
    T val[4];

    void comcopy(RA_1<T> const& rhs) {
        for (addr i=0; i<=3; ++i) val[i] = rhs.val[i];
    }
};





// =============================================================================
// 2-D version here ============================================================
// =============================================================================

template <class T> class RA_2 {
public:
    RA_2() {};      // Compiler wants a default constructor i dunno why

	// Copy, assignment constructors
	RA_2(RA_2<T> const& rhs) {comcopy(rhs);};
	RA_2<T>& operator= (RA_2<T> const& rhs) {
		if (this != &rhs) comcopy(rhs);
		return *this;
	}
	RA_2<T>& operator= (T rhs) {
        for (addr j=0; j<=3; ++j) {
            for (addr i=0; i<=3; ++i) val[j][i] = rhs;
        }
		return *this;
	}



    // Indexing operator
    inline T& operator() (int i,int j) {
		return val[j][i];
	}



    // Simple math operators.
    RA_2<T>& operator+= (T rhs) {
        for (addr j=0; j<=3; ++j) {
            for (addr i=0; i<=3; ++i) val[j][i] += rhs;
        }
        return *this;
    }
    RA_2<T>& operator*= (T rhs) {
         for (addr j=0; j<=3; ++j) {
            for (addr i=0; i<=3; ++i) val[j][i] *= rhs;
        }
        return *this;
    }
    RA_2<T>& operator+= (RA_2<T> rhs) {
        for (addr j=0; j<=3; ++j) {
            for (addr i=0; i<=3; ++i) val[j][i] += rhs.val[j][i];
        }
        return *this;
    }



private:
    T val[4][4];

    void comcopy(RA_2<T> const& rhs) {
        for (addr j=0; j<=3; ++j) {
            for (addr i=0; i<=3; ++i) val[j][i] = rhs.val[j][i];
        }
    }
};





// =============================================================================
// 3-D version here ============================================================
// =============================================================================

template <class T> class RA_3 {
public:
    RA_3() {};      // Compiler wants a default constructor i dunno why

	// Copy, assignment constructors
	RA_3(RA_3<T> const& rhs) {comcopy(rhs);};
	RA_3<T>& operator= (RA_3<T> const& rhs) {
		if (this != &rhs) comcopy(rhs);
		return *this;
	}
	RA_3<T>& operator= (T rhs) {
        for (addr k=0; k<=3; ++k) {
            for (addr j=0; j<=3; ++j) {
                for (addr i=0; i<=3; ++i) val[k][j][i] = rhs;
            }
        }
		return *this;
	}



    // Indexing operator
    inline T& operator() (int i,int j, int k) {
		return val[k][j][i];
	}



    // Simple math operators.
    RA_3<T>& operator+= (T rhs) {
        for (addr k=0; k<=3; ++k) {
            for (addr j=0; j<=3; ++j) {
                for (addr i=0; i<=3; ++i) val[k][j][i] += rhs;
            }
        }
        return *this;
    }
    RA_3<T>& operator*= (T rhs) {
        for (addr k=0; k<=3; ++k) {
            for (addr j=0; j<=3; ++j) {
                for (addr i=0; i<=3; ++i) val[k][j][i] *= rhs;
            }
        }
        return *this;
    }
    RA_3<T>& operator+= (RA_3<T> rhs) {
        for (addr k=0; k<=3; ++k) {
            for (addr j=0; j<=3; ++j) {
                for (addr i=0; i<=3; ++i) val[k][j][i] += rhs.val[k][j][i];
            }
        }
        return *this;
    }



private:
    T val[4][4][4];

    void comcopy(RA_3<T> const& rhs) {
        for (addr k=0; k<=3; ++k) {
            for (addr j=0; j<=3; ++j) {
                for (addr i=0; i<=3; ++i) val[k][j][i] = rhs.val[k][j][i];
            }
        }
    return;
    }
};





#endif

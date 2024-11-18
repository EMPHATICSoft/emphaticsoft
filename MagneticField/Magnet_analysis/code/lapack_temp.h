/*
 =================================================================================================
 Definitions and prototypes for LAPACK v3.2.1 as provided Apple Computer, modified by Leo Bellantoni
 	17 May 2024 for local temporary use in an Alma 9 environment

 Documentation of the LAPACK interfaces, including reference implementations, can be found on
 the web starting from the LAPACK FAQ page at this URL (verified live as of January 2010);:
 http://netlib.org/lapack/faq.html

 A hardcopy maanual is:
 LAPACK Users' Guide, Third Edition. 
 @BOOK{laug,
 AUTHOR = {Anderson, E. and Bai, Z. and Bischof, C. and
 Blackford, S. and Demmel, J. and Dongarra, J. and
 Du Croz, J. and Greenbaum, A. and Hammarling, S. and
 McKenney, A. and Sorensen, D.},
 TITLE = {{LAPACK} Users' Guide},
 EDITION = {Third},
 PUBLISHER = {Society for Industrial and Applied Mathematics},
 YEAR = {1999},
 ADDRESS = {Philadelphia, PA},
 ISBN = {0-89871-447-8 (paperback);} }

 =================================================================================================
 */

#ifndef __CLAPACK_H
#define __CLAPACK_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__LP64__) /* In LP64 match sizes with the 32 bit ABI */
    typedef int 		__CLPK_integer;
    typedef int 		__CLPK_logical;
    typedef float 		__CLPK_real;
    typedef double 		__CLPK_doublereal;
    
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
    typedef __CLPK_logical 	(*__CLPK_L_fp)();
#pragma clang diagnostic pop
    
    typedef int 		__CLPK_ftnlen;
#else
    typedef long int 	__CLPK_integer;
    typedef long int 	__CLPK_logical;
    typedef float 		__CLPK_real;
    typedef double 		__CLPK_doublereal;
    
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
    typedef __CLPK_logical 	(*__CLPK_L_fp)();
#pragma clang diagnostic pop
    
    typedef long int 	__CLPK_ftnlen;
#endif

typedef struct { __CLPK_real r, i; } __CLPK_complex;
typedef struct { __CLPK_doublereal r, i; } __CLPK_doublecomplex;

#include <stdint.h>

int cbdsqr_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__ncvt,
        __CLPK_integer *__nru, __CLPK_integer *__ncc, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_complex *__vt, __CLPK_integer *__ldvt,
        __CLPK_complex *__u, __CLPK_integer *__ldu, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__rwork,
        __CLPK_integer *__info);;


int cgbbrd_(char *__vect, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__ncc, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_complex *__ab, __CLPK_integer *__ldab, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_complex *__q, __CLPK_integer *__ldq,
        __CLPK_complex *__pt, __CLPK_integer *__ldpt, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);;


int cgbcon_(char *__norm, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);;


int cgbequ_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_real *__rowcnd,
        __CLPK_real *__colcnd, __CLPK_real *__amax,
        __CLPK_integer *__info);;


int cgbequb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_real *__rowcnd,
        __CLPK_real *__colcnd, __CLPK_real *__amax,
        __CLPK_integer *__info);;


int cgbrfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_integer *__nrhs, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__afb, __CLPK_integer *__ldafb,
        __CLPK_integer *__ipiv, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);;


int cgbsv_(__CLPK_integer *__n, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_integer *__nrhs, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);;


int cgbsvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__kl, __CLPK_integer *__ku, __CLPK_integer *__nrhs,
        __CLPK_complex *__ab, __CLPK_integer *__ldab, __CLPK_complex *__afb,
        __CLPK_integer *__ldafb, __CLPK_integer *__ipiv, char *__equed,
        __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__rcond, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);;


int cgbtf2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);;


int cgbtrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);;


int cgbtrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_integer *__nrhs, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);;


int cgebak_(char *__job, char *__side, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__scale,
        __CLPK_integer *__m, __CLPK_complex *__v, __CLPK_integer *__ldv,
        __CLPK_integer *__info);;


int cgebal_(char *__job, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__scale,
        __CLPK_integer *__info);;


int cgebd2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_complex *__tauq, __CLPK_complex *__taup, __CLPK_complex *__work,
        __CLPK_integer *__info);;


int cgebrd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_complex *__tauq, __CLPK_complex *__taup, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);;


int cgecon_(char *__norm, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);;


int cgeequ_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__r__, __CLPK_real *__c__,
        __CLPK_real *__rowcnd, __CLPK_real *__colcnd, __CLPK_real *__amax,
        __CLPK_integer *__info);;


int cgeequb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__r__, __CLPK_real *__c__,
        __CLPK_real *__rowcnd, __CLPK_real *__colcnd, __CLPK_real *__amax,
        __CLPK_integer *__info);;


int cgees_(char *__jobvs, char *__sort, __CLPK_L_fp __select,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__sdim, __CLPK_complex *__w, __CLPK_complex *__vs,
        __CLPK_integer *__ldvs, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int cgeesx_(char *__jobvs, char *__sort, __CLPK_L_fp __select, char *__sense,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__sdim, __CLPK_complex *__w, __CLPK_complex *__vs,
        __CLPK_integer *__ldvs, __CLPK_real *__rconde, __CLPK_real *__rcondv,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int cgeev_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__w,
        __CLPK_complex *__vl, __CLPK_integer *__ldvl, __CLPK_complex *__vr,
        __CLPK_integer *__ldvr, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgeevx_(char *__balanc, char *__jobvl, char *__jobvr, char *__sense,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__w, __CLPK_complex *__vl, __CLPK_integer *__ldvl,
        __CLPK_complex *__vr, __CLPK_integer *__ldvr, __CLPK_integer *__ilo,
        __CLPK_integer *__ihi, __CLPK_real *__scale, __CLPK_real *__abnrm,
        __CLPK_real *__rconde, __CLPK_real *__rcondv, __CLPK_complex *__work,
        __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgegs_(char *__jobvsl, char *__jobvsr, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__alpha, __CLPK_complex *__beta,
        __CLPK_complex *__vsl, __CLPK_integer *__ldvsl, __CLPK_complex *__vsr,
        __CLPK_integer *__ldvsr, __CLPK_complex *__work,
        __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgegv_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__alpha, __CLPK_complex *__beta,
        __CLPK_complex *__vl, __CLPK_integer *__ldvl, __CLPK_complex *__vr,
        __CLPK_integer *__ldvr, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgehd2_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int cgehrd_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cgelq2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cgelqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cgels_(char *__trans, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cgelsd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_real *__s, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int cgelss_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_real *__s, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgelsx_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_integer *__jpvt, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgelsy_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_integer *__jpvt, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgeql2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cgeqlf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cgeqp3_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_complex *__tau,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgeqpf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_complex *__tau,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgeqr2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cgeqrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cgerfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgerq2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cgerqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cgesc2_(__CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__rhs, __CLPK_integer *__ipiv, __CLPK_integer *__jpiv,
        __CLPK_real *__scale);


int cgesdd_(char *__jobz, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_real *__s,
        __CLPK_complex *__u, __CLPK_integer *__ldu, __CLPK_complex *__vt,
        __CLPK_integer *__ldvt, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int cgesv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cgesvd_(char *__jobu, char *__jobvt, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_real *__s, __CLPK_complex *__u, __CLPK_integer *__ldu,
        __CLPK_complex *__vt, __CLPK_integer *__ldvt, __CLPK_complex *__work,
        __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgesvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__af, __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        char *__equed, __CLPK_real *__r__, __CLPK_real *__c__,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__x,
        __CLPK_integer *__ldx, __CLPK_real *__rcond, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgetc2_(__CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_integer *__jpiv,
        __CLPK_integer *__info);


int cgetf2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int cgetrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int cgetri_(__CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cgetrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cggbak_(char *__job, char *__side, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__lscale,
        __CLPK_real *__rscale, __CLPK_integer *__m, __CLPK_complex *__v,
        __CLPK_integer *__ldv,
        __CLPK_integer *__info);


int cggbal_(char *__job, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__lscale,
        __CLPK_real *__rscale, __CLPK_real *__work,
        __CLPK_integer *__info);


int cgges_(char *__jobvsl, char *__jobvsr, char *__sort, __CLPK_L_fp __selctg,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_integer *__sdim,
        __CLPK_complex *__alpha, __CLPK_complex *__beta, __CLPK_complex *__vsl,
        __CLPK_integer *__ldvsl, __CLPK_complex *__vsr, __CLPK_integer *__ldvsr,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int cggesx_(char *__jobvsl, char *__jobvsr, char *__sort, __CLPK_L_fp __selctg,
        char *__sense, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__sdim, __CLPK_complex *__alpha, __CLPK_complex *__beta,
        __CLPK_complex *__vsl, __CLPK_integer *__ldvsl, __CLPK_complex *__vsr,
        __CLPK_integer *__ldvsr, __CLPK_real *__rconde, __CLPK_real *__rcondv,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int cggev_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__alpha, __CLPK_complex *__beta,
        __CLPK_complex *__vl, __CLPK_integer *__ldvl, __CLPK_complex *__vr,
        __CLPK_integer *__ldvr, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cggevx_(char *__balanc, char *__jobvl, char *__jobvr, char *__sense,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__alpha,
        __CLPK_complex *__beta, __CLPK_complex *__vl, __CLPK_integer *__ldvl,
        __CLPK_complex *__vr, __CLPK_integer *__ldvr, __CLPK_integer *__ilo,
        __CLPK_integer *__ihi, __CLPK_real *__lscale, __CLPK_real *__rscale,
        __CLPK_real *__abnrm, __CLPK_real *__bbnrm, __CLPK_real *__rconde,
        __CLPK_real *__rcondv, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork, __CLPK_integer *__iwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int cggglm_(__CLPK_integer *__n, __CLPK_integer *__m, __CLPK_integer *__p,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__d__, __CLPK_complex *__x,
        __CLPK_complex *__y, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cgghrd_(char *__compq, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__q, __CLPK_integer *__ldq, __CLPK_complex *__z__,
        __CLPK_integer *__ldz,
        __CLPK_integer *__info);


int cgglse_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__p,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__c__, __CLPK_complex *__d__,
        __CLPK_complex *__x, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cggqrf_(__CLPK_integer *__n, __CLPK_integer *__m, __CLPK_integer *__p,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__taua,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__taub,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cggrqf_(__CLPK_integer *__m, __CLPK_integer *__p, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__taua,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__taub,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cggsvd_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__p, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_real *__alpha,
        __CLPK_real *__beta, __CLPK_complex *__u, __CLPK_integer *__ldu,
        __CLPK_complex *__v, __CLPK_integer *__ldv, __CLPK_complex *__q,
        __CLPK_integer *__ldq, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int cggsvp_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__p, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_real *__tola, __CLPK_real *__tolb, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_complex *__u, __CLPK_integer *__ldu,
        __CLPK_complex *__v, __CLPK_integer *__ldv, __CLPK_complex *__q,
        __CLPK_integer *__ldq, __CLPK_integer *__iwork, __CLPK_real *__rwork,
        __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cgtcon_(char *__norm, __CLPK_integer *__n, __CLPK_complex *__dl,
        __CLPK_complex *__d__, __CLPK_complex *__du, __CLPK_complex *__du2,
        __CLPK_integer *__ipiv, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int cgtrfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__dl, __CLPK_complex *__d__, __CLPK_complex *__du,
        __CLPK_complex *__dlf, __CLPK_complex *__df, __CLPK_complex *__duf,
        __CLPK_complex *__du2, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgtsv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_complex *__dl,
        __CLPK_complex *__d__, __CLPK_complex *__du, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cgtsvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__dl, __CLPK_complex *__d__,
        __CLPK_complex *__du, __CLPK_complex *__dlf, __CLPK_complex *__df,
        __CLPK_complex *__duf, __CLPK_complex *__du2, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__x,
        __CLPK_integer *__ldx, __CLPK_real *__rcond, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cgttrf_(__CLPK_integer *__n, __CLPK_complex *__dl, __CLPK_complex *__d__,
        __CLPK_complex *__du, __CLPK_complex *__du2, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int cgttrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__dl, __CLPK_complex *__d__, __CLPK_complex *__du,
        __CLPK_complex *__du2, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cgtts2_(__CLPK_integer *__itrans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__dl, __CLPK_complex *__d__,
        __CLPK_complex *__du, __CLPK_complex *__du2, __CLPK_integer *__ipiv,
        __CLPK_complex *__b,
        __CLPK_integer *__ldb);


int chbev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__w, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chbevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__w, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int chbevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_complex *__q, __CLPK_integer *__ldq, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_complex *__z__, __CLPK_integer *__ldz, __CLPK_complex *__work,
        __CLPK_real *__rwork, __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int chbgst_(char *__vect, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__bb, __CLPK_integer *__ldbb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chbgv_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__bb, __CLPK_integer *__ldbb,
        __CLPK_real *__w, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chbgvd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__bb, __CLPK_integer *__ldbb,
        __CLPK_real *__w, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int chbgvx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__bb, __CLPK_integer *__ldbb,
        __CLPK_complex *__q, __CLPK_integer *__ldq, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_complex *__z__, __CLPK_integer *__ldz, __CLPK_complex *__work,
        __CLPK_real *__rwork, __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int chbtrd_(char *__vect, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_complex *__q,
        __CLPK_integer *__ldq, __CLPK_complex *__work,
        __CLPK_integer *__info);


int checon_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_real *__anorm,
        __CLPK_real *__rcond, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cheequb_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__s, __CLPK_real *__scond,
        __CLPK_real *__amax, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cheev_(char *__jobz, char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__w, __CLPK_complex *__work,
        __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cheevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_real *__w,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int cheevr_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_complex *__z__, __CLPK_integer *__ldz, __CLPK_integer *__isuppz,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int cheevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_complex *__z__, __CLPK_integer *__ldz, __CLPK_complex *__work,
        __CLPK_integer *__lwork, __CLPK_real *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int chegs2_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int chegst_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int chegv_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_real *__w,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chegvd_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_real *__w,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int chegvx_(__CLPK_integer *__itype, char *__jobz, char *__range, char *__uplo,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_complex *__z__, __CLPK_integer *__ldz, __CLPK_complex *__work,
        __CLPK_integer *__lwork, __CLPK_real *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int cherfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chesv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int chesvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__af, __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__x,
        __CLPK_integer *__ldx, __CLPK_real *__rcond, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chetd2_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_complex *__tau,
        __CLPK_integer *__info);


int chetf2_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int chetrd_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_complex *__tau, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int chetrf_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int chetri_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_complex *__work,
        __CLPK_integer *__info);


int chetrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int chfrk_(char *__transr, char *__uplo, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_real *__alpha, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__beta,
        __CLPK_complex *__c__);


int chgeqz_(char *__job, char *__compq, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_complex *__h__,
        __CLPK_integer *__ldh, __CLPK_complex *__t, __CLPK_integer *__ldt,
        __CLPK_complex *__alpha, __CLPK_complex *__beta, __CLPK_complex *__q,
        __CLPK_integer *__ldq, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__info);


void chla_transtype__(char *__ret_val, __CLPK_ftnlen __ret_val_len,
        __CLPK_integer *__trans);


int chpcon_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_integer *__ipiv, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int chpev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__ap, __CLPK_real *__w, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chpevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__ap, __CLPK_real *__w, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork, __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int chpevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__ap, __CLPK_real *__vl, __CLPK_real *__vu,
        __CLPK_integer *__il, __CLPK_integer *__iu, __CLPK_real *__abstol,
        __CLPK_integer *__m, __CLPK_real *__w, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int chpgst_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__ap, __CLPK_complex *__bp,
        __CLPK_integer *__info);


int chpgv_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_complex *__ap, __CLPK_complex *__bp,
        __CLPK_real *__w, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chpgvd_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_complex *__ap, __CLPK_complex *__bp,
        __CLPK_real *__w, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int chpgvx_(__CLPK_integer *__itype, char *__jobz, char *__range, char *__uplo,
        __CLPK_integer *__n, __CLPK_complex *__ap, __CLPK_complex *__bp,
        __CLPK_real *__vl, __CLPK_real *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_real *__abstol, __CLPK_integer *__m,
        __CLPK_real *__w, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_real *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int chprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_complex *__afp, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__x,
        __CLPK_integer *__ldx, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chpsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int chpsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__ap, __CLPK_complex *__afp,
        __CLPK_integer *__ipiv, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_real *__rcond,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int chptrd_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_complex *__tau,
        __CLPK_integer *__info);


int chptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int chptri_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_integer *__ipiv, __CLPK_complex *__work,
        __CLPK_integer *__info);


int chptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int chsein_(char *__side, char *__eigsrc, char *__initv,
        __CLPK_logical *__select, __CLPK_integer *__n, __CLPK_complex *__h__,
        __CLPK_integer *__ldh, __CLPK_complex *__w, __CLPK_complex *__vl,
        __CLPK_integer *__ldvl, __CLPK_complex *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__mm, __CLPK_integer *__m, __CLPK_complex *__work,
        __CLPK_real *__rwork, __CLPK_integer *__ifaill,
        __CLPK_integer *__ifailr,
        __CLPK_integer *__info);


int chseqr_(char *__job, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_complex *__h__,
        __CLPK_integer *__ldh, __CLPK_complex *__w, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int clabrd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_complex *__tauq, __CLPK_complex *__taup,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_complex *__y,
        __CLPK_integer *__ldy);


int clacgv_(__CLPK_integer *__n, __CLPK_complex *__x,
        __CLPK_integer *__incx);


int clacn2_(__CLPK_integer *__n, __CLPK_complex *__v, __CLPK_complex *__x,
        __CLPK_real *__est, __CLPK_integer *__kase,
        __CLPK_integer *__isave);


int clacon_(__CLPK_integer *__n, __CLPK_complex *__v, __CLPK_complex *__x,
        __CLPK_real *__est,
        __CLPK_integer *__kase);


int clacp2_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb);


int clacpy_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb);


int clacrm_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__c__, __CLPK_integer *__ldc,
        __CLPK_real *__rwork);


int clacrt_(__CLPK_integer *__n, __CLPK_complex *__cx, __CLPK_integer *__incx,
        __CLPK_complex *__cy, __CLPK_integer *__incy, __CLPK_complex *__c__,
        __CLPK_complex *__s);


void cladiv_(__CLPK_complex *__ret_val, __CLPK_complex *__x,
        __CLPK_complex *__y);


int claed0_(__CLPK_integer *__qsiz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_complex *__q, __CLPK_integer *__ldq,
        __CLPK_complex *__qstore, __CLPK_integer *__ldqs, __CLPK_real *__rwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int claed7_(__CLPK_integer *__n, __CLPK_integer *__cutpnt,
        __CLPK_integer *__qsiz, __CLPK_integer *__tlvls,
        __CLPK_integer *__curlvl, __CLPK_integer *__curpbm, __CLPK_real *__d__,
        __CLPK_complex *__q, __CLPK_integer *__ldq, __CLPK_real *__rho,
        __CLPK_integer *__indxq, __CLPK_real *__qstore, __CLPK_integer *__qptr,
        __CLPK_integer *__prmptr, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_real *__givnum, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int claed8_(__CLPK_integer *__k, __CLPK_integer *__n, __CLPK_integer *__qsiz,
        __CLPK_complex *__q, __CLPK_integer *__ldq, __CLPK_real *__d__,
        __CLPK_real *__rho, __CLPK_integer *__cutpnt, __CLPK_real *__z__,
        __CLPK_real *__dlamda, __CLPK_complex *__q2, __CLPK_integer *__ldq2,
        __CLPK_real *__w, __CLPK_integer *__indxp, __CLPK_integer *__indx,
        __CLPK_integer *__indxq, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_real *__givnum,
        __CLPK_integer *__info);


int claein_(__CLPK_logical *__rightv, __CLPK_logical *__noinit,
        __CLPK_integer *__n, __CLPK_complex *__h__, __CLPK_integer *__ldh,
        __CLPK_complex *__w, __CLPK_complex *__v, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_real *__rwork, __CLPK_real *__eps3,
        __CLPK_real *__smlnum,
        __CLPK_integer *__info);


int claesy_(__CLPK_complex *__a, __CLPK_complex *__b, __CLPK_complex *__c__,
        __CLPK_complex *__rt1, __CLPK_complex *__rt2, __CLPK_complex *__evscal,
        __CLPK_complex *__cs1,
        __CLPK_complex *__sn1);


int claev2_(__CLPK_complex *__a, __CLPK_complex *__b, __CLPK_complex *__c__,
        __CLPK_real *__rt1, __CLPK_real *__rt2, __CLPK_real *__cs1,
        __CLPK_complex *__sn1);


int clag2z_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__sa,
        __CLPK_integer *__ldsa, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int clags2_(__CLPK_logical *__upper, __CLPK_real *__a1, __CLPK_complex *__a2,
        __CLPK_real *__a3, __CLPK_real *__b1, __CLPK_complex *__b2,
        __CLPK_real *__b3, __CLPK_real *__csu, __CLPK_complex *__snu,
        __CLPK_real *__csv, __CLPK_complex *__snv, __CLPK_real *__csq,
        __CLPK_complex *__snq);


int clagtm_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__alpha, __CLPK_complex *__dl, __CLPK_complex *__d__,
        __CLPK_complex *__du, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__beta, __CLPK_complex *__b,
        __CLPK_integer *__ldb);


int clahef_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_integer *__kb, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_complex *__w, __CLPK_integer *__ldw,
        __CLPK_integer *__info);


int clahqr_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_complex *__h__, __CLPK_integer *__ldh, __CLPK_complex *__w,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz, __CLPK_complex *__z__,
        __CLPK_integer *__ldz,
        __CLPK_integer *__info);


int clahr2_(__CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__nb,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__t, __CLPK_integer *__ldt, __CLPK_complex *__y,
        __CLPK_integer *__ldy);


int clahrd_(__CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__nb,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__t, __CLPK_integer *__ldt, __CLPK_complex *__y,
        __CLPK_integer *__ldy);


int claic1_(__CLPK_integer *__job, __CLPK_integer *__j, __CLPK_complex *__x,
        __CLPK_real *__sest, __CLPK_complex *__w, __CLPK_complex *__gamma,
        __CLPK_real *__sestpr, __CLPK_complex *__s,
        __CLPK_complex *__c__);


int clals0_(__CLPK_integer *__icompq, __CLPK_integer *__nl,
        __CLPK_integer *__nr, __CLPK_integer *__sqre, __CLPK_integer *__nrhs,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__bx,
        __CLPK_integer *__ldbx, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_integer *__ldgcol, __CLPK_real *__givnum,
        __CLPK_integer *__ldgnum, __CLPK_real *__poles, __CLPK_real *__difl,
        __CLPK_real *__difr, __CLPK_real *__z__, __CLPK_integer *__k,
        __CLPK_real *__c__, __CLPK_real *__s, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int clalsa_(__CLPK_integer *__icompq, __CLPK_integer *__smlsiz,
        __CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__bx, __CLPK_integer *__ldbx,
        __CLPK_real *__u, __CLPK_integer *__ldu, __CLPK_real *__vt,
        __CLPK_integer *__k, __CLPK_real *__difl, __CLPK_real *__difr,
        __CLPK_real *__z__, __CLPK_real *__poles, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_integer *__ldgcol,
        __CLPK_integer *__perm, __CLPK_real *__givnum, __CLPK_real *__c__,
        __CLPK_real *__s, __CLPK_real *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int clalsd_(char *__uplo, __CLPK_integer *__smlsiz, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


__CLPK_doublereal clangb_(char *__norm, __CLPK_integer *__n,
        __CLPK_integer *__kl, __CLPK_integer *__ku, __CLPK_complex *__ab,
        __CLPK_integer *__ldab,
        __CLPK_real *__work);


__CLPK_doublereal clange_(char *__norm, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_real *__work);


__CLPK_doublereal clangt_(char *__norm, __CLPK_integer *__n,
        __CLPK_complex *__dl, __CLPK_complex *__d__,
        __CLPK_complex *__du);


__CLPK_doublereal clanhb_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__work);


__CLPK_doublereal clanhe_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_real *__work);


__CLPK_doublereal clanhf_(char *__norm, char *__transr, char *__uplo,
        __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_real *__work);


__CLPK_doublereal clanhp_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__ap,
        __CLPK_real *__work);


__CLPK_doublereal clanhs_(char *__norm, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_real *__work);


__CLPK_doublereal clanht_(char *__norm, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_complex *__e);


__CLPK_doublereal clansb_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__work);


__CLPK_doublereal clansp_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__ap,
        __CLPK_real *__work);


__CLPK_doublereal clansy_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_real *__work);


__CLPK_doublereal clantb_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__ab,
        __CLPK_integer *__ldab,
        __CLPK_real *__work);


__CLPK_doublereal clantp_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_real *__work);


__CLPK_doublereal clantr_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda,
        __CLPK_real *__work);


int clapll_(__CLPK_integer *__n, __CLPK_complex *__x, __CLPK_integer *__incx,
        __CLPK_complex *__y, __CLPK_integer *__incy,
        __CLPK_real *__ssmin);


int clapmt_(__CLPK_logical *__forwrd, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_integer *__k);


int claqgb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_real *__rowcnd,
        __CLPK_real *__colcnd, __CLPK_real *__amax,
        char *__equed);


int claqge_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__r__, __CLPK_real *__c__,
        __CLPK_real *__rowcnd, __CLPK_real *__colcnd, __CLPK_real *__amax,
        char *__equed);


int claqhb_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_complex *__ab, __CLPK_integer *__ldab, __CLPK_real *__s,
        __CLPK_real *__scond, __CLPK_real *__amax,
        char *__equed);


int claqhe_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__s, __CLPK_real *__scond,
        __CLPK_real *__amax, char *__equed);


int claqhp_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        char *__equed);


int claqp2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__offset,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_integer *__jpvt,
        __CLPK_complex *__tau, __CLPK_real *__vn1, __CLPK_real *__vn2,
        __CLPK_complex *__work);


int claqps_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__offset,
        __CLPK_integer *__nb, __CLPK_integer *__kb, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_complex *__tau,
        __CLPK_real *__vn1, __CLPK_real *__vn2, __CLPK_complex *__auxv,
        __CLPK_complex *__f,
        __CLPK_integer *__ldf);


int claqr0_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_complex *__h__, __CLPK_integer *__ldh, __CLPK_complex *__w,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int claqr1_(__CLPK_integer *__n, __CLPK_complex *__h__, __CLPK_integer *__ldh,
        __CLPK_complex *__s1, __CLPK_complex *__s2,
        __CLPK_complex *__v);


int claqr2_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ktop, __CLPK_integer *__kbot,
        __CLPK_integer *__nw, __CLPK_complex *__h__, __CLPK_integer *__ldh,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__ns, __CLPK_integer *__nd,
        __CLPK_complex *__sh, __CLPK_complex *__v, __CLPK_integer *__ldv,
        __CLPK_integer *__nh, __CLPK_complex *__t, __CLPK_integer *__ldt,
        __CLPK_integer *__nv, __CLPK_complex *__wv, __CLPK_integer *__ldwv,
        __CLPK_complex *__work,
        __CLPK_integer *__lwork);


int claqr3_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ktop, __CLPK_integer *__kbot,
        __CLPK_integer *__nw, __CLPK_complex *__h__, __CLPK_integer *__ldh,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__ns, __CLPK_integer *__nd,
        __CLPK_complex *__sh, __CLPK_complex *__v, __CLPK_integer *__ldv,
        __CLPK_integer *__nh, __CLPK_complex *__t, __CLPK_integer *__ldt,
        __CLPK_integer *__nv, __CLPK_complex *__wv, __CLPK_integer *__ldwv,
        __CLPK_complex *__work,
        __CLPK_integer *__lwork);


int claqr4_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_complex *__h__, __CLPK_integer *__ldh, __CLPK_complex *__w,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int claqr5_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__kacc22, __CLPK_integer *__n, __CLPK_integer *__ktop,
        __CLPK_integer *__kbot, __CLPK_integer *__nshfts, __CLPK_complex *__s,
        __CLPK_complex *__h__, __CLPK_integer *__ldh, __CLPK_integer *__iloz,
        __CLPK_integer *__ihiz, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__v, __CLPK_integer *__ldv, __CLPK_complex *__u,
        __CLPK_integer *__ldu, __CLPK_integer *__nv, __CLPK_complex *__wv,
        __CLPK_integer *__ldwv, __CLPK_integer *__nh, __CLPK_complex *__wh,
        __CLPK_integer *__ldwh);


int claqsb_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_complex *__ab, __CLPK_integer *__ldab, __CLPK_real *__s,
        __CLPK_real *__scond, __CLPK_real *__amax,
        char *__equed);


int claqsp_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        char *__equed);


int claqsy_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__s, __CLPK_real *__scond,
        __CLPK_real *__amax, char *__equed);


int clar1v_(__CLPK_integer *__n, __CLPK_integer *__b1, __CLPK_integer *__bn,
        __CLPK_real *__lambda, __CLPK_real *__d__, __CLPK_real *__l,
        __CLPK_real *__ld, __CLPK_real *__lld, __CLPK_real *__pivmin,
        __CLPK_real *__gaptol, __CLPK_complex *__z__, __CLPK_logical *__wantnc,
        __CLPK_integer *__negcnt, __CLPK_real *__ztz, __CLPK_real *__mingma,
        __CLPK_integer *__r__, __CLPK_integer *__isuppz, __CLPK_real *__nrminv,
        __CLPK_real *__resid, __CLPK_real *__rqcorr,
        __CLPK_real *__work);


int clar2v_(__CLPK_integer *__n, __CLPK_complex *__x, __CLPK_complex *__y,
        __CLPK_complex *__z__, __CLPK_integer *__incx, __CLPK_real *__c__,
        __CLPK_complex *__s,
        __CLPK_integer *__incc);


int clarcm_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__c__, __CLPK_integer *__ldc,
        __CLPK_real *__rwork);


int clarf_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__v, __CLPK_integer *__incv, __CLPK_complex *__tau,
        __CLPK_complex *__c__, __CLPK_integer *__ldc,
        __CLPK_complex *__work);


int clarfb_(char *__side, char *__trans, char *__direct, char *__storev,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__v, __CLPK_integer *__ldv, __CLPK_complex *__t,
        __CLPK_integer *__ldt, __CLPK_complex *__c__, __CLPK_integer *__ldc,
        __CLPK_complex *__work,
        __CLPK_integer *__ldwork);


int clarfg_(__CLPK_integer *__n, __CLPK_complex *__alpha, __CLPK_complex *__x,
        __CLPK_integer *__incx,
        __CLPK_complex *__tau);


int clarfp_(__CLPK_integer *__n, __CLPK_complex *__alpha, __CLPK_complex *__x,
        __CLPK_integer *__incx,
        __CLPK_complex *__tau);


int clarft_(char *__direct, char *__storev, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_complex *__v, __CLPK_integer *__ldv,
        __CLPK_complex *__tau, __CLPK_complex *__t,
        __CLPK_integer *__ldt);


int clarfx_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__v, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc,
        __CLPK_complex *__work);


int clargv_(__CLPK_integer *__n, __CLPK_complex *__x, __CLPK_integer *__incx,
        __CLPK_complex *__y, __CLPK_integer *__incy, __CLPK_real *__c__,
        __CLPK_integer *__incc);


int clarnv_(__CLPK_integer *__idist, __CLPK_integer *__iseed,
        __CLPK_integer *__n,
        __CLPK_complex *__x);


int clarrv_(__CLPK_integer *__n, __CLPK_real *__vl, __CLPK_real *__vu,
        __CLPK_real *__d__, __CLPK_real *__l, __CLPK_real *__pivmin,
        __CLPK_integer *__isplit, __CLPK_integer *__m, __CLPK_integer *__dol,
        __CLPK_integer *__dou, __CLPK_real *__minrgp, __CLPK_real *__rtol1,
        __CLPK_real *__rtol2, __CLPK_real *__w, __CLPK_real *__werr,
        __CLPK_real *__wgap, __CLPK_integer *__iblock, __CLPK_integer *__indexw,
        __CLPK_real *__gers, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__isuppz, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int clarscl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_complex *__x,
        __CLPK_integer *__ldx);


int clartg_(__CLPK_complex *__f, __CLPK_complex *__g, __CLPK_real *__cs,
        __CLPK_complex *__sn,
        __CLPK_complex *__r__);


int clartv_(__CLPK_integer *__n, __CLPK_complex *__x, __CLPK_integer *__incx,
        __CLPK_complex *__y, __CLPK_integer *__incy, __CLPK_real *__c__,
        __CLPK_complex *__s,
        __CLPK_integer *__incc);


int clarz_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__l, __CLPK_complex *__v, __CLPK_integer *__incv,
        __CLPK_complex *__tau, __CLPK_complex *__c__, __CLPK_integer *__ldc,
        __CLPK_complex *__work);


int clarzb_(char *__side, char *__trans, char *__direct, char *__storev,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_complex *__v, __CLPK_integer *__ldv,
        __CLPK_complex *__t, __CLPK_integer *__ldt, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__ldwork);


int clarzt_(char *__direct, char *__storev, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_complex *__v, __CLPK_integer *__ldv,
        __CLPK_complex *__tau, __CLPK_complex *__t,
        __CLPK_integer *__ldt);


int clascl_(char *__type__, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_real *__cfrom, __CLPK_real *__cto, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int clascl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_complex *__x,
        __CLPK_integer *__ldx);


int claset_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__alpha, __CLPK_complex *__beta, __CLPK_complex *__a,
        __CLPK_integer *__lda);


int clasr_(char *__side, char *__pivot, char *__direct, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__c__, __CLPK_real *__s,
        __CLPK_complex *__a,
        __CLPK_integer *__lda);


int classq_(__CLPK_integer *__n, __CLPK_complex *__x, __CLPK_integer *__incx,
        __CLPK_real *__scale,
        __CLPK_real *__sumsq);


int claswp_(__CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__k1, __CLPK_integer *__k2, __CLPK_integer *__ipiv,
        __CLPK_integer *__incx);


int clasyf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_integer *__kb, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_complex *__w, __CLPK_integer *__ldw,
        __CLPK_integer *__info);


int clatbs_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_integer *__kd, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__x, __CLPK_real *__scale,
        __CLPK_real *__cnorm,
        __CLPK_integer *__info);


int clatdf_(__CLPK_integer *__ijob, __CLPK_integer *__n, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_complex *__rhs, __CLPK_real *__rdsum,
        __CLPK_real *__rdscal, __CLPK_integer *__ipiv,
        __CLPK_integer *__jpiv);


int clatps_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_complex *__ap, __CLPK_complex *__x,
        __CLPK_real *__scale, __CLPK_real *__cnorm,
        __CLPK_integer *__info);


int clatrd_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_real *__e,
        __CLPK_complex *__tau, __CLPK_complex *__w,
        __CLPK_integer *__ldw);


int clatrs_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__x, __CLPK_real *__scale, __CLPK_real *__cnorm,
        __CLPK_integer *__info);


int clatrz_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__l,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work);


int clatzm_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__v, __CLPK_integer *__incv, __CLPK_complex *__tau,
        __CLPK_complex *__c1, __CLPK_complex *__c2, __CLPK_integer *__ldc,
        __CLPK_complex *__work);


int clauu2_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int clauum_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int cpbcon_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_complex *__ab, __CLPK_integer *__ldab, __CLPK_real *__anorm,
        __CLPK_real *__rcond, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cpbequ_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_complex *__ab, __CLPK_integer *__ldab, __CLPK_real *__s,
        __CLPK_real *__scond, __CLPK_real *__amax,
        __CLPK_integer *__info);


int cpbrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_complex *__afb, __CLPK_integer *__ldafb, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cpbstf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int cpbsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cpbsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__afb, __CLPK_integer *__ldafb,
        char *__equed, __CLPK_real *__s, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__rcond, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cpbtf2_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int cpbtrf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int cpbtrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cpftrf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a,
        __CLPK_integer *__info);


int cpftri_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a,
        __CLPK_integer *__info);


int cpftrs_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__a, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cpocon_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cpoequ_(__CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        __CLPK_integer *__info);


int cpoequb_(__CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        __CLPK_integer *__info);


int cporfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__af,
        __CLPK_integer *__ldaf, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cposv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cposvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__af, __CLPK_integer *__ldaf, char *__equed,
        __CLPK_real *__s, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_real *__rcond,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cpotf2_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int cpotrf_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int cpotri_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int cpotrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cppcon_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_real *__anorm, __CLPK_real *__rcond, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cppequ_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        __CLPK_integer *__info);


int cpprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_complex *__afp, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cppsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cppsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__ap, __CLPK_complex *__afp,
        char *__equed, __CLPK_real *__s, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__rcond, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cpptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_integer *__info);


int cpptri_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_integer *__info);


int cpptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cpstf2_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__piv, __CLPK_integer *__rank,
        __CLPK_real *__tol, __CLPK_real *__work,
        __CLPK_integer *__info);


int cpstrf_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__piv, __CLPK_integer *__rank,
        __CLPK_real *__tol, __CLPK_real *__work,
        __CLPK_integer *__info);


int cptcon_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_complex *__e,
        __CLPK_real *__anorm, __CLPK_real *__rcond, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cpteqr_(char *__compz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int cptrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__d__, __CLPK_complex *__e, __CLPK_real *__df,
        __CLPK_complex *__ef, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cptsv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_real *__d__,
        __CLPK_complex *__e, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cptsvx_(char *__fact, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__d__, __CLPK_complex *__e, __CLPK_real *__df,
        __CLPK_complex *__ef, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_real *__rcond,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cpttrf_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_complex *__e,
        __CLPK_integer *__info);


int cpttrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__d__, __CLPK_complex *__e, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cptts2_(__CLPK_integer *__iuplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__d__, __CLPK_complex *__e,
        __CLPK_complex *__b,
        __CLPK_integer *__ldb);


int crot_(__CLPK_integer *__n, __CLPK_complex *__cx, __CLPK_integer *__incx,
        __CLPK_complex *__cy, __CLPK_integer *__incy, __CLPK_real *__c__,
        __CLPK_complex *__s);


int cspcon_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_integer *__ipiv, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int cspmv_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__alpha,
        __CLPK_complex *__ap, __CLPK_complex *__x, __CLPK_integer *__incx,
        __CLPK_complex *__beta, __CLPK_complex *__y,
        __CLPK_integer *__incy);


int cspr_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__alpha,
        __CLPK_complex *__x, __CLPK_integer *__incx,
        __CLPK_complex *__ap);


int csprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_complex *__afp, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__x,
        __CLPK_integer *__ldx, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int cspsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int cspsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__ap, __CLPK_complex *__afp,
        __CLPK_integer *__ipiv, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_real *__rcond,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int csptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int csptri_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_integer *__ipiv, __CLPK_complex *__work,
        __CLPK_integer *__info);


int csptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__ap, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int csrscl_(__CLPK_integer *__n, __CLPK_real *__sa, __CLPK_complex *__sx,
        __CLPK_integer *__incx);


int cstedc_(char *__compz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_complex *__work, __CLPK_integer *__lwork, __CLPK_real *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int cstegr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_complex *__z__, __CLPK_integer *__ldz, __CLPK_integer *__isuppz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int cstein_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_integer *__m, __CLPK_real *__w, __CLPK_integer *__iblock,
        __CLPK_integer *__isplit, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int cstemr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_integer *__m, __CLPK_real *__w, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__nzc, __CLPK_integer *__isuppz,
        __CLPK_logical *__tryrac, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int csteqr_(char *__compz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int csycon_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_real *__anorm,
        __CLPK_real *__rcond, __CLPK_complex *__work,
        __CLPK_integer *__info);


int csyequb_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_real *__s, __CLPK_real *__scond,
        __CLPK_real *__amax, __CLPK_complex *__work,
        __CLPK_integer *__info);


int csymv_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__alpha,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__x,
        __CLPK_integer *__incx, __CLPK_complex *__beta, __CLPK_complex *__y,
        __CLPK_integer *__incy);


int csyr_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__alpha,
        __CLPK_complex *__x, __CLPK_integer *__incx, __CLPK_complex *__a,
        __CLPK_integer *__lda);


int csyrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int csysv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int csysvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__af, __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__x,
        __CLPK_integer *__ldx, __CLPK_real *__rcond, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int csytf2_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int csytrf_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int csytri_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_complex *__work,
        __CLPK_integer *__info);


int csytrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ctbcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_complex *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__rcond, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctbrfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__x, __CLPK_integer *__ldx, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctbtrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_complex *__ab,
        __CLPK_integer *__ldab, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ctfsm_(char *__transr, char *__side, char *__uplo, char *__trans,
        char *__diag, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__alpha, __CLPK_complex *__a, __CLPK_complex *__b,
        __CLPK_integer *__ldb);


int ctftri_(char *__transr, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_complex *__a,
        __CLPK_integer *__info);


int ctfttp_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__arf, __CLPK_complex *__ap,
        __CLPK_integer *__info);


int ctfttr_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__arf, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int ctgevc_(char *__side, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_complex *__s, __CLPK_integer *__lds,
        __CLPK_complex *__p, __CLPK_integer *__ldp, __CLPK_complex *__vl,
        __CLPK_integer *__ldvl, __CLPK_complex *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__mm, __CLPK_integer *__m, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctgex2_(__CLPK_logical *__wantq, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__q,
        __CLPK_integer *__ldq, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__j1,
        __CLPK_integer *__info);


int ctgexc_(__CLPK_logical *__wantq, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__q,
        __CLPK_integer *__ldq, __CLPK_complex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__ifst, __CLPK_integer *__ilst,
        __CLPK_integer *__info);


int ctgsen_(__CLPK_integer *__ijob, __CLPK_logical *__wantq,
        __CLPK_logical *__wantz, __CLPK_logical *__select, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__alpha, __CLPK_complex *__beta,
        __CLPK_complex *__q, __CLPK_integer *__ldq, __CLPK_complex *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__m, __CLPK_real *__pl,
        __CLPK_real *__pr, __CLPK_real *__dif, __CLPK_complex *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int ctgsja_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__p, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_real *__tola,
        __CLPK_real *__tolb, __CLPK_real *__alpha, __CLPK_real *__beta,
        __CLPK_complex *__u, __CLPK_integer *__ldu, __CLPK_complex *__v,
        __CLPK_integer *__ldv, __CLPK_complex *__q, __CLPK_integer *__ldq,
        __CLPK_complex *__work, __CLPK_integer *__ncycle,
        __CLPK_integer *__info);


int ctgsna_(char *__job, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__vl,
        __CLPK_integer *__ldvl, __CLPK_complex *__vr, __CLPK_integer *__ldvr,
        __CLPK_real *__s, __CLPK_real *__dif, __CLPK_integer *__mm,
        __CLPK_integer *__m, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int ctgsy2_(char *__trans, __CLPK_integer *__ijob, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__d__, __CLPK_integer *__ldd,
        __CLPK_complex *__e, __CLPK_integer *__lde, __CLPK_complex *__f,
        __CLPK_integer *__ldf, __CLPK_real *__scale, __CLPK_real *__rdsum,
        __CLPK_real *__rdscal,
        __CLPK_integer *__info);


int ctgsyl_(char *__trans, __CLPK_integer *__ijob, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__d__, __CLPK_integer *__ldd,
        __CLPK_complex *__e, __CLPK_integer *__lde, __CLPK_complex *__f,
        __CLPK_integer *__ldf, __CLPK_real *__scale, __CLPK_real *__dif,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int ctpcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_complex *__ap, __CLPK_real *__rcond, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctprfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__ap, __CLPK_complex *__b,
        __CLPK_integer *__ldb, __CLPK_complex *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_complex *__work,
        __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctptri_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_complex *__ap,
        __CLPK_integer *__info);


int ctptrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__ap, __CLPK_complex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ctpttf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__ap, __CLPK_complex *__arf,
        __CLPK_integer *__info);


int ctpttr_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int ctrcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_real *__rcond,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctrevc_(char *__side, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_complex *__t, __CLPK_integer *__ldt,
        __CLPK_complex *__vl, __CLPK_integer *__ldvl, __CLPK_complex *__vr,
        __CLPK_integer *__ldvr, __CLPK_integer *__mm, __CLPK_integer *__m,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctrexc_(char *__compq, __CLPK_integer *__n, __CLPK_complex *__t,
        __CLPK_integer *__ldt, __CLPK_complex *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__ifst, __CLPK_integer *__ilst,
        __CLPK_integer *__info);


int ctrrfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb, __CLPK_complex *__x,
        __CLPK_integer *__ldx, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_complex *__work, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctrsen_(char *__job, char *__compq, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_complex *__t, __CLPK_integer *__ldt,
        __CLPK_complex *__q, __CLPK_integer *__ldq, __CLPK_complex *__w,
        __CLPK_integer *__m, __CLPK_real *__s, __CLPK_real *__sep,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int ctrsna_(char *__job, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_complex *__t, __CLPK_integer *__ldt,
        __CLPK_complex *__vl, __CLPK_integer *__ldvl, __CLPK_complex *__vr,
        __CLPK_integer *__ldvr, __CLPK_real *__s, __CLPK_real *__sep,
        __CLPK_integer *__mm, __CLPK_integer *__m, __CLPK_complex *__work,
        __CLPK_integer *__ldwork, __CLPK_real *__rwork,
        __CLPK_integer *__info);


int ctrsyl_(char *__trana, char *__tranb, __CLPK_integer *__isgn,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_complex *__c__, __CLPK_integer *__ldc, __CLPK_real *__scale,
        __CLPK_integer *__info);


int ctrti2_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int ctrtri_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int ctrtrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ctrttf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__arf,
        __CLPK_integer *__info);


int ctrttp_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__ap,
        __CLPK_integer *__info);


int ctzrqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_integer *__info);


int ctzrzf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cung2l_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int cung2r_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int cungbr_(char *__vect, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__tau, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunghr_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cungl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int cunglq_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cungql_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cungqr_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cungr2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int cungrq_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cungtr_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunm2l_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cunm2r_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cunmbr_(char *__vect, char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunmhr_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__c__, __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunml2_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cunmlq_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunmql_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunmqr_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunmr2_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cunmr3_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__l,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__c__, __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__info);


int cunmrq_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_complex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__tau, __CLPK_complex *__c__,
        __CLPK_integer *__ldc, __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunmrz_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__l,
        __CLPK_complex *__a, __CLPK_integer *__lda, __CLPK_complex *__tau,
        __CLPK_complex *__c__, __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cunmtr_(char *__side, char *__uplo, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_complex *__a, __CLPK_integer *__lda,
        __CLPK_complex *__tau, __CLPK_complex *__c__, __CLPK_integer *__ldc,
        __CLPK_complex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int cupgtr_(char *__uplo, __CLPK_integer *__n, __CLPK_complex *__ap,
        __CLPK_complex *__tau, __CLPK_complex *__q, __CLPK_integer *__ldq,
        __CLPK_complex *__work,
        __CLPK_integer *__info);


int cupmtr_(char *__side, char *__uplo, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_complex *__ap, __CLPK_complex *__tau,
        __CLPK_complex *__c__, __CLPK_integer *__ldc, __CLPK_complex *__work,
        __CLPK_integer *__info);


int dbdsdc_(char *__uplo, char *__compq, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__vt,
        __CLPK_integer *__ldvt, __CLPK_doublereal *__q, __CLPK_integer *__iq,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dbdsqr_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__ncvt,
        __CLPK_integer *__nru, __CLPK_integer *__ncc, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__vt, __CLPK_integer *__ldvt,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__c__,
        __CLPK_integer *__ldc, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int ddisna_(char *__job, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__sep,
        __CLPK_integer *__info);


int dgbbrd_(char *__vect, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__ncc, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_doublereal *__pt,
        __CLPK_integer *__ldpt, __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgbcon_(char *__norm, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgbequ_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__r__, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__rowcnd, __CLPK_doublereal *__colcnd,
        __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int dgbequb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__r__, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__rowcnd, __CLPK_doublereal *__colcnd,
        __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int dgbrfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_integer *__nrhs, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__afb,
        __CLPK_integer *__ldafb, __CLPK_integer *__ipiv, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgbsv_(__CLPK_integer *__n, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dgbsvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__kl, __CLPK_integer *__ku, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__afb, __CLPK_integer *__ldafb,
        __CLPK_integer *__ipiv, char *__equed, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgbtf2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int dgbtrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int dgbtrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_integer *__nrhs, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_integer *__ipiv, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dgebak_(char *__job, char *__side, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__scale, __CLPK_integer *__m, __CLPK_doublereal *__v,
        __CLPK_integer *__ldv,
        __CLPK_integer *__info);


int dgebal_(char *__job, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__scale,
        __CLPK_integer *__info);


int dgebd2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__tauq, __CLPK_doublereal *__taup,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgebrd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__tauq, __CLPK_doublereal *__taup,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgecon_(char *__norm, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgeequ_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rowcnd,
        __CLPK_doublereal *__colcnd, __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int dgeequb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rowcnd,
        __CLPK_doublereal *__colcnd, __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int dgees_(char *__jobvs, char *__sort, __CLPK_L_fp __select,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__sdim, __CLPK_doublereal *__wr,
        __CLPK_doublereal *__wi, __CLPK_doublereal *__vs,
        __CLPK_integer *__ldvs, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int dgeesx_(char *__jobvs, char *__sort, __CLPK_L_fp __select, char *__sense,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__sdim, __CLPK_doublereal *__wr,
        __CLPK_doublereal *__wi, __CLPK_doublereal *__vs,
        __CLPK_integer *__ldvs, __CLPK_doublereal *__rconde,
        __CLPK_doublereal *__rcondv, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int dgeev_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__wr,
        __CLPK_doublereal *__wi, __CLPK_doublereal *__vl,
        __CLPK_integer *__ldvl, __CLPK_doublereal *__vr, __CLPK_integer *__ldvr,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgeevx_(char *__balanc, char *__jobvl, char *__jobvr, char *__sense,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__wr, __CLPK_doublereal *__wi,
        __CLPK_doublereal *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublereal *__vr, __CLPK_integer *__ldvr, __CLPK_integer *__ilo,
        __CLPK_integer *__ihi, __CLPK_doublereal *__scale,
        __CLPK_doublereal *__abnrm, __CLPK_doublereal *__rconde,
        __CLPK_doublereal *__rcondv, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgegs_(char *__jobvsl, char *__jobvsr, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__alphar,
        __CLPK_doublereal *__alphai, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__vsl, __CLPK_integer *__ldvsl,
        __CLPK_doublereal *__vsr, __CLPK_integer *__ldvsr,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgegv_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__alphar,
        __CLPK_doublereal *__alphai, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublereal *__vr, __CLPK_integer *__ldvr,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgehd2_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgehrd_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgejsv_(char *__joba, char *__jobu, char *__jobv, char *__jobr,
        char *__jobt, char *__jobp, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__sva,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__v,
        __CLPK_integer *__ldv, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgelq2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgelqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgels_(char *__trans, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgelsd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__s,
        __CLPK_doublereal *__rcond, __CLPK_integer *__rank,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgelss_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__s,
        __CLPK_doublereal *__rcond, __CLPK_integer *__rank,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgelsx_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_integer *__jpvt,
        __CLPK_doublereal *__rcond, __CLPK_integer *__rank,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgelsy_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_integer *__jpvt,
        __CLPK_doublereal *__rcond, __CLPK_integer *__rank,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgeql2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgeqlf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgeqp3_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgeqpf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgeqr2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgeqrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgerfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgerq2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgerqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgesc2_(__CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__rhs, __CLPK_integer *__ipiv,
        __CLPK_integer *__jpiv,
        __CLPK_doublereal *__scale);


int dgesdd_(char *__jobz, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__vt,
        __CLPK_integer *__ldvt, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgesv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dgesvd_(char *__jobu, char *__jobvt, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__s, __CLPK_doublereal *__u, __CLPK_integer *__ldu,
        __CLPK_doublereal *__vt, __CLPK_integer *__ldvt,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgesvj_(char *__joba, char *__jobu, char *__jobv, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__sva, __CLPK_integer *__mv, __CLPK_doublereal *__v,
        __CLPK_integer *__ldv, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgesvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__af, __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        char *__equed, __CLPK_doublereal *__r__, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgetc2_(__CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_integer *__jpiv,
        __CLPK_integer *__info);


int dgetf2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int dgetrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int dgetri_(__CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgetrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dggbak_(char *__job, char *__side, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__lscale, __CLPK_doublereal *__rscale,
        __CLPK_integer *__m, __CLPK_doublereal *__v, __CLPK_integer *__ldv,
        __CLPK_integer *__info);


int dggbal_(char *__job, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__lscale, __CLPK_doublereal *__rscale,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgges_(char *__jobvsl, char *__jobvsr, char *__sort, __CLPK_L_fp __selctg,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_integer *__sdim,
        __CLPK_doublereal *__alphar, __CLPK_doublereal *__alphai,
        __CLPK_doublereal *__beta, __CLPK_doublereal *__vsl,
        __CLPK_integer *__ldvsl, __CLPK_doublereal *__vsr,
        __CLPK_integer *__ldvsr, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int dggesx_(char *__jobvsl, char *__jobvsr, char *__sort, __CLPK_L_fp __selctg,
        char *__sense, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__sdim, __CLPK_doublereal *__alphar,
        __CLPK_doublereal *__alphai, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__vsl, __CLPK_integer *__ldvsl,
        __CLPK_doublereal *__vsr, __CLPK_integer *__ldvsr,
        __CLPK_doublereal *__rconde, __CLPK_doublereal *__rcondv,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int dggev_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__alphar,
        __CLPK_doublereal *__alphai, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublereal *__vr, __CLPK_integer *__ldvr,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dggevx_(char *__balanc, char *__jobvl, char *__jobvr, char *__sense,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__alphar, __CLPK_doublereal *__alphai,
        __CLPK_doublereal *__beta, __CLPK_doublereal *__vl,
        __CLPK_integer *__ldvl, __CLPK_doublereal *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__lscale, __CLPK_doublereal *__rscale,
        __CLPK_doublereal *__abnrm, __CLPK_doublereal *__bbnrm,
        __CLPK_doublereal *__rconde, __CLPK_doublereal *__rcondv,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int dggglm_(__CLPK_integer *__n, __CLPK_integer *__m, __CLPK_integer *__p,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__d__, __CLPK_doublereal *__x,
        __CLPK_doublereal *__y, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgghrd_(char *__compq, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_doublereal *__z__,
        __CLPK_integer *__ldz,
        __CLPK_integer *__info);


int dgglse_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__p,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__x,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dggqrf_(__CLPK_integer *__n, __CLPK_integer *__m, __CLPK_integer *__p,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__taua, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__taub,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dggrqf_(__CLPK_integer *__m, __CLPK_integer *__p, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__taua, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__taub,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dggsvd_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__p, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__v,
        __CLPK_integer *__ldv, __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dggsvp_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__p, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__tola, __CLPK_doublereal *__tolb,
        __CLPK_integer *__k, __CLPK_integer *__l, __CLPK_doublereal *__u,
        __CLPK_integer *__ldu, __CLPK_doublereal *__v, __CLPK_integer *__ldv,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_integer *__iwork,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dgsvj0_(char *__jobv, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__sva, __CLPK_integer *__mv, __CLPK_doublereal *__v,
        __CLPK_integer *__ldv, __CLPK_doublereal *__eps,
        __CLPK_doublereal *__sfmin, __CLPK_doublereal *__tol,
        __CLPK_integer *__nsweep, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgsvj1_(char *__jobv, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__n1, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__sva,
        __CLPK_integer *__mv, __CLPK_doublereal *__v, __CLPK_integer *__ldv,
        __CLPK_doublereal *__eps, __CLPK_doublereal *__sfmin,
        __CLPK_doublereal *__tol, __CLPK_integer *__nsweep,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dgtcon_(char *__norm, __CLPK_integer *__n, __CLPK_doublereal *__dl,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__du,
        __CLPK_doublereal *__du2, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgtrfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__dl, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__du, __CLPK_doublereal *__dlf,
        __CLPK_doublereal *__df, __CLPK_doublereal *__duf,
        __CLPK_doublereal *__du2, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgtsv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_doublereal *__dl,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__du,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dgtsvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__dl,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__du,
        __CLPK_doublereal *__dlf, __CLPK_doublereal *__df,
        __CLPK_doublereal *__duf, __CLPK_doublereal *__du2,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dgttrf_(__CLPK_integer *__n, __CLPK_doublereal *__dl,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__du,
        __CLPK_doublereal *__du2, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int dgttrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__dl, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__du, __CLPK_doublereal *__du2,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dgtts2_(__CLPK_integer *__itrans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__dl,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__du,
        __CLPK_doublereal *__du2, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__b,
        __CLPK_integer *__ldb);


int dhgeqz_(char *__job, char *__compq, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_doublereal *__h__,
        __CLPK_integer *__ldh, __CLPK_doublereal *__t, __CLPK_integer *__ldt,
        __CLPK_doublereal *__alphar, __CLPK_doublereal *__alphai,
        __CLPK_doublereal *__beta, __CLPK_doublereal *__q,
        __CLPK_integer *__ldq, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dhsein_(char *__side, char *__eigsrc, char *__initv,
        __CLPK_logical *__select, __CLPK_integer *__n, __CLPK_doublereal *__h__,
        __CLPK_integer *__ldh, __CLPK_doublereal *__wr, __CLPK_doublereal *__wi,
        __CLPK_doublereal *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublereal *__vr, __CLPK_integer *__ldvr, __CLPK_integer *__mm,
        __CLPK_integer *__m, __CLPK_doublereal *__work,
        __CLPK_integer *__ifaill, __CLPK_integer *__ifailr,
        __CLPK_integer *__info);


int dhseqr_(char *__job, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_doublereal *__h__,
        __CLPK_integer *__ldh, __CLPK_doublereal *__wr, __CLPK_doublereal *__wi,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);



        __CLPK_logical disnan_(__CLPK_doublereal *__din);


int dlabad_(__CLPK_doublereal *__small,
        __CLPK_doublereal *__large);


int dlabrd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__tauq,
        __CLPK_doublereal *__taup, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__y,
        __CLPK_integer *__ldy);


int dlacn2_(__CLPK_integer *__n, __CLPK_doublereal *__v, __CLPK_doublereal *__x,
        __CLPK_integer *__isgn, __CLPK_doublereal *__est,
        __CLPK_integer *__kase,
        __CLPK_integer *__isave);


int dlacon_(__CLPK_integer *__n, __CLPK_doublereal *__v, __CLPK_doublereal *__x,
        __CLPK_integer *__isgn, __CLPK_doublereal *__est,
        __CLPK_integer *__kase);


int dlacpy_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb);


int dladiv_(__CLPK_doublereal *__a, __CLPK_doublereal *__b,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__p,
        __CLPK_doublereal *__q);


int dlae2_(__CLPK_doublereal *__a, __CLPK_doublereal *__b,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rt1,
        __CLPK_doublereal *__rt2);


int dlaebz_(__CLPK_integer *__ijob, __CLPK_integer *__nitmax,
        __CLPK_integer *__n, __CLPK_integer *__mmax, __CLPK_integer *__minp,
        __CLPK_integer *__nbmin, __CLPK_doublereal *__abstol,
        __CLPK_doublereal *__reltol, __CLPK_doublereal *__pivmin,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__e2, __CLPK_integer *__nval,
        __CLPK_doublereal *__ab, __CLPK_doublereal *__c__,
        __CLPK_integer *__mout, __CLPK_integer *__nab,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlaed0_(__CLPK_integer *__icompq, __CLPK_integer *__qsiz,
        __CLPK_integer *__n, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_doublereal *__qstore, __CLPK_integer *__ldqs,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlaed1_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_integer *__indxq,
        __CLPK_doublereal *__rho, __CLPK_integer *__cutpnt,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlaed2_(__CLPK_integer *__k, __CLPK_integer *__n, __CLPK_integer *__n1,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__indxq, __CLPK_doublereal *__rho,
        __CLPK_doublereal *__z__, __CLPK_doublereal *__dlamda,
        __CLPK_doublereal *__w, __CLPK_doublereal *__q2, __CLPK_integer *__indx,
        __CLPK_integer *__indxc, __CLPK_integer *__indxp,
        __CLPK_integer *__coltyp,
        __CLPK_integer *__info);


int dlaed3_(__CLPK_integer *__k, __CLPK_integer *__n, __CLPK_integer *__n1,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_doublereal *__rho, __CLPK_doublereal *__dlamda,
        __CLPK_doublereal *__q2, __CLPK_integer *__indx, __CLPK_integer *__ctot,
        __CLPK_doublereal *__w, __CLPK_doublereal *__s,
        __CLPK_integer *__info);


int dlaed4_(__CLPK_integer *__n, __CLPK_integer *__i__,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__delta, __CLPK_doublereal *__rho,
        __CLPK_doublereal *__dlam,
        __CLPK_integer *__info);


int dlaed5_(__CLPK_integer *__i__, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__z__, __CLPK_doublereal *__delta,
        __CLPK_doublereal *__rho,
        __CLPK_doublereal *__dlam);


int dlaed6_(__CLPK_integer *__kniter, __CLPK_logical *__orgati,
        __CLPK_doublereal *__rho, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__z__, __CLPK_doublereal *__finit,
        __CLPK_doublereal *__tau,
        __CLPK_integer *__info);


int dlaed7_(__CLPK_integer *__icompq, __CLPK_integer *__n,
        __CLPK_integer *__qsiz, __CLPK_integer *__tlvls,
        __CLPK_integer *__curlvl, __CLPK_integer *__curpbm,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__indxq, __CLPK_doublereal *__rho,
        __CLPK_integer *__cutpnt, __CLPK_doublereal *__qstore,
        __CLPK_integer *__qptr, __CLPK_integer *__prmptr,
        __CLPK_integer *__perm, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_doublereal *__givnum,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlaed8_(__CLPK_integer *__icompq, __CLPK_integer *__k, __CLPK_integer *__n,
        __CLPK_integer *__qsiz, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_integer *__indxq,
        __CLPK_doublereal *__rho, __CLPK_integer *__cutpnt,
        __CLPK_doublereal *__z__, __CLPK_doublereal *__dlamda,
        __CLPK_doublereal *__q2, __CLPK_integer *__ldq2, __CLPK_doublereal *__w,
        __CLPK_integer *__perm, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_doublereal *__givnum,
        __CLPK_integer *__indxp, __CLPK_integer *__indx,
        __CLPK_integer *__info);


int dlaed9_(__CLPK_integer *__k, __CLPK_integer *__kstart,
        __CLPK_integer *__kstop, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_doublereal *__rho,
        __CLPK_doublereal *__dlamda, __CLPK_doublereal *__w,
        __CLPK_doublereal *__s, __CLPK_integer *__lds,
        __CLPK_integer *__info);


int dlaeda_(__CLPK_integer *__n, __CLPK_integer *__tlvls,
        __CLPK_integer *__curlvl, __CLPK_integer *__curpbm,
        __CLPK_integer *__prmptr, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_doublereal *__givnum, __CLPK_doublereal *__q,
        __CLPK_integer *__qptr, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__ztemp,
        __CLPK_integer *__info);


int dlaein_(__CLPK_logical *__rightv, __CLPK_logical *__noinit,
        __CLPK_integer *__n, __CLPK_doublereal *__h__, __CLPK_integer *__ldh,
        __CLPK_doublereal *__wr, __CLPK_doublereal *__wi,
        __CLPK_doublereal *__vr, __CLPK_doublereal *__vi,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__work, __CLPK_doublereal *__eps3,
        __CLPK_doublereal *__smlnum, __CLPK_doublereal *__bignum,
        __CLPK_integer *__info);


int dlaev2_(__CLPK_doublereal *__a, __CLPK_doublereal *__b,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rt1,
        __CLPK_doublereal *__rt2, __CLPK_doublereal *__cs1,
        __CLPK_doublereal *__sn1);


int dlaexc_(__CLPK_logical *__wantq, __CLPK_integer *__n,
        __CLPK_doublereal *__t, __CLPK_integer *__ldt, __CLPK_doublereal *__q,
        __CLPK_integer *__ldq, __CLPK_integer *__j1, __CLPK_integer *__n1,
        __CLPK_integer *__n2, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlag2_(__CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__safmin, __CLPK_doublereal *__scale1,
        __CLPK_doublereal *__scale2, __CLPK_doublereal *__wr1,
        __CLPK_doublereal *__wr2,
        __CLPK_doublereal *__wi);


int dlag2s_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_real *__sa, __CLPK_integer *__ldsa,
        __CLPK_integer *__info);


int dlags2_(__CLPK_logical *__upper, __CLPK_doublereal *__a1,
        __CLPK_doublereal *__a2, __CLPK_doublereal *__a3,
        __CLPK_doublereal *__b1, __CLPK_doublereal *__b2,
        __CLPK_doublereal *__b3, __CLPK_doublereal *__csu,
        __CLPK_doublereal *__snu, __CLPK_doublereal *__csv,
        __CLPK_doublereal *__snv, __CLPK_doublereal *__csq,
        __CLPK_doublereal *__snq);


int dlagtf_(__CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_doublereal *__lambda, __CLPK_doublereal *__b,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__tol,
        __CLPK_doublereal *__d__, __CLPK_integer *__in,
        __CLPK_integer *__info);


int dlagtm_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__dl,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__du,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__beta, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb);


int dlagts_(__CLPK_integer *__job, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_doublereal *__b, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__d__, __CLPK_integer *__in, __CLPK_doublereal *__y,
        __CLPK_doublereal *__tol,
        __CLPK_integer *__info);


int dlagv2_(__CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__alphar, __CLPK_doublereal *__alphai,
        __CLPK_doublereal *__beta, __CLPK_doublereal *__csl,
        __CLPK_doublereal *__snl, __CLPK_doublereal *__csr,
        __CLPK_doublereal *__snr);


int dlahqr_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__h__, __CLPK_integer *__ldh,
        __CLPK_doublereal *__wr, __CLPK_doublereal *__wi,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__info);


int dlahr2_(__CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__nb,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__t, __CLPK_integer *__ldt, __CLPK_doublereal *__y,
        __CLPK_integer *__ldy);


int dlahrd_(__CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__nb,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__t, __CLPK_integer *__ldt, __CLPK_doublereal *__y,
        __CLPK_integer *__ldy);


int dlaic1_(__CLPK_integer *__job, __CLPK_integer *__j, __CLPK_doublereal *__x,
        __CLPK_doublereal *__sest, __CLPK_doublereal *__w,
        __CLPK_doublereal *__gamma, __CLPK_doublereal *__sestpr,
        __CLPK_doublereal *__s,
        __CLPK_doublereal *__c__);


__CLPK_logical dlaisnan_(__CLPK_doublereal *__din1,
        __CLPK_doublereal *__din2);


int dlaln2_(__CLPK_logical *__ltrans, __CLPK_integer *__na,
        __CLPK_integer *__nw, __CLPK_doublereal *__smin,
        __CLPK_doublereal *__ca, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__d1, __CLPK_doublereal *__d2,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__wr,
        __CLPK_doublereal *__wi, __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__scale, __CLPK_doublereal *__xnorm,
        __CLPK_integer *__info);


int dlals0_(__CLPK_integer *__icompq, __CLPK_integer *__nl,
        __CLPK_integer *__nr, __CLPK_integer *__sqre, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__bx,
        __CLPK_integer *__ldbx, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_integer *__ldgcol, __CLPK_doublereal *__givnum,
        __CLPK_integer *__ldgnum, __CLPK_doublereal *__poles,
        __CLPK_doublereal *__difl, __CLPK_doublereal *__difr,
        __CLPK_doublereal *__z__, __CLPK_integer *__k, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__s, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlalsa_(__CLPK_integer *__icompq, __CLPK_integer *__smlsiz,
        __CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__bx, __CLPK_integer *__ldbx,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__vt,
        __CLPK_integer *__k, __CLPK_doublereal *__difl,
        __CLPK_doublereal *__difr, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__poles, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_integer *__ldgcol,
        __CLPK_integer *__perm, __CLPK_doublereal *__givnum,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlalsd_(char *__uplo, __CLPK_integer *__smlsiz, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__rcond, __CLPK_integer *__rank,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlamrg_(__CLPK_integer *__n1, __CLPK_integer *__n2, __CLPK_doublereal *__a,
        __CLPK_integer *__dtrd1, __CLPK_integer *__dtrd2,
        __CLPK_integer *__index);


__CLPK_integer dlaneg_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__lld, __CLPK_doublereal *__sigma,
        __CLPK_doublereal *__pivmin,
        __CLPK_integer *__r__);


__CLPK_doublereal dlangb_(char *__norm, __CLPK_integer *__n,
        __CLPK_integer *__kl, __CLPK_integer *__ku, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlange_(char *__norm, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlangt_(char *__norm, __CLPK_integer *__n,
        __CLPK_doublereal *__dl, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__du);


__CLPK_doublereal dlanhs_(char *__norm, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlansb_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlansf_(char *__norm, char *__transr, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlansp_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__ap,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlanst_(char *__norm, __CLPK_integer *__n,
        __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e);


__CLPK_doublereal dlansy_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlantb_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlantp_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__work);


__CLPK_doublereal dlantr_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


int dlanv2_(__CLPK_doublereal *__a, __CLPK_doublereal *__b,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__rt1r, __CLPK_doublereal *__rt1i,
        __CLPK_doublereal *__rt2r, __CLPK_doublereal *__rt2i,
        __CLPK_doublereal *__cs,
        __CLPK_doublereal *__sn);


int dlapll_(__CLPK_integer *__n, __CLPK_doublereal *__x, __CLPK_integer *__incx,
        __CLPK_doublereal *__y, __CLPK_integer *__incy,
        __CLPK_doublereal *__ssmin);


int dlapmt_(__CLPK_logical *__forwrd, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_integer *__k);


__CLPK_doublereal dlapy2_(__CLPK_doublereal *__x,
        __CLPK_doublereal *__y);


__CLPK_doublereal dlapy3_(__CLPK_doublereal *__x, __CLPK_doublereal *__y,
        __CLPK_doublereal *__z__);


int dlaqgb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__r__, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__rowcnd, __CLPK_doublereal *__colcnd,
        __CLPK_doublereal *__amax,
        char *__equed);


int dlaqge_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rowcnd,
        __CLPK_doublereal *__colcnd, __CLPK_doublereal *__amax,
        char *__equed);


int dlaqp2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__offset,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_integer *__jpvt,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__vn1,
        __CLPK_doublereal *__vn2,
        __CLPK_doublereal *__work);


int dlaqps_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__offset,
        __CLPK_integer *__nb, __CLPK_integer *__kb, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__vn1, __CLPK_doublereal *__vn2,
        __CLPK_doublereal *__auxv, __CLPK_doublereal *__f,
        __CLPK_integer *__ldf);


int dlaqr0_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__h__, __CLPK_integer *__ldh,
        __CLPK_doublereal *__wr, __CLPK_doublereal *__wi,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dlaqr1_(__CLPK_integer *__n, __CLPK_doublereal *__h__,
        __CLPK_integer *__ldh, __CLPK_doublereal *__sr1,
        __CLPK_doublereal *__si1, __CLPK_doublereal *__sr2,
        __CLPK_doublereal *__si2,
        __CLPK_doublereal *__v);


int dlaqr2_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ktop, __CLPK_integer *__kbot,
        __CLPK_integer *__nw, __CLPK_doublereal *__h__, __CLPK_integer *__ldh,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz, __CLPK_integer *__ns,
        __CLPK_integer *__nd, __CLPK_doublereal *__sr, __CLPK_doublereal *__si,
        __CLPK_doublereal *__v, __CLPK_integer *__ldv, __CLPK_integer *__nh,
        __CLPK_doublereal *__t, __CLPK_integer *__ldt, __CLPK_integer *__nv,
        __CLPK_doublereal *__wv, __CLPK_integer *__ldwv,
        __CLPK_doublereal *__work,
        __CLPK_integer *__lwork);


int dlaqr3_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ktop, __CLPK_integer *__kbot,
        __CLPK_integer *__nw, __CLPK_doublereal *__h__, __CLPK_integer *__ldh,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz, __CLPK_integer *__ns,
        __CLPK_integer *__nd, __CLPK_doublereal *__sr, __CLPK_doublereal *__si,
        __CLPK_doublereal *__v, __CLPK_integer *__ldv, __CLPK_integer *__nh,
        __CLPK_doublereal *__t, __CLPK_integer *__ldt, __CLPK_integer *__nv,
        __CLPK_doublereal *__wv, __CLPK_integer *__ldwv,
        __CLPK_doublereal *__work,
        __CLPK_integer *__lwork);


int dlaqr4_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__h__, __CLPK_integer *__ldh,
        __CLPK_doublereal *__wr, __CLPK_doublereal *__wi,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dlaqr5_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__kacc22, __CLPK_integer *__n, __CLPK_integer *__ktop,
        __CLPK_integer *__kbot, __CLPK_integer *__nshfts,
        __CLPK_doublereal *__sr, __CLPK_doublereal *__si,
        __CLPK_doublereal *__h__, __CLPK_integer *__ldh, __CLPK_integer *__iloz,
        __CLPK_integer *__ihiz, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__v, __CLPK_integer *__ldv, __CLPK_doublereal *__u,
        __CLPK_integer *__ldu, __CLPK_integer *__nv, __CLPK_doublereal *__wv,
        __CLPK_integer *__ldwv, __CLPK_integer *__nh, __CLPK_doublereal *__wh,
        __CLPK_integer *__ldwh);


int dlaqsb_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublereal *__ab, __CLPK_integer *__ldab, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        char *__equed);


int dlaqsp_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        char *__equed);


int dlaqsy_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        char *__equed);


int dlaqtr_(__CLPK_logical *__ltran, __CLPK_logical *__l__CLPK_real,
        __CLPK_integer *__n, __CLPK_doublereal *__t, __CLPK_integer *__ldt,
        __CLPK_doublereal *__b, __CLPK_doublereal *__w,
        __CLPK_doublereal *__scale, __CLPK_doublereal *__x,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlar1v_(__CLPK_integer *__n, __CLPK_integer *__b1, __CLPK_integer *__bn,
        __CLPK_doublereal *__lambda, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__l, __CLPK_doublereal *__ld,
        __CLPK_doublereal *__lld, __CLPK_doublereal *__pivmin,
        __CLPK_doublereal *__gaptol, __CLPK_doublereal *__z__,
        __CLPK_logical *__wantnc, __CLPK_integer *__negcnt,
        __CLPK_doublereal *__ztz, __CLPK_doublereal *__mingma,
        __CLPK_integer *__r__, __CLPK_integer *__isuppz,
        __CLPK_doublereal *__nrminv, __CLPK_doublereal *__resid,
        __CLPK_doublereal *__rqcorr,
        __CLPK_doublereal *__work);


int dlar2v_(__CLPK_integer *__n, __CLPK_doublereal *__x, __CLPK_doublereal *__y,
        __CLPK_doublereal *__z__, __CLPK_integer *__incx,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_integer *__incc);


int dlarf_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__v, __CLPK_integer *__incv,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__c__,
        __CLPK_integer *__ldc,
        __CLPK_doublereal *__work);


int dlarfb_(char *__side, char *__trans, char *__direct, char *__storev,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__v, __CLPK_integer *__ldv, __CLPK_doublereal *__t,
        __CLPK_integer *__ldt, __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__ldwork);


int dlarfg_(__CLPK_integer *__n, __CLPK_doublereal *__alpha,
        __CLPK_doublereal *__x, __CLPK_integer *__incx,
        __CLPK_doublereal *__tau);


int dlarfp_(__CLPK_integer *__n, __CLPK_doublereal *__alpha,
        __CLPK_doublereal *__x, __CLPK_integer *__incx,
        __CLPK_doublereal *__tau);


int dlarft_(char *__direct, char *__storev, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublereal *__v, __CLPK_integer *__ldv,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__t,
        __CLPK_integer *__ldt);


int dlarfx_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__v, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work);


int dlargv_(__CLPK_integer *__n, __CLPK_doublereal *__x, __CLPK_integer *__incx,
        __CLPK_doublereal *__y, __CLPK_integer *__incy,
        __CLPK_doublereal *__c__,
        __CLPK_integer *__incc);


int dlarnv_(__CLPK_integer *__idist, __CLPK_integer *__iseed,
        __CLPK_integer *__n,
        __CLPK_doublereal *__x);


int dlarra_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__e2,
        __CLPK_doublereal *__spltol, __CLPK_doublereal *__tnrm,
        __CLPK_integer *__nsplit, __CLPK_integer *__isplit,
        __CLPK_integer *__info);


int dlarrb_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__lld, __CLPK_integer *__ifirst,
        __CLPK_integer *__ilast, __CLPK_doublereal *__rtol1,
        __CLPK_doublereal *__rtol2, __CLPK_integer *__offset,
        __CLPK_doublereal *__w, __CLPK_doublereal *__wgap,
        __CLPK_doublereal *__werr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork, __CLPK_doublereal *__pivmin,
        __CLPK_doublereal *__spdiam, __CLPK_integer *__twist,
        __CLPK_integer *__info);


int dlarrc_(char *__jobt, __CLPK_integer *__n, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__pivmin,
        __CLPK_integer *__eigcnt, __CLPK_integer *__lcnt,
        __CLPK_integer *__rcnt,
        __CLPK_integer *__info);


int dlarrd_(char *__range, char *__order, __CLPK_integer *__n,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__gers,
        __CLPK_doublereal *__reltol, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__e2,
        __CLPK_doublereal *__pivmin, __CLPK_integer *__nsplit,
        __CLPK_integer *__isplit, __CLPK_integer *__m, __CLPK_doublereal *__w,
        __CLPK_doublereal *__werr, __CLPK_doublereal *__wl,
        __CLPK_doublereal *__wu, __CLPK_integer *__iblock,
        __CLPK_integer *__indexw, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlarre_(char *__range, __CLPK_integer *__n, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__e2, __CLPK_doublereal *__rtol1,
        __CLPK_doublereal *__rtol2, __CLPK_doublereal *__spltol,
        __CLPK_integer *__nsplit, __CLPK_integer *__isplit, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__werr,
        __CLPK_doublereal *__wgap, __CLPK_integer *__iblock,
        __CLPK_integer *__indexw, __CLPK_doublereal *__gers,
        __CLPK_doublereal *__pivmin, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlarrf_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__l, __CLPK_doublereal *__ld,
        __CLPK_integer *__clstrt, __CLPK_integer *__clend,
        __CLPK_doublereal *__w, __CLPK_doublereal *__wgap,
        __CLPK_doublereal *__werr, __CLPK_doublereal *__spdiam,
        __CLPK_doublereal *__clgapl, __CLPK_doublereal *__clgapr,
        __CLPK_doublereal *__pivmin, __CLPK_doublereal *__sigma,
        __CLPK_doublereal *__dplus, __CLPK_doublereal *__lplus,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlarrj_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e2, __CLPK_integer *__ifirst,
        __CLPK_integer *__ilast, __CLPK_doublereal *__rtol,
        __CLPK_integer *__offset, __CLPK_doublereal *__w,
        __CLPK_doublereal *__werr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork, __CLPK_doublereal *__pivmin,
        __CLPK_doublereal *__spdiam,
        __CLPK_integer *__info);


int dlarrk_(__CLPK_integer *__n, __CLPK_integer *__iw, __CLPK_doublereal *__gl,
        __CLPK_doublereal *__gu, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e2, __CLPK_doublereal *__pivmin,
        __CLPK_doublereal *__reltol, __CLPK_doublereal *__w,
        __CLPK_doublereal *__werr,
        __CLPK_integer *__info);


int dlarrr_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e,
        __CLPK_integer *__info);


int dlarrv_(__CLPK_integer *__n, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__l, __CLPK_doublereal *__pivmin,
        __CLPK_integer *__isplit, __CLPK_integer *__m, __CLPK_integer *__dol,
        __CLPK_integer *__dou, __CLPK_doublereal *__minrgp,
        __CLPK_doublereal *__rtol1, __CLPK_doublereal *__rtol2,
        __CLPK_doublereal *__w, __CLPK_doublereal *__werr,
        __CLPK_doublereal *__wgap, __CLPK_integer *__iblock,
        __CLPK_integer *__indexw, __CLPK_doublereal *__gers,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__isuppz, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlarscl2_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx);


int dlartg_(__CLPK_doublereal *__f, __CLPK_doublereal *__g,
        __CLPK_doublereal *__cs, __CLPK_doublereal *__sn,
        __CLPK_doublereal *__r__);


int dlartv_(__CLPK_integer *__n, __CLPK_doublereal *__x, __CLPK_integer *__incx,
        __CLPK_doublereal *__y, __CLPK_integer *__incy,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_integer *__incc);


int dlaruv_(__CLPK_integer *__iseed, __CLPK_integer *__n,
        __CLPK_doublereal *__x);


int dlarz_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__l, __CLPK_doublereal *__v, __CLPK_integer *__incv,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__c__,
        __CLPK_integer *__ldc,
        __CLPK_doublereal *__work);


int dlarzb_(char *__side, char *__trans, char *__direct, char *__storev,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_doublereal *__v, __CLPK_integer *__ldv,
        __CLPK_doublereal *__t, __CLPK_integer *__ldt, __CLPK_doublereal *__c__,
        __CLPK_integer *__ldc, __CLPK_doublereal *__work,
        __CLPK_integer *__ldwork);


int dlarzt_(char *__direct, char *__storev, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublereal *__v, __CLPK_integer *__ldv,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__t,
        __CLPK_integer *__ldt);


int dlas2_(__CLPK_doublereal *__f, __CLPK_doublereal *__g,
        __CLPK_doublereal *__h__, __CLPK_doublereal *__ssmin,
        __CLPK_doublereal *__ssmax);


int dlascl_(char *__type__, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_doublereal *__cfrom, __CLPK_doublereal *__cto,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dlascl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__x,
        __CLPK_integer *__ldx);


int dlasd0_(__CLPK_integer *__n, __CLPK_integer *__sqre,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__vt,
        __CLPK_integer *__ldvt, __CLPK_integer *__smlsiz,
        __CLPK_integer *__iwork, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlasd1_(__CLPK_integer *__nl, __CLPK_integer *__nr, __CLPK_integer *__sqre,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__alpha,
        __CLPK_doublereal *__beta, __CLPK_doublereal *__u,
        __CLPK_integer *__ldu, __CLPK_doublereal *__vt, __CLPK_integer *__ldvt,
        __CLPK_integer *__idxq, __CLPK_integer *__iwork,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlasd2_(__CLPK_integer *__nl, __CLPK_integer *__nr, __CLPK_integer *__sqre,
        __CLPK_integer *__k, __CLPK_doublereal *__d__, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__vt,
        __CLPK_integer *__ldvt, __CLPK_doublereal *__dsigma,
        __CLPK_doublereal *__u2, __CLPK_integer *__ldu2,
        __CLPK_doublereal *__vt2, __CLPK_integer *__ldvt2,
        __CLPK_integer *__idxp, __CLPK_integer *__idx, __CLPK_integer *__idxc,
        __CLPK_integer *__idxq, __CLPK_integer *__coltyp,
        __CLPK_integer *__info);


int dlasd3_(__CLPK_integer *__nl, __CLPK_integer *__nr, __CLPK_integer *__sqre,
        __CLPK_integer *__k, __CLPK_doublereal *__d__, __CLPK_doublereal *__q,
        __CLPK_integer *__ldq, __CLPK_doublereal *__dsigma,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__u2,
        __CLPK_integer *__ldu2, __CLPK_doublereal *__vt, __CLPK_integer *__ldvt,
        __CLPK_doublereal *__vt2, __CLPK_integer *__ldvt2,
        __CLPK_integer *__idxc, __CLPK_integer *__ctot,
        __CLPK_doublereal *__z__,
        __CLPK_integer *__info);


int dlasd4_(__CLPK_integer *__n, __CLPK_integer *__i__,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__delta, __CLPK_doublereal *__rho,
        __CLPK_doublereal *__sigma, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlasd5_(__CLPK_integer *__i__, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__z__, __CLPK_doublereal *__delta,
        __CLPK_doublereal *__rho, __CLPK_doublereal *__dsigma,
        __CLPK_doublereal *__work);


int dlasd6_(__CLPK_integer *__icompq, __CLPK_integer *__nl,
        __CLPK_integer *__nr, __CLPK_integer *__sqre, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__vf, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__beta,
        __CLPK_integer *__idxq, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_integer *__ldgcol, __CLPK_doublereal *__givnum,
        __CLPK_integer *__ldgnum, __CLPK_doublereal *__poles,
        __CLPK_doublereal *__difl, __CLPK_doublereal *__difr,
        __CLPK_doublereal *__z__, __CLPK_integer *__k, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__s, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlasd7_(__CLPK_integer *__icompq, __CLPK_integer *__nl,
        __CLPK_integer *__nr, __CLPK_integer *__sqre, __CLPK_integer *__k,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__zw, __CLPK_doublereal *__vf,
        __CLPK_doublereal *__vfw, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vlw, __CLPK_doublereal *__alpha,
        __CLPK_doublereal *__beta, __CLPK_doublereal *__dsigma,
        __CLPK_integer *__idx, __CLPK_integer *__idxp, __CLPK_integer *__idxq,
        __CLPK_integer *__perm, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_integer *__ldgcol,
        __CLPK_doublereal *__givnum, __CLPK_integer *__ldgnum,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_integer *__info);


int dlasd8_(__CLPK_integer *__icompq, __CLPK_integer *__k,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__vf, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__difl, __CLPK_doublereal *__difr,
        __CLPK_integer *__lddifr, __CLPK_doublereal *__dsigma,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlasda_(__CLPK_integer *__icompq, __CLPK_integer *__smlsiz,
        __CLPK_integer *__n, __CLPK_integer *__sqre, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__u, __CLPK_integer *__ldu,
        __CLPK_doublereal *__vt, __CLPK_integer *__k, __CLPK_doublereal *__difl,
        __CLPK_doublereal *__difr, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__poles, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_integer *__ldgcol,
        __CLPK_integer *__perm, __CLPK_doublereal *__givnum,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dlasdq_(char *__uplo, __CLPK_integer *__sqre, __CLPK_integer *__n,
        __CLPK_integer *__ncvt, __CLPK_integer *__nru, __CLPK_integer *__ncc,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__vt, __CLPK_integer *__ldvt, __CLPK_doublereal *__u,
        __CLPK_integer *__ldu, __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlasdt_(__CLPK_integer *__n, __CLPK_integer *__lvl, __CLPK_integer *__nd,
        __CLPK_integer *__inode, __CLPK_integer *__ndiml,
        __CLPK_integer *__ndimr,
        __CLPK_integer *__msub);


int dlaset_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__a,
        __CLPK_integer *__lda);


int dlasq1_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dlasq2_(__CLPK_integer *__n, __CLPK_doublereal *__z__,
        __CLPK_integer *__info);


int dlasq3_(__CLPK_integer *__i0, __CLPK_integer *__n0,
        __CLPK_doublereal *__z__, __CLPK_integer *__pp,
        __CLPK_doublereal *__dmin__, __CLPK_doublereal *__sigma,
        __CLPK_doublereal *__desig, __CLPK_doublereal *__qmax,
        __CLPK_integer *__nfail, __CLPK_integer *__iter, __CLPK_integer *__ndiv,
        __CLPK_logical *__ieee, __CLPK_integer *__ttype,
        __CLPK_doublereal *__dmin1, __CLPK_doublereal *__dmin2,
        __CLPK_doublereal *__dn, __CLPK_doublereal *__dn1,
        __CLPK_doublereal *__dn2, __CLPK_doublereal *__g,
        __CLPK_doublereal *__tau);


int dlasq4_(__CLPK_integer *__i0, __CLPK_integer *__n0,
        __CLPK_doublereal *__z__, __CLPK_integer *__pp, __CLPK_integer *__n0in,
        __CLPK_doublereal *__dmin__, __CLPK_doublereal *__dmin1,
        __CLPK_doublereal *__dmin2, __CLPK_doublereal *__dn,
        __CLPK_doublereal *__dn1, __CLPK_doublereal *__dn2,
        __CLPK_doublereal *__tau, __CLPK_integer *__ttype,
        __CLPK_doublereal *__g);


int dlasq5_(__CLPK_integer *__i0, __CLPK_integer *__n0,
        __CLPK_doublereal *__z__, __CLPK_integer *__pp,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__dmin__,
        __CLPK_doublereal *__dmin1, __CLPK_doublereal *__dmin2,
        __CLPK_doublereal *__dn, __CLPK_doublereal *__dnm1,
        __CLPK_doublereal *__dnm2,
        __CLPK_logical *__ieee);


int dlasq6_(__CLPK_integer *__i0, __CLPK_integer *__n0,
        __CLPK_doublereal *__z__, __CLPK_integer *__pp,
        __CLPK_doublereal *__dmin__, __CLPK_doublereal *__dmin1,
        __CLPK_doublereal *__dmin2, __CLPK_doublereal *__dn,
        __CLPK_doublereal *__dnm1,
        __CLPK_doublereal *__dnm2);


int dlasr_(char *__side, char *__pivot, char *__direct, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_doublereal *__a,
        __CLPK_integer *__lda);


int dlasrt_(char *__id, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_integer *__info);


int dlassq_(__CLPK_integer *__n, __CLPK_doublereal *__x, __CLPK_integer *__incx,
        __CLPK_doublereal *__scale,
        __CLPK_doublereal *__sumsq);


int dlasv2_(__CLPK_doublereal *__f, __CLPK_doublereal *__g,
        __CLPK_doublereal *__h__, __CLPK_doublereal *__ssmin,
        __CLPK_doublereal *__ssmax, __CLPK_doublereal *__snr,
        __CLPK_doublereal *__csr, __CLPK_doublereal *__snl,
        __CLPK_doublereal *__csl);


int dlaswp_(__CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__k1, __CLPK_integer *__k2, __CLPK_integer *__ipiv,
        __CLPK_integer *__incx);


int dlasy2_(__CLPK_logical *__ltranl, __CLPK_logical *__ltranr,
        __CLPK_integer *__isgn, __CLPK_integer *__n1, __CLPK_integer *__n2,
        __CLPK_doublereal *__tl, __CLPK_integer *__ldtl,
        __CLPK_doublereal *__tr, __CLPK_integer *__ldtr, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__scale,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__xnorm,
        __CLPK_integer *__info);


int dlasyf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_integer *__kb, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__w, __CLPK_integer *__ldw,
        __CLPK_integer *__info);


int dlat2s_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_real *__sa, __CLPK_integer *__ldsa,
        __CLPK_integer *__info);


int dlatbs_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_integer *__kd, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__x,
        __CLPK_doublereal *__scale, __CLPK_doublereal *__cnorm,
        __CLPK_integer *__info);


int dlatdf_(__CLPK_integer *__ijob, __CLPK_integer *__n,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__rhs, __CLPK_doublereal *__rdsum,
        __CLPK_doublereal *__rdscal, __CLPK_integer *__ipiv,
        __CLPK_integer *__jpiv);


int dlatps_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_doublereal *__ap, __CLPK_doublereal *__x,
        __CLPK_doublereal *__scale, __CLPK_doublereal *__cnorm,
        __CLPK_integer *__info);


int dlatrd_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__e,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__w,
        __CLPK_integer *__ldw);


int dlatrs_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__x, __CLPK_doublereal *__scale,
        __CLPK_doublereal *__cnorm,
        __CLPK_integer *__info);


int dlatrz_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__l,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work);


int dlatzm_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__v, __CLPK_integer *__incv,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__c1,
        __CLPK_doublereal *__c2, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work);


int dlauu2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dlauum_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dopgtr_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dopmtr_(char *__side, char *__uplo, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__ap, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dorg2l_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dorg2r_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dorgbr_(char *__vect, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dorghr_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dorgl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dorglq_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dorgql_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dorgqr_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dorgr2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dorgrq_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dorgtr_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dorm2l_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dorm2r_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dormbr_(char *__vect, char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dormhr_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dorml2_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dormlq_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dormql_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dormqr_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dormr2_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dormr3_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__l,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dormrq_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dormrz_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__l,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dormtr_(char *__side, char *__uplo, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__c__,
        __CLPK_integer *__ldc, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dpbcon_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dpbequ_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublereal *__ab, __CLPK_integer *__ldab, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int dpbrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__afb, __CLPK_integer *__ldafb,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dpbstf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int dpbsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dpbsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__afb,
        __CLPK_integer *__ldafb, char *__equed, __CLPK_doublereal *__s,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dpbtf2_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int dpbtrf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int dpbtrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dpftrf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a,
        __CLPK_integer *__info);


int dpftri_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a,
        __CLPK_integer *__info);


int dpftrs_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__a, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dpocon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dpoequ_(__CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int dpoequb_(__CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int dporfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__af,
        __CLPK_integer *__ldaf, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dposv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dposvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__af, __CLPK_integer *__ldaf, char *__equed,
        __CLPK_doublereal *__s, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dpotf2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dpotrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dpotri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dpotrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dppcon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dppequ_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int dpprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__afp,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dppsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dppsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__afp, char *__equed, __CLPK_doublereal *__s,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dpptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_integer *__info);


int dpptri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_integer *__info);


int dpptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dpstf2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__piv, __CLPK_integer *__rank,
        __CLPK_doublereal *__tol, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dpstrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__piv, __CLPK_integer *__rank,
        __CLPK_doublereal *__tol, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dptcon_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dpteqr_(char *__compz, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dptrfs_(__CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__df, __CLPK_doublereal *__ef,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dptsv_(__CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dptsvx_(char *__fact, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__df, __CLPK_doublereal *__ef,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dpttrf_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e,
        __CLPK_integer *__info);


int dpttrs_(__CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dptts2_(__CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__b,
        __CLPK_integer *__ldb);


int drscl_(__CLPK_integer *__n, __CLPK_doublereal *__sa,
        __CLPK_doublereal *__sx,
        __CLPK_integer *__incx);


int dsbev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dsbevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dsbevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int dsbgst_(char *__vect, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__bb, __CLPK_integer *__ldbb,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dsbgv_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__bb, __CLPK_integer *__ldbb,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dsbgvd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__bb, __CLPK_integer *__ldbb,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dsbgvx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__bb, __CLPK_integer *__ldbb,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int dsbtrd_(char *__vect, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dsfrk_(char *__transr, char *__uplo, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublereal *__alpha, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__c__);


int dsgesv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__work, __CLPK_real *__swork, __CLPK_integer *__iter,
        __CLPK_integer *__info);


int dspcon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dspev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__w,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dspevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__w,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dspevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int dspgst_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__bp,
        __CLPK_integer *__info);


int dspgv_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublereal *__ap, __CLPK_doublereal *__bp,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dspgvd_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublereal *__ap, __CLPK_doublereal *__bp,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dspgvx_(__CLPK_integer *__itype, char *__jobz, char *__range, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublereal *__ap, __CLPK_doublereal *__bp,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int dsposv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__work, __CLPK_real *__swork, __CLPK_integer *__iter,
        __CLPK_integer *__info);


int dsprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__afp,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dspsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__ap, __CLPK_integer *__ipiv, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dspsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__afp, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dsptrd_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__tau,
        __CLPK_integer *__info);


int dsptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int dsptri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dsptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__ap, __CLPK_integer *__ipiv, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dstebz_(char *__range, char *__order, __CLPK_integer *__n,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e, __CLPK_integer *__m,
        __CLPK_integer *__nsplit, __CLPK_doublereal *__w,
        __CLPK_integer *__iblock, __CLPK_integer *__isplit,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dstedc_(char *__compz, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dstegr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__isuppz, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dstein_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_integer *__m, __CLPK_doublereal *__w,
        __CLPK_integer *__iblock, __CLPK_integer *__isplit,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int dstemr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_integer *__m, __CLPK_doublereal *__w,
        __CLPK_doublereal *__z__, __CLPK_integer *__ldz, __CLPK_integer *__nzc,
        __CLPK_integer *__isuppz, __CLPK_logical *__tryrac,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dsteqr_(char *__compz, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dsterf_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e,
        __CLPK_integer *__info);


int dstev_(char *__jobz, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dstevd_(char *__jobz, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dstevr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__isuppz, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dstevx_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int dsycon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dsyequb_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dsyev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__w,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dsyevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__w,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dsyevr_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__isuppz, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dsyevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int dsygs2_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dsygst_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dsygv_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__w,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dsygvd_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__w,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dsygvx_(__CLPK_integer *__itype, char *__jobz, char *__range, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int dsyrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dsysv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dsysvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__af, __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dsytd2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__tau,
        __CLPK_integer *__info);


int dsytf2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int dsytrd_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__tau, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dsytrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dsytri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dsytrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dtbcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublereal *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtbrfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtbtrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_doublereal *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dtfsm_(char *__transr, char *__side, char *__uplo, char *__trans,
        char *__diag, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__a,
        __CLPK_doublereal *__b,
        __CLPK_integer *__ldb);


int dtftri_(char *__transr, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublereal *__a,
        __CLPK_integer *__info);


int dtfttp_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__arf, __CLPK_doublereal *__ap,
        __CLPK_integer *__info);


int dtfttr_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__arf, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dtgevc_(char *__side, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublereal *__s, __CLPK_integer *__lds,
        __CLPK_doublereal *__p, __CLPK_integer *__ldp, __CLPK_doublereal *__vl,
        __CLPK_integer *__ldvl, __CLPK_doublereal *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__mm, __CLPK_integer *__m, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dtgex2_(__CLPK_logical *__wantq, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__q,
        __CLPK_integer *__ldq, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__j1, __CLPK_integer *__n1, __CLPK_integer *__n2,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dtgexc_(__CLPK_logical *__wantq, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__q,
        __CLPK_integer *__ldq, __CLPK_doublereal *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__ifst, __CLPK_integer *__ilst,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int dtgsen_(__CLPK_integer *__ijob, __CLPK_logical *__wantq,
        __CLPK_logical *__wantz, __CLPK_logical *__select, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__alphar,
        __CLPK_doublereal *__alphai, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_doublereal *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__m, __CLPK_doublereal *__pl,
        __CLPK_doublereal *__pr, __CLPK_doublereal *__dif,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dtgsja_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__p, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__tola, __CLPK_doublereal *__tolb,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__beta,
        __CLPK_doublereal *__u, __CLPK_integer *__ldu, __CLPK_doublereal *__v,
        __CLPK_integer *__ldv, __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_doublereal *__work, __CLPK_integer *__ncycle,
        __CLPK_integer *__info);


int dtgsna_(char *__job, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__vl,
        __CLPK_integer *__ldvl, __CLPK_doublereal *__vr, __CLPK_integer *__ldvr,
        __CLPK_doublereal *__s, __CLPK_doublereal *__dif, __CLPK_integer *__mm,
        __CLPK_integer *__m, __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtgsy2_(char *__trans, __CLPK_integer *__ijob, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__c__,
        __CLPK_integer *__ldc, __CLPK_doublereal *__d__, __CLPK_integer *__ldd,
        __CLPK_doublereal *__e, __CLPK_integer *__lde, __CLPK_doublereal *__f,
        __CLPK_integer *__ldf, __CLPK_doublereal *__scale,
        __CLPK_doublereal *__rdsum, __CLPK_doublereal *__rdscal,
        __CLPK_integer *__iwork, __CLPK_integer *__pq,
        __CLPK_integer *__info);


int dtgsyl_(char *__trans, __CLPK_integer *__ijob, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__c__,
        __CLPK_integer *__ldc, __CLPK_doublereal *__d__, __CLPK_integer *__ldd,
        __CLPK_doublereal *__e, __CLPK_integer *__lde, __CLPK_doublereal *__f,
        __CLPK_integer *__ldf, __CLPK_doublereal *__scale,
        __CLPK_doublereal *__dif, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtpcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__rcond,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtprfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__ap, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtptri_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublereal *__ap,
        __CLPK_integer *__info);


int dtptrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__ap, __CLPK_doublereal *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dtpttf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__ap, __CLPK_doublereal *__arf,
        __CLPK_integer *__info);


int dtpttr_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__ap,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dtrcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtrevc_(char *__side, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublereal *__t, __CLPK_integer *__ldt,
        __CLPK_doublereal *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublereal *__vr, __CLPK_integer *__ldvr, __CLPK_integer *__mm,
        __CLPK_integer *__m, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dtrexc_(char *__compq, __CLPK_integer *__n, __CLPK_doublereal *__t,
        __CLPK_integer *__ldt, __CLPK_doublereal *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__ifst, __CLPK_integer *__ilst,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int dtrrfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb, __CLPK_doublereal *__x,
        __CLPK_integer *__ldx, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtrsen_(char *__job, char *__compq, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublereal *__t, __CLPK_integer *__ldt,
        __CLPK_doublereal *__q, __CLPK_integer *__ldq, __CLPK_doublereal *__wr,
        __CLPK_doublereal *__wi, __CLPK_integer *__m, __CLPK_doublereal *__s,
        __CLPK_doublereal *__sep, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int dtrsna_(char *__job, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublereal *__t, __CLPK_integer *__ldt,
        __CLPK_doublereal *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublereal *__vr, __CLPK_integer *__ldvr, __CLPK_doublereal *__s,
        __CLPK_doublereal *__sep, __CLPK_integer *__mm, __CLPK_integer *__m,
        __CLPK_doublereal *__work, __CLPK_integer *__ldwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int dtrsyl_(char *__trana, char *__tranb, __CLPK_integer *__isgn,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__scale,
        __CLPK_integer *__info);


int dtrti2_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dtrtri_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int dtrtrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int dtrttf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda, __CLPK_doublereal *__arf,
        __CLPK_integer *__info);


int dtrttp_(char *__uplo, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__ap,
        __CLPK_integer *__info);


int dtzrqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_integer *__info);


int dtzrzf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__tau,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


__CLPK_doublereal dzsum1_(__CLPK_integer *__n, __CLPK_doublecomplex *__cx,
        __CLPK_integer *__incx);


__CLPK_integer icmax1_(__CLPK_integer *__n, __CLPK_complex *__cx,
        __CLPK_integer *__incx);


__CLPK_integer ieeeck_(__CLPK_integer *__ispec, __CLPK_real *__zero,
        __CLPK_real *__one);


__CLPK_integer ilaclc_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__a,
        __CLPK_integer *__lda);


__CLPK_integer ilaclr_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_complex *__a,
        __CLPK_integer *__lda);


__CLPK_integer iladiag_(char *__diag);


__CLPK_integer iladlc_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__a,
        __CLPK_integer *__lda);


__CLPK_integer iladlr_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__a,
        __CLPK_integer *__lda);


__CLPK_integer ilaenv_(__CLPK_integer *__ispec, char *__name__, char *__opts,
        __CLPK_integer *__n1, __CLPK_integer *__n2, __CLPK_integer *__n3,
        __CLPK_integer *__n4);


__CLPK_integer ilaprec_(char *__prec);


__CLPK_integer ilaslc_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__a,
        __CLPK_integer *__lda);


__CLPK_integer ilaslr_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__a,
        __CLPK_integer *__lda);


__CLPK_integer ilatrans_(char *__trans);


__CLPK_integer ilauplo_(char *__uplo);


int ilaver_(__CLPK_integer *__vers_major__, __CLPK_integer *__vers_minor__,
        __CLPK_integer *__vers_patch__);


__CLPK_integer ilazlc_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda);


__CLPK_integer ilazlr_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda);


__CLPK_integer iparmq_(__CLPK_integer *__ispec, char *__name__, char *__opts,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_integer *__lwork);


__CLPK_integer izmax1_(__CLPK_integer *__n, __CLPK_doublecomplex *__cx,
        __CLPK_integer *__incx);


__CLPK_logical lsamen_(__CLPK_integer *__n, char *__ca,
        char *__cb);


__CLPK_integer smaxloc_(__CLPK_real *__a,
        __CLPK_integer *__dimm);


int sbdsdc_(char *__uplo, char *__compq, __CLPK_integer *__n,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__u,
        __CLPK_integer *__ldu, __CLPK_real *__vt, __CLPK_integer *__ldvt,
        __CLPK_real *__q, __CLPK_integer *__iq, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sbdsqr_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__ncvt,
        __CLPK_integer *__nru, __CLPK_integer *__ncc, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__vt, __CLPK_integer *__ldvt,
        __CLPK_real *__u, __CLPK_integer *__ldu, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


__CLPK_doublereal scsum1_(__CLPK_integer *__n, __CLPK_complex *__cx,
        __CLPK_integer *__incx);


int sdisna_(char *__job, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__d__, __CLPK_real *__sep,
        __CLPK_integer *__info);


int sgbbrd_(char *__vect, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__ncc, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_real *__ab, __CLPK_integer *__ldab, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__q, __CLPK_integer *__ldq,
        __CLPK_real *__pt, __CLPK_integer *__ldpt, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


int sgbcon_(char *__norm, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgbequ_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_real *__rowcnd,
        __CLPK_real *__colcnd, __CLPK_real *__amax,
        __CLPK_integer *__info);


int sgbequb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_real *__rowcnd,
        __CLPK_real *__colcnd, __CLPK_real *__amax,
        __CLPK_integer *__info);


int sgbrfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_integer *__nrhs, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__afb, __CLPK_integer *__ldafb,
        __CLPK_integer *__ipiv, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__x, __CLPK_integer *__ldx, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgbsv_(__CLPK_integer *__n, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_integer *__nrhs, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sgbsvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__kl, __CLPK_integer *__ku, __CLPK_integer *__nrhs,
        __CLPK_real *__ab, __CLPK_integer *__ldab, __CLPK_real *__afb,
        __CLPK_integer *__ldafb, __CLPK_integer *__ipiv, char *__equed,
        __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__rcond, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgbtf2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int sgbtrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int sgbtrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_integer *__nrhs, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_integer *__ipiv, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sgebak_(char *__job, char *__side, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__scale,
        __CLPK_integer *__m, __CLPK_real *__v, __CLPK_integer *__ldv,
        __CLPK_integer *__info);


int sgebal_(char *__job, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__scale,
        __CLPK_integer *__info);


int sgebd2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__tauq, __CLPK_real *__taup, __CLPK_real *__work,
        __CLPK_integer *__info);


int sgebrd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__tauq, __CLPK_real *__taup, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgecon_(char *__norm, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgeequ_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__r__, __CLPK_real *__c__,
        __CLPK_real *__rowcnd, __CLPK_real *__colcnd, __CLPK_real *__amax,
        __CLPK_integer *__info);


int sgeequb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__r__, __CLPK_real *__c__,
        __CLPK_real *__rowcnd, __CLPK_real *__colcnd, __CLPK_real *__amax,
        __CLPK_integer *__info);


int sgees_(char *__jobvs, char *__sort, __CLPK_L_fp __select,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__sdim, __CLPK_real *__wr, __CLPK_real *__wi,
        __CLPK_real *__vs, __CLPK_integer *__ldvs, __CLPK_real *__work,
        __CLPK_integer *__lwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int sgeesx_(char *__jobvs, char *__sort, __CLPK_L_fp __select, char *__sense,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__sdim, __CLPK_real *__wr, __CLPK_real *__wi,
        __CLPK_real *__vs, __CLPK_integer *__ldvs, __CLPK_real *__rconde,
        __CLPK_real *__rcondv, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int sgeev_(char *__jobvl, char *__jobvr, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__wr, __CLPK_real *__wi,
        __CLPK_real *__vl, __CLPK_integer *__ldvl, __CLPK_real *__vr,
        __CLPK_integer *__ldvr, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgeevx_(char *__balanc, char *__jobvl, char *__jobvr, char *__sense,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__wr, __CLPK_real *__wi, __CLPK_real *__vl,
        __CLPK_integer *__ldvl, __CLPK_real *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__scale,
        __CLPK_real *__abnrm, __CLPK_real *__rconde, __CLPK_real *__rcondv,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgegs_(char *__jobvsl, char *__jobvsr, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__alphar, __CLPK_real *__alphai,
        __CLPK_real *__beta, __CLPK_real *__vsl, __CLPK_integer *__ldvsl,
        __CLPK_real *__vsr, __CLPK_integer *__ldvsr, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgegv_(char *__jobvl, char *__jobvr, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__alphar, __CLPK_real *__alphai, __CLPK_real *__beta,
        __CLPK_real *__vl, __CLPK_integer *__ldvl, __CLPK_real *__vr,
        __CLPK_integer *__ldvr, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgehd2_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sgehrd_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgejsv_(char *__joba, char *__jobu, char *__jobv, char *__jobr,
        char *__jobt, char *__jobp, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__sva,
        __CLPK_real *__u, __CLPK_integer *__ldu, __CLPK_real *__v,
        __CLPK_integer *__ldv, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgelq2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__info);


int sgelqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgels_(char *__trans, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgelsd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__s, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgelss_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__s, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgelsx_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_integer *__jpvt, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_real *__work,
        __CLPK_integer *__info);


int sgelsy_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_integer *__jpvt, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgeql2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__info);


int sgeqlf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgeqp3_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_real *__tau,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgeqpf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_real *__tau,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sgeqr2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__info);


int sgeqrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgerfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgerq2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__info);


int sgerqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgesc2_(__CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__rhs, __CLPK_integer *__ipiv, __CLPK_integer *__jpiv,
        __CLPK_real *__scale);


int sgesdd_(char *__jobz, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__s,
        __CLPK_real *__u, __CLPK_integer *__ldu, __CLPK_real *__vt,
        __CLPK_integer *__ldvt, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgesv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sgesvd_(char *__jobu, char *__jobvt, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__s, __CLPK_real *__u, __CLPK_integer *__ldu,
        __CLPK_real *__vt, __CLPK_integer *__ldvt, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgesvj_(char *__joba, char *__jobu, char *__jobv, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__sva, __CLPK_integer *__mv, __CLPK_real *__v,
        __CLPK_integer *__ldv, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgesvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__af, __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        char *__equed, __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__rcond, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgetc2_(__CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_integer *__jpiv,
        __CLPK_integer *__info);


int sgetf2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int sgetrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int sgetri_(__CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgetrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sggbak_(char *__job, char *__side, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__lscale,
        __CLPK_real *__rscale, __CLPK_integer *__m, __CLPK_real *__v,
        __CLPK_integer *__ldv,
        __CLPK_integer *__info);


int sggbal_(char *__job, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__lscale,
        __CLPK_real *__rscale, __CLPK_real *__work,
        __CLPK_integer *__info);


int sgges_(char *__jobvsl, char *__jobvsr, char *__sort, __CLPK_L_fp __selctg,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_integer *__sdim,
        __CLPK_real *__alphar, __CLPK_real *__alphai, __CLPK_real *__beta,
        __CLPK_real *__vsl, __CLPK_integer *__ldvsl, __CLPK_real *__vsr,
        __CLPK_integer *__ldvsr, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int sggesx_(char *__jobvsl, char *__jobvsr, char *__sort, __CLPK_L_fp __selctg,
        char *__sense, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__sdim, __CLPK_real *__alphar, __CLPK_real *__alphai,
        __CLPK_real *__beta, __CLPK_real *__vsl, __CLPK_integer *__ldvsl,
        __CLPK_real *__vsr, __CLPK_integer *__ldvsr, __CLPK_real *__rconde,
        __CLPK_real *__rcondv, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int sggev_(char *__jobvl, char *__jobvr, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__alphar, __CLPK_real *__alphai, __CLPK_real *__beta,
        __CLPK_real *__vl, __CLPK_integer *__ldvl, __CLPK_real *__vr,
        __CLPK_integer *__ldvr, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sggevx_(char *__balanc, char *__jobvl, char *__jobvr, char *__sense,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__alphar,
        __CLPK_real *__alphai, __CLPK_real *__beta, __CLPK_real *__vl,
        __CLPK_integer *__ldvl, __CLPK_real *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__lscale,
        __CLPK_real *__rscale, __CLPK_real *__abnrm, __CLPK_real *__bbnrm,
        __CLPK_real *__rconde, __CLPK_real *__rcondv, __CLPK_real *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int sggglm_(__CLPK_integer *__n, __CLPK_integer *__m, __CLPK_integer *__p,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__d__, __CLPK_real *__x,
        __CLPK_real *__y, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgghrd_(char *__compq, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__q, __CLPK_integer *__ldq, __CLPK_real *__z__,
        __CLPK_integer *__ldz,
        __CLPK_integer *__info);


int sgglse_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__p,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__c__, __CLPK_real *__d__,
        __CLPK_real *__x, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sggqrf_(__CLPK_integer *__n, __CLPK_integer *__m, __CLPK_integer *__p,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__taua,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__taub,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sggrqf_(__CLPK_integer *__m, __CLPK_integer *__p, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__taua,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__taub,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sggsvd_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__p, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__alpha,
        __CLPK_real *__beta, __CLPK_real *__u, __CLPK_integer *__ldu,
        __CLPK_real *__v, __CLPK_integer *__ldv, __CLPK_real *__q,
        __CLPK_integer *__ldq, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sggsvp_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__p, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__tola, __CLPK_real *__tolb, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_real *__u, __CLPK_integer *__ldu,
        __CLPK_real *__v, __CLPK_integer *__ldv, __CLPK_real *__q,
        __CLPK_integer *__ldq, __CLPK_integer *__iwork, __CLPK_real *__tau,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sgsvj0_(char *__jobv, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__d__,
        __CLPK_real *__sva, __CLPK_integer *__mv, __CLPK_real *__v,
        __CLPK_integer *__ldv, __CLPK_real *__eps, __CLPK_real *__sfmin,
        __CLPK_real *__tol, __CLPK_integer *__nsweep, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgsvj1_(char *__jobv, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__n1, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__d__, __CLPK_real *__sva, __CLPK_integer *__mv,
        __CLPK_real *__v, __CLPK_integer *__ldv, __CLPK_real *__eps,
        __CLPK_real *__sfmin, __CLPK_real *__tol, __CLPK_integer *__nsweep,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sgtcon_(char *__norm, __CLPK_integer *__n, __CLPK_real *__dl,
        __CLPK_real *__d__, __CLPK_real *__du, __CLPK_real *__du2,
        __CLPK_integer *__ipiv, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgtrfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__dl, __CLPK_real *__d__, __CLPK_real *__du,
        __CLPK_real *__dlf, __CLPK_real *__df, __CLPK_real *__duf,
        __CLPK_real *__du2, __CLPK_integer *__ipiv, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgtsv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_real *__dl,
        __CLPK_real *__d__, __CLPK_real *__du, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sgtsvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__dl, __CLPK_real *__d__,
        __CLPK_real *__du, __CLPK_real *__dlf, __CLPK_real *__df,
        __CLPK_real *__duf, __CLPK_real *__du2, __CLPK_integer *__ipiv,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__x,
        __CLPK_integer *__ldx, __CLPK_real *__rcond, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sgttrf_(__CLPK_integer *__n, __CLPK_real *__dl, __CLPK_real *__d__,
        __CLPK_real *__du, __CLPK_real *__du2, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int sgttrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__dl, __CLPK_real *__d__, __CLPK_real *__du,
        __CLPK_real *__du2, __CLPK_integer *__ipiv, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sgtts2_(__CLPK_integer *__itrans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__dl, __CLPK_real *__d__,
        __CLPK_real *__du, __CLPK_real *__du2, __CLPK_integer *__ipiv,
        __CLPK_real *__b,
        __CLPK_integer *__ldb);


int shgeqz_(char *__job, char *__compq, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__h__,
        __CLPK_integer *__ldh, __CLPK_real *__t, __CLPK_integer *__ldt,
        __CLPK_real *__alphar, __CLPK_real *__alphai, __CLPK_real *__beta,
        __CLPK_real *__q, __CLPK_integer *__ldq, __CLPK_real *__z__,
        __CLPK_integer *__ldz, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int shsein_(char *__side, char *__eigsrc, char *__initv,
        __CLPK_logical *__select, __CLPK_integer *__n, __CLPK_real *__h__,
        __CLPK_integer *__ldh, __CLPK_real *__wr, __CLPK_real *__wi,
        __CLPK_real *__vl, __CLPK_integer *__ldvl, __CLPK_real *__vr,
        __CLPK_integer *__ldvr, __CLPK_integer *__mm, __CLPK_integer *__m,
        __CLPK_real *__work, __CLPK_integer *__ifaill, __CLPK_integer *__ifailr,
        __CLPK_integer *__info);


int shseqr_(char *__job, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_real *__h__,
        __CLPK_integer *__ldh, __CLPK_real *__wr, __CLPK_real *__wi,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);



        __CLPK_logical sisnan_(__CLPK_real *__sin__);


int slabad_(__CLPK_real *__small,
        __CLPK_real *__large);


int slabrd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__tauq, __CLPK_real *__taup,
        __CLPK_real *__x, __CLPK_integer *__ldx, __CLPK_real *__y,
        __CLPK_integer *__ldy);


int slacn2_(__CLPK_integer *__n, __CLPK_real *__v, __CLPK_real *__x,
        __CLPK_integer *__isgn, __CLPK_real *__est, __CLPK_integer *__kase,
        __CLPK_integer *__isave);


int slacon_(__CLPK_integer *__n, __CLPK_real *__v, __CLPK_real *__x,
        __CLPK_integer *__isgn, __CLPK_real *__est,
        __CLPK_integer *__kase);


int slacpy_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb);


int sladiv_(__CLPK_real *__a, __CLPK_real *__b, __CLPK_real *__c__,
        __CLPK_real *__d__, __CLPK_real *__p,
        __CLPK_real *__q);


int slae2_(__CLPK_real *__a, __CLPK_real *__b, __CLPK_real *__c__,
        __CLPK_real *__rt1,
        __CLPK_real *__rt2);


int slaebz_(__CLPK_integer *__ijob, __CLPK_integer *__nitmax,
        __CLPK_integer *__n, __CLPK_integer *__mmax, __CLPK_integer *__minp,
        __CLPK_integer *__nbmin, __CLPK_real *__abstol, __CLPK_real *__reltol,
        __CLPK_real *__pivmin, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__e2, __CLPK_integer *__nval, __CLPK_real *__ab,
        __CLPK_real *__c__, __CLPK_integer *__mout, __CLPK_integer *__nab,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slaed0_(__CLPK_integer *__icompq, __CLPK_integer *__qsiz,
        __CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__q, __CLPK_integer *__ldq, __CLPK_real *__qstore,
        __CLPK_integer *__ldqs, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slaed1_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__q,
        __CLPK_integer *__ldq, __CLPK_integer *__indxq, __CLPK_real *__rho,
        __CLPK_integer *__cutpnt, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slaed2_(__CLPK_integer *__k, __CLPK_integer *__n, __CLPK_integer *__n1,
        __CLPK_real *__d__, __CLPK_real *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__indxq, __CLPK_real *__rho, __CLPK_real *__z__,
        __CLPK_real *__dlamda, __CLPK_real *__w, __CLPK_real *__q2,
        __CLPK_integer *__indx, __CLPK_integer *__indxc,
        __CLPK_integer *__indxp, __CLPK_integer *__coltyp,
        __CLPK_integer *__info);


int slaed3_(__CLPK_integer *__k, __CLPK_integer *__n, __CLPK_integer *__n1,
        __CLPK_real *__d__, __CLPK_real *__q, __CLPK_integer *__ldq,
        __CLPK_real *__rho, __CLPK_real *__dlamda, __CLPK_real *__q2,
        __CLPK_integer *__indx, __CLPK_integer *__ctot, __CLPK_real *__w,
        __CLPK_real *__s,
        __CLPK_integer *__info);


int slaed4_(__CLPK_integer *__n, __CLPK_integer *__i__, __CLPK_real *__d__,
        __CLPK_real *__z__, __CLPK_real *__delta, __CLPK_real *__rho,
        __CLPK_real *__dlam,
        __CLPK_integer *__info);


int slaed5_(__CLPK_integer *__i__, __CLPK_real *__d__, __CLPK_real *__z__,
        __CLPK_real *__delta, __CLPK_real *__rho,
        __CLPK_real *__dlam);


int slaed6_(__CLPK_integer *__kniter, __CLPK_logical *__orgati,
        __CLPK_real *__rho, __CLPK_real *__d__, __CLPK_real *__z__,
        __CLPK_real *__finit, __CLPK_real *__tau,
        __CLPK_integer *__info);


int slaed7_(__CLPK_integer *__icompq, __CLPK_integer *__n,
        __CLPK_integer *__qsiz, __CLPK_integer *__tlvls,
        __CLPK_integer *__curlvl, __CLPK_integer *__curpbm, __CLPK_real *__d__,
        __CLPK_real *__q, __CLPK_integer *__ldq, __CLPK_integer *__indxq,
        __CLPK_real *__rho, __CLPK_integer *__cutpnt, __CLPK_real *__qstore,
        __CLPK_integer *__qptr, __CLPK_integer *__prmptr,
        __CLPK_integer *__perm, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_real *__givnum, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slaed8_(__CLPK_integer *__icompq, __CLPK_integer *__k, __CLPK_integer *__n,
        __CLPK_integer *__qsiz, __CLPK_real *__d__, __CLPK_real *__q,
        __CLPK_integer *__ldq, __CLPK_integer *__indxq, __CLPK_real *__rho,
        __CLPK_integer *__cutpnt, __CLPK_real *__z__, __CLPK_real *__dlamda,
        __CLPK_real *__q2, __CLPK_integer *__ldq2, __CLPK_real *__w,
        __CLPK_integer *__perm, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_real *__givnum,
        __CLPK_integer *__indxp, __CLPK_integer *__indx,
        __CLPK_integer *__info);


int slaed9_(__CLPK_integer *__k, __CLPK_integer *__kstart,
        __CLPK_integer *__kstop, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__q, __CLPK_integer *__ldq, __CLPK_real *__rho,
        __CLPK_real *__dlamda, __CLPK_real *__w, __CLPK_real *__s,
        __CLPK_integer *__lds,
        __CLPK_integer *__info);


int slaeda_(__CLPK_integer *__n, __CLPK_integer *__tlvls,
        __CLPK_integer *__curlvl, __CLPK_integer *__curpbm,
        __CLPK_integer *__prmptr, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_real *__givnum, __CLPK_real *__q, __CLPK_integer *__qptr,
        __CLPK_real *__z__, __CLPK_real *__ztemp,
        __CLPK_integer *__info);


int slaein_(__CLPK_logical *__rightv, __CLPK_logical *__noinit,
        __CLPK_integer *__n, __CLPK_real *__h__, __CLPK_integer *__ldh,
        __CLPK_real *__wr, __CLPK_real *__wi, __CLPK_real *__vr,
        __CLPK_real *__vi, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__work, __CLPK_real *__eps3, __CLPK_real *__smlnum,
        __CLPK_real *__bignum,
        __CLPK_integer *__info);


int slaev2_(__CLPK_real *__a, __CLPK_real *__b, __CLPK_real *__c__,
        __CLPK_real *__rt1, __CLPK_real *__rt2, __CLPK_real *__cs1,
        __CLPK_real *__sn1);


int slaexc_(__CLPK_logical *__wantq, __CLPK_integer *__n, __CLPK_real *__t,
        __CLPK_integer *__ldt, __CLPK_real *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__j1, __CLPK_integer *__n1, __CLPK_integer *__n2,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int slag2_(__CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__safmin, __CLPK_real *__scale1,
        __CLPK_real *__scale2, __CLPK_real *__wr1, __CLPK_real *__wr2,
        __CLPK_real *__wi);


int slag2d_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__sa,
        __CLPK_integer *__ldsa, __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int slags2_(__CLPK_logical *__upper, __CLPK_real *__a1, __CLPK_real *__a2,
        __CLPK_real *__a3, __CLPK_real *__b1, __CLPK_real *__b2,
        __CLPK_real *__b3, __CLPK_real *__csu, __CLPK_real *__snu,
        __CLPK_real *__csv, __CLPK_real *__snv, __CLPK_real *__csq,
        __CLPK_real *__snq);


int slagtf_(__CLPK_integer *__n, __CLPK_real *__a, __CLPK_real *__lambda,
        __CLPK_real *__b, __CLPK_real *__c__, __CLPK_real *__tol,
        __CLPK_real *__d__, __CLPK_integer *__in,
        __CLPK_integer *__info);


int slagtm_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__alpha, __CLPK_real *__dl, __CLPK_real *__d__,
        __CLPK_real *__du, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__beta, __CLPK_real *__b,
        __CLPK_integer *__ldb);


int slagts_(__CLPK_integer *__job, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_real *__b, __CLPK_real *__c__, __CLPK_real *__d__,
        __CLPK_integer *__in, __CLPK_real *__y, __CLPK_real *__tol,
        __CLPK_integer *__info);


int slagv2_(__CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__alphar, __CLPK_real *__alphai,
        __CLPK_real *__beta, __CLPK_real *__csl, __CLPK_real *__snl,
        __CLPK_real *__csr,
        __CLPK_real *__snr);


int slahqr_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__h__, __CLPK_integer *__ldh, __CLPK_real *__wr,
        __CLPK_real *__wi, __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__info);


int slahr2_(__CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__nb,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__t, __CLPK_integer *__ldt, __CLPK_real *__y,
        __CLPK_integer *__ldy);


int slahrd_(__CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__nb,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__t, __CLPK_integer *__ldt, __CLPK_real *__y,
        __CLPK_integer *__ldy);


int slaic1_(__CLPK_integer *__job, __CLPK_integer *__j, __CLPK_real *__x,
        __CLPK_real *__sest, __CLPK_real *__w, __CLPK_real *__gamma,
        __CLPK_real *__sestpr, __CLPK_real *__s,
        __CLPK_real *__c__);


__CLPK_logical slaisnan_(__CLPK_real *__sin1,
        __CLPK_real *__sin2);


int slaln2_(__CLPK_logical *__ltrans, __CLPK_integer *__na,
        __CLPK_integer *__nw, __CLPK_real *__smin, __CLPK_real *__ca,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__d1,
        __CLPK_real *__d2, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__wr, __CLPK_real *__wi, __CLPK_real *__x,
        __CLPK_integer *__ldx, __CLPK_real *__scale, __CLPK_real *__xnorm,
        __CLPK_integer *__info);


int slals0_(__CLPK_integer *__icompq, __CLPK_integer *__nl,
        __CLPK_integer *__nr, __CLPK_integer *__sqre, __CLPK_integer *__nrhs,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__bx,
        __CLPK_integer *__ldbx, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_integer *__ldgcol, __CLPK_real *__givnum,
        __CLPK_integer *__ldgnum, __CLPK_real *__poles, __CLPK_real *__difl,
        __CLPK_real *__difr, __CLPK_real *__z__, __CLPK_integer *__k,
        __CLPK_real *__c__, __CLPK_real *__s, __CLPK_real *__work,
        __CLPK_integer *__info);


int slalsa_(__CLPK_integer *__icompq, __CLPK_integer *__smlsiz,
        __CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__bx, __CLPK_integer *__ldbx,
        __CLPK_real *__u, __CLPK_integer *__ldu, __CLPK_real *__vt,
        __CLPK_integer *__k, __CLPK_real *__difl, __CLPK_real *__difr,
        __CLPK_real *__z__, __CLPK_real *__poles, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_integer *__ldgcol,
        __CLPK_integer *__perm, __CLPK_real *__givnum, __CLPK_real *__c__,
        __CLPK_real *__s, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slalsd_(char *__uplo, __CLPK_integer *__smlsiz, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__rcond,
        __CLPK_integer *__rank, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slamrg_(__CLPK_integer *__n1, __CLPK_integer *__n2, __CLPK_real *__a,
        __CLPK_integer *__strd1, __CLPK_integer *__strd2,
        __CLPK_integer *__index);


__CLPK_integer slaneg_(__CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__lld, __CLPK_real *__sigma, __CLPK_real *__pivmin,
        __CLPK_integer *__r__);


__CLPK_doublereal slangb_(char *__norm, __CLPK_integer *__n,
        __CLPK_integer *__kl, __CLPK_integer *__ku, __CLPK_real *__ab,
        __CLPK_integer *__ldab,
        __CLPK_real *__work);


__CLPK_doublereal slange_(char *__norm, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__work);


__CLPK_doublereal slangt_(char *__norm, __CLPK_integer *__n, __CLPK_real *__dl,
        __CLPK_real *__d__,
        __CLPK_real *__du);


__CLPK_doublereal slanhs_(char *__norm, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_real *__work);


__CLPK_doublereal slansb_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__work);


__CLPK_doublereal slansf_(char *__norm, char *__transr, char *__uplo,
        __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_real *__work);


__CLPK_doublereal slansp_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__ap,
        __CLPK_real *__work);


__CLPK_doublereal slanst_(char *__norm, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e);


__CLPK_doublereal slansy_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__work);


__CLPK_doublereal slantb_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__ab,
        __CLPK_integer *__ldab,
        __CLPK_real *__work);


__CLPK_doublereal slantp_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__work);


__CLPK_doublereal slantr_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_real *__work);


int slanv2_(__CLPK_real *__a, __CLPK_real *__b, __CLPK_real *__c__,
        __CLPK_real *__d__, __CLPK_real *__rt1r, __CLPK_real *__rt1i,
        __CLPK_real *__rt2r, __CLPK_real *__rt2i, __CLPK_real *__cs,
        __CLPK_real *__sn);


int slapll_(__CLPK_integer *__n, __CLPK_real *__x, __CLPK_integer *__incx,
        __CLPK_real *__y, __CLPK_integer *__incy,
        __CLPK_real *__ssmin);


int slapmt_(__CLPK_logical *__forwrd, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_integer *__k);


__CLPK_doublereal slapy2_(__CLPK_real *__x,
        __CLPK_real *__y);


__CLPK_doublereal slapy3_(__CLPK_real *__x, __CLPK_real *__y,
        __CLPK_real *__z__);


int slaqgb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__r__, __CLPK_real *__c__, __CLPK_real *__rowcnd,
        __CLPK_real *__colcnd, __CLPK_real *__amax,
        char *__equed);


int slaqge_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__r__, __CLPK_real *__c__,
        __CLPK_real *__rowcnd, __CLPK_real *__colcnd, __CLPK_real *__amax,
        char *__equed);


int slaqp2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__offset,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_integer *__jpvt,
        __CLPK_real *__tau, __CLPK_real *__vn1, __CLPK_real *__vn2,
        __CLPK_real *__work);


int slaqps_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__offset,
        __CLPK_integer *__nb, __CLPK_integer *__kb, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt, __CLPK_real *__tau,
        __CLPK_real *__vn1, __CLPK_real *__vn2, __CLPK_real *__auxv,
        __CLPK_real *__f,
        __CLPK_integer *__ldf);


int slaqr0_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__h__, __CLPK_integer *__ldh, __CLPK_real *__wr,
        __CLPK_real *__wi, __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int slaqr1_(__CLPK_integer *__n, __CLPK_real *__h__, __CLPK_integer *__ldh,
        __CLPK_real *__sr1, __CLPK_real *__si1, __CLPK_real *__sr2,
        __CLPK_real *__si2,
        __CLPK_real *__v);


int slaqr2_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ktop, __CLPK_integer *__kbot,
        __CLPK_integer *__nw, __CLPK_real *__h__, __CLPK_integer *__ldh,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz, __CLPK_real *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__ns, __CLPK_integer *__nd,
        __CLPK_real *__sr, __CLPK_real *__si, __CLPK_real *__v,
        __CLPK_integer *__ldv, __CLPK_integer *__nh, __CLPK_real *__t,
        __CLPK_integer *__ldt, __CLPK_integer *__nv, __CLPK_real *__wv,
        __CLPK_integer *__ldwv, __CLPK_real *__work,
        __CLPK_integer *__lwork);


int slaqr3_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ktop, __CLPK_integer *__kbot,
        __CLPK_integer *__nw, __CLPK_real *__h__, __CLPK_integer *__ldh,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz, __CLPK_real *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__ns, __CLPK_integer *__nd,
        __CLPK_real *__sr, __CLPK_real *__si, __CLPK_real *__v,
        __CLPK_integer *__ldv, __CLPK_integer *__nh, __CLPK_real *__t,
        __CLPK_integer *__ldt, __CLPK_integer *__nv, __CLPK_real *__wv,
        __CLPK_integer *__ldwv, __CLPK_real *__work,
        __CLPK_integer *__lwork);


int slaqr4_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__h__, __CLPK_integer *__ldh, __CLPK_real *__wr,
        __CLPK_real *__wi, __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int slaqr5_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__kacc22, __CLPK_integer *__n, __CLPK_integer *__ktop,
        __CLPK_integer *__kbot, __CLPK_integer *__nshfts, __CLPK_real *__sr,
        __CLPK_real *__si, __CLPK_real *__h__, __CLPK_integer *__ldh,
        __CLPK_integer *__iloz, __CLPK_integer *__ihiz, __CLPK_real *__z__,
        __CLPK_integer *__ldz, __CLPK_real *__v, __CLPK_integer *__ldv,
        __CLPK_real *__u, __CLPK_integer *__ldu, __CLPK_integer *__nv,
        __CLPK_real *__wv, __CLPK_integer *__ldwv, __CLPK_integer *__nh,
        __CLPK_real *__wh,
        __CLPK_integer *__ldwh);


int slaqsb_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_real *__ab, __CLPK_integer *__ldab, __CLPK_real *__s,
        __CLPK_real *__scond, __CLPK_real *__amax,
        char *__equed);


int slaqsp_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        char *__equed);


int slaqsy_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__s, __CLPK_real *__scond,
        __CLPK_real *__amax, char *__equed);


int slaqtr_(__CLPK_logical *__ltran, __CLPK_logical *__l__CLPK_real,
        __CLPK_integer *__n, __CLPK_real *__t, __CLPK_integer *__ldt,
        __CLPK_real *__b, __CLPK_real *__w, __CLPK_real *__scale,
        __CLPK_real *__x, __CLPK_real *__work,
        __CLPK_integer *__info);


int slar1v_(__CLPK_integer *__n, __CLPK_integer *__b1, __CLPK_integer *__bn,
        __CLPK_real *__lambda, __CLPK_real *__d__, __CLPK_real *__l,
        __CLPK_real *__ld, __CLPK_real *__lld, __CLPK_real *__pivmin,
        __CLPK_real *__gaptol, __CLPK_real *__z__, __CLPK_logical *__wantnc,
        __CLPK_integer *__negcnt, __CLPK_real *__ztz, __CLPK_real *__mingma,
        __CLPK_integer *__r__, __CLPK_integer *__isuppz, __CLPK_real *__nrminv,
        __CLPK_real *__resid, __CLPK_real *__rqcorr,
        __CLPK_real *__work);


int slar2v_(__CLPK_integer *__n, __CLPK_real *__x, __CLPK_real *__y,
        __CLPK_real *__z__, __CLPK_integer *__incx, __CLPK_real *__c__,
        __CLPK_real *__s,
        __CLPK_integer *__incc);


int slarf_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__v, __CLPK_integer *__incv, __CLPK_real *__tau,
        __CLPK_real *__c__, __CLPK_integer *__ldc,
        __CLPK_real *__work);


int slarfb_(char *__side, char *__trans, char *__direct, char *__storev,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__v, __CLPK_integer *__ldv, __CLPK_real *__t,
        __CLPK_integer *__ldt, __CLPK_real *__c__, __CLPK_integer *__ldc,
        __CLPK_real *__work,
        __CLPK_integer *__ldwork);


int slarfg_(__CLPK_integer *__n, __CLPK_real *__alpha, __CLPK_real *__x,
        __CLPK_integer *__incx,
        __CLPK_real *__tau);


int slarfp_(__CLPK_integer *__n, __CLPK_real *__alpha, __CLPK_real *__x,
        __CLPK_integer *__incx,
        __CLPK_real *__tau);


int slarft_(char *__direct, char *__storev, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_real *__v, __CLPK_integer *__ldv,
        __CLPK_real *__tau, __CLPK_real *__t,
        __CLPK_integer *__ldt);


int slarfx_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__v, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc,
        __CLPK_real *__work);


int slargv_(__CLPK_integer *__n, __CLPK_real *__x, __CLPK_integer *__incx,
        __CLPK_real *__y, __CLPK_integer *__incy, __CLPK_real *__c__,
        __CLPK_integer *__incc);


int slarnv_(__CLPK_integer *__idist, __CLPK_integer *__iseed,
        __CLPK_integer *__n,
        __CLPK_real *__x);


int slarra_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__e2, __CLPK_real *__spltol, __CLPK_real *__tnrm,
        __CLPK_integer *__nsplit, __CLPK_integer *__isplit,
        __CLPK_integer *__info);


int slarrb_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__lld,
        __CLPK_integer *__ifirst, __CLPK_integer *__ilast, __CLPK_real *__rtol1,
        __CLPK_real *__rtol2, __CLPK_integer *__offset, __CLPK_real *__w,
        __CLPK_real *__wgap, __CLPK_real *__werr, __CLPK_real *__work,
        __CLPK_integer *__iwork, __CLPK_real *__pivmin, __CLPK_real *__spdiam,
        __CLPK_integer *__twist,
        __CLPK_integer *__info);


int slarrc_(char *__jobt, __CLPK_integer *__n, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__pivmin, __CLPK_integer *__eigcnt, __CLPK_integer *__lcnt,
        __CLPK_integer *__rcnt,
        __CLPK_integer *__info);


int slarrd_(char *__range, char *__order, __CLPK_integer *__n,
        __CLPK_real *__vl, __CLPK_real *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_real *__gers, __CLPK_real *__reltol,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__e2,
        __CLPK_real *__pivmin, __CLPK_integer *__nsplit,
        __CLPK_integer *__isplit, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__werr, __CLPK_real *__wl, __CLPK_real *__wu,
        __CLPK_integer *__iblock, __CLPK_integer *__indexw, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slarre_(char *__range, __CLPK_integer *__n, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__e2,
        __CLPK_real *__rtol1, __CLPK_real *__rtol2, __CLPK_real *__spltol,
        __CLPK_integer *__nsplit, __CLPK_integer *__isplit, __CLPK_integer *__m,
        __CLPK_real *__w, __CLPK_real *__werr, __CLPK_real *__wgap,
        __CLPK_integer *__iblock, __CLPK_integer *__indexw, __CLPK_real *__gers,
        __CLPK_real *__pivmin, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slarrf_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__l,
        __CLPK_real *__ld, __CLPK_integer *__clstrt, __CLPK_integer *__clend,
        __CLPK_real *__w, __CLPK_real *__wgap, __CLPK_real *__werr,
        __CLPK_real *__spdiam, __CLPK_real *__clgapl, __CLPK_real *__clgapr,
        __CLPK_real *__pivmin, __CLPK_real *__sigma, __CLPK_real *__dplus,
        __CLPK_real *__lplus, __CLPK_real *__work,
        __CLPK_integer *__info);


int slarrj_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e2,
        __CLPK_integer *__ifirst, __CLPK_integer *__ilast, __CLPK_real *__rtol,
        __CLPK_integer *__offset, __CLPK_real *__w, __CLPK_real *__werr,
        __CLPK_real *__work, __CLPK_integer *__iwork, __CLPK_real *__pivmin,
        __CLPK_real *__spdiam,
        __CLPK_integer *__info);


int slarrk_(__CLPK_integer *__n, __CLPK_integer *__iw, __CLPK_real *__gl,
        __CLPK_real *__gu, __CLPK_real *__d__, __CLPK_real *__e2,
        __CLPK_real *__pivmin, __CLPK_real *__reltol, __CLPK_real *__w,
        __CLPK_real *__werr,
        __CLPK_integer *__info);


int slarrr_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_integer *__info);


int slarrv_(__CLPK_integer *__n, __CLPK_real *__vl, __CLPK_real *__vu,
        __CLPK_real *__d__, __CLPK_real *__l, __CLPK_real *__pivmin,
        __CLPK_integer *__isplit, __CLPK_integer *__m, __CLPK_integer *__dol,
        __CLPK_integer *__dou, __CLPK_real *__minrgp, __CLPK_real *__rtol1,
        __CLPK_real *__rtol2, __CLPK_real *__w, __CLPK_real *__werr,
        __CLPK_real *__wgap, __CLPK_integer *__iblock, __CLPK_integer *__indexw,
        __CLPK_real *__gers, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__isuppz, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slarscl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__x,
        __CLPK_integer *__ldx);


int slartg_(__CLPK_real *__f, __CLPK_real *__g, __CLPK_real *__cs,
        __CLPK_real *__sn,
        __CLPK_real *__r__);


int slartv_(__CLPK_integer *__n, __CLPK_real *__x, __CLPK_integer *__incx,
        __CLPK_real *__y, __CLPK_integer *__incy, __CLPK_real *__c__,
        __CLPK_real *__s,
        __CLPK_integer *__incc);


int slaruv_(__CLPK_integer *__iseed, __CLPK_integer *__n,
        __CLPK_real *__x);


int slarz_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__l, __CLPK_real *__v, __CLPK_integer *__incv,
        __CLPK_real *__tau, __CLPK_real *__c__, __CLPK_integer *__ldc,
        __CLPK_real *__work);


int slarzb_(char *__side, char *__trans, char *__direct, char *__storev,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_real *__v, __CLPK_integer *__ldv,
        __CLPK_real *__t, __CLPK_integer *__ldt, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__ldwork);


int slarzt_(char *__direct, char *__storev, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_real *__v, __CLPK_integer *__ldv,
        __CLPK_real *__tau, __CLPK_real *__t,
        __CLPK_integer *__ldt);


int slas2_(__CLPK_real *__f, __CLPK_real *__g, __CLPK_real *__h__,
        __CLPK_real *__ssmin,
        __CLPK_real *__ssmax);


int slascl_(char *__type__, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_real *__cfrom, __CLPK_real *__cto, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int slascl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__x,
        __CLPK_integer *__ldx);


int slasd0_(__CLPK_integer *__n, __CLPK_integer *__sqre, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__u, __CLPK_integer *__ldu,
        __CLPK_real *__vt, __CLPK_integer *__ldvt, __CLPK_integer *__smlsiz,
        __CLPK_integer *__iwork, __CLPK_real *__work,
        __CLPK_integer *__info);


int slasd1_(__CLPK_integer *__nl, __CLPK_integer *__nr, __CLPK_integer *__sqre,
        __CLPK_real *__d__, __CLPK_real *__alpha, __CLPK_real *__beta,
        __CLPK_real *__u, __CLPK_integer *__ldu, __CLPK_real *__vt,
        __CLPK_integer *__ldvt, __CLPK_integer *__idxq, __CLPK_integer *__iwork,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int slasd2_(__CLPK_integer *__nl, __CLPK_integer *__nr, __CLPK_integer *__sqre,
        __CLPK_integer *__k, __CLPK_real *__d__, __CLPK_real *__z__,
        __CLPK_real *__alpha, __CLPK_real *__beta, __CLPK_real *__u,
        __CLPK_integer *__ldu, __CLPK_real *__vt, __CLPK_integer *__ldvt,
        __CLPK_real *__dsigma, __CLPK_real *__u2, __CLPK_integer *__ldu2,
        __CLPK_real *__vt2, __CLPK_integer *__ldvt2, __CLPK_integer *__idxp,
        __CLPK_integer *__idx, __CLPK_integer *__idxc, __CLPK_integer *__idxq,
        __CLPK_integer *__coltyp,
        __CLPK_integer *__info);


int slasd3_(__CLPK_integer *__nl, __CLPK_integer *__nr, __CLPK_integer *__sqre,
        __CLPK_integer *__k, __CLPK_real *__d__, __CLPK_real *__q,
        __CLPK_integer *__ldq, __CLPK_real *__dsigma, __CLPK_real *__u,
        __CLPK_integer *__ldu, __CLPK_real *__u2, __CLPK_integer *__ldu2,
        __CLPK_real *__vt, __CLPK_integer *__ldvt, __CLPK_real *__vt2,
        __CLPK_integer *__ldvt2, __CLPK_integer *__idxc, __CLPK_integer *__ctot,
        __CLPK_real *__z__,
        __CLPK_integer *__info);


int slasd4_(__CLPK_integer *__n, __CLPK_integer *__i__, __CLPK_real *__d__,
        __CLPK_real *__z__, __CLPK_real *__delta, __CLPK_real *__rho,
        __CLPK_real *__sigma, __CLPK_real *__work,
        __CLPK_integer *__info);


int slasd5_(__CLPK_integer *__i__, __CLPK_real *__d__, __CLPK_real *__z__,
        __CLPK_real *__delta, __CLPK_real *__rho, __CLPK_real *__dsigma,
        __CLPK_real *__work);


int slasd6_(__CLPK_integer *__icompq, __CLPK_integer *__nl,
        __CLPK_integer *__nr, __CLPK_integer *__sqre, __CLPK_real *__d__,
        __CLPK_real *__vf, __CLPK_real *__vl, __CLPK_real *__alpha,
        __CLPK_real *__beta, __CLPK_integer *__idxq, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_integer *__ldgcol, __CLPK_real *__givnum,
        __CLPK_integer *__ldgnum, __CLPK_real *__poles, __CLPK_real *__difl,
        __CLPK_real *__difr, __CLPK_real *__z__, __CLPK_integer *__k,
        __CLPK_real *__c__, __CLPK_real *__s, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slasd7_(__CLPK_integer *__icompq, __CLPK_integer *__nl,
        __CLPK_integer *__nr, __CLPK_integer *__sqre, __CLPK_integer *__k,
        __CLPK_real *__d__, __CLPK_real *__z__, __CLPK_real *__zw,
        __CLPK_real *__vf, __CLPK_real *__vfw, __CLPK_real *__vl,
        __CLPK_real *__vlw, __CLPK_real *__alpha, __CLPK_real *__beta,
        __CLPK_real *__dsigma, __CLPK_integer *__idx, __CLPK_integer *__idxp,
        __CLPK_integer *__idxq, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_integer *__ldgcol, __CLPK_real *__givnum,
        __CLPK_integer *__ldgnum, __CLPK_real *__c__, __CLPK_real *__s,
        __CLPK_integer *__info);


int slasd8_(__CLPK_integer *__icompq, __CLPK_integer *__k, __CLPK_real *__d__,
        __CLPK_real *__z__, __CLPK_real *__vf, __CLPK_real *__vl,
        __CLPK_real *__difl, __CLPK_real *__difr, __CLPK_integer *__lddifr,
        __CLPK_real *__dsigma, __CLPK_real *__work,
        __CLPK_integer *__info);


int slasda_(__CLPK_integer *__icompq, __CLPK_integer *__smlsiz,
        __CLPK_integer *__n, __CLPK_integer *__sqre, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__u, __CLPK_integer *__ldu,
        __CLPK_real *__vt, __CLPK_integer *__k, __CLPK_real *__difl,
        __CLPK_real *__difr, __CLPK_real *__z__, __CLPK_real *__poles,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_integer *__ldgcol, __CLPK_integer *__perm, __CLPK_real *__givnum,
        __CLPK_real *__c__, __CLPK_real *__s, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int slasdq_(char *__uplo, __CLPK_integer *__sqre, __CLPK_integer *__n,
        __CLPK_integer *__ncvt, __CLPK_integer *__nru, __CLPK_integer *__ncc,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__vt,
        __CLPK_integer *__ldvt, __CLPK_real *__u, __CLPK_integer *__ldu,
        __CLPK_real *__c__, __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


int slasdt_(__CLPK_integer *__n, __CLPK_integer *__lvl, __CLPK_integer *__nd,
        __CLPK_integer *__inode, __CLPK_integer *__ndiml,
        __CLPK_integer *__ndimr,
        __CLPK_integer *__msub);


int slaset_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__alpha, __CLPK_real *__beta, __CLPK_real *__a,
        __CLPK_integer *__lda);


int slasq1_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int slasq2_(__CLPK_integer *__n, __CLPK_real *__z__,
        __CLPK_integer *__info);


int slasq3_(__CLPK_integer *__i0, __CLPK_integer *__n0, __CLPK_real *__z__,
        __CLPK_integer *__pp, __CLPK_real *__dmin__, __CLPK_real *__sigma,
        __CLPK_real *__desig, __CLPK_real *__qmax, __CLPK_integer *__nfail,
        __CLPK_integer *__iter, __CLPK_integer *__ndiv, __CLPK_logical *__ieee,
        __CLPK_integer *__ttype, __CLPK_real *__dmin1, __CLPK_real *__dmin2,
        __CLPK_real *__dn, __CLPK_real *__dn1, __CLPK_real *__dn2,
        __CLPK_real *__g,
        __CLPK_real *__tau);


int slasq4_(__CLPK_integer *__i0, __CLPK_integer *__n0, __CLPK_real *__z__,
        __CLPK_integer *__pp, __CLPK_integer *__n0in, __CLPK_real *__dmin__,
        __CLPK_real *__dmin1, __CLPK_real *__dmin2, __CLPK_real *__dn,
        __CLPK_real *__dn1, __CLPK_real *__dn2, __CLPK_real *__tau,
        __CLPK_integer *__ttype,
        __CLPK_real *__g);


int slasq5_(__CLPK_integer *__i0, __CLPK_integer *__n0, __CLPK_real *__z__,
        __CLPK_integer *__pp, __CLPK_real *__tau, __CLPK_real *__dmin__,
        __CLPK_real *__dmin1, __CLPK_real *__dmin2, __CLPK_real *__dn,
        __CLPK_real *__dnm1, __CLPK_real *__dnm2,
        __CLPK_logical *__ieee);


int slasq6_(__CLPK_integer *__i0, __CLPK_integer *__n0, __CLPK_real *__z__,
        __CLPK_integer *__pp, __CLPK_real *__dmin__, __CLPK_real *__dmin1,
        __CLPK_real *__dmin2, __CLPK_real *__dn, __CLPK_real *__dnm1,
        __CLPK_real *__dnm2);


int slasr_(char *__side, char *__pivot, char *__direct, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__c__, __CLPK_real *__s,
        __CLPK_real *__a,
        __CLPK_integer *__lda);


int slasrt_(char *__id, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_integer *__info);


int slassq_(__CLPK_integer *__n, __CLPK_real *__x, __CLPK_integer *__incx,
        __CLPK_real *__scale,
        __CLPK_real *__sumsq);


int slasv2_(__CLPK_real *__f, __CLPK_real *__g, __CLPK_real *__h__,
        __CLPK_real *__ssmin, __CLPK_real *__ssmax, __CLPK_real *__snr,
        __CLPK_real *__csr, __CLPK_real *__snl,
        __CLPK_real *__csl);


int slaswp_(__CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__k1, __CLPK_integer *__k2, __CLPK_integer *__ipiv,
        __CLPK_integer *__incx);


int slasy2_(__CLPK_logical *__ltranl, __CLPK_logical *__ltranr,
        __CLPK_integer *__isgn, __CLPK_integer *__n1, __CLPK_integer *__n2,
        __CLPK_real *__tl, __CLPK_integer *__ldtl, __CLPK_real *__tr,
        __CLPK_integer *__ldtr, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__scale, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__xnorm,
        __CLPK_integer *__info);


int slasyf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_integer *__kb, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_real *__w, __CLPK_integer *__ldw,
        __CLPK_integer *__info);


int slatbs_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_integer *__kd, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__x, __CLPK_real *__scale,
        __CLPK_real *__cnorm,
        __CLPK_integer *__info);


int slatdf_(__CLPK_integer *__ijob, __CLPK_integer *__n, __CLPK_real *__z__,
        __CLPK_integer *__ldz, __CLPK_real *__rhs, __CLPK_real *__rdsum,
        __CLPK_real *__rdscal, __CLPK_integer *__ipiv,
        __CLPK_integer *__jpiv);


int slatps_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_real *__ap, __CLPK_real *__x,
        __CLPK_real *__scale, __CLPK_real *__cnorm,
        __CLPK_integer *__info);


int slatrd_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__e,
        __CLPK_real *__tau, __CLPK_real *__w,
        __CLPK_integer *__ldw);


int slatrs_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__x, __CLPK_real *__scale, __CLPK_real *__cnorm,
        __CLPK_integer *__info);


int slatrz_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__l,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work);


int slatzm_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__v, __CLPK_integer *__incv, __CLPK_real *__tau,
        __CLPK_real *__c1, __CLPK_real *__c2, __CLPK_integer *__ldc,
        __CLPK_real *__work);


int slauu2_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int slauum_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int sopgtr_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__tau, __CLPK_real *__q, __CLPK_integer *__ldq,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sopmtr_(char *__side, char *__uplo, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__ap, __CLPK_real *__tau,
        __CLPK_real *__c__, __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


int sorg2l_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sorg2r_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sorgbr_(char *__vect, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__tau, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sorghr_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sorgl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sorglq_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sorgql_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sorgqr_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sorgr2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sorgrq_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sorgtr_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sorm2l_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


int sorm2r_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


int sormbr_(char *__vect, char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sormhr_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__c__, __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sorml2_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


int sormlq_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sormql_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sormqr_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sormr2_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


int sormr3_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__l,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__c__, __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__info);


int sormrq_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sormrz_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__l,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_real *__c__, __CLPK_integer *__ldc, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int sormtr_(char *__side, char *__uplo, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__tau, __CLPK_real *__c__, __CLPK_integer *__ldc,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int spbcon_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_real *__ab, __CLPK_integer *__ldab, __CLPK_real *__anorm,
        __CLPK_real *__rcond, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int spbequ_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_real *__ab, __CLPK_integer *__ldab, __CLPK_real *__s,
        __CLPK_real *__scond, __CLPK_real *__amax,
        __CLPK_integer *__info);


int spbrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__afb, __CLPK_integer *__ldafb, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int spbstf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int spbsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int spbsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__afb, __CLPK_integer *__ldafb,
        char *__equed, __CLPK_real *__s, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__rcond, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int spbtf2_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int spbtrf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int spbtrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int spftrf_(char *__transr, char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__info);


int spftri_(char *__transr, char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__info);


int spftrs_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__a, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int spocon_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int spoequ_(__CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        __CLPK_integer *__info);


int spoequb_(__CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        __CLPK_integer *__info);


int sporfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__af,
        __CLPK_integer *__ldaf, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__x, __CLPK_integer *__ldx, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sposv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sposvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__af, __CLPK_integer *__ldaf, char *__equed,
        __CLPK_real *__s, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__x, __CLPK_integer *__ldx, __CLPK_real *__rcond,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int spotf2_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int spotrf_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int spotri_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int spotrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sppcon_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__anorm, __CLPK_real *__rcond, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sppequ_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__s, __CLPK_real *__scond, __CLPK_real *__amax,
        __CLPK_integer *__info);


int spprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__ap, __CLPK_real *__afp, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sppsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__ap, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sppsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__ap, __CLPK_real *__afp,
        char *__equed, __CLPK_real *__s, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__rcond, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int spptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_integer *__info);


int spptri_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_integer *__info);


int spptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__ap, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int spstf2_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__piv, __CLPK_integer *__rank,
        __CLPK_real *__tol, __CLPK_real *__work,
        __CLPK_integer *__info);


int spstrf_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__piv, __CLPK_integer *__rank,
        __CLPK_real *__tol, __CLPK_real *__work,
        __CLPK_integer *__info);


int sptcon_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__anorm, __CLPK_real *__rcond, __CLPK_real *__work,
        __CLPK_integer *__info);


int spteqr_(char *__compz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sptrfs_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__df, __CLPK_real *__ef,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__x,
        __CLPK_integer *__ldx, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sptsv_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sptsvx_(char *__fact, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__df,
        __CLPK_real *__ef, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__x, __CLPK_integer *__ldx, __CLPK_real *__rcond,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__info);


int spttrf_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_integer *__info);


int spttrs_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sptts2_(__CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__b,
        __CLPK_integer *__ldb);


int srscl_(__CLPK_integer *__n, __CLPK_real *__sa, __CLPK_real *__sx,
        __CLPK_integer *__incx);


int ssbev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int ssbevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int ssbevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__q, __CLPK_integer *__ldq, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_real *__work,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int ssbgst_(char *__vect, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__bb, __CLPK_integer *__ldbb,
        __CLPK_real *__x, __CLPK_integer *__ldx, __CLPK_real *__work,
        __CLPK_integer *__info);


int ssbgv_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__bb, __CLPK_integer *__ldbb,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int ssbgvd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__bb, __CLPK_integer *__ldbb,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int ssbgvx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__bb, __CLPK_integer *__ldbb,
        __CLPK_real *__q, __CLPK_integer *__ldq, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_real *__work,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int ssbtrd_(char *__vect, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__q,
        __CLPK_integer *__ldq, __CLPK_real *__work,
        __CLPK_integer *__info);


int ssfrk_(char *__transr, char *__uplo, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_real *__alpha, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__beta,
        __CLPK_real *__c__);


int sspcon_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_integer *__ipiv, __CLPK_real *__anorm, __CLPK_real *__rcond,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sspev_(char *__jobz, char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sspevd_(char *__jobz, char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int sspevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__ap, __CLPK_real *__vl, __CLPK_real *__vu,
        __CLPK_integer *__il, __CLPK_integer *__iu, __CLPK_real *__abstol,
        __CLPK_integer *__m, __CLPK_real *__w, __CLPK_real *__z__,
        __CLPK_integer *__ldz, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int sspgst_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__ap, __CLPK_real *__bp,
        __CLPK_integer *__info);


int sspgv_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_real *__ap, __CLPK_real *__bp,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sspgvd_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_real *__ap, __CLPK_real *__bp,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int sspgvx_(__CLPK_integer *__itype, char *__jobz, char *__range, char *__uplo,
        __CLPK_integer *__n, __CLPK_real *__ap, __CLPK_real *__bp,
        __CLPK_real *__vl, __CLPK_real *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_real *__abstol, __CLPK_integer *__m,
        __CLPK_real *__w, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int ssprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__ap, __CLPK_real *__afp, __CLPK_integer *__ipiv,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__x,
        __CLPK_integer *__ldx, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sspsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__ap, __CLPK_integer *__ipiv, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sspsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__ap, __CLPK_real *__afp,
        __CLPK_integer *__ipiv, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__x, __CLPK_integer *__ldx, __CLPK_real *__rcond,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int ssptrd_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__tau,
        __CLPK_integer *__info);


int ssptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int ssptri_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_integer *__ipiv, __CLPK_real *__work,
        __CLPK_integer *__info);


int ssptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__ap, __CLPK_integer *__ipiv, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int sstebz_(char *__range, char *__order, __CLPK_integer *__n,
        __CLPK_real *__vl, __CLPK_real *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_real *__abstol, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_integer *__m, __CLPK_integer *__nsplit,
        __CLPK_real *__w, __CLPK_integer *__iblock, __CLPK_integer *__isplit,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int sstedc_(char *__compz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int sstegr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_integer *__isuppz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int sstein_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_integer *__m, __CLPK_real *__w, __CLPK_integer *__iblock,
        __CLPK_integer *__isplit, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int sstemr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_integer *__m, __CLPK_real *__w, __CLPK_real *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__nzc, __CLPK_integer *__isuppz,
        __CLPK_logical *__tryrac, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int ssteqr_(char *__compz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int ssterf_(__CLPK_integer *__n, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_integer *__info);


int sstev_(char *__jobz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int sstevd_(char *__jobz, __CLPK_integer *__n, __CLPK_real *__d__,
        __CLPK_real *__e, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int sstevr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_integer *__isuppz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int sstevx_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_real *__d__, __CLPK_real *__e, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_real *__work,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int ssycon_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_real *__anorm,
        __CLPK_real *__rcond, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int ssyequb_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__s, __CLPK_real *__scond,
        __CLPK_real *__amax, __CLPK_real *__work,
        __CLPK_integer *__info);


int ssyev_(char *__jobz, char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__w, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int ssyevd_(char *__jobz, char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__w, __CLPK_real *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int ssyevr_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_integer *__isuppz,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int ssyevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_real *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int ssygs2_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ssygst_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ssygv_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__w,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int ssygvd_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__w,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int ssygvx_(__CLPK_integer *__itype, char *__jobz, char *__range, char *__uplo,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__vl,
        __CLPK_real *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_real *__abstol, __CLPK_integer *__m, __CLPK_real *__w,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_real *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int ssyrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int ssysv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int ssysvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__af, __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__x,
        __CLPK_integer *__ldx, __CLPK_real *__rcond, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int ssytd2_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__tau,
        __CLPK_integer *__info);


int ssytf2_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int ssytrd_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__d__, __CLPK_real *__e,
        __CLPK_real *__tau, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int ssytrf_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int ssytri_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_real *__work,
        __CLPK_integer *__info);


int ssytrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int stbcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_real *__ab, __CLPK_integer *__ldab,
        __CLPK_real *__rcond, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int stbrfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__x, __CLPK_integer *__ldx, __CLPK_real *__ferr,
        __CLPK_real *__berr, __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int stbtrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs, __CLPK_real *__ab,
        __CLPK_integer *__ldab, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int stfsm_(char *__transr, char *__side, char *__uplo, char *__trans,
        char *__diag, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_real *__alpha, __CLPK_real *__a, __CLPK_real *__b,
        __CLPK_integer *__ldb);


int stftri_(char *__transr, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_real *__a,
        __CLPK_integer *__info);


int stfttp_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__arf, __CLPK_real *__ap,
        __CLPK_integer *__info);


int stfttr_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__arf, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int stgevc_(char *__side, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_real *__s, __CLPK_integer *__lds,
        __CLPK_real *__p, __CLPK_integer *__ldp, __CLPK_real *__vl,
        __CLPK_integer *__ldvl, __CLPK_real *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__mm, __CLPK_integer *__m, __CLPK_real *__work,
        __CLPK_integer *__info);


int stgex2_(__CLPK_logical *__wantq, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__q,
        __CLPK_integer *__ldq, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__j1, __CLPK_integer *__n1, __CLPK_integer *__n2,
        __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int stgexc_(__CLPK_logical *__wantq, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__q,
        __CLPK_integer *__ldq, __CLPK_real *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__ifst, __CLPK_integer *__ilst, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int stgsen_(__CLPK_integer *__ijob, __CLPK_logical *__wantq,
        __CLPK_logical *__wantz, __CLPK_logical *__select, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__alphar, __CLPK_real *__alphai,
        __CLPK_real *__beta, __CLPK_real *__q, __CLPK_integer *__ldq,
        __CLPK_real *__z__, __CLPK_integer *__ldz, __CLPK_integer *__m,
        __CLPK_real *__pl, __CLPK_real *__pr, __CLPK_real *__dif,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int stgsja_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__p, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__tola,
        __CLPK_real *__tolb, __CLPK_real *__alpha, __CLPK_real *__beta,
        __CLPK_real *__u, __CLPK_integer *__ldu, __CLPK_real *__v,
        __CLPK_integer *__ldv, __CLPK_real *__q, __CLPK_integer *__ldq,
        __CLPK_real *__work, __CLPK_integer *__ncycle,
        __CLPK_integer *__info);


int stgsna_(char *__job, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__vl,
        __CLPK_integer *__ldvl, __CLPK_real *__vr, __CLPK_integer *__ldvr,
        __CLPK_real *__s, __CLPK_real *__dif, __CLPK_integer *__mm,
        __CLPK_integer *__m, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int stgsy2_(char *__trans, __CLPK_integer *__ijob, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__d__, __CLPK_integer *__ldd,
        __CLPK_real *__e, __CLPK_integer *__lde, __CLPK_real *__f,
        __CLPK_integer *__ldf, __CLPK_real *__scale, __CLPK_real *__rdsum,
        __CLPK_real *__rdscal, __CLPK_integer *__iwork, __CLPK_integer *__pq,
        __CLPK_integer *__info);


int stgsyl_(char *__trans, __CLPK_integer *__ijob, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__c__,
        __CLPK_integer *__ldc, __CLPK_real *__d__, __CLPK_integer *__ldd,
        __CLPK_real *__e, __CLPK_integer *__lde, __CLPK_real *__f,
        __CLPK_integer *__ldf, __CLPK_real *__scale, __CLPK_real *__dif,
        __CLPK_real *__work, __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int stpcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_real *__ap, __CLPK_real *__rcond, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int stprfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__ap, __CLPK_real *__b,
        __CLPK_integer *__ldb, __CLPK_real *__x, __CLPK_integer *__ldx,
        __CLPK_real *__ferr, __CLPK_real *__berr, __CLPK_real *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int stptri_(char *__uplo, char *__diag, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_integer *__info);


int stptrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__ap, __CLPK_real *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int stpttf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_real *__ap, __CLPK_real *__arf,
        __CLPK_integer *__info);


int stpttr_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__ap,
        __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int strcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_real *__a, __CLPK_integer *__lda, __CLPK_real *__rcond,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int strevc_(char *__side, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_real *__t, __CLPK_integer *__ldt,
        __CLPK_real *__vl, __CLPK_integer *__ldvl, __CLPK_real *__vr,
        __CLPK_integer *__ldvr, __CLPK_integer *__mm, __CLPK_integer *__m,
        __CLPK_real *__work,
        __CLPK_integer *__info);


int strexc_(char *__compq, __CLPK_integer *__n, __CLPK_real *__t,
        __CLPK_integer *__ldt, __CLPK_real *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__ifst, __CLPK_integer *__ilst, __CLPK_real *__work,
        __CLPK_integer *__info);


int strrfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb, __CLPK_real *__x,
        __CLPK_integer *__ldx, __CLPK_real *__ferr, __CLPK_real *__berr,
        __CLPK_real *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int strsen_(char *__job, char *__compq, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_real *__t, __CLPK_integer *__ldt,
        __CLPK_real *__q, __CLPK_integer *__ldq, __CLPK_real *__wr,
        __CLPK_real *__wi, __CLPK_integer *__m, __CLPK_real *__s,
        __CLPK_real *__sep, __CLPK_real *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int strsna_(char *__job, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_real *__t, __CLPK_integer *__ldt,
        __CLPK_real *__vl, __CLPK_integer *__ldvl, __CLPK_real *__vr,
        __CLPK_integer *__ldvr, __CLPK_real *__s, __CLPK_real *__sep,
        __CLPK_integer *__mm, __CLPK_integer *__m, __CLPK_real *__work,
        __CLPK_integer *__ldwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int strsyl_(char *__trana, char *__tranb, __CLPK_integer *__isgn,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_real *__c__, __CLPK_integer *__ldc, __CLPK_real *__scale,
        __CLPK_integer *__info);


int strti2_(char *__uplo, char *__diag, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int strtri_(char *__uplo, char *__diag, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int strtrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_real *__a, __CLPK_integer *__lda,
        __CLPK_real *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int strttf_(char *__transr, char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__arf,
        __CLPK_integer *__info);


int strttp_(char *__uplo, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__ap,
        __CLPK_integer *__info);


int stzrqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau,
        __CLPK_integer *__info);


int stzrzf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_real *__a,
        __CLPK_integer *__lda, __CLPK_real *__tau, __CLPK_real *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zbdsqr_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__ncvt,
        __CLPK_integer *__nru, __CLPK_integer *__ncc, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublecomplex *__vt,
        __CLPK_integer *__ldvt, __CLPK_doublecomplex *__u,
        __CLPK_integer *__ldu, __CLPK_doublecomplex *__c__,
        __CLPK_integer *__ldc, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zcgesv_(__CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublecomplex *__work, __CLPK_complex *__swork,
        __CLPK_doublereal *__rwork, __CLPK_integer *__iter,
        __CLPK_integer *__info);


int zcposv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublecomplex *__work, __CLPK_complex *__swork,
        __CLPK_doublereal *__rwork, __CLPK_integer *__iter,
        __CLPK_integer *__info);


int zdrscl_(__CLPK_integer *__n, __CLPK_doublereal *__sa,
        __CLPK_doublecomplex *__sx,
        __CLPK_integer *__incx);


int zgbbrd_(char *__vect, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__ncc, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__pt, __CLPK_integer *__ldpt,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgbcon_(char *__norm, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgbequ_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rowcnd,
        __CLPK_doublereal *__colcnd, __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int zgbequb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rowcnd,
        __CLPK_doublereal *__colcnd, __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int zgbrfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublecomplex *__afb, __CLPK_integer *__ldafb,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgbsv_(__CLPK_integer *__n, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zgbsvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__kl, __CLPK_integer *__ku, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublecomplex *__afb, __CLPK_integer *__ldafb,
        __CLPK_integer *__ipiv, char *__equed, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgbtf2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zgbtrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zgbtrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zgebak_(char *__job, char *__side, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__scale, __CLPK_integer *__m,
        __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_integer *__info);


int zgebal_(char *__job, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__scale,
        __CLPK_integer *__info);


int zgebd2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublecomplex *__tauq, __CLPK_doublecomplex *__taup,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zgebrd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublecomplex *__tauq, __CLPK_doublecomplex *__taup,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgecon_(char *__norm, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgeequ_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rowcnd,
        __CLPK_doublereal *__colcnd, __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int zgeequb_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__r__, __CLPK_doublereal *__c__,
        __CLPK_doublereal *__rowcnd, __CLPK_doublereal *__colcnd,
        __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int zgees_(char *__jobvs, char *__sort, __CLPK_L_fp __select,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__sdim, __CLPK_doublecomplex *__w,
        __CLPK_doublecomplex *__vs, __CLPK_integer *__ldvs,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int zgeesx_(char *__jobvs, char *__sort, __CLPK_L_fp __select, char *__sense,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__sdim, __CLPK_doublecomplex *__w,
        __CLPK_doublecomplex *__vs, __CLPK_integer *__ldvs,
        __CLPK_doublereal *__rconde, __CLPK_doublereal *__rcondv,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork, __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int zgeev_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__w, __CLPK_doublecomplex *__vl,
        __CLPK_integer *__ldvl, __CLPK_doublecomplex *__vr,
        __CLPK_integer *__ldvr, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgeevx_(char *__balanc, char *__jobvl, char *__jobvr, char *__sense,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__w, __CLPK_doublecomplex *__vl,
        __CLPK_integer *__ldvl, __CLPK_doublecomplex *__vr,
        __CLPK_integer *__ldvr, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__scale, __CLPK_doublereal *__abnrm,
        __CLPK_doublereal *__rconde, __CLPK_doublereal *__rcondv,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgegs_(char *__jobvsl, char *__jobvsr, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__alpha, __CLPK_doublecomplex *__beta,
        __CLPK_doublecomplex *__vsl, __CLPK_integer *__ldvsl,
        __CLPK_doublecomplex *__vsr, __CLPK_integer *__ldvsr,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgegv_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__alpha, __CLPK_doublecomplex *__beta,
        __CLPK_doublecomplex *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublecomplex *__vr, __CLPK_integer *__ldvr,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgehd2_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zgehrd_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgelq2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zgelqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgels_(char *__trans, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgelsd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__s, __CLPK_doublereal *__rcond,
        __CLPK_integer *__rank, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int zgelss_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__s, __CLPK_doublereal *__rcond,
        __CLPK_integer *__rank, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgelsx_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__jpvt, __CLPK_doublereal *__rcond,
        __CLPK_integer *__rank, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgelsy_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__jpvt, __CLPK_doublereal *__rcond,
        __CLPK_integer *__rank, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgeql2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zgeqlf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgeqp3_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgeqpf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgeqr2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zgeqrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgerfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__af, __CLPK_integer *__ldaf,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgerq2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zgerqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgesc2_(__CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__rhs,
        __CLPK_integer *__ipiv, __CLPK_integer *__jpiv,
        __CLPK_doublereal *__scale);


int zgesdd_(char *__jobz, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__s, __CLPK_doublecomplex *__u,
        __CLPK_integer *__ldu, __CLPK_doublecomplex *__vt,
        __CLPK_integer *__ldvt, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int zgesv_(__CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zgesvd_(char *__jobu, char *__jobvt, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__s, __CLPK_doublecomplex *__u,
        __CLPK_integer *__ldu, __CLPK_doublecomplex *__vt,
        __CLPK_integer *__ldvt, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgesvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv, char *__equed,
        __CLPK_doublereal *__r__, __CLPK_doublereal *__c__,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgetc2_(__CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv, __CLPK_integer *__jpiv,
        __CLPK_integer *__info);


int zgetf2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zgetrf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zgetri_(__CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgetrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zggbak_(char *__job, char *__side, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__lscale, __CLPK_doublereal *__rscale,
        __CLPK_integer *__m, __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_integer *__info);


int zggbal_(char *__job, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__lscale, __CLPK_doublereal *__rscale,
        __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int zgges_(char *__jobvsl, char *__jobvsr, char *__sort, __CLPK_L_fp __selctg,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__sdim, __CLPK_doublecomplex *__alpha,
        __CLPK_doublecomplex *__beta, __CLPK_doublecomplex *__vsl,
        __CLPK_integer *__ldvsl, __CLPK_doublecomplex *__vsr,
        __CLPK_integer *__ldvsr, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int zggesx_(char *__jobvsl, char *__jobvsr, char *__sort, __CLPK_L_fp __selctg,
        char *__sense, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__sdim, __CLPK_doublecomplex *__alpha,
        __CLPK_doublecomplex *__beta, __CLPK_doublecomplex *__vsl,
        __CLPK_integer *__ldvsl, __CLPK_doublecomplex *__vsr,
        __CLPK_integer *__ldvsr, __CLPK_doublereal *__rconde,
        __CLPK_doublereal *__rcondv, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int zggev_(char *__jobvl, char *__jobvr, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__alpha, __CLPK_doublecomplex *__beta,
        __CLPK_doublecomplex *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublecomplex *__vr, __CLPK_integer *__ldvr,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zggevx_(char *__balanc, char *__jobvl, char *__jobvr, char *__sense,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__alpha, __CLPK_doublecomplex *__beta,
        __CLPK_doublecomplex *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublecomplex *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublereal *__lscale, __CLPK_doublereal *__rscale,
        __CLPK_doublereal *__abnrm, __CLPK_doublereal *__bbnrm,
        __CLPK_doublereal *__rconde, __CLPK_doublereal *__rcondv,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork, __CLPK_integer *__iwork,
        __CLPK_logical *__bwork,
        __CLPK_integer *__info);


int zggglm_(__CLPK_integer *__n, __CLPK_integer *__m, __CLPK_integer *__p,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__d__, __CLPK_doublecomplex *__x,
        __CLPK_doublecomplex *__y, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zgghrd_(char *__compq, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__info);


int zgglse_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__p,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__c__, __CLPK_doublecomplex *__d__,
        __CLPK_doublecomplex *__x, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zggqrf_(__CLPK_integer *__n, __CLPK_integer *__m, __CLPK_integer *__p,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__taua, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__taub,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zggrqf_(__CLPK_integer *__m, __CLPK_integer *__p, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__taua, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__taub,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zggsvd_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__p, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__beta,
        __CLPK_doublecomplex *__u, __CLPK_integer *__ldu,
        __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int zggsvp_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__p, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__tola, __CLPK_doublereal *__tolb,
        __CLPK_integer *__k, __CLPK_integer *__l, __CLPK_doublecomplex *__u,
        __CLPK_integer *__ldu, __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__iwork, __CLPK_doublereal *__rwork,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zgtcon_(char *__norm, __CLPK_integer *__n, __CLPK_doublecomplex *__dl,
        __CLPK_doublecomplex *__d__, __CLPK_doublecomplex *__du,
        __CLPK_doublecomplex *__du2, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zgtrfs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__dl, __CLPK_doublecomplex *__d__,
        __CLPK_doublecomplex *__du, __CLPK_doublecomplex *__dlf,
        __CLPK_doublecomplex *__df, __CLPK_doublecomplex *__duf,
        __CLPK_doublecomplex *__du2, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgtsv_(__CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__dl, __CLPK_doublecomplex *__d__,
        __CLPK_doublecomplex *__du, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zgtsvx_(char *__fact, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__dl,
        __CLPK_doublecomplex *__d__, __CLPK_doublecomplex *__du,
        __CLPK_doublecomplex *__dlf, __CLPK_doublecomplex *__df,
        __CLPK_doublecomplex *__duf, __CLPK_doublecomplex *__du2,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zgttrf_(__CLPK_integer *__n, __CLPK_doublecomplex *__dl,
        __CLPK_doublecomplex *__d__, __CLPK_doublecomplex *__du,
        __CLPK_doublecomplex *__du2, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zgttrs_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__dl, __CLPK_doublecomplex *__d__,
        __CLPK_doublecomplex *__du, __CLPK_doublecomplex *__du2,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zgtts2_(__CLPK_integer *__itrans, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__dl,
        __CLPK_doublecomplex *__d__, __CLPK_doublecomplex *__du,
        __CLPK_doublecomplex *__du2, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb);


int zhbev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhbevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork, __CLPK_integer *__lrwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zhbevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__q,
        __CLPK_integer *__ldq, __CLPK_doublereal *__vl, __CLPK_doublereal *__vu,
        __CLPK_integer *__il, __CLPK_integer *__iu, __CLPK_doublereal *__abstol,
        __CLPK_integer *__m, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int zhbgst_(char *__vect, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__bb,
        __CLPK_integer *__ldbb, __CLPK_doublecomplex *__x,
        __CLPK_integer *__ldx, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhbgv_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__bb,
        __CLPK_integer *__ldbb, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhbgvd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__bb,
        __CLPK_integer *__ldbb, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork, __CLPK_integer *__lrwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zhbgvx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__ka, __CLPK_integer *__kb, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__bb,
        __CLPK_integer *__ldbb, __CLPK_doublecomplex *__q,
        __CLPK_integer *__ldq, __CLPK_doublereal *__vl, __CLPK_doublereal *__vu,
        __CLPK_integer *__il, __CLPK_integer *__iu, __CLPK_doublereal *__abstol,
        __CLPK_integer *__m, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int zhbtrd_(char *__vect, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublecomplex *__q,
        __CLPK_integer *__ldq, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zhecon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zheequb_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zheev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zheevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zheevr_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__isuppz,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork, __CLPK_integer *__lrwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zheevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int zhegs2_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zhegst_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zhegv_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhegvd_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zhegvx_(__CLPK_integer *__itype, char *__jobz, char *__range, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork, __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int zherfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__af, __CLPK_integer *__ldaf,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhesv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zhesvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhetd2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublecomplex *__tau,
        __CLPK_integer *__info);


int zhetf2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zhetrd_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zhetrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zhetri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zhetrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zhfrk_(char *__transr, char *__uplo, char *__trans, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublereal *__alpha,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__beta,
        __CLPK_doublecomplex *__c__);


int zhgeqz_(char *__job, char *__compq, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__h__, __CLPK_integer *__ldh,
        __CLPK_doublecomplex *__t, __CLPK_integer *__ldt,
        __CLPK_doublecomplex *__alpha, __CLPK_doublecomplex *__beta,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhpcon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zhpev_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhpevd_(char *__jobz, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork, __CLPK_integer *__lrwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zhpevx_(char *__jobz, char *__range, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int zhpgst_(__CLPK_integer *__itype, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap, __CLPK_doublecomplex *__bp,
        __CLPK_integer *__info);


int zhpgv_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__bp, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhpgvd_(__CLPK_integer *__itype, char *__jobz, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__bp, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_doublereal *__rwork, __CLPK_integer *__lrwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zhpgvx_(__CLPK_integer *__itype, char *__jobz, char *__range, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__bp, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_integer *__il, __CLPK_integer *__iu,
        __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int zhprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_doublecomplex *__afp,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhpsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zhpsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__afp, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zhptrd_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublecomplex *__tau,
        __CLPK_integer *__info);


int zhptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zhptri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zhptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zhsein_(char *__side, char *__eigsrc, char *__initv,
        __CLPK_logical *__select, __CLPK_integer *__n,
        __CLPK_doublecomplex *__h__, __CLPK_integer *__ldh,
        __CLPK_doublecomplex *__w, __CLPK_doublecomplex *__vl,
        __CLPK_integer *__ldvl, __CLPK_doublecomplex *__vr,
        __CLPK_integer *__ldvr, __CLPK_integer *__mm, __CLPK_integer *__m,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__ifaill, __CLPK_integer *__ifailr,
        __CLPK_integer *__info);


int zhseqr_(char *__job, char *__compz, __CLPK_integer *__n,
        __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__h__, __CLPK_integer *__ldh,
        __CLPK_doublecomplex *__w, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zlabrd_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublecomplex *__tauq, __CLPK_doublecomplex *__taup,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublecomplex *__y,
        __CLPK_integer *__ldy);


int zlacgv_(__CLPK_integer *__n, __CLPK_doublecomplex *__x,
        __CLPK_integer *__incx);


int zlacn2_(__CLPK_integer *__n, __CLPK_doublecomplex *__v,
        __CLPK_doublecomplex *__x, __CLPK_doublereal *__est,
        __CLPK_integer *__kase,
        __CLPK_integer *__isave);


int zlacon_(__CLPK_integer *__n, __CLPK_doublecomplex *__v,
        __CLPK_doublecomplex *__x, __CLPK_doublereal *__est,
        __CLPK_integer *__kase);


int zlacp2_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb);


int zlacpy_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb);


int zlacrm_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__rwork);


int zlacrt_(__CLPK_integer *__n, __CLPK_doublecomplex *__cx,
        __CLPK_integer *__incx, __CLPK_doublecomplex *__cy,
        __CLPK_integer *__incy, __CLPK_doublecomplex *__c__,
        __CLPK_doublecomplex *__s);


void zladiv_(__CLPK_doublecomplex *__ret_val, __CLPK_doublecomplex *__x,
        __CLPK_doublecomplex *__y);


int zlaed0_(__CLPK_integer *__qsiz, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__qstore, __CLPK_integer *__ldqs,
        __CLPK_doublereal *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int zlaed7_(__CLPK_integer *__n, __CLPK_integer *__cutpnt,
        __CLPK_integer *__qsiz, __CLPK_integer *__tlvls,
        __CLPK_integer *__curlvl, __CLPK_integer *__curpbm,
        __CLPK_doublereal *__d__, __CLPK_doublecomplex *__q,
        __CLPK_integer *__ldq, __CLPK_doublereal *__rho,
        __CLPK_integer *__indxq, __CLPK_doublereal *__qstore,
        __CLPK_integer *__qptr, __CLPK_integer *__prmptr,
        __CLPK_integer *__perm, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_doublereal *__givnum,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int zlaed8_(__CLPK_integer *__k, __CLPK_integer *__n, __CLPK_integer *__qsiz,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__rho,
        __CLPK_integer *__cutpnt, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__dlamda, __CLPK_doublecomplex *__q2,
        __CLPK_integer *__ldq2, __CLPK_doublereal *__w, __CLPK_integer *__indxp,
        __CLPK_integer *__indx, __CLPK_integer *__indxq, __CLPK_integer *__perm,
        __CLPK_integer *__givptr, __CLPK_integer *__givcol,
        __CLPK_doublereal *__givnum,
        __CLPK_integer *__info);


int zlaein_(__CLPK_logical *__rightv, __CLPK_logical *__noinit,
        __CLPK_integer *__n, __CLPK_doublecomplex *__h__, __CLPK_integer *__ldh,
        __CLPK_doublecomplex *__w, __CLPK_doublecomplex *__v,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__rwork, __CLPK_doublereal *__eps3,
        __CLPK_doublereal *__smlnum,
        __CLPK_integer *__info);


int zlaesy_(__CLPK_doublecomplex *__a, __CLPK_doublecomplex *__b,
        __CLPK_doublecomplex *__c__, __CLPK_doublecomplex *__rt1,
        __CLPK_doublecomplex *__rt2, __CLPK_doublecomplex *__evscal,
        __CLPK_doublecomplex *__cs1,
        __CLPK_doublecomplex *__sn1);


int zlaev2_(__CLPK_doublecomplex *__a, __CLPK_doublecomplex *__b,
        __CLPK_doublecomplex *__c__, __CLPK_doublereal *__rt1,
        __CLPK_doublereal *__rt2, __CLPK_doublereal *__cs1,
        __CLPK_doublecomplex *__sn1);


int zlag2c_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__sa, __CLPK_integer *__ldsa,
        __CLPK_integer *__info);


int zlags2_(__CLPK_logical *__upper, __CLPK_doublereal *__a1,
        __CLPK_doublecomplex *__a2, __CLPK_doublereal *__a3,
        __CLPK_doublereal *__b1, __CLPK_doublecomplex *__b2,
        __CLPK_doublereal *__b3, __CLPK_doublereal *__csu,
        __CLPK_doublecomplex *__snu, __CLPK_doublereal *__csv,
        __CLPK_doublecomplex *__snv, __CLPK_doublereal *__csq,
        __CLPK_doublecomplex *__snq);


int zlagtm_(char *__trans, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__alpha, __CLPK_doublecomplex *__dl,
        __CLPK_doublecomplex *__d__, __CLPK_doublecomplex *__du,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__beta, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb);


int zlahef_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_integer *__kb, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__w,
        __CLPK_integer *__ldw,
        __CLPK_integer *__info);


int zlahqr_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__h__, __CLPK_integer *__ldh,
        __CLPK_doublecomplex *__w, __CLPK_integer *__iloz,
        __CLPK_integer *__ihiz, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz,
        __CLPK_integer *__info);


int zlahr2_(__CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__nb,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__t,
        __CLPK_integer *__ldt, __CLPK_doublecomplex *__y,
        __CLPK_integer *__ldy);


int zlahrd_(__CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__nb,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__t,
        __CLPK_integer *__ldt, __CLPK_doublecomplex *__y,
        __CLPK_integer *__ldy);


int zlaic1_(__CLPK_integer *__job, __CLPK_integer *__j,
        __CLPK_doublecomplex *__x, __CLPK_doublereal *__sest,
        __CLPK_doublecomplex *__w, __CLPK_doublecomplex *__gamma,
        __CLPK_doublereal *__sestpr, __CLPK_doublecomplex *__s,
        __CLPK_doublecomplex *__c__);


int zlals0_(__CLPK_integer *__icompq, __CLPK_integer *__nl,
        __CLPK_integer *__nr, __CLPK_integer *__sqre, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__bx, __CLPK_integer *__ldbx,
        __CLPK_integer *__perm, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_integer *__ldgcol,
        __CLPK_doublereal *__givnum, __CLPK_integer *__ldgnum,
        __CLPK_doublereal *__poles, __CLPK_doublereal *__difl,
        __CLPK_doublereal *__difr, __CLPK_doublereal *__z__,
        __CLPK_integer *__k, __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zlalsa_(__CLPK_integer *__icompq, __CLPK_integer *__smlsiz,
        __CLPK_integer *__n, __CLPK_integer *__nrhs, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__bx,
        __CLPK_integer *__ldbx, __CLPK_doublereal *__u, __CLPK_integer *__ldu,
        __CLPK_doublereal *__vt, __CLPK_integer *__k, __CLPK_doublereal *__difl,
        __CLPK_doublereal *__difr, __CLPK_doublereal *__z__,
        __CLPK_doublereal *__poles, __CLPK_integer *__givptr,
        __CLPK_integer *__givcol, __CLPK_integer *__ldgcol,
        __CLPK_integer *__perm, __CLPK_doublereal *__givnum,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_doublereal *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int zlalsd_(char *__uplo, __CLPK_integer *__smlsiz, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublereal *__rcond,
        __CLPK_integer *__rank, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


__CLPK_doublereal zlangb_(char *__norm, __CLPK_integer *__n,
        __CLPK_integer *__kl, __CLPK_integer *__ku, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlange_(char *__norm, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlangt_(char *__norm, __CLPK_integer *__n,
        __CLPK_doublecomplex *__dl, __CLPK_doublecomplex *__d__,
        __CLPK_doublecomplex *__du);


__CLPK_doublereal zlanhb_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlanhe_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlanhf_(char *__norm, char *__transr, char *__uplo,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlanhp_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlanhs_(char *__norm, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlanht_(char *__norm, __CLPK_integer *__n,
        __CLPK_doublereal *__d__,
        __CLPK_doublecomplex *__e);


__CLPK_doublereal zlansb_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlansp_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlansy_(char *__norm, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlantb_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlantp_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublereal *__work);


__CLPK_doublereal zlantr_(char *__norm, char *__uplo, char *__diag,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_doublereal *__work);


int zlapll_(__CLPK_integer *__n, __CLPK_doublecomplex *__x,
        __CLPK_integer *__incx, __CLPK_doublecomplex *__y,
        __CLPK_integer *__incy,
        __CLPK_doublereal *__ssmin);


int zlapmt_(__CLPK_logical *__forwrd, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_integer *__k);


int zlaqgb_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__kl,
        __CLPK_integer *__ku, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rowcnd,
        __CLPK_doublereal *__colcnd, __CLPK_doublereal *__amax,
        char *__equed);


int zlaqge_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__r__,
        __CLPK_doublereal *__c__, __CLPK_doublereal *__rowcnd,
        __CLPK_doublereal *__colcnd, __CLPK_doublereal *__amax,
        char *__equed);


int zlaqhb_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        char *__equed);


int zlaqhe_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        char *__equed);


int zlaqhp_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        char *__equed);


int zlaqp2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__offset,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__jpvt, __CLPK_doublecomplex *__tau,
        __CLPK_doublereal *__vn1, __CLPK_doublereal *__vn2,
        __CLPK_doublecomplex *__work);


int zlaqps_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__offset,
        __CLPK_integer *__nb, __CLPK_integer *__kb, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__jpvt,
        __CLPK_doublecomplex *__tau, __CLPK_doublereal *__vn1,
        __CLPK_doublereal *__vn2, __CLPK_doublecomplex *__auxv,
        __CLPK_doublecomplex *__f,
        __CLPK_integer *__ldf);


int zlaqr0_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__h__, __CLPK_integer *__ldh,
        __CLPK_doublecomplex *__w, __CLPK_integer *__iloz,
        __CLPK_integer *__ihiz, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zlaqr1_(__CLPK_integer *__n, __CLPK_doublecomplex *__h__,
        __CLPK_integer *__ldh, __CLPK_doublecomplex *__s1,
        __CLPK_doublecomplex *__s2,
        __CLPK_doublecomplex *__v);


int zlaqr2_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ktop, __CLPK_integer *__kbot,
        __CLPK_integer *__nw, __CLPK_doublecomplex *__h__,
        __CLPK_integer *__ldh, __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__ns, __CLPK_integer *__nd, __CLPK_doublecomplex *__sh,
        __CLPK_doublecomplex *__v, __CLPK_integer *__ldv, __CLPK_integer *__nh,
        __CLPK_doublecomplex *__t, __CLPK_integer *__ldt, __CLPK_integer *__nv,
        __CLPK_doublecomplex *__wv, __CLPK_integer *__ldwv,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork);


int zlaqr3_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ktop, __CLPK_integer *__kbot,
        __CLPK_integer *__nw, __CLPK_doublecomplex *__h__,
        __CLPK_integer *__ldh, __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__ns, __CLPK_integer *__nd, __CLPK_doublecomplex *__sh,
        __CLPK_doublecomplex *__v, __CLPK_integer *__ldv, __CLPK_integer *__nh,
        __CLPK_doublecomplex *__t, __CLPK_integer *__ldt, __CLPK_integer *__nv,
        __CLPK_doublecomplex *__wv, __CLPK_integer *__ldwv,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork);


int zlaqr4_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__h__, __CLPK_integer *__ldh,
        __CLPK_doublecomplex *__w, __CLPK_integer *__iloz,
        __CLPK_integer *__ihiz, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zlaqr5_(__CLPK_logical *__wantt, __CLPK_logical *__wantz,
        __CLPK_integer *__kacc22, __CLPK_integer *__n, __CLPK_integer *__ktop,
        __CLPK_integer *__kbot, __CLPK_integer *__nshfts,
        __CLPK_doublecomplex *__s, __CLPK_doublecomplex *__h__,
        __CLPK_integer *__ldh, __CLPK_integer *__iloz, __CLPK_integer *__ihiz,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_doublecomplex *__u, __CLPK_integer *__ldu, __CLPK_integer *__nv,
        __CLPK_doublecomplex *__wv, __CLPK_integer *__ldwv,
        __CLPK_integer *__nh, __CLPK_doublecomplex *__wh,
        __CLPK_integer *__ldwh);


int zlaqsb_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        char *__equed);


int zlaqsp_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        char *__equed);


int zlaqsy_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        char *__equed);


int zlar1v_(__CLPK_integer *__n, __CLPK_integer *__b1, __CLPK_integer *__bn,
        __CLPK_doublereal *__lambda, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__l, __CLPK_doublereal *__ld,
        __CLPK_doublereal *__lld, __CLPK_doublereal *__pivmin,
        __CLPK_doublereal *__gaptol, __CLPK_doublecomplex *__z__,
        __CLPK_logical *__wantnc, __CLPK_integer *__negcnt,
        __CLPK_doublereal *__ztz, __CLPK_doublereal *__mingma,
        __CLPK_integer *__r__, __CLPK_integer *__isuppz,
        __CLPK_doublereal *__nrminv, __CLPK_doublereal *__resid,
        __CLPK_doublereal *__rqcorr,
        __CLPK_doublereal *__work);


int zlar2v_(__CLPK_integer *__n, __CLPK_doublecomplex *__x,
        __CLPK_doublecomplex *__y, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__incx, __CLPK_doublereal *__c__,
        __CLPK_doublecomplex *__s,
        __CLPK_integer *__incc);


int zlarcm_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__rwork);


int zlarf_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__v, __CLPK_integer *__incv,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__c__,
        __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work);


int zlarfb_(char *__side, char *__trans, char *__direct, char *__storev,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_doublecomplex *__t, __CLPK_integer *__ldt,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__ldwork);


int zlarfg_(__CLPK_integer *__n, __CLPK_doublecomplex *__alpha,
        __CLPK_doublecomplex *__x, __CLPK_integer *__incx,
        __CLPK_doublecomplex *__tau);


int zlarfp_(__CLPK_integer *__n, __CLPK_doublecomplex *__alpha,
        __CLPK_doublecomplex *__x, __CLPK_integer *__incx,
        __CLPK_doublecomplex *__tau);


int zlarft_(char *__direct, char *__storev, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__t,
        __CLPK_integer *__ldt);


int zlarfx_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__v, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work);


int zlargv_(__CLPK_integer *__n, __CLPK_doublecomplex *__x,
        __CLPK_integer *__incx, __CLPK_doublecomplex *__y,
        __CLPK_integer *__incy, __CLPK_doublereal *__c__,
        __CLPK_integer *__incc);


int zlarnv_(__CLPK_integer *__idist, __CLPK_integer *__iseed,
        __CLPK_integer *__n,
        __CLPK_doublecomplex *__x);


int zlarrv_(__CLPK_integer *__n, __CLPK_doublereal *__vl,
        __CLPK_doublereal *__vu, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__l, __CLPK_doublereal *__pivmin,
        __CLPK_integer *__isplit, __CLPK_integer *__m, __CLPK_integer *__dol,
        __CLPK_integer *__dou, __CLPK_doublereal *__minrgp,
        __CLPK_doublereal *__rtol1, __CLPK_doublereal *__rtol2,
        __CLPK_doublereal *__w, __CLPK_doublereal *__werr,
        __CLPK_doublereal *__wgap, __CLPK_integer *__iblock,
        __CLPK_integer *__indexw, __CLPK_doublereal *__gers,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__isuppz, __CLPK_doublereal *__work,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int zlarscl2_(__CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublecomplex *__x,
        __CLPK_integer *__ldx);


int zlartg_(__CLPK_doublecomplex *__f, __CLPK_doublecomplex *__g,
        __CLPK_doublereal *__cs, __CLPK_doublecomplex *__sn,
        __CLPK_doublecomplex *__r__);


int zlartv_(__CLPK_integer *__n, __CLPK_doublecomplex *__x,
        __CLPK_integer *__incx, __CLPK_doublecomplex *__y,
        __CLPK_integer *__incy, __CLPK_doublereal *__c__,
        __CLPK_doublecomplex *__s,
        __CLPK_integer *__incc);


int zlarz_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__l, __CLPK_doublecomplex *__v, __CLPK_integer *__incv,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__c__,
        __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work);


int zlarzb_(char *__side, char *__trans, char *__direct, char *__storev,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_doublecomplex *__t, __CLPK_integer *__ldt,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__ldwork);


int zlarzt_(char *__direct, char *__storev, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__t,
        __CLPK_integer *__ldt);


int zlascl_(char *__type__, __CLPK_integer *__kl, __CLPK_integer *__ku,
        __CLPK_doublereal *__cfrom, __CLPK_doublereal *__cto,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int zlascl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublecomplex *__x,
        __CLPK_integer *__ldx);


int zlaset_(char *__uplo, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__alpha, __CLPK_doublecomplex *__beta,
        __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda);


int zlasr_(char *__side, char *__pivot, char *__direct, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublereal *__c__, __CLPK_doublereal *__s,
        __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda);


int zlassq_(__CLPK_integer *__n, __CLPK_doublecomplex *__x,
        __CLPK_integer *__incx, __CLPK_doublereal *__scale,
        __CLPK_doublereal *__sumsq);


int zlaswp_(__CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__k1, __CLPK_integer *__k2,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__incx);


int zlasyf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_integer *__kb, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__w,
        __CLPK_integer *__ldw,
        __CLPK_integer *__info);


int zlat2c_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_complex *__sa, __CLPK_integer *__ldsa,
        __CLPK_integer *__info);


int zlatbs_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_integer *__kd, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__x,
        __CLPK_doublereal *__scale, __CLPK_doublereal *__cnorm,
        __CLPK_integer *__info);


int zlatdf_(__CLPK_integer *__ijob, __CLPK_integer *__n,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublecomplex *__rhs, __CLPK_doublereal *__rdsum,
        __CLPK_doublereal *__rdscal, __CLPK_integer *__ipiv,
        __CLPK_integer *__jpiv);


int zlatps_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__x, __CLPK_doublereal *__scale,
        __CLPK_doublereal *__cnorm,
        __CLPK_integer *__info);


int zlatrd_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nb,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__e, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__w,
        __CLPK_integer *__ldw);


int zlatrs_(char *__uplo, char *__trans, char *__diag, char *__normin,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__x, __CLPK_doublereal *__scale,
        __CLPK_doublereal *__cnorm,
        __CLPK_integer *__info);


int zlatrz_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__l,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work);


int zlatzm_(char *__side, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__v, __CLPK_integer *__incv,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__c1,
        __CLPK_doublecomplex *__c2, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work);


int zlauu2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int zlauum_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int zpbcon_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zpbequ_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int zpbrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__afb,
        __CLPK_integer *__ldafb, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zpbstf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int zpbsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zpbsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublecomplex *__afb, __CLPK_integer *__ldafb, char *__equed,
        __CLPK_doublereal *__s, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zpbtf2_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int zpbtrf_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_integer *__info);


int zpbtrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__kd,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zpftrf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a,
        __CLPK_integer *__info);


int zpftri_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a,
        __CLPK_integer *__info);


int zpftrs_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__a,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zpocon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zpoequ_(__CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int zpoequb_(__CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int zporfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__af, __CLPK_integer *__ldaf,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zposv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zposvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__af,
        __CLPK_integer *__ldaf, char *__equed, __CLPK_doublereal *__s,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zpotf2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int zpotrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int zpotri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int zpotrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zppcon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zppequ_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublereal *__s, __CLPK_doublereal *__scond,
        __CLPK_doublereal *__amax,
        __CLPK_integer *__info);


int zpprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_doublecomplex *__afp,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zppsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zppsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__afp, char *__equed, __CLPK_doublereal *__s,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zpptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__info);


int zpptri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__info);


int zpptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zpstf2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__piv, __CLPK_integer *__rank,
        __CLPK_doublereal *__tol, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int zpstrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__piv, __CLPK_integer *__rank,
        __CLPK_doublereal *__tol, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int zptcon_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublecomplex *__e, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zpteqr_(char *__compz, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int zptrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublecomplex *__e,
        __CLPK_doublereal *__df, __CLPK_doublecomplex *__ef,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zptsv_(__CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublecomplex *__e,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zptsvx_(char *__fact, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublecomplex *__e,
        __CLPK_doublereal *__df, __CLPK_doublecomplex *__ef,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zpttrf_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublecomplex *__e,
        __CLPK_integer *__info);


int zpttrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublereal *__d__, __CLPK_doublecomplex *__e,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zptts2_(__CLPK_integer *__iuplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublereal *__d__,
        __CLPK_doublecomplex *__e, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb);


int zrot_(__CLPK_integer *__n, __CLPK_doublecomplex *__cx,
        __CLPK_integer *__incx, __CLPK_doublecomplex *__cy,
        __CLPK_integer *__incy, __CLPK_doublereal *__c__,
        __CLPK_doublecomplex *__s);


int zspcon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__ipiv, __CLPK_doublereal *__anorm,
        __CLPK_doublereal *__rcond, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zspmv_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__alpha,
        __CLPK_doublecomplex *__ap, __CLPK_doublecomplex *__x,
        __CLPK_integer *__incx, __CLPK_doublecomplex *__beta,
        __CLPK_doublecomplex *__y,
        __CLPK_integer *__incy);


int zspr_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__alpha,
        __CLPK_doublecomplex *__x, __CLPK_integer *__incx,
        __CLPK_doublecomplex *__ap);


int zsprfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_doublecomplex *__afp,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zspsv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zspsvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__afp, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zsptrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zsptri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zsptrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ap, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int zstedc_(char *__compz, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__lrwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zstegr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_doublereal *__abstol, __CLPK_integer *__m,
        __CLPK_doublereal *__w, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_integer *__isuppz,
        __CLPK_doublereal *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork, __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zstein_(__CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_integer *__m, __CLPK_doublereal *__w,
        __CLPK_integer *__iblock, __CLPK_integer *__isplit,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_doublereal *__work, __CLPK_integer *__iwork,
        __CLPK_integer *__ifail,
        __CLPK_integer *__info);


int zstemr_(char *__jobz, char *__range, __CLPK_integer *__n,
        __CLPK_doublereal *__d__, __CLPK_doublereal *__e,
        __CLPK_doublereal *__vl, __CLPK_doublereal *__vu, __CLPK_integer *__il,
        __CLPK_integer *__iu, __CLPK_integer *__m, __CLPK_doublereal *__w,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__nzc, __CLPK_integer *__isuppz,
        __CLPK_logical *__tryrac, __CLPK_doublereal *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int zsteqr_(char *__compz, __CLPK_integer *__n, __CLPK_doublereal *__d__,
        __CLPK_doublereal *__e, __CLPK_doublecomplex *__z__,
        __CLPK_integer *__ldz, __CLPK_doublereal *__work,
        __CLPK_integer *__info);


int zsycon_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublereal *__anorm, __CLPK_doublereal *__rcond,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zsyequb_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublereal *__s,
        __CLPK_doublereal *__scond, __CLPK_doublereal *__amax,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zsymv_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__alpha,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__x, __CLPK_integer *__incx,
        __CLPK_doublecomplex *__beta, __CLPK_doublecomplex *__y,
        __CLPK_integer *__incy);


int zsyr_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__alpha,
        __CLPK_doublecomplex *__x, __CLPK_integer *__incx,
        __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda);


int zsyrfs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__af, __CLPK_integer *__ldaf,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zsysv_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zsysvx_(char *__fact, char *__uplo, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__af,
        __CLPK_integer *__ldaf, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__rcond, __CLPK_doublereal *__ferr,
        __CLPK_doublereal *__berr, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int zsytf2_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_integer *__info);


int zsytrf_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zsytri_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_integer *__ipiv,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zsytrs_(char *__uplo, __CLPK_integer *__n, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__ipiv, __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ztbcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_doublecomplex *__ab,
        __CLPK_integer *__ldab, __CLPK_doublereal *__rcond,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztbrfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztbtrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__kd, __CLPK_integer *__nrhs,
        __CLPK_doublecomplex *__ab, __CLPK_integer *__ldab,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ztfsm_(char *__transr, char *__side, char *__uplo, char *__trans,
        char *__diag, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_doublecomplex *__alpha, __CLPK_doublecomplex *__a,
        __CLPK_doublecomplex *__b,
        __CLPK_integer *__ldb);


int ztftri_(char *__transr, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a,
        __CLPK_integer *__info);


int ztfttp_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__arf, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__info);


int ztfttr_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__arf, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda,
        __CLPK_integer *__info);


int ztgevc_(char *__side, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublecomplex *__s, __CLPK_integer *__lds,
        __CLPK_doublecomplex *__p, __CLPK_integer *__ldp,
        __CLPK_doublecomplex *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublecomplex *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__mm, __CLPK_integer *__m, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztgex2_(__CLPK_logical *__wantq, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__j1,
        __CLPK_integer *__info);


int ztgexc_(__CLPK_logical *__wantq, __CLPK_logical *__wantz,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz,
        __CLPK_integer *__ifst, __CLPK_integer *__ilst,
        __CLPK_integer *__info);


int ztgsen_(__CLPK_integer *__ijob, __CLPK_logical *__wantq,
        __CLPK_logical *__wantz, __CLPK_logical *__select, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__alpha, __CLPK_doublecomplex *__beta,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__z__, __CLPK_integer *__ldz, __CLPK_integer *__m,
        __CLPK_doublereal *__pl, __CLPK_doublereal *__pr,
        __CLPK_doublereal *__dif, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__liwork,
        __CLPK_integer *__info);


int ztgsja_(char *__jobu, char *__jobv, char *__jobq, __CLPK_integer *__m,
        __CLPK_integer *__p, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_integer *__l, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublereal *__tola, __CLPK_doublereal *__tolb,
        __CLPK_doublereal *__alpha, __CLPK_doublereal *__beta,
        __CLPK_doublecomplex *__u, __CLPK_integer *__ldu,
        __CLPK_doublecomplex *__v, __CLPK_integer *__ldv,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__work, __CLPK_integer *__ncycle,
        __CLPK_integer *__info);


int ztgsna_(char *__job, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublecomplex *__vr, __CLPK_integer *__ldvr,
        __CLPK_doublereal *__s, __CLPK_doublereal *__dif, __CLPK_integer *__mm,
        __CLPK_integer *__m, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork, __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int ztgsy2_(char *__trans, __CLPK_integer *__ijob, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__d__, __CLPK_integer *__ldd,
        __CLPK_doublecomplex *__e, __CLPK_integer *__lde,
        __CLPK_doublecomplex *__f, __CLPK_integer *__ldf,
        __CLPK_doublereal *__scale, __CLPK_doublereal *__rdsum,
        __CLPK_doublereal *__rdscal,
        __CLPK_integer *__info);


int ztgsyl_(char *__trans, __CLPK_integer *__ijob, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__d__, __CLPK_integer *__ldd,
        __CLPK_doublecomplex *__e, __CLPK_integer *__lde,
        __CLPK_doublecomplex *__f, __CLPK_integer *__ldf,
        __CLPK_doublereal *__scale, __CLPK_doublereal *__dif,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__iwork,
        __CLPK_integer *__info);


int ztpcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap, __CLPK_doublereal *__rcond,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztprfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztptri_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap,
        __CLPK_integer *__info);


int ztptrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ztpttf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__ap, __CLPK_doublecomplex *__arf,
        __CLPK_integer *__info);


int ztpttr_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int ztrcon_(char *__norm, char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublereal *__rcond, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztrevc_(char *__side, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublecomplex *__t, __CLPK_integer *__ldt,
        __CLPK_doublecomplex *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublecomplex *__vr, __CLPK_integer *__ldvr,
        __CLPK_integer *__mm, __CLPK_integer *__m, __CLPK_doublecomplex *__work,
        __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztrexc_(char *__compq, __CLPK_integer *__n, __CLPK_doublecomplex *__t,
        __CLPK_integer *__ldt, __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_integer *__ifst, __CLPK_integer *__ilst,
        __CLPK_integer *__info);


int ztrrfs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__x, __CLPK_integer *__ldx,
        __CLPK_doublereal *__ferr, __CLPK_doublereal *__berr,
        __CLPK_doublecomplex *__work, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztrsen_(char *__job, char *__compq, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublecomplex *__t, __CLPK_integer *__ldt,
        __CLPK_doublecomplex *__q, __CLPK_integer *__ldq,
        __CLPK_doublecomplex *__w, __CLPK_integer *__m, __CLPK_doublereal *__s,
        __CLPK_doublereal *__sep, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int ztrsna_(char *__job, char *__howmny, __CLPK_logical *__select,
        __CLPK_integer *__n, __CLPK_doublecomplex *__t, __CLPK_integer *__ldt,
        __CLPK_doublecomplex *__vl, __CLPK_integer *__ldvl,
        __CLPK_doublecomplex *__vr, __CLPK_integer *__ldvr,
        __CLPK_doublereal *__s, __CLPK_doublereal *__sep, __CLPK_integer *__mm,
        __CLPK_integer *__m, __CLPK_doublecomplex *__work,
        __CLPK_integer *__ldwork, __CLPK_doublereal *__rwork,
        __CLPK_integer *__info);


int ztrsyl_(char *__trana, char *__tranb, __CLPK_integer *__isgn,
        __CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublereal *__scale,
        __CLPK_integer *__info);


int ztrti2_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int ztrtri_(char *__uplo, char *__diag, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_integer *__info);


int ztrtrs_(char *__uplo, char *__trans, char *__diag, __CLPK_integer *__n,
        __CLPK_integer *__nrhs, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__b, __CLPK_integer *__ldb,
        __CLPK_integer *__info);


int ztrttf_(char *__transr, char *__uplo, __CLPK_integer *__n,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__arf,
        __CLPK_integer *__info);


int ztrttp_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__ap,
        __CLPK_integer *__info);


int ztzrqf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_integer *__info);


int ztzrzf_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zung2l_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zung2r_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zungbr_(char *__vect, __CLPK_integer *__m, __CLPK_integer *__n,
        __CLPK_integer *__k, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunghr_(__CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zungl2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zunglq_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zungql_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zungqr_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zungr2_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zungrq_(__CLPK_integer *__m, __CLPK_integer *__n, __CLPK_integer *__k,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zungtr_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunm2l_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zunm2r_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zunmbr_(char *__vect, char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunmhr_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__ilo, __CLPK_integer *__ihi,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__c__,
        __CLPK_integer *__ldc, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunml2_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zunmlq_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunmql_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunmqr_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunmr2_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zunmr3_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__l,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__c__,
        __CLPK_integer *__ldc, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zunmrq_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_doublecomplex *__a,
        __CLPK_integer *__lda, __CLPK_doublecomplex *__tau,
        __CLPK_doublecomplex *__c__, __CLPK_integer *__ldc,
        __CLPK_doublecomplex *__work, __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunmrz_(char *__side, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_integer *__k, __CLPK_integer *__l,
        __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__c__,
        __CLPK_integer *__ldc, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zunmtr_(char *__side, char *__uplo, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublecomplex *__a, __CLPK_integer *__lda,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__c__,
        __CLPK_integer *__ldc, __CLPK_doublecomplex *__work,
        __CLPK_integer *__lwork,
        __CLPK_integer *__info);


int zupgtr_(char *__uplo, __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__q,
        __CLPK_integer *__ldq, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int zupmtr_(char *__side, char *__uplo, char *__trans, __CLPK_integer *__m,
        __CLPK_integer *__n, __CLPK_doublecomplex *__ap,
        __CLPK_doublecomplex *__tau, __CLPK_doublecomplex *__c__,
        __CLPK_integer *__ldc, __CLPK_doublecomplex *__work,
        __CLPK_integer *__info);


int dlamc1_(__CLPK_integer *__beta, __CLPK_integer *__t, __CLPK_logical *__rnd,
        __CLPK_logical *__ieee1);


int ilaver_(__CLPK_integer *__vers_major__, __CLPK_integer *__vers_minor__,
        __CLPK_integer *__vers_patch__);


__CLPK_doublereal slamch_(char *__cmach);


int slamc1_(__CLPK_integer *__beta, __CLPK_integer *__t, __CLPK_logical *__rnd,
        __CLPK_logical *__ieee1);


int slamc2_(__CLPK_integer *__beta, __CLPK_integer *__t, __CLPK_logical *__rnd,
        __CLPK_real *__eps, __CLPK_integer *__emin, __CLPK_real *__rmin,
        __CLPK_integer *__emax,
        __CLPK_real *__rmax);


__CLPK_doublereal slamc3_(__CLPK_real *__a,
        __CLPK_real *__b);


int slamc4_(__CLPK_integer *__emin, __CLPK_real *__start,
        __CLPK_integer *__base);


int slamc5_(__CLPK_integer *__beta, __CLPK_integer *__p, __CLPK_integer *__emin,
        __CLPK_logical *__ieee, __CLPK_integer *__emax,
        __CLPK_real *__rmax);



__CLPK_doublereal dlamch_(char *__cmach);


int dlamc1_(__CLPK_integer *__beta, __CLPK_integer *__t, __CLPK_logical *__rnd,
        __CLPK_logical *__ieee1);


int dlamc2_(__CLPK_integer *__beta, __CLPK_integer *__t, __CLPK_logical *__rnd,
        __CLPK_doublereal *__eps, __CLPK_integer *__emin,
        __CLPK_doublereal *__rmin, __CLPK_integer *__emax,
        __CLPK_doublereal *__rmax);


__CLPK_doublereal dlamc3_(__CLPK_doublereal *__a,
        __CLPK_doublereal *__b);


int dlamc4_(__CLPK_integer *__emin, __CLPK_doublereal *__start,
        __CLPK_integer *__base);


int dlamc5_(__CLPK_integer *__beta, __CLPK_integer *__p, __CLPK_integer *__emin,
        __CLPK_logical *__ieee, __CLPK_integer *__emax,
        __CLPK_doublereal *__rmax);


__CLPK_integer ilaenv_(__CLPK_integer *__ispec, char *__name__, char *__opts,
        __CLPK_integer *__n1, __CLPK_integer *__n2, __CLPK_integer *__n3,
        __CLPK_integer *__n4);



#ifdef __cplusplus
}
#endif
#endif /* __CLAPACK_H */

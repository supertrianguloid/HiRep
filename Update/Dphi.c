/***************************************************************************\
* Copyright (c) 2008, Claudio Pica                                          *   
* All rights reserved.                                                      * 
\***************************************************************************/

/*******************************************************************************
*
* File Dphi.c
*
* Action of the Wilson-Dirac operator D and hermitian g5D on a given 
* double-precision spinor field
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "suN.h"
#include "global.h"
#include "error.h"
#include "dirac.h"
#include "linear_algebra.h"
#include "spinor_field.h"
#include "geometry.h"
#include "communications.h"
#include "memory.h"

/*
 * the following variable is used to keep trace of
 * matrix-vector multiplication.
 * we count how many time the function Dphi_ is called
 */
static unsigned long int MVMcounter=0;

unsigned long int getMVM() {
	unsigned long int res=MVMcounter>>1; /* divide by two */
	MVMcounter=0; /* reset counter */

	return res;
}

/*
 * This function defines the massless Dirac operator
 * It can act on spinors defined on the whole lattice 
 * or on spinors with definite parity
 */

void Dphi_(spinor_field *out, spinor_field *in)
{
   int iy;
   _DECLARE_INT_ITERATOR(ix);
   suNf *up,*um;
   suNf_vector psi,chi;
   suNf_spinor *r=0,*sp,*sm;

   error((in==NULL)||(out==NULL),1,"Dphi_ [Dphi.c]",
         "Attempt to access unallocated memory space");
   
   error(in==out,1,"Dphi_ [Dphi.c]",
         "Input and output fields must be different");

#ifndef CHECK_SPINOR_MATCHING
   error(out->type==&glat_even && in->type!=&glat_odd,1,"Dphi_ [Dphi.c]", "Spinors don't match! (1)");
   error(out->type==&glat_odd && in->type!=&glat_even,1,"Dphi_ [Dphi.c]", "Spinors don't match! (2)");
   error(out->type==&glattice && in->type!=&glattice,1,"Dphi_ [Dphi.c]", "Spinors don't match! (3)");
#endif

   ++MVMcounter; /* count matrix calls */
   if(out->type==&glattice) ++MVMcounter;
 
/************************ loop over all lattice sites *************************/
   /* start communication of input spinor field */
   start_sf_sendrecv(in);

   _PIECE_FOR(out->type,ix) {
     _SITE_FOR(out->type,ix) {
       r=_FIELD_AT(out,ix);
 
       /******************************* direction +0 *********************************/

       iy=iup(ix,0);
       sp=_FIELD_AT(in,iy);
       up=pu_gauge_f(ix,0);
      
       _vector_add_f(psi,(*sp).c[0],(*sp).c[2]);
       _suNf_multiply(chi,(*up),psi);
      
       (*r).c[0]=chi;
       (*r).c[2]=chi;

       _vector_add_f(psi,(*sp).c[1],(*sp).c[3]);
       _suNf_multiply(chi,(*up),psi);
            
       (*r).c[1]=chi;
       (*r).c[3]=chi;

       /******************************* direction -0 *********************************/

       iy=idn(ix,0);
       sm=_FIELD_AT(in,iy);
       um=pu_gauge_f(iy,0);
      
       _vector_sub_f(psi,(*sm).c[0],(*sm).c[2]);
       _suNf_inverse_multiply(chi,(*um),psi);

       _vector_add_assign_f((*r).c[0],chi);
       _vector_sub_assign_f((*r).c[2],chi);

       _vector_sub_f(psi,(*sm).c[1],(*sm).c[3]);
       _suNf_inverse_multiply(chi,(*um),psi);
      
       _vector_add_assign_f((*r).c[1],chi);
       _vector_sub_assign_f((*r).c[3],chi);

       /******************************* direction +1 *********************************/

       iy=iup(ix,1);
       sp=_FIELD_AT(in,iy);
       up=pu_gauge_f(ix,1);
      
       _vector_i_add_f(psi,(*sp).c[0],(*sp).c[3]);
       _suNf_multiply(chi,(*up),psi);

       _vector_add_assign_f((*r).c[0],chi);
       _vector_i_sub_assign_f((*r).c[3],chi);

       _vector_i_add_f(psi,(*sp).c[1],(*sp).c[2]);
       _suNf_multiply(chi,(*up),psi);

       _vector_add_assign_f((*r).c[1],chi);
       _vector_i_sub_assign_f((*r).c[2],chi);

       /******************************* direction -1 *********************************/

       iy=idn(ix,1);
       sm=_FIELD_AT(in,iy);
       um=pu_gauge_f(iy,1);
      
       _vector_i_sub_f(psi,(*sm).c[0],(*sm).c[3]);
       _suNf_inverse_multiply(chi,(*um),psi);

       _vector_add_assign_f((*r).c[0],chi);
       _vector_i_add_assign_f((*r).c[3],chi);

       _vector_i_sub_f(psi,(*sm).c[1],(*sm).c[2]);
       _suNf_inverse_multiply(chi,(*um),psi);

       _vector_add_assign_f((*r).c[1],chi);
       _vector_i_add_assign_f((*r).c[2],chi);

       /******************************* direction +2 *********************************/

       iy=iup(ix,2);
       sp=_FIELD_AT(in,iy);
       up=pu_gauge_f(ix,2);
      
       _vector_add_f(psi,(*sp).c[0],(*sp).c[3]);
       _suNf_multiply(chi,(*up),psi);

       _vector_add_assign_f((*r).c[0],chi);
       _vector_add_assign_f((*r).c[3],chi);

       _vector_sub_f(psi,(*sp).c[1],(*sp).c[2]);
       _suNf_multiply(chi,(*up),psi);
      
       _vector_add_assign_f((*r).c[1],chi);
       _vector_sub_assign_f((*r).c[2],chi);

       /******************************* direction -2 *********************************/

       iy=idn(ix,2);
       sm=_FIELD_AT(in,iy);
       um=pu_gauge_f(iy,2);
      
       _vector_sub_f(psi,(*sm).c[0],(*sm).c[3]);
       _suNf_inverse_multiply(chi,(*um),psi);

       _vector_add_assign_f((*r).c[0],chi);
       _vector_sub_assign_f((*r).c[3],chi);

       _vector_add_f(psi,(*sm).c[1],(*sm).c[2]);
       _suNf_inverse_multiply(chi,(*um),psi);
      
       _vector_add_assign_f((*r).c[1],chi);
       _vector_add_assign_f((*r).c[2],chi);

       /******************************* direction +3 *********************************/

       iy=iup(ix,3);
       sp=_FIELD_AT(in,iy);
       up=pu_gauge_f(ix,3);
      
       _vector_i_add_f(psi,(*sp).c[0],(*sp).c[2]);
       _suNf_multiply(chi,(*up),psi);

       _vector_add_assign_f((*r).c[0],chi);
       _vector_i_sub_assign_f((*r).c[2],chi);

       _vector_i_sub_f(psi,(*sp).c[1],(*sp).c[3]);
       _suNf_multiply(chi,(*up),psi);

       _vector_add_assign_f((*r).c[1],chi);
       _vector_i_add_assign_f((*r).c[3],chi);

       /******************************* direction -3 *********************************/

       iy=idn(ix,3);
       sm=_FIELD_AT(in,iy);
       um=pu_gauge_f(iy,3);
      
       _vector_i_sub_f(psi,(*sm).c[0],(*sm).c[2]);
       _suNf_inverse_multiply(chi,(*um),psi);
      
       _vector_add_assign_f((*r).c[0],chi);
       _vector_i_add_assign_f((*r).c[2],chi);

       _vector_i_add_f(psi,(*sm).c[1],(*sm).c[3]);
       _suNf_inverse_multiply(chi,(*um),psi);

       _vector_add_assign_f((*r).c[1],chi);
       _vector_i_sub_assign_f((*r).c[3],chi);
      
       /******************************** end of loop *********************************/

       _spinor_mul_f(*r,-0.5f,*r);

     } /* SITE_FOR */
     if(_PIECE_INDEX(ix)==0) {
       /* wait for spinor to be transfered */
       complete_sf_sendrecv(in);
     }
   } /* PIECE FOR */
}



/*
 * this function takes 2 spinors defined on the whole lattice
 */
void Dphi(double m0, spinor_field *out, spinor_field *in)
{
   double rho;

   error((in==NULL)||(out==NULL),1,"Dphi [Dphi.c]",
         "Attempt to access unallocated memory space");
   
   error(in==out,1,"Dphi [Dphi.c]",
         "Input and output fields must be different");

#ifdef CHECK_SPINOR_MATCHING
   error(out->type!=&glattice || in->type!=&glattice,1,"Dphi [Dphi.c]", "Spinors are not defined on all the lattice!");
#endif /* CHECK_SPINOR_MATCHING */

   Dphi_(out, in);

   rho=4.+m0;
   spinor_field_mul_add_assign_f(out,rho,in);

}

void g5Dphi(double m0, spinor_field *out, spinor_field *in)
{
   double rho;

   error((in==NULL)||(out==NULL),1,"g5Dphi [Dphi.c]",
         "Attempt to access unallocated memory space");

   error(in==out,1,"g5Dphi [Dphi.c]",
         "Input and output fields must be different");

#ifdef CHECK_SPINOR_MATCHING
   error(out->type!=&glattice || in->type!=&glattice,1,"g5Dphi [Dphi.c]", "Spinors are not defined on all the lattice!");
#endif /* CHECK_SPINOR_MATCHING */

   Dphi_(out, in);
   
   rho=4.+m0;

   spinor_field_mul_add_assign_f(out,rho,in);
   spinor_field_g5_assign_f(out);

}

int init=1;
static spinor_field *gtmp=NULL;
static spinor_field *etmp=NULL;
static spinor_field *otmp=NULL;

static void free_mem() {
  if (gtmp!=NULL) { free_spinor_field(gtmp); etmp=NULL; }
  if (etmp!=NULL) { free_spinor_field(etmp); etmp=NULL; }
  if (otmp!=NULL) { free_spinor_field(otmp); otmp=NULL; }
  init=1;
}

void init_Dirac() {
  if (init) {
    gtmp=alloc_spinor_field_f(1,&glattice);
    etmp=alloc_spinor_field_f(1,&glat_even);
    otmp=alloc_spinor_field_f(1,&glat_odd);
    atexit(&free_mem);
    init=0;
  }
}


/* Even/Odd preconditioned dirac operator
 * this function takes 2 spinors defined on the even lattice
 * Dphi in = (4+m0)^2*in - D_EO D_OE in
 *
 */
void Dphi_eopre(double m0, spinor_field *out, spinor_field *in)
{
  double rho;
  
  error((in==NULL)||(out==NULL),1,"Dphi_eopre [Dphi.c]",
	"Attempt to access unallocated memory space");
  
  error(in==out,1,"Dphi_eopre [Dphi.c]",
	"Input and output fields must be different");
  
#ifdef CHECK_SPINOR_MATCHING
  error(out->type!=&glat_even || in->type!=&glat_even,1,"Dphi_eopre " __FILE__, "Spinors are not defined on even lattice!");
#endif /* CHECK_SPINOR_MATCHING */

  /* alloc memory for temporary spinor field */
  if (init) { init_Dirac(); init=0; }
  
  Dphi_(otmp, in);
  Dphi_(out, otmp);
  
  rho=4.0+m0;
  rho*=-rho; /* this minus sign is taken into account below */
  
  spinor_field_mul_add_assign_f(out,rho,in);
  spinor_field_minus_f(out,out);
}


/* Even/Odd preconditioned dirac operator
 * this function takes 2 spinors defined on the odd lattice
 * Dphi in = (4+m0)^2*in - D_OE D_EO in
 *
 */
void Dphi_oepre(double m0, spinor_field *out, spinor_field *in)
{
  double rho;
  
  error((in==NULL)||(out==NULL),1,"Dphi_oepre [Dphi.c]",
	"Attempt to access unallocated memory space");
  
  error(in==out,1,"Dphi_oepre [Dphi.c]",
	"Input and output fields must be different");
  
#ifdef CHECK_SPINOR_MATCHING
  error(out->type!=&glat_odd || in->type!=&glat_odd,1,"Dphi_oepre " __FILE__, "Spinors are not defined on odd lattice!");
#endif /* CHECK_SPINOR_MATCHING */

  /* alloc memory for temporary spinor field */
  if (init) { init_Dirac(); init=0; }
  
  Dphi_(etmp, in);
  Dphi_(out, etmp);
  
  rho=4.0+m0;
  rho*=-rho; /* this minus sign is taken into account below */
  
  spinor_field_mul_add_assign_f(out,rho,in);
  spinor_field_minus_f(out,out);
}



void g5Dphi_eopre(double m0, spinor_field *out, spinor_field *in)
{
  double rho;
  
  error((in==NULL)||(out==NULL),1,"g5Dphi_eopre [Dphi.c]",
	"Attempt to access unallocated memory space");
  
  error(in==out,1,"Dphi_eopre [Dphi.c]",
	"Input and output fields must be different");
  
#ifdef CHECK_SPINOR_MATCHING
  error(out->type!=&glat_even || in->type!=&glat_even,1,"g5Dphi_eopre " __FILE__, "Spinors are not defined on even lattice!");
#endif /* CHECK_SPINOR_MATCHING */

  /* alloc memory for temporary spinor field */
  if (init) { init_Dirac(); init=0; }
  
  Dphi_(otmp, in);
  Dphi_(out, otmp);
  
  rho=4.0+m0;
  rho*=-rho; /* this minus sign is taken into account below */
  
  spinor_field_mul_add_assign_f(out,rho,in);
  spinor_field_minus_f(out,out);
  spinor_field_g5_assign_f(out);
  
}

/* g5Dphi_eopre ^2 */
void g5Dphi_eopre_sq(double m0, spinor_field *out, spinor_field *in) {
  /* alloc memory for temporary spinor field */
  if (init) { init_Dirac(); init=0; }

  g5Dphi_eopre(m0, etmp, in);
  g5Dphi_eopre(m0, out, etmp);
  
}

/* g5Dhi ^2 */
void g5Dphi_sq(double m0, spinor_field *out, spinor_field *in) {
  /* alloc memory for temporary spinor field */
  if (init) { init_Dirac(); init=0; }
  
  g5Dphi(m0, gtmp, in);
  g5Dphi(m0, out, gtmp);

}

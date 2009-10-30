/***************************************************************************\
* Copyright (c) 2008, Claudio Pica                                          *   
* All rights reserved.                                                      * 
\***************************************************************************/

#include "global.h"
#include "update.h"
#include "suN.h"
#include "utils.h"
#include "representation.h"
#include "logger.h"
#include "communications.h"

#include <stdio.h>
#include <math.h>

extern rhmc_par _update_par;

#define _print_avect(a) printf("(%3.5f,%3.5f,%3.5f,%3.5f,%3.5f,%3.5f,%3.5f,%3.5f)\n",(a).c1,(a).c2,(a).c3,(a).c4,(a).c5,(a).c6,(a).c7,(a).c8)

#define _print_mat(a) printf("(%3.5f,%3.5f,%3.5f)\n(%3.5f,%3.5f,%3.5f)\n(%3.5f,%3.5f,%3.5f)\n",(a).c1_1.re,(a).c1_2.re,(a).c1_3.re,(a).c2_1.re,(a).c2_2.re,(a).c2_3.re,(a).c3_1.re,(a).c3_2.re,(a).c3_3.re);printf("(%3.5f,%3.5f,%3.5f)\n(%3.5f,%3.5f,%3.5f)\n(%3.5f,%3.5f,%3.5f)\n",(a).c1_1.im,(a).c1_2.im,(a).c1_3.im,(a).c2_1.im,(a).c2_2.im,(a).c2_3.im,(a).c3_1.im,(a).c3_2.im,(a).c3_3.im)


void Force0(double dt, suNg_av_field *force){
  static suNg s1,s2;
  static suNg_algebra_vector f;
  double forcestat[2]={0.,0.}; /* used for computation of avr and max force */
  double nsq;
  int mu,x;
  _DECLARE_INT_ITERATOR(i);

  /* check input types */
#ifndef CHECK_SPINOR_MATCHING
  _TWO_SPINORS_MATCHING(u_gauge,force);
#endif

  _MASTER_FOR(&glattice,i) {
    for (mu=0; mu<4; ++mu) {
      staples(i,mu,&s1);
      _suNg_times_suNg_dagger(s2,*_4FIELD_AT(u_gauge,i,mu),s1);
    
      /* the projection itself takes the TA: proj(M) = proj(TA(M)) */
      _fund_algebra_project(f,s2);
    
      _algebra_vector_mul_add_assign_g(*_4FIELD_AT(force,i,mu), dt*(-_update_par.beta/((double)(NG))), f);

      _algebra_vector_sqnorm_g(nsq,f);
      forcestat[0]+=sqrt(nsq);
      for(x=0;x<NG*NG-1;++x){
	if(forcestat[1]<fabs(*(((double*)&f)+x))) forcestat[1]=fabs(*(((double*)&f)+x));
      }
    }
  }
	
  global_sum(forcestat,2);
  forcestat[0]*=dt*_update_par.beta/((double)(NG*4*GLB_T*GLB_X*GLB_Y*GLB_Z));
  forcestat[1]*=dt*_update_par.beta/((double)NG);
  lprintf("FORCE0",50,"avr |force| = %1.8e maxforce = %1.8e\n",forcestat[0],forcestat[1]);
  
   #ifdef SCHRODINGER_FUNCTIONAL
       	SF_force_bcs(force);
   #endif

  }

void Force(double dt, suNg_av_field *force){
  Force0(dt, force);
  Force_rhmc_f(dt, force);
}

void SF_force_bcs(suNg_av_field *force)
{
  _DECLARE_INT_ITERATOR(i);
  int ix,iy,iz,mu;

  /* check input types */
#ifndef CHECK_SPINOR_MATCHING
  _TWO_SPINORS_MATCHING(u_gauge,force);
#endif

if(COORD[0]==0)
{
  _MASTER_FOR(&glattice,i)
  {
       	for (ix=0; ix<GLB_X/NP_X; ++ix)
        for (iy=0; iy<GLB_Y/NP_Y; ++iy)
        for (iz=0; iz<GLB_Z/NP_Z; ++iz)
	{
	{
	{
		if (ipt(0,ix,iy,iz)==i)
		{
			for (mu=0; mu<4; ++mu)
			{
				_algebra_vector_zero_g(*_4FIELD_AT(force,i,mu));
      			}
    		}
		if (ipt(1,ix,iy,iz)==i)
		{
			for (mu=1; mu<4; ++mu)
			{
				_algebra_vector_zero_g(*_4FIELD_AT(force,i,mu));
      			}
    		}
	}
	}
	}
   }
}

if(COORD[0]==NP_T-1)
{
  _MASTER_FOR(&glattice,i)
  {
       	for (ix=0; ix<GLB_X/NP_X; ++ix)
        for (iy=0; iy<GLB_Y/NP_Y; ++iy)
        for (iz=0; iz<GLB_Z/NP_Z; ++iz)
	{
	{
	{
		if (ipt((GLB_T/NP_T)-1,ix,iy,iz)==i)
		{
			for (mu=0; mu<4; ++mu)
			{
				_algebra_vector_zero_g(*_4FIELD_AT(force,i,mu));
      			}
    		}
	}
	}
	}
   }
}

}

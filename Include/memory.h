/*******************************************************************************
*
* File memory.h
* 
* Memory handling functions
*
*******************************************************************************/


#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include "suN.h"

void *amalloc(size_t size,int p);
void afree(void *addr);

void free_field(void *u);
suNg* alloc_gfield();
suNf* alloc_gfield_f();
suNg_dble* alloc_gfield_dble();
suNf_spinor* alloc_spinor_field_f();
suNg_algebra_vector* alloc_momenta();


#endif
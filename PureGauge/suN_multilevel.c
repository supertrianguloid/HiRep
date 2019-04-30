/***************************************************************************\
* Copyright (c) 2008, Claudio Pica                                          *   
* All rights reserved.                                                      * 
\***************************************************************************/

/*******************************************************************************
*
* Main pure gauge program
*
*******************************************************************************/

#define MAIN_PROGRAM

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "io.h"
#include "ranlux.h"
#include "geometry.h"
#include "update.h"
#include "global.h"
#include "observables.h"
#include "dirac.h"
#include "logger.h"
#include "memory.h"
#include "communications.h"
#include "observables.h"
#include "utils.h"
#include "suN_utils_multilevel.h"
#include "setup.h"
#include "glueballs.h"

pg_flow_ml flow = init_pg_flow_ml(flow);

int main(int argc, char *argv[])
{
  int i;

  setup_process(&argc, &argv);

  setup_gauge_fields();

  /* Init Monte Carlo */
  init_mc_ml(&flow, get_input_filename());

  /* Thermalization */
  for (i = 0; i < flow.therm; ++i)
  {
    update(flow.pg_v->beta, flow.pg_v->nhb, flow.pg_v->nor);
    if ((i % 10) == 0)
      lprintf("MAIN", 0, "%d", i);
    else
      lprintf("MAIN", 0, ".");
  }
  if (i)
    lprintf("MAIN", 0, "%d\nThermalization done.\n", i);

  /* Measures */
  for (i = flow.start; i < flow.end; ++i)
  {
    struct timeval start, end, etime; /* //for trajectory timing */
    lprintf("BLOCK", 0, " Start %d\n", i);
    lprintf("MAIN", 0, "ML Measure #%d...\n", i);

    gettimeofday(&start, 0);

    update_hb_multilevel_gb_measure(0, &(flow.pg_v->beta), flow.pg_v->nhb, flow.pg_v->nor, flow.pg_v->ml_niteration, flow.pg_v->ml_nskip, flow.pg_v->nblk, &(flow.pg_v->APEsmear), &(flow.pg_v->corrs));

    gettimeofday(&end, 0);
    timeval_subtract(&etime, &end, &start);
    lprintf("MAIN", 0, "ML Measure #%d: generated in [%ld sec %ld usec]\n", i, etime.tv_sec, etime.tv_usec);

    if (i < flow.end - 1)
    {
      gettimeofday(&start, 0);

      for (int j = 0; j < flow.nskip; ++j)
        update(flow.pg_v->beta, flow.pg_v->nhb, flow.pg_v->nor);
      gettimeofday(&end, 0);
      timeval_subtract(&etime, &end, &start);
      lprintf("MAIN", 0, "Skipped %d Trajectories: [%ld sec %ld usec]\n", flow.nskip, etime.tv_sec, etime.tv_usec);
    }

    if ((i % flow.save_freq) == 0)
    {
      save_conf(&flow, i);
      if (rlx_var.rlxd_state[0] != '\0')
      {
        lprintf("MAIN", 0, "Saving rlxd state to file %s\n", rlx_var.rlxd_state);
        write_ranlxd_state(rlx_var.rlxd_state);
      }
    }

    lprintf("BLOCK", 0, " End %d\n", i);
  }

  /* save final configuration */
  if (((--i) % flow.save_freq) != 0)
  {
    save_conf(&flow, i);
    /* Only save state if we have a file to save to */
    if (rlx_var.rlxd_state[0] != '\0')
    {
      lprintf("MAIN", 0, "Saving rlxd state to file %s\n", rlx_var.rlxd_state);
      write_ranlxd_state(rlx_var.rlxd_state);
    }
  }

  /* finalize Monte Carlo */
  end_mc_ml();

  /* close communications */
  finalize_process();

  return 0;
}

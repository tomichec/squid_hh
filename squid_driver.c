#include <stdio.h>
#include <math.h>		/* to include exp */
#include <stdlib.h>		/* calloc, exit, free */

#define T_END 10.0
#define DT 0.01

enum
{
#define VAR(name, init) var_##name,
#include "squid_var.h"
#undef VAR
  NEQ
};

typedef struct currents
{
  double I_Na;
  double I_K;
  double I_l;
} s;


/* function called by the solver */
int HH_1952 (double t, double *y, double *ydot, s * user_data);

int
main ()
{
  int i, j;			/* loop counters */
  int N = T_END / DT;		/* number of time steps */

  double t;			/* time */
  double y[NEQ], ydot[NEQ];	/* states and states increment */
  s user_data;			/* currents */

#define VAR(name, init) y[var_##name] = init;
#include "squid_var.h"
#undef VAR

  for (i = 0; i < N; i++)
    {
      t = i * DT;		/* current time */
      /* compute states increment and currents */
      HH_1952 (t, y, ydot, &user_data);

      /* fe step */
      for (j = 0; j < NEQ; j++)
	{
	  y[j] += DT * ydot[j];
	  if (y[j] != y[j])
	    {
	      fprintf (stderr, "NaN detected for y[%d].\n", j);
	      exit (1);
	    }
	}

      /* saving data */
      fprintf (stdout, "%.3f", t);	/* time */
      for (j = 0; j < NEQ; j++)
	{
	  /* print dynamical states */
	  fprintf (stdout, "\t%.6g", y[j]);
	}
      /* currents */
      fprintf (stdout, "\t%.6g", (&user_data)->I_Na);
      fprintf (stdout, "\t%.6g", (&user_data)->I_K);
      fprintf (stdout, "\n");
    }

  return (0);
}

int
HH_1952 (double t, double *y, double *ydot, s * user_data)
{
  double C_m = 1.0;

#define VAR(name, init) double name = y[var_##name];
#include "squid_var.h"
#undef VAR

#define PAR(p, c)            const double	p = c;
  /* maximum conductance of current I_Na (mS/cm^2) */
  PAR (G_Na, 120);
  /* maximum conductance of current I_K (mS/cm^2)*  */
  PAR (G_K, 36);
  /* maximum conductance of leakage current: I_l (mS/cm^2) */
  PAR (G_l, 0.3);
  /* the sign from HH1952 is according new convention */
  /* reversal potential of current I_Na (mV)  */
  PAR (E_Na, 115);
  /* reversal potential of current I_K (mV)  */
  PAR (E_K, -12);
  /* reversal potential of current I_l (mV)  */
  PAR (E_l, 10.613);
#undef PAR
  /* currents */
  double I_K = user_data->I_K = G_K * n * n * n * n * (V_m - E_K);
  double I_Na = user_data->I_Na = G_Na * m * m * m * h * (V_m - E_Na);
  double I_l = user_data->I_l = G_l * (V_m - E_l);

/* gate transition rates */
  double alpha_n = 0.01 * (-V_m + 10.0) / (exp ((-V_m + 10.0) / 10.0) - 1.0);
  double beta_n = 0.125 * exp (-V_m / 80.0);

  double alpha_m = 0.1 * (-V_m + 25.0) / (exp ((-V_m + 25.0) / 10.0) - 1.0);
  double beta_m = 4.0 * exp (-V_m / 18.0);

  double alpha_h = 0.07 * exp (-V_m / 20.0);
  double beta_h = 1.0 / (exp ((-V_m + 30.0) / 10.0) + 1.0);

/* model equations */
  double dot_V_m = -1.0 / C_m * (I_Na + I_K + I_l);
  double dot_n = alpha_n * (1 - n) - beta_n * n;
  double dot_m = alpha_m * (1 - m) - beta_m * m;
  double dot_h = alpha_h * (1 - h) - beta_h * h;

#define VAR(name, init) ydot[var_##name] = dot_##name;
#include "squid_var.h"
#undef VAR

  return (0);
}

/* Generator for Decimal Floating Point (TS 18661 part 2) functions

   Copyright (C) 2019 Free Software Foundation, Inc.

   This file is part of the Decimal Floating Point C Library.

   The Decimal Floating Point C Library is free software; you can
   redistribute it and/or modify it under the terms of the GNU Lesser
   General Public License version 2.1.

   The Decimal Floating Point C Library is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU Lesser General Public License version 2.1 for more details.

   You should have received a copy of the GNU Lesser General Public
   License version 2.1 along with the Decimal Floating Point C Library;
   if not, write to the Free Software Foundation, Inc., 59 Temple Place,
   Suite 330, Boston, MA 02111-1307 USA.

   Please see libdfp/COPYING.txt for more information. */

/*
   gcc gen-auto-dfp-tests.c -Wall -O2 -o gen-auto-dfp-tests -lmpfr -lgmp -I..

   This framework is similar in spirit to the glibc math benchmarking
   program. As of writing, no readily available, and licensing compatible
   frameworks exist to produce correctly rounded results for the majority
   of decimal IEEE 754 functions.

   For trigonometric and transcendental functions, we defer to mpfr. It
   is generally accepted as the oracle. We choose a sufficiently high
   precision and round. This should give us results correct to 1ULP.
   This is not perfect, but it gives us a starting point.

   For functions which can be correctly computed via libdecnumber, we
   defer.

   Ideally, we could copy and extend the glibc version, but in practice
   most if it is highly tailored to the nuances of binary floating point.

   TODO: Correctly round decimal values from MPFR
   TODO: Correctly rounded values for all 5 rounding modes



   For now, we take as input a file which lists tests in form:

   # Optional comment lines
   func arg1 arg2 ... argN

   For each function, we generate file:

   auto-dfp-test-out-FUNC

   of the form:
   func arg1 arg2 ... argN
   = func format arg1 arg2 ... argN : out1 out2 ... outN

*/

#include <mpfr.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define __STDC_WANT_IEC_60559_DFP_EXT__
#define __STDC_WANT_DEC_FP__
#include <dfp/math.h>


/* libbid uses the level of precision converting. Let's start here. */
#define DFP_MPFR_PREC 384
#define DFP_DEFAULT_RND MPFR_RNDN

enum func_type
{
  mpfr_d_d,
};

/* Table 3.6 in IEEE 754-2008 */
struct dec_fmt_param
{
  int p;
  int emax;
  int emin;
  mpfr_t frmax;
  mpfr_t frmin;
};

enum dec_type
{
  decimal32,
  decimal64,
  decimal128,
  decimal_type_cnt,
};

const char *fmt_str[] =
{
  "decimal32",
  "decimal64",
  "decimal128"
};


struct dec_fmt_param dec_fmt_param[] =
{
  {7, 96, -95},
  {16, 384, -383},
  {34, 6144, -6143},
};

/* Keep the results as strings. */
struct decimal_value
{
  /* Enough to hold <sign><digits>e<sign><exp digits> for the largest type */
  char v[3][1 + 34 + 2 + 4 + 1];
};

typedef union result
  {
  int si;
  long long int di;
  struct decimal_value d_;
  } result_t;

struct test
{
  int line;
  const char *stest; /* allocated */
  mpfr_t frin;
  mpfr_t frout;
  int excepts;
  result_t input; /* TODO: only one type support now. */
  result_t result; /* TODO: only one type supported now. */
  const struct testf *tf;
};

union func_ptr
{
  int (*mpfr_d_d) (mpfr_t out, mpfr_t const in, mpfr_rnd_t rnd);
};

struct testf
{
  enum func_type ftype;
  union func_ptr func;
  const char *fname;
  size_t testno;
  struct test *tests;
};

#define DECL_TESTF_D_D(func) { mpfr_d_d, { .mpfr_d_d = mpfr_ ## func,  }, \
                               #func, 0, NULL }

struct testf testlist[] =
{
  DECL_TESTF_D_D(cos),
  DECL_TESTF_D_D(acos),
  DECL_TESTF_D_D(sin),
  DECL_TESTF_D_D(asin),
  DECL_TESTF_D_D(tan),
  DECL_TESTF_D_D(atan),
  DECL_TESTF_D_D(log),
  DECL_TESTF_D_D(exp),
  { }
};

char *
xstrdup(char *in)
{
  char * out = strdup (in);
  if (!out)
    error (EXIT_FAILURE, errno, "strdup");
  return out;
}

void
alloc_test(struct testf *tf, mpfr_t v, char *test)
{
  struct test *t = NULL;

  tf->testno++;
  tf->tests = realloc (tf->tests, tf->testno * sizeof (tf->tests[0]));
  t = &tf->tests[tf->testno - 1];

  if (!tf->tests)
    error (EXIT_FAILURE, errno, "realloc");

  memset (t, 0, sizeof (*t));
  mpfr_init_set(t->frin, v, DFP_DEFAULT_RND);
  mpfr_init (t->frout);
  t->stest = test;
  t->tf = tf;
}

/*
 * For now, only parse lines of the form:
 *  func value
 *
 *  Add more complexity as required for testing.
 */
void
parse_line(char *line, const char *filename, size_t lineno)
{
  char *test = xstrdup (line);
  char *val = strchr (line, ' ');
  char *func = NULL;
  struct testf *t = NULL;
  mpfr_t frin;

  if (!val)
    goto failure;

  func = line;
  *val = '\0';
  val++;

  for (t = &testlist[0]; t->fname; t++)
    if (!strcmp (t->fname, func))
      break;

  if (!t->fname)
    goto failure;

  errno = 0;
  mpfr_init (frin);
  mpfr_strtofr (frin, val, NULL, 10, DFP_DEFAULT_RND);
  if (!errno)
    alloc_test (t, frin, test);
  mpfr_clear (frin);
  if (errno)
    goto failure;

  return; 

failure:
  error_at_line (EXIT_FAILURE, 0, filename, lineno, "failed to parse '%s'", test);
  if (test)
    free (test);
};

/* Slightly misleaning... round bfp to dfp result using default rounding mode. */
void
round_result(mpfr_t in, struct decimal_value *out)
{
  /* TODO: ignore nans */
  if (mpfr_nan_p (in))
    error(EXIT_FAILURE, 0, "Encountered NaN");

  for (int i = 0; i < decimal_type_cnt; i++)
    {
      /* Handle 0 and inf cases */
      if (mpfr_cmpabs (in, dec_fmt_param[i].frmax) > 0)
        sprintf (out->v[i], "%cInf", mpfr_signbit(in) ? '-' : ' ' );
      else if (mpfr_cmpabs(dec_fmt_param[i].frmin, in) > 0)
        sprintf (out->v[i], "%c0", mpfr_signbit(in) ? '-' : '+' );
      else
        {
          mpfr_exp_t exp;
          uintptr_t adj = mpfr_signbit (in) ? 1 : 0;

          /* Remove implicit radix point by shifting the exponent per format */
          mpfr_get_str (out->v[i], &exp, 10, dec_fmt_param[i].p, in, DFP_DEFAULT_RND);
          exp -= dec_fmt_param[i].p;
          if (exp > dec_fmt_param[i].emax || exp < dec_fmt_param[i].emin)
            error(EXIT_FAILURE, 0, "Conversion failure. Exponent out of range");
          sprintf (out->v[i] + dec_fmt_param[i].p + adj, "e%d", (int) exp);
        }
    }
}

void
compute(struct test *t)
{
  switch (t->tf->ftype)
    {
      case mpfr_d_d:
        t->tf->func.mpfr_d_d (t->frout, t->frin, DFP_DEFAULT_RND);
        round_result (t->frin, &t->input.d_);
        round_result (t->frout, &t->result.d_);
        break;
      default:
        error (EXIT_FAILURE, 0, "Unknown function type %d", (int)t->tf->ftype);
        break;
    }
}

void
init_fmt(void)
{
  struct decimal_value d;
 
  /* get approximate maximum finite value in radix-2 */ 
  for (int i = 0; i < decimal_type_cnt; i++)
    {
      memset(d.v[i], '9', dec_fmt_param[i].p);
      sprintf(d.v[i] + dec_fmt_param[i].p,"e%d",dec_fmt_param[i].emax);
      mpfr_init_set_str(dec_fmt_param[i].frmax, d.v[i], 10, MPFR_RNDZ);
      sprintf(d.v[i] + dec_fmt_param[i].p,"e%d",dec_fmt_param[i].emin);
      mpfr_init_set_str(dec_fmt_param[i].frmin, d.v[i], 10, MPFR_RNDZ);
      /* mpfr_printf("%.*Re\n", dec_fmt_param[i].p - 1, dec_fmt_param[i].frmin); */
      /* mpfr_printf("%.*Re\n", dec_fmt_param[i].p - 1, dec_fmt_param[i].frmax); */
    }
}

/*
  For the moment, recycle the existing *.input format until more
  meaningful refactoring can occur.

  Likewise, generate a makefile stub ensure these tests
  are built.
*/
void
gen_output(const char *fprefix)
{
  char fname[128];
  FILE *out = stdout;
  bool debug = !strcmp (fprefix, "-");
  FILE *automf = NULL;

  if (!debug)
    {
      automf = fopen ("Makefile.autotest", "w");
      if (!automf)
        error (EXIT_FAILURE, 0, "Failed to open makefile file");
      fprintf (automf, "# The is a generated file from gen-auto-dfp-tests\n"
                       "libdfp-auto-tests =");
    }

  for (struct testf *tf = &testlist[0]; tf->fname; tf++)
    {
      if (!tf->testno)
        continue;

      if (!debug)
        {
          snprintf (fname, sizeof (fname) - 1, "%s%s.input", fprefix, tf->fname);
          out = fopen (fname, "w");
          fprintf (automf, " %s", tf->fname);
        }

      if (!out)
        error(EXIT_FAILURE, errno, "failed to open for writing %s", fname);

      fprintf (out, "# name %s\n"
                    "# arg1 decimal\n"
                    "# ret  decimal\n", tf->fname);

      for (int i = 0; i < tf->testno; i++)
        {
          struct test *t = &tf->tests[i];

          /* fprintf (out, "%s\n", t->stest); */
          for(int fmt = 0; fmt < decimal_type_cnt; fmt++)
            {
              switch(t->tf->ftype)
                {
                case mpfr_d_d:
                  /* fprintf (out, "= %s %s %s : %s\n", t->tf->fname, fmt_str[fmt], t->input.d_.v[fmt], t->result.d_.v[fmt]); */
                  fprintf (out, "%s %s %s\n", t->input.d_.v[fmt], t->result.d_.v[fmt], fmt_str[fmt]);
                  break;

                default:
                  error (EXIT_FAILURE, 0, "Unhandled type %d", t->tf->ftype);
                  break;
                }
            }
        }

      if (!debug)
        fclose(out);
    }

  if (automf)
    {
      fprintf (automf, "\n");
      fclose (automf);
    }
}

int
main(int args, char **argv)
{
  FILE *in = NULL;
  char *line = NULL;
  size_t linelen = 0;
  size_t lineno = 0;
  ssize_t ilinelen;

  mpfr_set_default_prec( DFP_MPFR_PREC );

  if (args < 3)
    error(EXIT_FAILURE, 0, "Usage %s: <auto-dfp-test-in> <output-prefix>", argv[0]);

  in = fopen (argv[1], "r");
  if (!in)
    error (EXIT_FAILURE, errno, "Cannot open %s", argv[1]);

  init_fmt();

  /* Generate tests */
  while ( ((ilinelen = getline (&line, &linelen, in))) != -1)
  {
    lineno++;
    /* Ignore empty or comment lines */
    if (!linelen || *line == '#' || *line == '\n')
      continue;

    /* chomp trailing whitespace/delimiter if any */
    while(isspace(line[ilinelen - 1]))
      {
        line[ilinelen - 1] = '\0';
        ilinelen--;
      }

    parse_line (line, argv[1], lineno);
  }

  /* Compute expected values. TODO: +/- 1ULP for now. */
  for (struct testf *tf = &testlist[0]; tf->fname; tf++)
    for (int i = 0; i < tf->testno; i++)
      compute (&tf->tests[i]);

  /* Generate output */
  gen_output(argv[2]);

  /* cleanup */
  fclose (in);
  if (line)
    free (line);

  return EXIT_SUCCESS;
}

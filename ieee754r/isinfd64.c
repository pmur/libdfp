/* Returns non-zero if the _Decimal64 is infinite

   Copyright (C) 2006 IBM Corporation.
   Copyright (C) 2007-2014 Free Software Foundation, Inc.

   This file is part of the Decimal Floating Point C Library.

   Author(s): Joseph Kerian <jkerian@us.ibm.com>

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

   Please see libdfp/COPYING.txt for more information.  */

#include <math.h>
#include <math_private.h>
#include <ieee754r_private.h>

int
__isinfd64 (_Decimal64 x)
{
  uint64_t hx;
  GET_DEC64_WORD64 (hx, x);

  /* 0 1111000 ... == sINF  */
  return ((hx & DEC64_NAN_MASK64) == DEC64_INF_MASK64) ?
   ((hx & UINT64_C(0x8000000000000000)) ? -1 : 1) : 0;
}
hidden_def (__isinfd64)
weak_alias (__isinfd64, isinfd64)

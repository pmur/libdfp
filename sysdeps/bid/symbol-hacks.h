/* Hacks for symbol bindings.

   Copyright (C) 2014-2015 Free Software Foundation, Inc.

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

   Please see libdfp/COPYING.txt for more information.  */

#if !defined __ASSEMBLER__ && !defined NOT_IN_libdfp && defined SHARED

/* GCC generates intrisics calls to some functions for _Decimal to/from
   convertions. These bindings avoids intra library  PLT calls generations,
   since libdfp provides such symbols.  */

#if HAVE_UINT128_T
asm ("__bid_floattisd  = __GI___bid_floattisd");
asm ("__bid_floatunstisd  = __GI___bid_floatunstisd");
asm ("__bid_floattidd  = __GI___bid_floattidd");
asm ("__bid_floatunstidd  = __GI___bid_floatunstidd");
asm ("__bid_floattitd  = __GI___bid_floattitd");
asm ("__bid_floatunstitd  = __GI___bid_floatunstitd");

asm ("__bid_fixtdti = __GI___bid_fixtdti");
asm ("__bid_fixunstdti = __GI___bid_fixunstdti");
asm ("__bid_fixddti = __GI___bid_fixddti");
asm ("__bid_fixunsddti = __GI___bid_fixunsddti");
asm ("__bid_fixsdti = __GI___bid_fixsdti");
asm ("__bid_fixunssdti = __GI___bid_fixunssdti");
#endif

#endif

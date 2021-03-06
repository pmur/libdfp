/* Decimal Floating Point 128-bit compare >=.  P6 PowerPC32 version.

   Copyright (C) 2006-2015 Free Software Foundation, Inc.

   This file is part of the Decimal Floating Point C Library.

   Author(s): Steven J. Munroe  <sjmunroe@us.ibm.com>
              Ryan S. Arnold  <rsa@us.ibm.com>

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

#include <sysdep.h>


	.machine	"power6"
/* int __dpd_gttd2 (_Decimal128 x, _Decimal128 y)  */
ENTRY (__dpd_gttd2)
	dcmpuq	cr0,fp2,fp4 	/* if ((x > y) && !unordered(x,y)) */
	li	r3, 1		/*   return 1  */
	bgtlr	cr0		/* else	if ((x == y) && !unordered(x,y)) */
	li	r3, 0		/*   return 0  */
	beqlr	cr0		/* else	 */
	li	r3, -1		/*   return -1  */
	blr
END (__dpd_gttd2)
hidden_def (__dpd_gttd2)

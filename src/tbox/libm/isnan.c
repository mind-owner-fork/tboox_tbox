/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-2020, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        isnan.c
 * @ingroup     libm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "math.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_isnan(tb_double_t x)
{
#if 0
    tb_ieee_double_t e; e.d = x;
    tb_int32_t      t = e.i.h & 0x7fffffff;
    t |= (tb_uint32_t)(e.i.l | (tb_uint32_t)(-(tb_int32_t)e.i.l)) >> 31;
    t = 0x7ff00000 - t;
    return (tb_long_t)(((tb_uint32_t)t) >> 31);
#else
    // optimization
    return x != x;
#endif
}

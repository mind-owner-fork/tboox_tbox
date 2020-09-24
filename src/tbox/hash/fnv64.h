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
 * @author      alexyer, ruki
 * @file        fnv64.h
 * @ingroup     hash
 *
 */
#ifndef TB_HASH_FNV64_H
#define TB_HASH_FNV64_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! make fnv64 hash
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv64 value
 */
tb_uint64_t         tb_fnv64_make(tb_byte_t const* data, tb_size_t size, tb_uint64_t seed);

/*! make fnv64 hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv64 value
 */
tb_uint64_t         tb_fnv64_make_from_cstr(tb_char_t const* cstr, tb_uint64_t seed);

/*! make fnv64(1a) hash
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv64 value
 */
tb_uint64_t         tb_fnv64_1a_make(tb_byte_t const* data, tb_size_t size, tb_uint64_t seed);

/*! make fnv64(1a) hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the fnv64 value
 */
tb_uint64_t         tb_fnv64_1a_make_from_cstr(tb_char_t const* cstr, tb_uint64_t seed);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

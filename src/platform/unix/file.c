/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		file.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../file.h"
#include "../path.h"
#include "../../stream/stream.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <unistd.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

// file
tb_handle_t tb_file_init(tb_char_t const* path, tb_size_t mode)
{
	// check
	tb_assert_and_check_return_val(path, tb_null);

	// the full path
	tb_char_t full[TB_PATH_MAXN];
	path = tb_path_full(path, full, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_null);

	// flags
	tb_size_t flags = 0;
	if (mode & TB_FILE_MODE_RO) flags |= O_RDONLY;
	else if (mode & TB_FILE_MODE_WO) flags |= O_WRONLY;
	else if (mode & TB_FILE_MODE_RW) flags |= O_RDWR;

	if (mode & TB_FILE_MODE_CREAT) flags |= O_CREAT;
	if (mode & TB_FILE_MODE_APPEND) flags |= O_APPEND;
	if (mode & TB_FILE_MODE_TRUNC) flags |= O_TRUNC;

	// dma mode, no cache
#ifdef TB_CONFIG_OS_LINUX
	if (mode & TB_FILE_MODE_DIRECT) flags |= O_DIRECT;
#endif

	// for native aio aicp
#if defined(TB_CONFIG_ASIO_HAVE_NAIO)
	if (mode & TB_FILE_MODE_AICP) flags |= O_DIRECT;
#endif

	// noblock
	flags |= O_NONBLOCK;

	// modes
	tb_size_t modes = 0;
	if (mode & TB_FILE_MODE_CREAT) 
	{
		//if ((mode & TB_FILE_MODE_RO) | (mode & TB_FILE_MODE_RW)) modes |= S_IREAD;
		//if ((mode & TB_FILE_MODE_WO) | (mode & TB_FILE_MODE_RW)) modes |= S_IWRITE;
		modes = 0777;
	}

	// open it
	tb_long_t fd = open(path, flags, modes);
	if (fd < 0 && (mode & TB_FILE_MODE_CREAT))
	{
		// make directory
		tb_char_t 			temp[TB_PATH_MAXN] = {0};
		tb_char_t const* 	p = full;
		tb_char_t* 			t = temp;
		tb_char_t const* 	e = temp + TB_PATH_MAXN - 1;
		for (; t < e && *p; t++) 
		{
			*t = *p;
			if (*p == '/')
			{
				// make directory if not exists
				if (!tb_file_info(temp, tb_null)) mkdir(temp, S_IRWXU);

				// skip repeat '/'
				while (*p && *p == '/') p++;
			}
			else p++;
		}

		// open it again
		fd = open(path, flags, modes);
	}
 
	// ok?
	return tb_fd2handle(fd);
}
tb_bool_t tb_file_exit(tb_handle_t file)
{
	// check
	tb_assert_and_check_return_val(file, tb_false);

	// close it
	return !close(tb_handle2fd(file))? tb_true : tb_false;
}
tb_long_t tb_file_read(tb_handle_t file, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(file, -1);

	// read it
	return read(tb_handle2fd(file), data, size);
}
tb_long_t tb_file_writ(tb_handle_t file, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(file, -1);

	// writ it
	return write(tb_handle2fd(file), data, size);
}
tb_long_t tb_file_pread(tb_handle_t file, tb_byte_t* data, tb_size_t size, tb_hize_t offset)
{
	// check
	tb_assert_and_check_return_val(file, -1);

	// read it
#ifdef TB_CONFIG_OS_LINUX
	return pread64(tb_handle2fd(file), data, (size_t)size, offset);
#else
	return pread(tb_handle2fd(file), data, (size_t)size, offset);
#endif
}
tb_long_t tb_file_pwrit(tb_handle_t file, tb_byte_t const* data, tb_size_t size, tb_hize_t offset)
{
	// check
	tb_assert_and_check_return_val(file, -1);

	// writ it
#ifdef TB_CONFIG_OS_LINUX
	return pwrite64(tb_handle2fd(file), data, (size_t)size, offset);
#else
	return pwrite(tb_handle2fd(file), data, (size_t)size, offset);
#endif
}
tb_long_t tb_file_readv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(file && list && size, -1);

	// check iovec
	tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
	tb_assert_return_val(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base), -1);
	tb_assert_return_val(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len), -1);

	// read it
	return readv(tb_handle2fd(file), (struct iovec const*)list, size);
}
tb_long_t tb_file_writv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(file && list && size, -1);

	// check iovec
	tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
	tb_assert_return_val(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base), -1);
	tb_assert_return_val(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len), -1);

	// writ it
	return writev(tb_handle2fd(file), (struct iovec const*)list, size);
}
tb_long_t tb_file_preadv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
	// check
	tb_assert_and_check_return_val(file && list && size, -1);

	// check iovec
	tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
	tb_assert_return_val(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base), -1);
	tb_assert_return_val(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len), -1);

	// read it
#ifdef TB_CONFIG_OS_LINUX
	return preadv(tb_handle2fd(file), (struct iovec const*)list, size, offset);
#else
 
	// FIXME: lock it

	// save offset
	tb_hong_t current = tb_file_offset(file);
	tb_assert_and_check_return_val(current, -1);

	// seek it
	if (tb_file_seek(file, offset, TB_FILE_SEEK_BEG) != offset) return -1;

	// read it
	tb_long_t real = tb_file_readv(file, list, size);

	// restore offset
	if (tb_file_seek(file, current, TB_FILE_SEEK_BEG) != current) return -1;

	// ok
	return real;
#endif
}
tb_long_t tb_file_pwritv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
	// check
	tb_assert_and_check_return_val(file && list && size, -1);

	// check iovec
	tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
	tb_assert_return_val(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base), -1);
	tb_assert_return_val(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len), -1);

	// writ it
#ifdef TB_CONFIG_OS_LINUX
	return pwritev(tb_handle2fd(file), (struct iovec const*)list, size, offset);
#else

	// FIXME: lock it

	// save offset
	tb_hong_t current = tb_file_offset(file);
	tb_assert_and_check_return_val(current, -1);

	// seek it
	if (tb_file_seek(file, offset, TB_FILE_SEEK_BEG) != offset) return -1;

	// writ it
	tb_long_t real = tb_file_writv(file, list, size);

	// restore offset
	if (tb_file_seek(file, current, TB_FILE_SEEK_BEG) != current) return -1;

	// ok
	return real;
#endif
}
tb_bool_t tb_file_sync(tb_handle_t file)
{
	// check
	tb_assert_and_check_return_val(file, tb_false);

	// sync
#ifdef TB_CONFIG_OS_LINUX
	return !fdatasync(tb_handle2fd(file))? tb_true : tb_false;
#else
	return !fsync(tb_handle2fd(file))? tb_true : tb_false;
#endif
}
tb_hong_t tb_file_seek(tb_handle_t file, tb_hong_t offset, tb_size_t mode)
{
	// check
	tb_assert_and_check_return_val(file, -1);

	// seek
	return lseek(tb_handle2fd(file), offset, mode);
}
tb_hong_t tb_file_offset(tb_handle_t file)
{
	// check
	tb_assert_and_check_return_val(file, -1);

	// the offset
	return tb_file_seek(file, (tb_hong_t)0, TB_FILE_SEEK_CUR);
}
tb_hize_t tb_file_size(tb_handle_t file)
{
	// check
	tb_assert_and_check_return_val(file, 0);

	// the file size
	struct stat st = {0};
	return !fstat(tb_handle2fd(file), &st) && st.st_size >= 0? (tb_hize_t)st.st_size : 0;
}
tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);

	// the full path
	tb_char_t full[TB_PATH_MAXN];
	path = tb_path_full(path, full, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// exists?
	tb_check_return_val(!access(path, F_OK), tb_false);

	// get info
	if (info)
	{
		// init info
		tb_memset(info, 0, sizeof(tb_file_info_t));

		// get stat
		struct stat st = {0};
		if (!stat(path, &st))
		{
			// file type
			if (S_ISDIR(st.st_mode)) info->type = TB_FILE_TYPE_DIRECTORY;
			else info->type = TB_FILE_TYPE_FILE;

			// file size
			info->size = st.st_size >= 0? (tb_hize_t)st.st_size : 0;

			// the last access time
			info->atime = (tb_time_t)st.st_atime;

			// the last modify time
			info->mtime = (tb_time_t)st.st_mtime;
		}
	}

	// ok
	return tb_true;
}
tb_bool_t tb_file_copy(tb_char_t const* path, tb_char_t const* dest)
{
	// check
	tb_assert_and_check_return_val(path && dest, tb_false);

	// the full path
	tb_char_t full0[TB_PATH_MAXN];
	path = tb_path_full(path, full0, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// the dest path
	tb_char_t full1[TB_PATH_MAXN];
	dest = tb_path_full(dest, full1, TB_PATH_MAXN);
	tb_assert_and_check_return_val(dest, tb_false);

	// copy file
	tb_bool_t 	ok = tb_false;
	tb_handle_t ist = tb_null;
	tb_handle_t ost = tb_null;
	do
	{
		// init stream
		ist = tb_gstream_init_from_url(path);
		ost = tb_gstream_init_from_url(dest);
		tb_assert_and_check_break(ist && ost);

		// ctrl stream
		tb_gstream_ctrl(ost, TB_GSTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);

		// open stream
		if (!tb_gstream_bopen(ist)) break;
		if (!tb_gstream_bopen(ost)) break;

		// copy stream
		tb_hize_t size = tb_gstream_size(ist);
		tb_hize_t save = tb_gstream_save(ist, ost);

		// ok?
		ok = save == size? tb_true : tb_false;

	} while (0);

	// exit stream
	if (ist) tb_gstream_exit(ist);
	if (ost) tb_gstream_exit(ost);

	// ok?
	return ok;
}
tb_bool_t tb_file_create(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);

	// make it
	tb_handle_t file = tb_file_init(path, TB_FILE_MODE_CREAT | TB_FILE_MODE_WO | TB_FILE_MODE_TRUNC);
	if (file) tb_file_exit(file);

	// ok?
	return file? tb_true : tb_false;
}
tb_bool_t tb_file_remove(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);

	// the full path
	tb_char_t full[TB_PATH_MAXN];
	path = tb_path_full(path, full, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// remove it
	return !remove(path)? tb_true : tb_false;
}
tb_bool_t tb_file_rename(tb_char_t const* path, tb_char_t const* dest)
{
	// check
	tb_assert_and_check_return_val(path && dest, tb_false);

	// the full path
	tb_char_t full0[TB_PATH_MAXN];
	path = tb_path_full(path, full0, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// the dest path
	tb_char_t full1[TB_PATH_MAXN];
	dest = tb_path_full(dest, full1, TB_PATH_MAXN);
	tb_assert_and_check_return_val(dest, tb_false);

	// rename
	return !rename(path, dest)? tb_true : tb_false;
}
tb_bool_t tb_file_link(tb_char_t const* path, tb_char_t const* dest)
{
	// check
	tb_assert_and_check_return_val(path && dest, tb_false);

	// the full path
	tb_char_t full0[TB_PATH_MAXN];
	path = tb_path_full(path, full0, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// the dest path
	tb_char_t full1[TB_PATH_MAXN];
	dest = tb_path_full(dest, full1, TB_PATH_MAXN);
	tb_assert_and_check_return_val(dest, tb_false);

	// symlink
	return !symlink(path, dest)? tb_true : tb_false;
}

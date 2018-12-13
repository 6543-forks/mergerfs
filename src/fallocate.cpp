/*
  Copyright (c) 2016, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "errno.hpp"
#include "fileinfo.hpp"
#include "fs_base_fallocate.hpp"

#include <fuse.h>

static
int
_fallocate(const int   fd,
           const int   mode,
           const off_t offset,
           const off_t len)
{
  int rv;

  rv = fs::fallocate(fd,mode,offset,len);

  return ((rv == -1) ? -errno : 0);
}

namespace mergerfs
{
  namespace fuse
  {
    int
    fallocate(const char     *fusepath,
              int             mode,
              off_t           offset,
              off_t           len,
              fuse_file_info *ffi)
    {
      FileInfo *fi = reinterpret_cast<FileInfo*>(ffi->fh);

      return _fallocate(fi->fd,
                        mode,
                        offset,
                        len);
    }
  }
}

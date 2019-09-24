/*
  Copyright (c) 2018, Antonio SJ Musumeci <trapexit@spawn.link>

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

#include "buildvector.hpp"
#include "category.hpp"
#include "config.hpp"
#include "errno.hpp"
#include "fs_base_listxattr.hpp"
#include "fs_path.hpp"
#include "rwlock.hpp"
#include "ugid.hpp"
#include "xattr.hpp"

#include <fuse.h>

#include <string>
#include <vector>

#include <string.h>

using std::string;
using std::vector;

namespace l
{
  static
  int
  listxattr_controlfile(char         *list_,
                        const size_t  size_)
  {
    string xattrs;
    vector<string> keys;

    Config::keys(keys);
    for(uint64_t i = 0; i < keys.size(); i++)
      keys[i] = "user.mergerfs." + keys[i];

    xattrs.reserve(1024);
    for(size_t i = 0; i < keys.size(); i++)
      xattrs += (keys[i] + '\0');

    if(size_ == 0)
      return xattrs.size();

    if(size_ < xattrs.size())
      return -ERANGE;

    memcpy(list_,xattrs.c_str(),xattrs.size());

    return xattrs.size();
  }

  static
  int
  listxattr(Policy::Func::Search  searchFunc_,
            const Branches       &branches_,
            const uint64_t        minfreespace_,
            const char           *fusepath_,
            char                 *list_,
            const size_t          size_)
  {
    int rv;
    string fullpath;
    vector<string> basepaths;

    rv = searchFunc_(branches_,fusepath_,minfreespace_,&basepaths);
    if(rv == -1)
      return -errno;

    fullpath = fs::path::make(basepaths[0],fusepath_);

    rv = fs::llistxattr(fullpath,list_,size_);

    return ((rv == -1) ? -errno : rv);
  }
}

namespace FUSE
{
  int
  listxattr(const char *fusepath_,
            char       *list_,
            size_t      size_)
  {
    const Config &config = Config::get();

    if(fusepath_ == config.controlfile)
      return l::listxattr_controlfile(list_,size_);

    switch(config.xattr)
      {
      case Config::XAttr::ENUM::INVALID:
      case Config::XAttr::ENUM::PASSTHROUGH:
        break;
      case Config::XAttr::ENUM::NOATTR:
        return 0;
      case Config::XAttr::ENUM::NOSYS:
        return -ENOSYS;
      }

    const fuse_context *fc = fuse_get_context();
    const ugid::Set     ugid(fc->uid,fc->gid);

    return l::listxattr(config.func.listxattr.policy,
                        config.branches,
                        config.minfreespace,
                        fusepath_,
                        list_,
                        size_);
  }
}

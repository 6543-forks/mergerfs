/*
  ISC License

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

#pragma once

#include "tofrom_string.hpp"

#include <string>
#include <vector>

#include <pthread.h>

class Branch
{
public:
  enum Mode
    {
      INVALID,
      RO,
      RW,
      NC
    };

  Mode        mode;
  std::string path;

  bool ro(void) const;
  bool nc(void) const;
  bool ro_or_nc(void) const;
};

class Branches : public std::vector<Branch>, public ToFromString
{
public:
  int from_string(const std::string &str);
  std::string to_string(void) const;

public:
  void to_paths(std::vector<std::string> &vec) const;
  void locked_to_paths(std::vector<std::string> &vec) const;

public:
  void set(const std::string &str);
  void add_begin(const std::string &str);
  void add_end(const std::string &str);
  void erase_begin(void);
  void erase_end(void);
  void erase_fnmatch(const std::string &str);

public:
  mutable pthread_rwlock_t lock;
};

class SrcMounts : public ToFromString
{
public:
  SrcMounts(Branches &b_);

public:
  int from_string(const std::string &str);
  std::string to_string(void) const;

private:
  Branches &_branches;
};

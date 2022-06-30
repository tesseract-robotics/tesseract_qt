/**
 * @author Davide Faconti <davide.faconti@gmail.com>
 *
 * @copyright Copyright (C) 2015-2018 Davide Faconti <davide.faconti@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 * @par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * @par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * @par
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef TESSERACT_QT_PLOT_STRING_REF_SSO_H
#define TESSERACT_QT_PLOT_STRING_REF_SSO_H

#include <iostream>
#include <string.h>

namespace tesseract_gui
{
/**
 * @brief Super simple, unmutable, string_view with
 * small string optimization.
 * If the string is 15 bytes or less, it is copied, otherwise,
 * StringRef store a not-owning reference.
 */
class StringRef
{
private:
  static const uint64_t TYPE_BIT = uint64_t(1) << (sizeof(size_t) * 8 - 1);

  struct noSSO
  {
    const char* data;
    size_t size;
  };

  static const uint8_t SSO_SIZE = sizeof(noSSO) - 1;

  struct SSO
  {
    char data[sizeof(noSSO)];
  };

  union
  {
    noSSO no_sso;
    SSO sso;
  } _storage;

public:
  bool isSSO() const { return !(_storage.no_sso.size & TYPE_BIT); }

  StringRef() : StringRef(nullptr, 0) {}

  StringRef(const std::string& str) : StringRef(str.data(), str.size()) {}

  StringRef(const char* str) : StringRef(str, strlen(str)) {}

  explicit StringRef(const char* data_ptr, size_t length)
  {
    _storage.no_sso.data = nullptr;
    _storage.no_sso.size = 0;

    if (length <= SSO_SIZE)
    {
      memcpy(_storage.sso.data, data_ptr, length);
      _storage.sso.data[SSO_SIZE] = SSO_SIZE - length;
    }
    else
    {
      _storage.no_sso.data = data_ptr;
      _storage.no_sso.size = length;
      _storage.no_sso.size |= TYPE_BIT;
    }
  }

  const char* data() const { return isSSO() ? _storage.sso.data : _storage.no_sso.data; }

  size_t size() const { return isSSO() ? (SSO_SIZE - _storage.sso.data[SSO_SIZE]) : _storage.no_sso.size & ~TYPE_BIT; }
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLOT_STRING_REF_SSO_H

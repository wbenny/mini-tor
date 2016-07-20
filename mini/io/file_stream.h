#pragma once
#include "stream.h"
#include <mini/string.h>

#include <windows.h>

namespace mini::io {

enum class file_access : DWORD
{
  read            = GENERIC_READ,
  write           = GENERIC_WRITE,
  read_write      = GENERIC_READ | GENERIC_WRITE,
};

enum class file_mode : DWORD
{
  append,
  create          = CREATE_ALWAYS,
  create_new      = CREATE_NEW,
  open            = OPEN_EXISTING,
  open_or_create  = OPEN_ALWAYS,
  truncate        = TRUNCATE_EXISTING,
};

enum class file_share : DWORD
{
  none            = 0,
  dispose         = FILE_SHARE_DELETE,
  read            = FILE_SHARE_READ,
  read_write      = FILE_SHARE_READ | FILE_SHARE_WRITE,
  write           = FILE_SHARE_WRITE,
};

enum class file_options : DWORD
{
  none            = 0,
  delete_on_close = FILE_FLAG_DELETE_ON_CLOSE,
  random_access   = FILE_FLAG_RANDOM_ACCESS,
  sequential_scan = FILE_FLAG_SEQUENTIAL_SCAN,
  write_through   = FILE_FLAG_WRITE_THROUGH,
};

class file_stream
  : public stream
{
  public:
    file_stream(
      void
      )
      : _file_handle(INVALID_HANDLE_VALUE)
      , _access((file_access)0)
      , _share((file_share)0)
      , _mode((file_mode)0)
      , _options((file_options)0)
    {

    }

    file_stream(
      const string_ref path,
      file_access access = file_access::read_write,
      file_mode mode = file_mode::open_or_create,
      file_share share = file_share::read,
      file_options options = file_options::none
      )
    {
      open(path,
        access,
        mode,
        share,
        options);
    }

    ~file_stream(
      void
      )
    {
      close();
    }

    void
    open(
      const string_ref path,
      file_access access = file_access::read_write,
      file_mode mode = file_mode::open_or_create,
      file_share share = file_share::none,
      file_options options = file_options::none
      )
    {
      _path    = path;
      _access  = access;
      _mode    = mode;
      _share   = share;
      _options = options;

      DWORD real_mode = (DWORD)_mode;

      //
      // do not create new file when we're accessing
      // it only for read.
      //
      if (_access == file_access::read &&
          _mode   == file_mode::open_or_create)
      {
        _mode = file_mode::open;
        real_mode = (DWORD)_mode;
      }

      //
      // append is realy just open_or_create + seek at the end
      //
      if (_mode == file_mode::append)
      {
        real_mode = (DWORD)file_mode::open_or_create;
      }

      _file_handle = CreateFile(
        path.get_buffer(),
        (DWORD)_access,
        (DWORD)_share,
        NULL,
        (DWORD)real_mode,
        (DWORD)_options,
        NULL);

      if (_mode == file_mode::append)
      {
        seek(0, stream::end);
      }
    }

    void
    close(
      void
      )
    {
      if (_file_handle != INVALID_HANDLE_VALUE)
      {
        //         _access  = (file_access)0;
        //         _mode    = (file_mode)0;
        //         _share   = (file_share)0;
        //         _options = (file_options)0;

        CloseHandle(_file_handle);

        _file_handle = INVALID_HANDLE_VALUE;
      }
    }

    bool
    can_read(
      void
      ) const override
    {
      return ((DWORD)_access & (DWORD)file_access::read) != 0;
    }

    bool
    can_write(
      void
      ) const override
    {
      return ((DWORD)_access & (DWORD)file_access::write) != 0;
    }

    bool
    can_seek(
      void
      ) const override
    {
      return true;
    }

    size_t
    read(
      void* buffer,
      size_t size
      ) override
    {
      DWORD bytes_read;
      ReadFile(
        _file_handle,
        buffer,
        (DWORD)size,
        &bytes_read,
        NULL);

      return bytes_read;
    }

    size_t
    write(
      const void* buffer,
      size_t size
      ) override
    {
      DWORD bytes_written;
      WriteFile(_file_handle,
        buffer,
        (DWORD)size,
        &bytes_written,
        NULL);

      return bytes_written;
    }

    size_t
    seek(
      intptr_t offset,
      seek_origin origin = seek_origin::current
      ) override
    {
      DWORD move_method = origin == seek_origin::begin
         ? FILE_BEGIN   : origin == seek_origin::current
         ? FILE_CURRENT : origin == seek_origin::end
         ? FILE_END     : FILE_CURRENT;

      return SetFilePointer(_file_handle,
        (DWORD)offset,
        NULL,
        move_method);
    }

    void
    flush(
      void
      ) override
    {
      FlushFileBuffers(_file_handle);
    }

    size_t
    get_size(
      void
      ) const override
    {
      return GetFileSize(
        _file_handle,
        NULL);
    }

    size_t
    get_position(
      void
      ) const override
    {
      return const_cast<file_stream*>(this)->seek(0, seek_origin::current);
    }

    bool
    is_open(
      void
      ) const
    {
      return _file_handle != INVALID_HANDLE_VALUE;
    }

  private:
    HANDLE _file_handle;
    string _path;
    file_access _access;
    file_mode _mode;
    file_share _share;
    file_options _options;
};

}

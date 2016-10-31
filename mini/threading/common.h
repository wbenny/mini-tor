#pragma once
#include <windows.h>

namespace mini::threading {

enum class wait_result : DWORD
{
  success   = WAIT_OBJECT_0,
  abandoned = WAIT_ABANDONED_0,
  timeout   = WAIT_TIMEOUT,
  failed    = WAIT_FAILED,
};

#define mini_wait_success(result) ((result) == ::mini::threading::wait_result::success)

}

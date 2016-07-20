// #ifndef _DEBUG
//
// #include <windows.h>
//
// extern "C" {
//
// int _Init_global_epoch = INT_MIN;
// /*__declspec(thread)*/ int _Init_thread_epoch = INT_MIN;
//
// const int Uninitialized     = 0;
// const int BeingInitialized  = -1;
// const int EpochStart        = INT_MIN;
//
// // Helper functions for accessing the mutex and condition variable.  Can be replaced with
// // more suitable data structures provided by the CRT, preferably ones that use the most
// // efficient synchronization primitives available on the platform.
// void __cdecl
// _Init_thread_lock(
//   void
//   )
// {
//
// }
//
// void __cdecl
// _Init_thread_unlock(
//   void
//   )
// {
//
// }
//
// // Wait on the condition variable.  In the XP implementation using only a Windows event
// // we can't guarantee that we'll ever actually receive the notification signal, so we
// // must use a non-infinite timeout.  This is not optimal: we may wake up early if the
// // initializer is long-running, or we may miss the signal and not wake up until the
// // timeout expires.  The signal may be missed because the sleeping threads may be
// // stolen by the kernel to service an APC, or due to the race condition between the
// // unlock call and the WaitForSingleObject call.
// bool __cdecl
// _Init_thread_wait(
//   DWORD const timeout
//   )
// {
//   return true;
// }
//
// void __cdecl
// _Init_thread_notify(
//   void
//   )
// {
//
// }
//
// // Control access to the initialization expression.  Only one thread may leave
// // this function before the variable has completed initialization, this thread
// // will perform initialization.  All other threads are blocked until the
// // initialization completes or fails due to an exception.
// void __cdecl
// _Init_thread_header(
//   int* const pOnce
//   )
// {
//   *pOnce = BeingInitialized;
// }
//
// // Abort processing of the initializer due to an exception.  Reset the state
// // to uninitialized and release waiting threads (one of which will take over
// // initialization, any remaining will again sleep).
// void __cdecl
// _Init_thread_abort(
//   int* const pOnce
//   )
// {
//
// }
//
// // Called by the thread that completes initialization of a variable.
// // Increment the global and per thread counters, mark the variable as
// // initialized, and release waiting threads.
// void __cdecl
// _Init_thread_footer(
//   int* const pOnce
//   )
// {
//   ++_Init_global_epoch;
//   *pOnce = _Init_global_epoch;
// }
//
// }
//
// #endif

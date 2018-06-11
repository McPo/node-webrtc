/* Copyright (c) 2018 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include <stdio.h>
#include <string.h>

#include "nan.h"

#define WARN(msg) fprintf(stdout, "\033[01;33m native:%s \033[00m\n", msg)
#define ERROR(msg) fprintf(stdout, "\033[01;32m native:%s \033[00m\n", msg)
#define INFO(msg) fprintf(stdout, "\033[01;34m native:%s \033[00m\n", msg)

#if defined(TRACING)

#define TRACE(msg) printf("   TRACE: %s\n", msg)
#define TRACE_S(msg, s) printf("   TRACE: %s : %s\n", msg, s)
#define TRACE_I(msg, i) printf("   TRACE: %s : %d\n", msg, i)
#define TRACE_U(msg, u) printf("   TRACE: %s : %u\n", msg, u)
#define TRACE_X(msg, x) printf("   TRACE: %s : 0x%x\n", msg, x)
#define TRACE_PTR(msg, p) printf("   TRACE: %s : %p\n", msg, p)
#define TRACE_CALL printf("-> TRACE: Call::%s\n", __PRETTY_FUNCTION__)
#define TRACE_CALL_I(p1) printf("-> TRACE: Call::%s(%d)\n", __PRETTY_FUNCTION__, p1)
#define TRACE_CALL_P(p1) printf("-> TRACE: Call::%s(%lx)\n", __PRETTY_FUNCTION__, p1)
#define TRACE_END printf("<- Call::%s\n", __PRETTY_FUNCTION__)

#else

#define TRACE(msg)
#define TRACE_S(msg, s)
#define TRACE_I(msg, i)
#define TRACE_U(msg, u)
#define TRACE_X(msg, x)
#define TRACE_PTR(msg, x)
#define TRACE_CALL
#define TRACE_CALL_I(p1)
#define TRACE_CALL_P(p1)
#define TRACE_END

#endif

#define THROW_TYPE_ERROR(MSG) \
  return Nan::ThrowTypeError(MSG);

#define CHECK_ARG(I, CHECK, DO_TRUE, DO_FALSE) \
  if (info.Length() <= (I) || !info[I]->CHECK) { DO_FALSE; } else { DO_TRUE; }

#define REQUIRE_ARG(I, CHECK) \
  CHECK_ARG(I, CHECK, , THROW_TYPE_ERROR("Argument " #I " must be an object"))

#define REQ_OBJ_ARG(I, VAR) \
  REQUIRE_ARG(I, IsObject()) \
  Local<Object> VAR = Local<Object>::Cast(info[I])

#define OPT_INT_ARG(I, VAR, DEFAULT) \
  int VAR; \
  CHECK_ARG(I, IsNumber(), VAR = info[I]->Int32Value(), VAR = DEFAULT)

#define REQ_INT_ARG(I, VAR) \
  REQUIRE_ARG(I, IsNumber()) \
  int VAR = info[I]->Int32Value();

#define REQ_FUN_ARG(I, VAR)                                         \
  if (info.Length() <= (I) || !info[I]->IsFunction())               \
    return Nan::ThrowTypeError("Argument " #I " must be a function"); \
  Local<Function> VAR = Local<Function>::Cast(info[I]);

#define CREATE_BUFFER(name, data, length) \
  Local<Object> name ## _buf = Nan::NewBuffer(length).ToLocalChecked(); \
  memcpy(Buffer::Data(name ## _buf), data, length); \
  Local<Object> name; \
  Handle<Value> ctorArgs[3] = { name ## _buf, Nan::New<Integer>(length), Nan::New<Integer>(0) }; \
  name = Local<Function>::Cast(\
          Nan::GetCurrentContext() \
          ->Global() \
          ->Get(Nan::New("Buffer").ToLocalChecked()) \
      )->NewInstance(3, ctorArgs);

//////////// NEW STARTS HERE

#if defined(_MSC_VER)
// warning C4355: 'this' : used in base member initializer list
#pragma warning(disable:4355)
#endif

//////////////////////////////////////////////////////////////////////
// General Utilities
//////////////////////////////////////////////////////////////////////

#ifndef RTC_UNUSED
#define RTC_UNUSED(x) RtcUnused(static_cast<const void*>(&x))
#define RTC_UNUSED2(x, y) RtcUnused(static_cast<const void*>(&x)); \
    RtcUnused(static_cast<const void*>(&y))
#define RTC_UNUSED3(x, y, z) RtcUnused(static_cast<const void*>(&x)); \
    RtcUnused(static_cast<const void*>(&y)); \
    RtcUnused(static_cast<const void*>(&z))
#define RTC_UNUSED4(x, y, z, a) RtcUnused(static_cast<const void*>(&x)); \
    RtcUnused(static_cast<const void*>(&y)); \
    RtcUnused(static_cast<const void*>(&z)); \
    RtcUnused(static_cast<const void*>(&a))
#define RTC_UNUSED5(x, y, z, a, b) RtcUnused(static_cast<const void*>(&x)); \
    RtcUnused(static_cast<const void*>(&y)); \
    RtcUnused(static_cast<const void*>(&z)); \
    RtcUnused(static_cast<const void*>(&a)); \
    RtcUnused(static_cast<const void*>(&b))
inline void RtcUnused(const void*) {}
#endif  // RTC_UNUSED

#if !defined(WEBRTC_WIN)

#ifndef strnicmp
#define strnicmp(x, y, n) strncasecmp(x, y, n)
#endif

#ifndef stricmp
#define stricmp(x, y) strcasecmp(x, y)
#endif

#endif  // !defined(WEBRTC_WIN)

/////////////////////////////////////////////////////////////////////////////
// Assertions
/////////////////////////////////////////////////////////////////////////////

#ifndef ENABLE_DEBUG
#if !defined(NDEBUG)
#define ENABLE_DEBUG 1
#else
#define ENABLE_DEBUG 0
#endif
#endif  // !defined(ENABLE_DEBUG)

// Even for release builds, allow for the override of LogAssert. Though no
// macro is provided, this can still be used for explicit runtime asserts
// and allow applications to override the assert behavior.

namespace rtc {


// If a debugger is attached, triggers a debugger breakpoint. If a debugger is
// not attached, forces program termination.
void Break();

// LogAssert writes information about an assertion to the log. It's called by
// Assert (and from the ASSERT macro in debug mode) before any other action
// is taken (e.g. breaking the debugger, abort()ing, etc.).
void LogAssert(const char* function, const char* file, int line,
               const char* expression);

typedef void (*AssertLogger)(const char* function,
                             const char* file,
                             int line,
                             const char* expression);

// Sets a custom assert logger to be used instead of the default LogAssert
// behavior. To clear the custom assert logger, pass NULL for |logger| and the
// default behavior will be restored. Only one custom assert logger can be set
// at a time, so this should generally be set during application startup and
// only by one component.
void SetCustomAssertLogger(AssertLogger logger);

bool IsOdd(int n);

bool IsEven(int n);

}  // namespace rtc

#if ENABLE_DEBUG

namespace rtc {

inline bool Assert(bool result, const char* function, const char* file,
                   int line, const char* expression) {
  if (!result) {
    LogAssert(function, file, line, expression);
    Break();
  }
  return result;
}

// Same as Assert above, but does not call Break().  Used in assert macros
// that implement their own breaking.
inline bool AssertNoBreak(bool result, const char* function, const char* file,
                          int line, const char* expression) {
  if (!result)
    LogAssert(function, file, line, expression);
  return result;
}

}  // namespace rtc

#if defined(_MSC_VER) && _MSC_VER < 1300
#define __FUNCTION__ ""
#endif

#ifndef ASSERT
#if defined(WIN32)
// Using debugbreak() inline on Windows directly in the ASSERT macro, has the
// benefit of breaking exactly where the failing expression is and not two
// calls up the stack.
#define ASSERT(x) \
    (rtc::AssertNoBreak((x), __FUNCTION__, __FILE__, __LINE__, #x) ? \
     (void)(1) : __debugbreak())
#else
#define ASSERT(x) \
    (void)rtc::Assert((x), __FUNCTION__, __FILE__, __LINE__, #x)
#endif
#endif

#ifndef VERIFY
#if defined(WIN32)
#define VERIFY(x) \
    (rtc::AssertNoBreak((x), __FUNCTION__, __FILE__, __LINE__, #x) ? \
     true : (__debugbreak(), false))
#else
#define VERIFY(x) rtc::Assert((x), __FUNCTION__, __FILE__, __LINE__, #x)
#endif
#endif

#else  // !ENABLE_DEBUG

namespace rtc {

inline bool ImplicitCastToBool(bool result) { return result; }

}  // namespace rtc

#ifndef ASSERT
#define ASSERT(x) (void)0
#endif

#ifndef VERIFY
#define VERIFY(x) rtc::ImplicitCastToBool(x)
#endif

#endif  // !ENABLE_DEBUG

#define COMPILE_TIME_ASSERT(expr)       char CTA_UNIQUE_NAME[expr]
#define CTA_UNIQUE_NAME                 CTA_MAKE_NAME(__LINE__)
#define CTA_MAKE_NAME(line)             CTA_MAKE_NAME2(line)
#define CTA_MAKE_NAME2(line)            constraint_ ## line

// Forces compiler to inline, even against its better judgement. Use wisely.
#if defined(__GNUC__)
#define FORCE_INLINE __attribute__ ((__always_inline__))
#elif defined(WEBRTC_WIN)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE
#endif

// Annotate a function indicating the caller must examine the return value.
// Use like:
//   int foo() WARN_UNUSED_RESULT;
// To explicitly ignore a result, see |ignore_result()| in <base/basictypes.h>.
// TODO(ajm): Hack to avoid multiple definitions until the base/ of webrtc and
// libjingle are merged.
#if !defined(WARN_UNUSED_RESULT)
#if defined(__GNUC__) || defined(__clang__)
#define WARN_UNUSED_RESULT __attribute__ ((__warn_unused_result__))
#else
#define WARN_UNUSED_RESULT
#endif
#endif  // WARN_UNUSED_RESULT

//////////// OLD STOPS HERE

#endif  // SRC_COMMON_H_

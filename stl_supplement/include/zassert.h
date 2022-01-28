#pragma once
#ifndef ZSTL_ASSERT_H
#define ZSTL_ASSERT_H

#include <assert.h>

#define ZASSERT(cond, msg) \
  assert((cond) && (msg))

#endif // ZSTL_ASSERT_H
//
//  vortex.cpp
//  Sources of main context features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../include/vortex.h"
#ifndef VORTEX_DISABLE
#include "../include/vortex_internals.h"

// runtime pointer
#ifndef CVortexMaker
std::weak_ptr<VxContext> CVortexMaker;
#endif

std::shared_ptr<VxContext> vxe::create_context() {
  auto prev_ctx = get_current_context();
  auto ctx = std::make_shared<VxContext>();

  set_current_context(ctx);
  initialize();

  if (prev_ctx != nullptr)
    set_current_context(prev_ctx);

  return ctx;
}

void vxe::set_current_context(std::shared_ptr<VxContext> ctx) {
  CVortexMaker = ctx;
}

std::shared_ptr<VxContext> vxe::get_current_context() {
  return CVortexMaker.lock();
}

void vxe::destroy_context(std::shared_ptr<VxContext> ctx) {
  auto prev_ctx = get_current_context();
  if (ctx == nullptr)
    ctx = prev_ctx;

  set_current_context((prev_ctx != ctx) ? prev_ctx : nullptr);
}

void vxe::initialize() {
  auto ctx = get_current_context();
  assert(ctx != nullptr && "initialize() called without context !");
  ctx->initialized = true;
}

#endif  // VORTEX_DISABLE
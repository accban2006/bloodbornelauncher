// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "common/arch.h"
#include "common/types.h"

namespace Common {

void* GetXmmPointer(void* ctx, u8 index);

void* GetRip(void* ctx);

#ifdef ARCH_X86_64
void IncrementRip(void* ctx, u64 length);
enum class ContextGpr { Rdi, R10 };
void SetGpr(void* ctx, ContextGpr reg, u64 value);
#endif

bool IsWriteError(void* ctx);

} // namespace Common

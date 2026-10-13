// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "common/arch.h"
#include "common/signal_context.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__FreeBSD__)
#include <machine/npx.h>
#include <sys/ucontext.h>
#else
#include <sys/ucontext.h>
#endif

namespace Common {

void* GetRip(void* ctx) {
#if defined(_WIN32)
    return (void*)((EXCEPTION_POINTERS*)ctx)->ContextRecord->Rip;
#elif defined(__APPLE__) && defined(ARCH_X86_64)
    return (void*)((ucontext_t*)ctx)->uc_mcontext->__ss.__rip;
#elif defined(__APPLE__) && defined(ARCH_ARM64)
    return (void*)((ucontext_t*)ctx)->uc_mcontext->__ss.__pc;
#elif defined(__FreeBSD__)
    return (void*)((ucontext_t*)ctx)->uc_mcontext.mc_rip;
#elif defined(ARCH_X86_64)
    return (void*)((ucontext_t*)ctx)->uc_mcontext.gregs[REG_RIP];
#else
#error "Unsupported architecture"
#endif
}

bool IsWriteError(void* ctx) {
#if defined(_WIN32)
    return ((EXCEPTION_POINTERS*)ctx)->ExceptionRecord->ExceptionInformation[0] == 1;
#elif defined(__APPLE__) && defined(ARCH_X86_64)
    return ((ucontext_t*)ctx)->uc_mcontext->__es.__err & 0x2;
#elif defined(__APPLE__) && defined(ARCH_ARM64)
    return ((ucontext_t*)ctx)->uc_mcontext->__es.__esr & 0x40;
#elif defined(__FreeBSD__) && defined(ARCH_X86_64)
    return ((ucontext_t*)ctx)->uc_mcontext.mc_err & 0x2;
#elif defined(ARCH_X86_64)
    return ((ucontext_t*)ctx)->uc_mcontext.gregs[REG_ERR] & 0x2;
#else
#error "Unsupported architecture"
#endif
}

#ifdef ARCH_X86_64
void IncrementRip(void* ctx, u64 length) {
#if defined(_WIN32)
    ((EXCEPTION_POINTERS*)ctx)->ContextRecord->Rip += length;
#elif defined(__APPLE__) && defined(ARCH_X86_64)
    ((ucontext_t*)ctx)->uc_mcontext->__ss.__rip += length;
#elif defined(__FreeBSD__) && defined(ARCH_X86_64)
    ((ucontext_t*)ctx)->uc_mcontext.mc_rip += length;
#elif defined(ARCH_X86_64)
    ((ucontext_t*)ctx)->uc_mcontext.gregs[REG_RIP] += length;
#else
#error "Unsupported architecture"
#endif
}

void SetGpr(void* ctx, ContextGpr reg, u64 value) {
#if defined(_WIN32)
    auto& state = *((EXCEPTION_POINTERS*)ctx)->ContextRecord;
    (reg == ContextGpr::Rdi ? state.Rdi : state.R10) = value;
#elif defined(__APPLE__) && defined(ARCH_X86_64)
    auto& state = ((ucontext_t*)ctx)->uc_mcontext->__ss;
    (reg == ContextGpr::Rdi ? state.__rdi : state.__r10) = value;
#elif defined(__FreeBSD__) && defined(ARCH_X86_64)
    auto& state = ((ucontext_t*)ctx)->uc_mcontext;
    (reg == ContextGpr::Rdi ? state.mc_rdi : state.mc_r10) = value;
#elif defined(ARCH_X86_64)
    auto& state = ((ucontext_t*)ctx)->uc_mcontext;
    state.gregs[reg == ContextGpr::Rdi ? REG_RDI : REG_R10] = value;
#else
#error "Unsupported architecture"
#endif
}
#endif

} // namespace Common

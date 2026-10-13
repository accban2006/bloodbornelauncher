#include <windows.h>
#include "common/signal_context.h"

int main() {
    CONTEXT state{};
    EXCEPTION_RECORD record{};
    EXCEPTION_POINTERS exception{&record, &state};
    state.Rip = 0x1234567890;
    state.Rdi = 0xffffffffffffffff;
    state.R10 = 0xffffffffffffffff;
    state.Rsi = 123;
    state.EFlags = 0x246;
    Common::SetGpr(&exception, Common::ContextGpr::Rdi, 0);
    Common::IncrementRip(&exception, 7);
    if (state.Rdi != 0 || state.R10 != 0xffffffffffffffff ||
        state.Rsi != 123 || state.EFlags != 0x246 || state.Rip != 0x1234567897) return 1;
    Common::SetGpr(&exception, Common::ContextGpr::R10, 0);
    if (state.R10 != 0 || Common::GetRip(&exception) != reinterpret_cast<void*>(state.Rip)) return 2;
    record.ExceptionInformation[0] = 1;
    if (!Common::IsWriteError(&exception)) return 3;
    record.ExceptionInformation[0] = 0;
    if (Common::IsWriteError(&exception)) return 4;
}

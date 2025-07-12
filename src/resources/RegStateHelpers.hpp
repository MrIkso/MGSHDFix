#pragma once
#include <safetyhook.hpp>

// ==========================================================
// ContextHelpers
//
// Helper functions for working with SafetyHook Context:
//    8-bit GPRs (AL/AH, BL/BH, ..., R8B-R15B)
//    16-bit GPRs (AX, BX, ..., R8W-R15W)
//    32-bit GPRs (EAX, EBX, ..., R8D-R15D)
//    RFLAGS bits (ZF, CF, SF, OF, PF, AF, etc.)
//
//    Example usage:
/*        void MyHook(safetyhook::Context& regs)
          {
            // RAX family
            reghelpers::SetAL(regs, 0x12);       // 8-bit low
            reghelpers::SetAH(regs, 0x34);       // 8-bit high
            reghelpers::SetAX(regs, 0x5678);     // 16-bit
            reghelpers::SetEAX(regs, 0x12345678);// 32-bit
            uint16_t ax = reghelpers::GetAX(regs);

            // Flags
            reghelpers::SetZF(regs, true);
            bool zeroFlag = reghelpers::GetZF(regs);
            reghelpers::SetCF(regs, false);
            bool carryFlag = reghelpers::GetCF(regs);
*/
//        }*/
// ==========================================================

namespace reghelpers
{

#if defined(_M_X64) || defined(__x86_64__)

    // ==========================
    // x64 (64-bit) helpers
    // ==========================

    // --- 8-bit GPR helpers (AL, AH, BL, BH, etc.) ---

    // RAX
    inline void SetAL(safetyhook::Context& regs, uint8_t val)
    {
        regs.rax = (regs.rax & ~0xFFULL) | val;
    }
    inline uint8_t GetAL(const safetyhook::Context& regs)
    {
        return regs.rax & 0xFF;
    }
    inline void SetAH(safetyhook::Context& regs, uint8_t val)
    {
        regs.rax = (regs.rax & ~(0xFFULL << 8)) | (uint64_t(val) << 8);
    }
    inline uint8_t GetAH(const safetyhook::Context& regs)
    {
        return (regs.rax >> 8) & 0xFF;
    }

    // RBX
    inline void SetBL(safetyhook::Context& regs, uint8_t val)
    {
        regs.rbx = (regs.rbx & ~0xFFULL) | val;
    }
    inline uint8_t GetBL(const safetyhook::Context& regs)
    {
        return regs.rbx & 0xFF;
    }
    inline void SetBH(safetyhook::Context& regs, uint8_t val)
    {
        regs.rbx = (regs.rbx & ~(0xFFULL << 8)) | (uint64_t(val) << 8);
    }
    inline uint8_t GetBH(const safetyhook::Context& regs)
    {
        return (regs.rbx >> 8) & 0xFF;
    }

    // RCX
    inline void SetCL(safetyhook::Context& regs, uint8_t val)
    {
        regs.rcx = (regs.rcx & ~0xFFULL) | val;
    }
    inline uint8_t GetCL(const safetyhook::Context& regs)
    {
        return regs.rcx & 0xFF;
    }
    inline void SetCH(safetyhook::Context& regs, uint8_t val)
    {
        regs.rcx = (regs.rcx & ~(0xFFULL << 8)) | (uint64_t(val) << 8);
    }
    inline uint8_t GetCH(const safetyhook::Context& regs)
    {
        return (regs.rcx >> 8) & 0xFF;
    }

    // RDX
    inline void SetDL(safetyhook::Context& regs, uint8_t val)
    {
        regs.rdx = (regs.rdx & ~0xFFULL) | val;
    }
    inline uint8_t GetDL(const safetyhook::Context& regs)
    {
        return regs.rdx & 0xFF;
    }
    inline void SetDH(safetyhook::Context& regs, uint8_t val)
    {
        regs.rdx = (regs.rdx & ~(0xFFULL << 8)) | (uint64_t(val) << 8);
    }
    inline uint8_t GetDH(const safetyhook::Context& regs)
    {
        return (regs.rdx >> 8) & 0xFF;
    }

    // RSI, RDI, RBP, RSP (only low 8 bits available)
#define DEFINE_8BIT_LOW_HELPERS(reg) \
inline void Set##reg##l(safetyhook::Context& regs, uint8_t val) { regs.reg = (regs.reg & ~0xFFULL) | val; } \
inline uint8_t Get##reg##l(const safetyhook::Context& regs) { return regs.reg & 0xFF; }

        DEFINE_8BIT_LOW_HELPERS(rsi)
        DEFINE_8BIT_LOW_HELPERS(rdi)
        DEFINE_8BIT_LOW_HELPERS(rbp)
        DEFINE_8BIT_LOW_HELPERS(rsp)

#undef DEFINE_8BIT_LOW_HELPERS

        // R8 - R15 low 8 bits
#define DEFINE_R8_15_8BIT_HELPERS(num) \
inline void Setr##num##b(safetyhook::Context& regs, uint8_t val) { regs.r##num = (regs.r##num & ~0xFFULL) | val; } \
inline uint8_t Getr##num##b(const safetyhook::Context& regs) { return regs.r##num & 0xFF; }

        DEFINE_R8_15_8BIT_HELPERS(8)
        DEFINE_R8_15_8BIT_HELPERS(9)
        DEFINE_R8_15_8BIT_HELPERS(10)
        DEFINE_R8_15_8BIT_HELPERS(11)
        DEFINE_R8_15_8BIT_HELPERS(12)
        DEFINE_R8_15_8BIT_HELPERS(13)
        DEFINE_R8_15_8BIT_HELPERS(14)
        DEFINE_R8_15_8BIT_HELPERS(15)

#undef DEFINE_R8_15_8BIT_HELPERS

        // --- 16-bit helpers ---

#define DEFINE_16BIT_HELPERS(reg) \
inline void Set##reg##w(safetyhook::Context& regs, uint16_t val) { regs.reg = (regs.reg & ~0xFFFFULL) | val; } \
inline uint16_t Get##reg##w(const safetyhook::Context& regs) { return regs.reg & 0xFFFF; }

        DEFINE_16BIT_HELPERS(rax)
        DEFINE_16BIT_HELPERS(rbx)
        DEFINE_16BIT_HELPERS(rcx)
        DEFINE_16BIT_HELPERS(rdx)
        DEFINE_16BIT_HELPERS(rsi)
        DEFINE_16BIT_HELPERS(rdi)
        DEFINE_16BIT_HELPERS(rbp)
        DEFINE_16BIT_HELPERS(rsp)
        DEFINE_16BIT_HELPERS(r8)
        DEFINE_16BIT_HELPERS(r9)
        DEFINE_16BIT_HELPERS(r10)
        DEFINE_16BIT_HELPERS(r11)
        DEFINE_16BIT_HELPERS(r12)
        DEFINE_16BIT_HELPERS(r13)
        DEFINE_16BIT_HELPERS(r14)
        DEFINE_16BIT_HELPERS(r15)

#undef DEFINE_16BIT_HELPERS

        // --- 32-bit helpers ---

#define DEFINE_32BIT_HELPERS(reg) \
inline void Set##reg##d(safetyhook::Context& regs, uint32_t val) { regs.reg = (regs.reg & ~0xFFFFFFFFULL) | val; } \
inline uint32_t Get##reg##d(const safetyhook::Context& regs) { return regs.reg & 0xFFFFFFFF; }

        DEFINE_32BIT_HELPERS(rax)
        DEFINE_32BIT_HELPERS(rbx)
        DEFINE_32BIT_HELPERS(rcx)
        DEFINE_32BIT_HELPERS(rdx)
        DEFINE_32BIT_HELPERS(rsi)
        DEFINE_32BIT_HELPERS(rdi)
        DEFINE_32BIT_HELPERS(rbp)
        DEFINE_32BIT_HELPERS(rsp)
        DEFINE_32BIT_HELPERS(r8)
        DEFINE_32BIT_HELPERS(r9)
        DEFINE_32BIT_HELPERS(r10)
        DEFINE_32BIT_HELPERS(r11)
        DEFINE_32BIT_HELPERS(r12)
        DEFINE_32BIT_HELPERS(r13)
        DEFINE_32BIT_HELPERS(r14)
        DEFINE_32BIT_HELPERS(r15)

#undef DEFINE_32BIT_HELPERS

        // --- RFLAGS flags ---

    constexpr uint64_t FLAG_CF = 1ULL << 0;
    constexpr uint64_t FLAG_PF = 1ULL << 2;
    constexpr uint64_t FLAG_AF = 1ULL << 4;
    constexpr uint64_t FLAG_ZF = 1ULL << 6;
    constexpr uint64_t FLAG_SF = 1ULL << 7;
    constexpr uint64_t FLAG_OF = 1ULL << 11;

    inline bool GetFlag(const safetyhook::Context& regs, uint64_t mask)
    {
        return (regs.rflags & mask) != 0;
    }
    inline void SetFlag(safetyhook::Context& regs, uint64_t mask, bool value)
    {
        if (value) regs.rflags |= mask; else regs.rflags &= ~mask;
    }

    inline bool GetZF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_ZF);
    }
    inline void SetZF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_ZF, val);
    }
    inline bool GetCF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_CF);
    }
    inline void SetCF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_CF, val);
    }
    inline bool GetSF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_SF);
    }
    inline void SetSF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_SF, val);
    }
    inline bool GetOF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_OF);
    }
    inline void SetOF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_OF, val);
    }
    inline bool GetPF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_PF);
    }
    inline void SetPF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_PF, val);
    }
    inline bool GetAF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_AF);
    }
    inline void SetAF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_AF, val);
    }

#elif defined(_M_IX86) || defined(__i386__)

    // ==========================
    // x86 (32-bit) helpers
    // ==========================

    // --- 8-bit GPR helpers (AL, AH, BL, BH, etc.) ---

    inline void SetAL(safetyhook::Context& regs, uint8_t val)
    {
        regs.eax = (regs.eax & ~0xFFU) | val;
    }
    inline uint8_t GetAL(const safetyhook::Context& regs)
    {
        return regs.eax & 0xFF;
    }
    inline void SetAH(safetyhook::Context& regs, uint8_t val)
    {
        regs.eax = (regs.eax & ~(0xFFU << 8)) | (uint32_t(val) << 8);
    }
    inline uint8_t GetAH(const safetyhook::Context& regs)
    {
        return (regs.eax >> 8) & 0xFF;
    }

    inline void SetBL(safetyhook::Context& regs, uint8_t val)
    {
        regs.ebx = (regs.ebx & ~0xFFU) | val;
    }
    inline uint8_t GetBL(const safetyhook::Context& regs)
    {
        return regs.ebx & 0xFF;
    }
    inline void SetBH(safetyhook::Context& regs, uint8_t val)
    {
        regs.ebx = (regs.ebx & ~(0xFFU << 8)) | (uint32_t(val) << 8);
    }
    inline uint8_t GetBH(const safetyhook::Context& regs)
    {
        return (regs.ebx >> 8) & 0xFF;
    }

    inline void SetCL(safetyhook::Context& regs, uint8_t val)
    {
        regs.ecx = (regs.ecx & ~0xFFU) | val;
    }
    inline uint8_t GetCL(const safetyhook::Context& regs)
    {
        return regs.ecx & 0xFF;
    }
    inline void SetCH(safetyhook::Context& regs, uint8_t val)
    {
        regs.ecx = (regs.ecx & ~(0xFFU << 8)) | (uint32_t(val) << 8);
    }
    inline uint8_t GetCH(const safetyhook::Context& regs)
    {
        return (regs.ecx >> 8) & 0xFF;
    }

    inline void SetDL(safetyhook::Context& regs, uint8_t val)
    {
        regs.edx = (regs.edx & ~0xFFU) | val;
    }
    inline uint8_t GetDL(const safetyhook::Context& regs)
    {
        return regs.edx & 0xFF;
    }
    inline void SetDH(safetyhook::Context& regs, uint8_t val)
    {
        regs.edx = (regs.edx & ~(0xFFU << 8)) | (uint32_t(val) << 8);
    }
    inline uint8_t GetDH(const safetyhook::Context& regs)
    {
        return (regs.edx >> 8) & 0xFF;
    }

    // --- 16-bit helpers ---

#define DEFINE_16BIT_HELPERS(reg) \
inline void Set##reg##W(safetyhook::Context& regs, uint16_t val) { regs.reg = (regs.reg & ~0xFFFFU) | val; } \
inline uint16_t Get##reg##W(const safetyhook::Context& regs) { return regs.reg & 0xFFFF; }

    DEFINE_16BIT_HELPERS(eax)
        DEFINE_16BIT_HELPERS(ebx)
        DEFINE_16BIT_HELPERS(ecx)
        DEFINE_16BIT_HELPERS(edx)
        DEFINE_16BIT_HELPERS(esi)
        DEFINE_16BIT_HELPERS(edi)
        DEFINE_16BIT_HELPERS(ebp)
        DEFINE_16BIT_HELPERS(esp)

#undef DEFINE_16BIT_HELPERS

        // --- 32-bit helpers ---

#define DEFINE_32BIT_HELPERS(reg) \
inline void Set##reg##D(safetyhook::Context& regs, uint32_t val) { regs.reg = val; } \
inline uint32_t Get##reg##D(const safetyhook::Context& regs) { return regs.reg; }

        DEFINE_32BIT_HELPERS(eax)
        DEFINE_32BIT_HELPERS(ebx)
        DEFINE_32BIT_HELPERS(ecx)
        DEFINE_32BIT_HELPERS(edx)
        DEFINE_32BIT_HELPERS(esi)
        DEFINE_32BIT_HELPERS(edi)
        DEFINE_32BIT_HELPERS(ebp)
        DEFINE_32BIT_HELPERS(esp)

#undef DEFINE_32BIT_HELPERS

        // --- EFLAGS flags ---

        constexpr uint32_t FLAG_CF = 1U << 0;
    constexpr uint32_t FLAG_PF = 1U << 2;
    constexpr uint32_t FLAG_AF = 1U << 4;
    constexpr uint32_t FLAG_ZF = 1U << 6;
    constexpr uint32_t FLAG_SF = 1U << 7;
    constexpr uint32_t FLAG_OF = 1U << 11;

    inline bool GetFlag(const safetyhook::Context& regs, uint32_t mask)
    {
        return (regs.eflags & mask) != 0;
    }
    inline void SetFlag(safetyhook::Context& regs, uint32_t mask, bool value)
    {
        if (value) regs.eflags |= mask; else regs.eflags &= ~mask;
    }

    inline bool GetZF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_ZF);
    }
    inline void SetZF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_ZF, val);
    }
    inline bool GetCF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_CF);
    }
    inline void SetCF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_CF, val);
    }
    inline bool GetSF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_SF);
    }
    inline void SetSF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_SF, val);
    }
    inline bool GetOF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_OF);
    }
    inline void SetOF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_OF, val);
    }
    inline bool GetPF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_PF);
    }
    inline void SetPF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_PF, val);
    }
    inline bool GetAF(const safetyhook::Context& regs)
    {
        return GetFlag(regs, FLAG_AF);
    }
    inline void SetAF(safetyhook::Context& regs, bool val)
    {
        SetFlag(regs, FLAG_AF, val);
    }

#else
#error Unsupported architecture: only x86 and x64 supported
#endif

} // namespace reghelpers

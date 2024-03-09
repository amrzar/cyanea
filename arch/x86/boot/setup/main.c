/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "string.h"
#include "setup.h"

#include "../boot.h"

#if (CONFIG_PHYSICAL_ALIGN & (CONFIG_PHYSICAL_ALIGN - 1))
# error 'CONFIG_PHYSICAL_ALIGN' should be power-of-two.
#endif

#if (CONFIG_PHYSICAL_ALIGN < (1UL << BOOT_ALIGN_SHIFT))
# error 'CONFIG_PHYSICAL_ALIGN' is less than minimum kernel alignment.
#endif

#if (CONFIG_PHYSICAL_START & (CONFIG_PHYSICAL_ALIGN - 1))
# error 'CONFIG_PHYSICAL_START' should meet alignment restriction.
#endif

struct boot_params boot_params __aligned(16);

void __noreturn cpu_hlt(void)
{
    while (1)
        asm volatile("hlt");
}

/* PASTE IT HERE. */

/* The OS notifies the BIOS about the expected operating mode using the
 * ''Detect Target Operating Mode'' (INT 0x15, function 0xEC00). Based on the target
 * operating mode, the BIOS can enable or disable mode specific performance and
 * functional optimizations that are not visible to system software.
 *
 * This does not change the operating mode; it only declares the target mode to the
 * BIOS. It should be executed only once by the BSP before the first transition
 * into long mode.
 *
 * The default operating mode assumed by the BIOS is ''Legacy Mode Target Only''.
 * If this is not the target operating mode, system software must change it before
 * transitioning to long mode for the first time. If the target operating mode is
 * ''Legacy Mode Target Only'', it does not need to be executed.
 *
 * The ''Detect Target Operating Mode'' inputs are stored in the 'AX' and 'BL' registers.
 * 'AX' has a value of 0xEC00, selecting the ''Detect Target Operating Mode'' function.
 * One of the following values in the 'BL' register selects the operating mode:
 *
 *   0x01 - ''Legacy Mode Target Only''.
 *         All enabled processors will operate in legacy mode only.
 *   0x02 - ''Long Mode Target Only''.
 *         All enabled processor will switch into long mode once.
 *   0x03 - ''Mixed Mode Target''.
 *         Processors may switch between legacy mode and long mode, or the preferred
 *         mode for system software is unknown. This value instructs the BIOS to
 *         use settings that are valid in all modes.
 *
 *   All other values are reserved.
 *
 * The ''Detect Target Operating Mode'' outputs are stored in the 'AH' register and 'CF'
 * (carry flag in the 'EFLAGS' register), and the values of other registers
 *  are not modified. The following output values are possible:
 *
 *   ''AH = 0x00 and CF = 0''.
 *         If this function is implemented and the value in BL is supported.
 *   ''AH = 0x00 and CF = 1''.
 *         If this function is implemented and the value in BL is reserved.
 *         This indicates an error; the target operating mode is set to
 *         ''Legacy Mode Target Only''.
 *   ''AH = 0x86 and CF = 1''.
 *         If this function is not supported.
 */

static void set_bios_mode(void)
{
#ifdef CONFIG_X86_64
    u32 eax = 0xec00;
    u32 ebx = 0x2;

    asm volatile(
        "int $0x15"
        : "+a" (eax), "+b" (ebx)
        : : "ecx", "edx", "esi", "edi"
    );
#endif /* CONFIG_X86_64 */
}

void __c_setup(void)
{
    memcpy(&boot_params.hdr, &hdr, sizeof(hdr));

    if (!boot_params.hdr.cmd_line_ptr)
        error("boot-loader does not support the 2.02+ protocol.\n");

    early_console_init();

    set_bios_mode();
    detect_memory();

    go_to_protected_mode();

    /* ''SHOULD NEVER GET HERE!'' */
    cpu_hlt();
}

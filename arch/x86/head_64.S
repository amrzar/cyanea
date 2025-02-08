# SPDX-License-Identifier: GPL-2.0-or-later

#
# ''64-bit entry to ukernel''.
#
# At this point the CPU runs in 64-bit mode CS.L = 1 CS.D = 0, and someone has
# loaded an identity mapped page table. These identity mapped page tables map
# all of the kernel pages and possibly all of memory.
#
# RSI holds the physical address of the bootparams provided by the boot-loader.
#

#include <asm-offsets.h>
#include <cyanea/pgtable.h>
#include <asm/segment.h>
#include <asm/cr_eflags_defs.h>
#include <asm/irq_vector.h>
#include <asm/msr.h>
#include <asm/page.h>
#include <asm/setup.h>

	.section ".head.text", "ax"

	.global	startup_64
	.balign	16, 0x90
startup_64:
	cli

	leaq	boot_gdt_descr(%rip), %rax
	addq	%rax, 2(%rax)			# Patch 'boot_gdt_descr'.
	lgdt	(%rax)

	xorl	%eax, %eax			# 'null-selector'
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss			# No GP(0) here!! See 'head_64.S'. 

	leaq	__end_utask_init(%rip), %rsp	# Setup the stack.

	pushq	$__BOOT_CS
	leaq	.Lon_boot_cs(%rip), %rax
	pushq	%rax

	lretq

.Lon_boot_cs:

	leaq	_text(%rip), %rdi
	pushq	%rsi				# Keep 'RSI' safe.
	call	__startup_64			# Perform page-table initialization.
	popq	%rsi

	leaq	early_top_pgt(%rip), %rax	# Here, runtime virtual address is equal to
	movq	%rax, %cr3			# physical address (i.e. ident-mapping).

	jmp	*1f(%rip)			# Branch to '.Ldefault_entry' at its virtual address.

	.type	startup_64, @function
	.size	startup_64, . - startup_64

	.section ".init.rodata", "a"
1:	.quad	.Ldefault_entry

#
# Application CPU entry point; entered from trampoline.S.
#

	.text

	.global	startup_64_smp
	.balign	16, 0x90
startup_64_smp:



.Ldefault_entry:

	movq	$(_CR4_PAE | _CR4_PGE), %rcx 
	movq	%rcx, %cr4

	#
	# Do a global TLB flush after the 'CR3' switch to make sure the TLB entries
	# from the original identity mappings are flushed.
	#

	movq	%cr4, %rcx
	movq	%rcx, %rax
	xorq	$_CR4_PGE, %rcx
	movq	%rcx, %cr4
	movq	%rax, %cr4

	movq	$1f, %rax			# Move to virtual address.
	jmp	 *%rax
1:

	movl	smpboot_cpu(%rip), %ecx

	movq	__per_cpu_offset(,%rcx,8), %rdx

	# Update 'GDT'. Use per-cpu 'gdt_page'.

	subq	$16, %rsp
	movw	$(GDT_SIZE - 1), (%rsp)
	leaq	gdt_page(%rdx), %rax
	movq	%rax, 2(%rsp)
	lgdt	(%rsp)				# See 'desc_defs.h'.
	addq	$16, %rsp

	xorl	%eax, %eax			# 'null-selector'
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss			# No GP(0) here!! See 'head_64.S'. 

	movq	percpu_hot+X86_top_of_stack(%rdx), %rsp		# Setup the stack.

	# 'RDX' already has the offset value that should be stored in 'GS'.

	movl	$MSR_GS_BASE, %ecx
	movl	%edx, %eax
	shrq	$32, %rdx
	wrmsr

	#
	# TODO. '_EFER_NXE' is supported!?
	#

	movl	$MSR_EFER, %ecx
	rdmsr
	btsl	$_EFER_NXE, %eax
	btsl	$_EFER_SCE, %eax
	wrmsr
	
	movl	$CR0_STATE, %eax
	movq	%rax, %cr0

	pushq	$0
	popfq					# Reset the flags.

	movq	%rsi, %rdi			# Pass 'boot_params' address.

	pushq	$1f
	pushq	$__KERNEL_CS
	movq	initial_code(%rip), %rax
	pushq	%rax

	lretq

1:	jmp	1b				# SHOULD NOT GET HERE!

	.type	startup_64_smp, @function
	.size	startup_64_smp, . - startup_64_smp

#

	.section ".init.data", "aw"

boot_gdt_descr:
	.word	boot_gdt_end - boot_gdt - 1	# GDT size.
	.quad	boot_gdt - boot_gdt_descr	# GDT base, filled above.

	.type	boot_gdt_descr, @object
	.size	boot_gdt_descr, . - boot_gdt_descr

	.balign	16				# Recommended by Intel.
boot_gdt:
	.quad	0x0000000000000000		# Reserved.
	.quad	GDT_ENTRY(0xC09A, 0, 0xFFFFF)	# '__BOOT32_CS', R/X, 4GiB, Base 0.
	.quad	GDT_ENTRY(0xA09A, 0, 0xFFFFF)	# '__BOOT_CS', 'long mode' enabled. 
	.quad	GDT_ENTRY(0xC092, 0, 0xFFFFF)	# '__BOOT_DS', R/W, 4GiB, Base 0.
boot_gdt_end:
	.type	boot_gdt_end, @object

	.type	boot_gdt, @object
	.size	boot_gdt, . - boot_gdt

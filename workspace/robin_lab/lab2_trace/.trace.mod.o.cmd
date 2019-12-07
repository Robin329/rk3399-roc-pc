cmd_/labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.o := aarch64-linux-gnu-gcc -Wp,-MD,/labs/linux-lab/workspace/robin_lab/lab2_trace/.trace.mod.o.d  -nostdinc -isystem /usr/local/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include -I/labs/linux-lab/linux-stable/arch/arm64/include -I./arch/arm64/include/generated -I/labs/linux-lab/linux-stable/include -I./include -I/labs/linux-lab/linux-stable/arch/arm64/include/uapi -I./arch/arm64/include/generated/uapi -I/labs/linux-lab/linux-stable/include/uapi -I./include/generated/uapi -include /labs/linux-lab/linux-stable/include/linux/kconfig.h -include /labs/linux-lab/linux-stable/include/linux/compiler_types.h -D__KERNEL__ -mlittle-endian -DKASAN_SHADOW_SCALE_SHIFT=3 -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Wno-format-security -std=gnu89 -mgeneral-regs-only -DCONFIG_BROKEN_GAS_INST=1 -fno-asynchronous-unwind-tables -Wno-psabi -mabi=lp64 -DKASAN_SHADOW_SCALE_SHIFT=3 -fno-delete-null-pointer-checks -O2 --param=allow-store-data-races=0 -Wframe-larger-than=2048 -fstack-protector-strong -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -g -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -fno-strict-overflow -fno-merge-all-constants -fmerge-constants -fno-stack-check -fconserve-stack -Werror=date-time  -DKBUILD_BASENAME='"trace.mod"' -DKBUILD_MODNAME='"trace"' -DMODULE  -c -o /labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.o /labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.c

source_/labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.o := /labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.c

deps_/labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.o := \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/retpoline.h) \
  /labs/linux-lab/linux-stable/include/linux/kconfig.h \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/booger.h) \
    $(wildcard include/config/foo.h) \
  /labs/linux-lab/linux-stable/include/linux/compiler_types.h \
    $(wildcard include/config/have/arch/compiler/h.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/optimize/inlining.h) \
  /labs/linux-lab/linux-stable/include/linux/compiler_attributes.h \
  /labs/linux-lab/linux-stable/include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  /labs/linux-lab/linux-stable/include/linux/build-salt.h \
    $(wildcard include/config/build/salt.h) \
  /labs/linux-lab/linux-stable/include/linux/elfnote.h \
  /labs/linux-lab/linux-stable/include/linux/elf.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/elf.h \
    $(wildcard include/config/arm64/force/52bit.h) \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/compat/vdso.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/hwcap.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/hwcap.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/cpufeature.h \
    $(wildcard include/config/arm64/sw/ttbr0/pan.h) \
    $(wildcard include/config/arm64/sve.h) \
    $(wildcard include/config/arm64/cnp.h) \
    $(wildcard include/config/arm64/ptr/auth.h) \
    $(wildcard include/config/arm64/pseudo/nmi.h) \
    $(wildcard include/config/arm64/debug/priority/masking.h) \
    $(wildcard include/config/arm64/ssbd.h) \
    $(wildcard include/config/arm64/pa/bits.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/cpucaps.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/cputype.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/sysreg.h \
    $(wildcard include/config/broken/gas/inst.h) \
    $(wildcard include/config/arm64/pa/bits/52.h) \
    $(wildcard include/config/arm64/4k/pages.h) \
    $(wildcard include/config/arm64/16k/pages.h) \
    $(wildcard include/config/arm64/64k/pages.h) \
  /labs/linux-lab/linux-stable/include/linux/bits.h \
  /labs/linux-lab/linux-stable/include/linux/const.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/const.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/bitsperlong.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64bit.h) \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/bitsperlong.h \
  /labs/linux-lab/linux-stable/include/linux/stringify.h \
  /labs/linux-lab/linux-stable/include/linux/build_bug.h \
  /labs/linux-lab/linux-stable/include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/stack/validation.h) \
    $(wildcard include/config/kasan.h) \
  /labs/linux-lab/linux-stable/include/linux/compiler_types.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/types.h \
  arch/arm64/include/generated/uapi/asm/types.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/types.h \
  /labs/linux-lab/linux-stable/include/asm-generic/int-ll64.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/int-ll64.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/posix_types.h \
  /labs/linux-lab/linux-stable/include/linux/stddef.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/stddef.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/posix_types.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/posix_types.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/barrier.h \
  /labs/linux-lab/linux-stable/include/linux/kasan-checks.h \
  /labs/linux-lab/linux-stable/include/linux/types.h \
    $(wildcard include/config/have/uid16.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
  /labs/linux-lab/linux-stable/include/asm-generic/barrier.h \
    $(wildcard include/config/smp.h) \
  /labs/linux-lab/linux-stable/include/linux/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/bug/on/data/corruption.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/bug.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/asm-bug.h \
    $(wildcard include/config/debug/bugverbose.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/brk-imm.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /labs/linux-lab/linux-stable/include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/arch/has/refcount.h) \
    $(wildcard include/config/panic/timeout.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /usr/local/lib/gcc/aarch64-linux-gnu/4.9.3/include/stdarg.h \
  /labs/linux-lab/linux-stable/include/linux/limits.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/limits.h \
  /labs/linux-lab/linux-stable/include/linux/linkage.h \
  /labs/linux-lab/linux-stable/include/linux/export.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/module/rel/crcs.h) \
    $(wildcard include/config/have/arch/prel32/relocations.h) \
    $(wildcard include/config/trim/unused/ksyms.h) \
    $(wildcard include/config/unused/symbols.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/linkage.h \
  /labs/linux-lab/linux-stable/include/linux/bitops.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/bitops.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/builtin-__ffs.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/builtin-ffs.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/builtin-__fls.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/builtin-fls.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/ffz.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/fls64.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/sched.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/hweight.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/arch_hweight.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/const_hweight.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/atomic.h \
  /labs/linux-lab/linux-stable/include/linux/atomic.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/atomic.h \
    $(wildcard include/config/arm64/lse/atomics.h) \
    $(wildcard include/config/as/lse.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/lse.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/atomic_ll_sc.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/cmpxchg.h \
  /labs/linux-lab/linux-stable/include/asm-generic/atomic-instrumented.h \
  /labs/linux-lab/linux-stable/include/linux/atomic-fallback.h \
    $(wildcard include/config/generic/atomic64.h) \
  /labs/linux-lab/linux-stable/include/asm-generic/atomic-long.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/lock.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/non-atomic.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/le.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/byteorder.h \
  /labs/linux-lab/linux-stable/include/linux/byteorder/little_endian.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/byteorder/little_endian.h \
  /labs/linux-lab/linux-stable/include/linux/swab.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/swab.h \
  arch/arm64/include/generated/uapi/asm/swab.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/swab.h \
  /labs/linux-lab/linux-stable/include/linux/byteorder/generic.h \
  /labs/linux-lab/linux-stable/include/asm-generic/bitops/ext2-atomic-setbit.h \
  /labs/linux-lab/linux-stable/include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /labs/linux-lab/linux-stable/include/linux/typecheck.h \
  /labs/linux-lab/linux-stable/include/linux/printk.h \
    $(wildcard include/config/message/loglevel/default.h) \
    $(wildcard include/config/console/loglevel/default.h) \
    $(wildcard include/config/console/loglevel/quiet.h) \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk/nmi.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  /labs/linux-lab/linux-stable/include/linux/init.h \
    $(wildcard include/config/strict/kernel/rwx.h) \
    $(wildcard include/config/strict/module/rwx.h) \
  /labs/linux-lab/linux-stable/include/linux/kern_levels.h \
  /labs/linux-lab/linux-stable/include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/kernel.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/sysinfo.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/cache.h \
    $(wildcard include/config/kasan/sw/tags.h) \
  arch/arm64/include/generated/asm/div64.h \
  /labs/linux-lab/linux-stable/include/asm-generic/div64.h \
  /labs/linux-lab/linux-stable/include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
    $(wildcard include/config/have/arch/jump/label/relative.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/ptrace.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/ptrace.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/sve_context.h \
  arch/arm64/include/generated/asm/user.h \
  /labs/linux-lab/linux-stable/include/asm-generic/user.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/processor.h \
    $(wildcard include/config/kuser/helpers.h) \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/gcc/plugin/stackleak.h) \
  /labs/linux-lab/linux-stable/include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
    $(wildcard include/config/fortify/source.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/string.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/string.h \
    $(wildcard include/config/arch/has/uaccess/flushcache.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/alternative.h \
    $(wildcard include/config/arm64/uao.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/insn.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/hw_breakpoint.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/virt.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/sections.h \
  /labs/linux-lab/linux-stable/include/asm-generic/sections.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/pgtable-hwdef.h \
    $(wildcard include/config/pgtable/levels.h) \
    $(wildcard include/config/arm64/user/va/bits/52.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/memory.h \
    $(wildcard include/config/arm64/va/bits.h) \
    $(wildcard include/config/vmap/stack.h) \
    $(wildcard include/config/debug/align/rodata.h) \
    $(wildcard include/config/debug/virtual.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/efi.h) \
    $(wildcard include/config/arm/gic/v3/its.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/page-def.h \
    $(wildcard include/config/arm64/page/shift.h) \
    $(wildcard include/config/arm64/cont/shift.h) \
  /labs/linux-lab/linux-stable/include/linux/sizes.h \
  /labs/linux-lab/linux-stable/include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/vm/pgflags.h) \
  /labs/linux-lab/linux-stable/include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem.h) \
  /labs/linux-lab/linux-stable/include/linux/pfn.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/pointer_auth.h \
  /labs/linux-lab/linux-stable/include/linux/random.h \
    $(wildcard include/config/arch/random.h) \
  /labs/linux-lab/linux-stable/include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  /labs/linux-lab/linux-stable/include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
    $(wildcard include/config/page/poisoning/zero.h) \
  /labs/linux-lab/linux-stable/include/linux/once.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/random.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/ioctl.h \
  arch/arm64/include/generated/uapi/asm/ioctl.h \
  /labs/linux-lab/linux-stable/include/asm-generic/ioctl.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/ioctl.h \
  /labs/linux-lab/linux-stable/include/linux/irqnr.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/irqnr.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/fpsimd.h \
  arch/arm64/include/generated/uapi/asm/errno.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/errno.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/errno-base.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/sigcontext.h \
  /labs/linux-lab/linux-stable/include/linux/bitmap.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/elf.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/elf-em.h \
  /labs/linux-lab/linux-stable/include/linux/module.h \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/modules/tree/lookup.h) \
    $(wildcard include/config/livepatch.h) \
    $(wildcard include/config/module/sig.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tree/srcu.h) \
    $(wildcard include/config/bpf/events.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/function/error/injection.h) \
  /labs/linux-lab/linux-stable/include/linux/stat.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/stat.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/stat.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/stat.h \
  /labs/linux-lab/linux-stable/include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  /labs/linux-lab/linux-stable/include/linux/seqlock.h \
    $(wildcard include/config/debug/lock/alloc.h) \
  /labs/linux-lab/linux-stable/include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/preempt.h) \
  /labs/linux-lab/linux-stable/include/linux/preempt.h \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/trace/preempt/toggle.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/preempt.h \
  /labs/linux-lab/linux-stable/include/linux/thread_info.h \
    $(wildcard include/config/thread/info/in/task.h) \
    $(wildcard include/config/have/arch/within/stack/frames.h) \
    $(wildcard include/config/hardened/usercopy.h) \
  /labs/linux-lab/linux-stable/include/linux/restart_block.h \
  /labs/linux-lab/linux-stable/include/linux/time64.h \
  /labs/linux-lab/linux-stable/include/linux/math64.h \
    $(wildcard include/config/arch/supports/int128.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/time.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/time_types.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/current.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/thread_info.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/stack_pointer.h \
  /labs/linux-lab/linux-stable/include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/irqflags.h \
  /labs/linux-lab/linux-stable/include/linux/bottom_half.h \
  arch/arm64/include/generated/asm/mmiowb.h \
  /labs/linux-lab/linux-stable/include/asm-generic/mmiowb.h \
    $(wildcard include/config/mmiowb.h) \
  /labs/linux-lab/linux-stable/include/linux/spinlock_types.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/spinlock_types.h \
  /labs/linux-lab/linux-stable/include/asm-generic/qspinlock_types.h \
    $(wildcard include/config/paravirt.h) \
    $(wildcard include/config/nr/cpus.h) \
  /labs/linux-lab/linux-stable/include/asm-generic/qrwlock_types.h \
  /labs/linux-lab/linux-stable/include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
  /labs/linux-lab/linux-stable/include/linux/rwlock_types.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/spinlock.h \
  arch/arm64/include/generated/asm/qrwlock.h \
  /labs/linux-lab/linux-stable/include/asm-generic/qrwlock.h \
  arch/arm64/include/generated/asm/qspinlock.h \
  /labs/linux-lab/linux-stable/include/asm-generic/qspinlock.h \
  /labs/linux-lab/linux-stable/include/linux/rwlock.h \
  /labs/linux-lab/linux-stable/include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/uninline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
    $(wildcard include/config/generic/lockbreak.h) \
  /labs/linux-lab/linux-stable/include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  /labs/linux-lab/linux-stable/include/linux/time32.h \
  /labs/linux-lab/linux-stable/include/linux/timex.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/timex.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/param.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/param.h \
  /labs/linux-lab/linux-stable/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/param.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/timex.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/arch_timer.h \
    $(wildcard include/config/arm/arch/timer/ool/workaround.h) \
  /labs/linux-lab/linux-stable/include/linux/smp.h \
    $(wildcard include/config/up/late/init.h) \
  /labs/linux-lab/linux-stable/include/linux/errno.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/errno.h \
  /labs/linux-lab/linux-stable/include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  /labs/linux-lab/linux-stable/include/linux/threads.h \
    $(wildcard include/config/base/small.h) \
  /labs/linux-lab/linux-stable/include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/smp.h \
    $(wildcard include/config/arm64/acpi/parking/protocol.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/percpu.h \
  /labs/linux-lab/linux-stable/include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /labs/linux-lab/linux-stable/include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
    $(wildcard include/config/virtualization.h) \
    $(wildcard include/config/amd/mem/encrypt.h) \
  /labs/linux-lab/linux-stable/include/clocksource/arm_arch_timer.h \
    $(wildcard include/config/arm/arch/timer.h) \
  /labs/linux-lab/linux-stable/include/linux/timecounter.h \
  /labs/linux-lab/linux-stable/include/asm-generic/timex.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/compat.h \
  /labs/linux-lab/linux-stable/include/linux/sched.h \
    $(wildcard include/config/virt/cpu/accounting/native.h) \
    $(wildcard include/config/sched/info.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/uclamp/task.h) \
    $(wildcard include/config/uclamp/buckets/count.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/tasks/rcu.h) \
    $(wildcard include/config/psi.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/stackprotector.h) \
    $(wildcard include/config/arch/has/scaled/cputime.h) \
    $(wildcard include/config/virt/cpu/accounting/gen.h) \
    $(wildcard include/config/no/hz/full.h) \
    $(wildcard include/config/posix/timers.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/ubsan.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/x86/cpu/resctrl.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/rseq.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/kcov.h) \
    $(wildcard include/config/uprobes.h) \
    $(wildcard include/config/bcache.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/arch/task/struct/on/stack.h) \
    $(wildcard include/config/debug/rseq.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/sched.h \
  /labs/linux-lab/linux-stable/include/linux/pid.h \
  /labs/linux-lab/linux-stable/include/linux/rculist.h \
  /labs/linux-lab/linux-stable/include/linux/rcupdate.h \
    $(wildcard include/config/rcu/stall/common.h) \
    $(wildcard include/config/rcu/nocb/cpu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/prove/rcu.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/arch/weak/release/acquire.h) \
  /labs/linux-lab/linux-stable/include/linux/rcutree.h \
  /labs/linux-lab/linux-stable/include/linux/wait.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/wait.h \
  /labs/linux-lab/linux-stable/include/linux/refcount.h \
    $(wildcard include/config/refcount/full.h) \
  /labs/linux-lab/linux-stable/include/linux/sem.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/sem.h \
  /labs/linux-lab/linux-stable/include/linux/ipc.h \
  /labs/linux-lab/linux-stable/include/linux/uidgid.h \
    $(wildcard include/config/multiuser.h) \
    $(wildcard include/config/user/ns.h) \
  /labs/linux-lab/linux-stable/include/linux/highuid.h \
  /labs/linux-lab/linux-stable/include/linux/rhashtable-types.h \
  /labs/linux-lab/linux-stable/include/linux/mutex.h \
    $(wildcard include/config/mutex/spin/on/owner.h) \
  /labs/linux-lab/linux-stable/include/linux/osq_lock.h \
  /labs/linux-lab/linux-stable/include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  /labs/linux-lab/linux-stable/include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
    $(wildcard include/config/wq/watchdog.h) \
  /labs/linux-lab/linux-stable/include/linux/timer.h \
    $(wildcard include/config/debug/objects/timers.h) \
    $(wildcard include/config/no/hz/common.h) \
  /labs/linux-lab/linux-stable/include/linux/ktime.h \
  /labs/linux-lab/linux-stable/include/linux/jiffies.h \
  include/generated/timeconst.h \
  /labs/linux-lab/linux-stable/include/linux/timekeeping.h \
  /labs/linux-lab/linux-stable/include/linux/timekeeping32.h \
  /labs/linux-lab/linux-stable/include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/ipc.h \
  arch/arm64/include/generated/uapi/asm/ipcbuf.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/ipcbuf.h \
  arch/arm64/include/generated/uapi/asm/sembuf.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/sembuf.h \
  /labs/linux-lab/linux-stable/include/linux/shm.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/page.h \
  /labs/linux-lab/linux-stable/include/linux/personality.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/personality.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/pgtable-types.h \
  /labs/linux-lab/linux-stable/include/asm-generic/pgtable-nopud.h \
  /labs/linux-lab/linux-stable/include/asm-generic/pgtable-nop4d-hack.h \
  /labs/linux-lab/linux-stable/include/asm-generic/5level-fixup.h \
  /labs/linux-lab/linux-stable/include/asm-generic/getorder.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/shm.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/hugetlb_encode.h \
  arch/arm64/include/generated/uapi/asm/shmbuf.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/shmbuf.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/shmparam.h \
  /labs/linux-lab/linux-stable/include/asm-generic/shmparam.h \
  /labs/linux-lab/linux-stable/include/linux/kcov.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/kcov.h \
  /labs/linux-lab/linux-stable/include/linux/plist.h \
    $(wildcard include/config/debug/plist.h) \
  /labs/linux-lab/linux-stable/include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/time/low/res.h) \
    $(wildcard include/config/timerfd.h) \
  /labs/linux-lab/linux-stable/include/linux/hrtimer_defs.h \
  /labs/linux-lab/linux-stable/include/linux/rbtree.h \
  /labs/linux-lab/linux-stable/include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
  /labs/linux-lab/linux-stable/include/linux/timerqueue.h \
  /labs/linux-lab/linux-stable/include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/have/arch/seccomp/filter.h) \
    $(wildcard include/config/seccomp/filter.h) \
    $(wildcard include/config/checkpoint/restore.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/seccomp.h \
  /labs/linux-lab/linux-stable/include/linux/nodemask.h \
    $(wildcard include/config/highmem.h) \
  /labs/linux-lab/linux-stable/include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  /labs/linux-lab/linux-stable/include/linux/resource.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/resource.h \
  arch/arm64/include/generated/uapi/asm/resource.h \
  /labs/linux-lab/linux-stable/include/asm-generic/resource.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/resource.h \
  /labs/linux-lab/linux-stable/include/linux/latencytop.h \
  /labs/linux-lab/linux-stable/include/linux/sched/prio.h \
  /labs/linux-lab/linux-stable/include/linux/signal_types.h \
    $(wildcard include/config/old/sigaction.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/signal.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/signal.h \
  /labs/linux-lab/linux-stable/include/asm-generic/signal.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/signal.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/signal-defs.h \
  arch/arm64/include/generated/uapi/asm/siginfo.h \
  /labs/linux-lab/linux-stable/include/uapi/asm-generic/siginfo.h \
  /labs/linux-lab/linux-stable/include/linux/mm_types_task.h \
    $(wildcard include/config/arch/want/batched/unmap/tlb/flush.h) \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/arch/enable/split/pmd/ptlock.h) \
  /labs/linux-lab/linux-stable/include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/rseq.h \
  /labs/linux-lab/linux-stable/include/linux/sched/task_stack.h \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/debug/stack/usage.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/magic.h \
  /labs/linux-lab/linux-stable/include/asm-generic/compat.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/stat.h \
  /labs/linux-lab/linux-stable/include/linux/kmod.h \
  /labs/linux-lab/linux-stable/include/linux/umh.h \
  /labs/linux-lab/linux-stable/include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/zone/device.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/contig/alloc.h) \
    $(wildcard include/config/cma.h) \
  /labs/linux-lab/linux-stable/include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/shuffle/page/allocator.h) \
    $(wildcard include/config/zsmalloc.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/page/extension.h) \
    $(wildcard include/config/deferred/struct/page/init.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  /labs/linux-lab/linux-stable/include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  /labs/linux-lab/linux-stable/include/linux/page-flags-layout.h \
  include/generated/bounds.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/sparsemem.h \
  /labs/linux-lab/linux-stable/include/linux/mm_types.h \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/userfaultfd.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/have/arch/compat/mmap/bases.h) \
    $(wildcard include/config/membarrier.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/hmm/mirror.h) \
  /labs/linux-lab/linux-stable/include/linux/auxvec.h \
  /labs/linux-lab/linux-stable/include/uapi/linux/auxvec.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/uapi/asm/auxvec.h \
  /labs/linux-lab/linux-stable/include/linux/rwsem.h \
    $(wildcard include/config/rwsem/spin/on/owner.h) \
  /labs/linux-lab/linux-stable/include/linux/err.h \
  /labs/linux-lab/linux-stable/include/linux/completion.h \
  /labs/linux-lab/linux-stable/include/linux/uprobes.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/uprobes.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/debug-monitors.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/esr.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/probes.h \
    $(wildcard include/config/kprobes.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/mmu.h \
    $(wildcard include/config/unmap/kernel/at/el0.h) \
    $(wildcard include/config/randomize/base.h) \
    $(wildcard include/config/cavium/erratum/27456.h) \
    $(wildcard include/config/harden/branch/predictor.h) \
    $(wildcard include/config/harden/el2/vectors.h) \
  /labs/linux-lab/linux-stable/include/linux/page-flags.h \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/idle/page/tracking.h) \
    $(wildcard include/config/thp/swap.h) \
    $(wildcard include/config/ksm.h) \
  /labs/linux-lab/linux-stable/include/linux/memory_hotplug.h \
    $(wildcard include/config/arch/has/add/pages.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  /labs/linux-lab/linux-stable/include/linux/notifier.h \
  /labs/linux-lab/linux-stable/include/linux/srcu.h \
    $(wildcard include/config/tiny/srcu.h) \
    $(wildcard include/config/srcu.h) \
  /labs/linux-lab/linux-stable/include/linux/rcu_segcblist.h \
  /labs/linux-lab/linux-stable/include/linux/srcutree.h \
  /labs/linux-lab/linux-stable/include/linux/rcu_node_tree.h \
    $(wildcard include/config/rcu/fanout.h) \
    $(wildcard include/config/rcu/fanout/leaf.h) \
  /labs/linux-lab/linux-stable/include/linux/topology.h \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
    $(wildcard include/config/sched/smt.h) \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/topology.h \
  /labs/linux-lab/linux-stable/include/linux/arch_topology.h \
  /labs/linux-lab/linux-stable/include/asm-generic/topology.h \
  /labs/linux-lab/linux-stable/include/linux/sysctl.h \
    $(wildcard include/config/sysctl.h) \
  /labs/linux-lab/linux-stable/include/uapi/linux/sysctl.h \
  /labs/linux-lab/linux-stable/include/linux/kobject.h \
    $(wildcard include/config/uevent/helper.h) \
    $(wildcard include/config/debug/kobject/release.h) \
  /labs/linux-lab/linux-stable/include/linux/sysfs.h \
  /labs/linux-lab/linux-stable/include/linux/kernfs.h \
    $(wildcard include/config/kernfs.h) \
  /labs/linux-lab/linux-stable/include/linux/idr.h \
  /labs/linux-lab/linux-stable/include/linux/radix-tree.h \
  /labs/linux-lab/linux-stable/include/linux/xarray.h \
    $(wildcard include/config/xarray/multi.h) \
  /labs/linux-lab/linux-stable/include/linux/kconfig.h \
  /labs/linux-lab/linux-stable/include/linux/kobject_ns.h \
  /labs/linux-lab/linux-stable/include/linux/kref.h \
  /labs/linux-lab/linux-stable/include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  /labs/linux-lab/linux-stable/include/linux/rbtree_latch.h \
  /labs/linux-lab/linux-stable/include/linux/error-injection.h \
  /labs/linux-lab/linux-stable/include/asm-generic/error-injection.h \
  /labs/linux-lab/linux-stable/include/linux/tracepoint-defs.h \
  /labs/linux-lab/linux-stable/include/linux/static_key.h \
  /labs/linux-lab/linux-stable/arch/arm64/include/asm/module.h \
    $(wildcard include/config/arm64/module/plts.h) \
    $(wildcard include/config/dynamic/ftrace.h) \
    $(wildcard include/config/arm64/erratum/843419.h) \
  /labs/linux-lab/linux-stable/include/asm-generic/module.h \
    $(wildcard include/config/have/mod/arch/specific.h) \
    $(wildcard include/config/modules/use/elf/rel.h) \
    $(wildcard include/config/modules/use/elf/rela.h) \
  /labs/linux-lab/linux-stable/include/linux/vermagic.h \
  include/generated/utsrelease.h \

/labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.o: $(deps_/labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.o)

$(deps_/labs/linux-lab/workspace/robin_lab/lab2_trace/trace.mod.o):

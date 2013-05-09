cmd_/mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.o := powerpc-none-linux-gnuspe-gcc -m32 -Wp,-MD,/mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/.mpcdrveth_ipv4.o.d  -nostdinc -isystem /opt/freescale/usr/local/gcc-4.3.74-eglibc-2.8.74-dp-2/powerpc-none-linux-gnuspe/lib/gcc/powerpc-none-linux-gnuspe/4.3.2/include -Iinclude  -I/home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include -include include/linux/autoconf.h -D__KERNEL__ -Iarch/powerpc -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -msoft-float -pipe -Iarch/powerpc -ffixed-r2 -mmultiple -mno-altivec -mno-spe -mspe=no -funit-at-a-time -mno-string -Wa,-me500 -fno-stack-protector -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow  -DMODULE -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(mpcdrveth_ipv4)"  -D"KBUILD_MODNAME=KBUILD_STR(mpcdrv)"  -c -o /mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.o /mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.c

deps_/mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.o := \
  /mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.c \
    $(wildcard include/config/xfrm.h) \
  include/linux/sys.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/types.h \
    $(wildcard include/config/phys/64bit.h) \
  include/asm-generic/int-ll64.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/posix_types.h \
  include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/sysfs.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/prefetch.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/processor.h \
    $(wildcard include/config/vsx.h) \
    $(wildcard include/config/ppc32.h) \
    $(wildcard include/config/ppc/prep.h) \
    $(wildcard include/config/task/size.h) \
    $(wildcard include/config/kernel/start.h) \
    $(wildcard include/config/ppc64.h) \
    $(wildcard include/config/4xx.h) \
    $(wildcard include/config/booke.h) \
    $(wildcard include/config/altivec.h) \
    $(wildcard include/config/spe.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/reg.h \
    $(wildcard include/config/40x.h) \
    $(wildcard include/config/fsl/emb/perfmon.h) \
    $(wildcard include/config/8xx.h) \
    $(wildcard include/config/ppc/book3s/64.h) \
    $(wildcard include/config/ppc/book3s/32.h) \
    $(wildcard include/config/ppc/book3e/64.h) \
    $(wildcard include/config/e200.h) \
    $(wildcard include/config/ppc/cell.h) \
  include/linux/stringify.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/cputable.h \
    $(wildcard include/config/mpc10x/bridge.h) \
    $(wildcard include/config/ppc/83xx.h) \
    $(wildcard include/config/8260.h) \
    $(wildcard include/config/ppc/mpc52xx.h) \
    $(wildcard include/config/bdi/switch.h) \
    $(wildcard include/config/power3.h) \
    $(wildcard include/config/power4.h) \
    $(wildcard include/config/44x.h) \
    $(wildcard include/config/e500.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/asm-compat.h \
    $(wildcard include/config/power4/only.h) \
    $(wildcard include/config/ibm405/err77.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/feature-fixups.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/reg_booke.h \
    $(wildcard include/config/debug/cw.h) \
    $(wildcard include/config/403gcx.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/reg_fsl_emb.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/ptrace.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/cache.h \
    $(wildcard include/config/ppc/e500mc.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/system.h \
    $(wildcard include/config/debugger.h) \
    $(wildcard include/config/kexec.h) \
    $(wildcard include/config/6xx.h) \
    $(wildcard include/config/booke/wdt.h) \
    $(wildcard include/config/virt/cpu/accounting.h) \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/numa.h) \
  /opt/freescale/usr/local/gcc-4.3.74-eglibc-2.8.74-dp-2/powerpc-none-linux-gnuspe/lib/gcc/powerpc-none-linux-gnuspe/4.3.2/include/stdarg.h \
  include/linux/linkage.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/linkage.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/bitops.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/synch.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/find.h \
  include/asm-generic/bitops/sched.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/typecheck.h \
  include/linux/ratelimit.h \
  include/linux/param.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_debug.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/byteorder.h \
  include/linux/byteorder/big_endian.h \
  include/linux/swab.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/x86.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/irqflags.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/hw_irq.h \
    $(wildcard include/config/ppc/book3e.h) \
    $(wildcard include/config/perf/events.h) \
  include/linux/errno.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/asm-generic/cmpxchg-local.h \
  include/linux/stat.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/stat.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/seqlock.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/thread_info.h \
    $(wildcard include/config/ppc/256k/pages.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/page.h \
    $(wildcard include/config/ppc/64k/pages.h) \
    $(wildcard include/config/ppc/16k/pages.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/physical/start.h) \
    $(wildcard include/config/relocatable.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/ppc/std/mmu/64.h) \
    $(wildcard include/config/ppc/smlpar.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/kdump.h \
    $(wildcard include/config/crash/dump.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/page_32.h \
    $(wildcard include/config/physical/align.h) \
    $(wildcard include/config/not/coherent/cache.h) \
    $(wildcard include/config/pte/64bit.h) \
  include/asm-generic/getorder.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/spinlock_types_up.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/generic/hardirqs.h) \
  include/linux/spinlock_up.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/atomic.h \
  include/asm-generic/atomic64.h \
  include/asm-generic/atomic-long.h \
  include/linux/spinlock_api_up.h \
  include/linux/math64.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/div64.h \
  include/asm-generic/div64.h \
  include/linux/kmod.h \
  include/linux/gfp.h \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/debug/vm.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/wait.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/current.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/init.h \
    $(wildcard include/config/hotplug.h) \
  include/linux/nodemask.h \
  include/linux/bitmap.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/string.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/bounds.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/memory/hotremove.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/rwsem.h \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/topology.h \
    $(wildcard include/config/pci.h) \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/elf.h \
  include/linux/elf-em.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/elf.h \
    $(wildcard include/config/spu/base.h) \
  include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/no/hz.h) \
    $(wildcard include/config/detect/softlockup.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/user/sched.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/debug/stack/usage.h) \
    $(wildcard include/config/group/sched.h) \
    $(wildcard include/config/mm/owner.h) \
  include/linux/capability.h \
    $(wildcard include/config/security/file/capabilities.h) \
  include/linux/timex.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/timex.h \
  include/linux/jiffies.h \
  include/linux/rbtree.h \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/mmu/notifier.h) \
  include/linux/auxvec.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/auxvec.h \
  include/linux/prio_tree.h \
  include/linux/completion.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/mmu.h \
    $(wildcard include/config/ppc/std/mmu/32.h) \
    $(wildcard include/config/ppc/book3e/mmu.h) \
    $(wildcard include/config/ppc/8xx.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/mmu-book3e.h \
    $(wildcard include/config/ppc/4k/pages.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/sem.h \
  include/linux/ipc.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/ipcbuf.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/sembuf.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/tree/rcu.h) \
  include/linux/rcutree.h \
  include/linux/signal.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/signal.h \
  include/asm-generic/signal-defs.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/path.h \
  include/linux/pid.h \
  include/linux/percpu.h \
    $(wildcard include/config/have/legacy/per/cpu/area.h) \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/debug/kmemleak.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
  include/linux/slub_def.h \
    $(wildcard include/config/slub/stats.h) \
    $(wildcard include/config/slub/debug.h) \
    $(wildcard include/config/kmemtrace.h) \
  include/linux/workqueue.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kref.h \
  include/linux/kmemtrace.h \
  include/trace/events/kmem.h \
  include/linux/tracepoint.h \
  include/trace/define_trace.h \
  include/linux/kmemleak.h \
  include/linux/pfn.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  include/linux/proportions.h \
  include/linux/percpu_counter.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/seccomp.h \
  include/linux/unistd.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/unistd.h \
  include/linux/rculist.h \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
    $(wildcard include/config/security.h) \
  include/linux/key.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/sysctl.h \
  include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  include/linux/aio.h \
  include/linux/aio_abi.h \
  include/linux/uio.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/local.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/module.h \
    $(wildcard include/config/dynamic/ftrace.h) \
  include/trace/events/module.h \
  include/linux/vmalloc.h \
  include/linux/ptrace.h \
  include/linux/ioport.h \
  include/linux/interrupt.h \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/debug/shirq.h) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/linux/hardirq.h \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/hardirq.h \
  include/asm-generic/hardirq.h \
  include/linux/irq.h \
    $(wildcard include/config/s390.h) \
    $(wildcard include/config/irq/per/cpu.h) \
    $(wildcard include/config/irq/release/method.h) \
    $(wildcard include/config/intr/remap.h) \
    $(wildcard include/config/generic/pending/irq.h) \
    $(wildcard include/config/sparse/irq.h) \
    $(wildcard include/config/numa/irq/desc.h) \
    $(wildcard include/config/generic/hardirqs/no//do/irq.h) \
    $(wildcard include/config/cpumasks/offstack.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/irq.h \
    $(wildcard include/config/.h) \
    $(wildcard include/config/irqstacks.h) \
  include/linux/radix-tree.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/irq_regs.h \
  include/asm-generic/irq_regs.h \
  include/linux/irq_cpustat.h \
  include/linux/freezer.h \
    $(wildcard include/config/freezer.h) \
    $(wildcard include/config/cgroup/freezer.h) \
  include/linux/delay.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/delay.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/time.h \
    $(wildcard include/config/ppc/iseries.h) \
    $(wildcard include/config/power.h) \
    $(wildcard include/config/8xx/cpu6.h) \
  include/linux/skbuff.h \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/gfar/skbuff/recycling.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/net/cls/act.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/net/dma.h) \
    $(wildcard include/config/network/secmark.h) \
  include/linux/kmemcheck.h \
  include/linux/net.h \
  include/linux/socket.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/socket.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/sockios.h \
  include/linux/sockios.h \
  include/linux/random.h \
  include/linux/ioctl.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/fcntl.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  include/linux/textsearch.h \
  include/linux/err.h \
  include/net/checksum.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/uaccess.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/checksum.h \
  include/linux/dmaengine.h \
    $(wildcard include/config/dma/engine.h) \
    $(wildcard include/config/async/tx/dma.h) \
    $(wildcard include/config/async/tx/disable/channel/switch.h) \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/devtmpfs.h) \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/pm/runtime.h) \
  include/linux/semaphore.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/device.h \
    $(wildcard include/config/swiotlb.h) \
  include/linux/pm_wakeup.h \
    $(wildcard include/config/pm.h) \
  include/linux/dma-mapping.h \
    $(wildcard include/config/have/dma/attrs.h) \
  include/linux/dma-attrs.h \
  include/linux/bug.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/scatterlist.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/dma.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/io.h \
    $(wildcard include/config/ra.h) \
    $(wildcard include/config/rd.h) \
    $(wildcard include/config/ppc/indirect/io.h) \
    $(wildcard include/config/eeh.h) \
  include/linux/io.h \
    $(wildcard include/config/has/ioport.h) \
  include/asm-generic/iomap.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/io-defs.h \
  include/linux/mm.h \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/pgtable.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/pgtable-ppc32.h \
    $(wildcard include/config/consistent/size.h) \
    $(wildcard include/config/fsl/booke.h) \
  include/asm-generic/pgtable-nopmd.h \
  include/asm-generic/pgtable-nopud.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/pte-fsl-booke.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/pte-common.h \
    $(wildcard include/config/ppc/std/mmu.h) \
    $(wildcard include/config/kgdb.h) \
    $(wildcard include/config/xmon.h) \
    $(wildcard include/config/kprobes.h) \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/have/mlocked/page/bit.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/dma-mapping.h \
  include/linux/dma-debug.h \
    $(wildcard include/config/dma/api/debug.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/swiotlb.h \
  include/linux/swiotlb.h \
  include/asm-generic/dma-mapping-common.h \
  include/linux/netdevice.h \
    $(wildcard include/config/dcb.h) \
    $(wildcard include/config/wlan/80211.h) \
    $(wildcard include/config/ax25.h) \
    $(wildcard include/config/mac80211/mesh.h) \
    $(wildcard include/config/tr.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/net/ipgre.h) \
    $(wildcard include/config/ipv6/sit.h) \
    $(wildcard include/config/ipv6/tunnel.h) \
    $(wildcard include/config/netpoll.h) \
    $(wildcard include/config/net/gianfar/fp.h) \
    $(wildcard include/config/net/poll/controller.h) \
    $(wildcard include/config/fcoe.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/net/dsa.h) \
    $(wildcard include/config/net/ns.h) \
    $(wildcard include/config/net/dsa/tag/dsa.h) \
    $(wildcard include/config/net/dsa/tag/trailer.h) \
    $(wildcard include/config/netpoll/trap.h) \
  include/linux/if.h \
  include/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/if_packet.h \
  include/linux/ethtool.h \
  include/net/net_namespace.h \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/ip/dccp.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/net.h) \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/xfrm/statistics.h) \
  include/net/snmp.h \
  include/linux/snmp.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/ip/multiple/tables.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/ip/pimsm/v1.h) \
    $(wildcard include/config/ip/pimsm/v2.h) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/ipv6/multiple/tables.h) \
    $(wildcard include/config/ipv6/mroute.h) \
    $(wildcard include/config/ipv6/pimsm/v2.h) \
  include/net/dst_ops.h \
  include/net/netns/dccp.h \
  include/net/netns/x_tables.h \
    $(wildcard include/config/bridge/nf/ebtables.h) \
  include/linux/netfilter.h \
    $(wildcard include/config/netfilter/debug.h) \
    $(wildcard include/config/nf/nat/needed.h) \
  include/linux/in.h \
  include/linux/in6.h \
  include/net/netns/xfrm.h \
  include/linux/xfrm.h \
  include/linux/seq_file_net.h \
  include/linux/seq_file.h \
  include/net/dsa.h \
  include/linux/inet.h \
  include/linux/inetdevice.h \
  include/linux/rtnetlink.h \
  include/linux/netlink.h \
  include/linux/if_link.h \
  include/linux/if_addr.h \
  include/linux/neighbour.h \
  include/linux/if_arp.h \
  include/linux/if_vlan.h \
    $(wildcard include/config/vlan/8021q.h) \
  include/linux/etherdevice.h \
    $(wildcard include/config/have/efficient/unaligned/access.h) \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/unaligned.h \
  include/linux/unaligned/access_ok.h \
  include/linux/unaligned/generic.h \
  include/linux/ip.h \
  include/linux/ipv6.h \
    $(wildcard include/config/ipv6/privacy.h) \
    $(wildcard include/config/ipv6/router/pref.h) \
    $(wildcard include/config/ipv6/route/info.h) \
    $(wildcard include/config/ipv6/optimistic/dad.h) \
    $(wildcard include/config/ipv6/mip6.h) \
    $(wildcard include/config/ipv6/subtrees.h) \
  include/linux/icmpv6.h \
  include/linux/tcp.h \
    $(wildcard include/config/tcp/md5sig.h) \
  include/net/sock.h \
  include/linux/list_nulls.h \
  include/linux/security.h \
    $(wildcard include/config/security/path.h) \
    $(wildcard include/config/security/network.h) \
    $(wildcard include/config/security/network/xfrm.h) \
    $(wildcard include/config/securityfs.h) \
  include/linux/fs.h \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/inotify.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/limits.h \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/fiemap.h \
  include/linux/quota.h \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/binfmts.h \
  include/linux/shm.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/shmparam.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/shmbuf.h \
  include/linux/msg.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/msgbuf.h \
  include/net/flow.h \
  include/linux/filter.h \
  include/linux/rculist_nulls.h \
  include/linux/poll.h \
  /home/kosta/ltib-p1020rdb_p2020rdb-20101029/rpm/BUILD/linux-cloud-master/arch/powerpc/include/asm/poll.h \
  include/asm-generic/poll.h \
  include/net/dst.h \
    $(wildcard include/config/net/cls/route.h) \
  include/net/neighbour.h \
  include/net/rtnetlink.h \
  include/net/netlink.h \
  include/net/inet_connection_sock.h \
  include/net/inet_sock.h \
  include/linux/jhash.h \
  include/net/request_sock.h \
  include/net/netns/hash.h \
  include/net/inet_timewait_sock.h \
  include/net/tcp_states.h \
  include/net/timewait_sock.h \
  include/linux/udp.h \
  include/linux/proc_fs.h \
    $(wildcard include/config/proc/devicetree.h) \
    $(wildcard include/config/proc/kcore.h) \
  include/linux/magic.h \
  include/linux/kthread.h \
  include/net/ipv6.h \
  include/net/if_inet6.h \
  include/net/ndisc.h \
  include/net/addrconf.h \
  include/net/xfrm.h \
    $(wildcard include/config/xfrm/sub/policy.h) \
    $(wildcard include/config/xfrm/migrate.h) \
  include/linux/pfkeyv2.h \
  include/linux/ipsec.h \
  include/linux/audit.h \
    $(wildcard include/config/change.h) \
  include/net/ip.h \
    $(wildcard include/config/inet.h) \
  include/net/route.h \
  include/net/inetpeer.h \
  include/linux/in_route.h \
  include/linux/route.h \
  include/net/ip6_fib.h \
  include/linux/ipv6_route.h \
  include/linux/uaccess.h \
  /mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.h \
  /mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h \

/mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.o: $(deps_/mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.o)

$(deps_/mnt/SHARE/MPC2020-xE1/target_board/drv/mpcdrv/linux/mpcdrveth_ipv4.o):

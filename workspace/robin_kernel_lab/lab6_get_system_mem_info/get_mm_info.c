#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <asm/memory.h>
#include <asm/pgtable-hwdef.h>


#define PRT(a, b) pr_info("%-15s=%10d %10ld %8ld\n", \
			 a, b, (PAGE_SIZE*b)/1024, (PAGE_SIZE*b)/1024/1024)

static int __init my_init(void)
{
	struct page *p;
	unsigned long i, pfn, valid = 0;
	int free = 0, locked = 0, reserved = 0, swapcache = 0,
	    referenced = 0, slab = 0, private = 0, uptodate = 0,
	    dirty = 0, active = 0, writeback = 0, mappedtodisk = 0;

	unsigned long num_physpages;

	num_physpages = get_num_physpages();
	pr_err("VA_BITS = %d\n", VA_BITS);
	pr_err("VA_START = 0x%016lx\n", VA_START);
	pr_err("PAGE_OFFSET = 0x%016lx\n", PAGE_OFFSET);
	pr_err("KIMAGE_VADDR = 0x%016lx\n", KIMAGE_VADDR);
	pr_err("BPF_JIT_REGION_START = 0x%lx\n", BPF_JIT_REGION_START);
	pr_err("BPF_JIT_REGION_SIZE = %dMB\n", BPF_JIT_REGION_SIZE / (1024 * 1024));
	pr_err("MODULES_END = 0x%016lx\n", MODULES_END);
	pr_err("MODULES_VADDR = 0x%016lx\n", MODULES_VADDR);
	pr_err("MODULES_VSIZE = %dMB\n", MODULES_VSIZE / (1024 * 1024));
	pr_err("PCI_IO_END = 0x%016lx\n", PCI_IO_END);
	pr_err("PHYS_PFN_OFFSET = 0x%016llx\n", PHYS_PFN_OFFSET);
	pr_err("PHYS_OFFSET = 0x%016llx\n", PHYS_OFFSET);
	pr_err("FIXADDR_TOP = 0x%016lx\n", FIXADDR_TOP);
	pr_err("PAGE_SHIFT = 0x%016x\n", PAGE_SHIFT);
	pr_err("VMEMMAP_SIZE = %ld GB\n", VMEMMAP_SIZE / (1024 * 1024 * 1024));
	pr_err("VMALLOC_START = 0x%016lx, VMALLOC_END = 0x%016lx\n", VMALLOC_START, VMALLOC_END);
	pr_err("PUD_SHIFT = %lu PUD_SIZE = %luMB PUD_MASK = 0x%016llx\n", PUD_SHIFT, PUD_SIZE / (1024 * 1024), PUD_MASK);
	pr_err("PMD_SHIFT = %lu PMD_SIZE = %luMB PMD_MASK = 0x%016llx\n", PMD_SHIFT, PMD_SIZE / (1024 * 1024), PMD_MASK);
	pr_err("PGDIR_SHIFT = %lu PGDIR_SIZE = %luMB PGDIR_MASK = 0x%016llx\n", PGDIR_SHIFT, PGDIR_SIZE / (1024 * 1024), PGDIR_MASK);
	pr_err("SECTION_SHIFT = %lu SECTION_SIZE = %luMB SECTION_MASK = 0x%016llx\n", SECTION_SHIFT, SECTION_SIZE / (1024 * 1024), SECTION_MASK);
	
	pr_err("CONFIG_ARM64_PA_BITS = %d PHYS_MASK_SHIFT = %d\n", CONFIG_ARM64_PA_BITS, CONFIG_ARM64_PA_BITS);
	pr_err("PHYS_MASK = 0x%016llx\n", PHYS_MASK);



	for (i = 0; i < num_physpages; i++) {

		/* Most of ARM systems have ARCH_PFN_OFFSET */
		pfn = i + ARCH_PFN_OFFSET;
		/* may be holes due to remapping */
		if (!pfn_valid(pfn))
			continue;

		valid++;
		p = pfn_to_page(pfn);
		if (!p)
			continue;
		/* page_count(page) == 0 is a free page. */
		if (!page_count(p)) {
			free++;
			continue;
		}
		if (PageLocked(p))
			locked++;
		if (PageReserved(p))
			reserved++;
		if (PageSwapCache(p))
			swapcache++;
		if (PageReferenced(p))
			referenced++;
		if (PageSlab(p))
			slab++;
		if (PagePrivate(p))
			private++;
		if (PageUptodate(p))
			uptodate++;
		if (PageDirty(p))
			dirty++;
		if (PageActive(p))
			active++;
		if (PageWriteback(p))
			writeback++;
		if (PageMappedToDisk(p))
			mappedtodisk++;
	}

	pr_info("\nExamining %ld pages (num_phys_pages) = %ld MB\n",
		num_physpages, num_physpages * PAGE_SIZE / 1024 / 1024);
	pr_info("Pages with valid PFN's=%ld, = %ld MB\n", valid,
		valid * PAGE_SIZE / 1024 / 1024);
	pr_info("\n                     Pages         KB       MB\n\n");

	PRT("free", free);
	PRT("locked", locked);
	PRT("reserved", reserved);
	PRT("swapcache", swapcache);
	PRT("referenced", referenced);
	PRT("slab", slab);
	PRT("private", private);
	PRT("uptodate", uptodate);
	PRT("dirty", dirty);
	PRT("active", active);
	PRT("writeback", writeback);
	PRT("mappedtodisk", mappedtodisk);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Module exit\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Ben Shushu");
MODULE_LICENSE("GPL v2");

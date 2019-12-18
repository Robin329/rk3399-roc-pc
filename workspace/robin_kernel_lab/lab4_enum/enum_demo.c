/*
 * =====================================================================================
 *
 *       Filename:  enum_demo.c
 *
 *    Description: Enum value test. 
 *
 *        Version:  1.0
 *        Created:  2019年12月10日 14时12分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jiangrenbin (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>

enum fixed_address {
	FIX_HOLE,

		/*
	 * Reserve a virtual window for the FDT that is 2 MB larger than the
	 * maximum supported size, and put it at the top of the fixmap region.
	 * The additional space ensures that any FDT that does not exceed
	 * MAX_FDT_SIZE can be mapped regardless of whether it crosses any
	 * 2 MB alignment boundaries.
	 *
	 * Keep this at the top so it remains 2 MB aligned.
	 */
#define FIX_FDT_SIZE		(0x00200000 + 0x00200000)
	FIX_FDT_END,
	FIX_FDT = FIX_FDT_END + FIX_FDT_SIZE / 4096 - 1,

	FIX_EARLYCON_MEM_BASE,
	FIX_TEXT_POKE0,
	__end_of_permanent_fixed_addresses,

	/*
	 * Temporary boot-time mappings, used by early_ioremap(),
	 * before ioremap() is functional.
	 */
#define NR_FIX_BTMAPS		(0x00040000 / 4096)
#define FIX_BTMAPS_SLOTS	7
#define TOTAL_FIX_BTMAPS	(NR_FIX_BTMAPS * FIX_BTMAPS_SLOTS)

	FIX_BTMAP_END = __end_of_permanent_fixed_addresses,
	FIX_BTMAP_BEGIN = FIX_BTMAP_END + TOTAL_FIX_BTMAPS - 1,

	/*
	 * Used for kernel page table creation, so unmapped memory may be used
	 * for tables.
	 */
	FIX_PTE,
	FIX_PMD,
	FIX_PUD,
	FIX_PGD,

	__end_of_fixed_addresses
};

int main(int argc, char * argv[])
{
	printf("FIX_HOLE <%p> <%d>\n"
		   "FIX_FDT_END <%p> <%d>\n"
			"FIX_EARLYCON_MEM_BASE <%p> <%d>\n", \
			FIX_HOLE, FIX_HOLE, FIX_FDT_END, FIX_FDT_END, \
			FIX_EARLYCON_MEM_BASE, FIX_EARLYCON_MEM_BASE);	
}

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Linux kernel module to walk page tables.");
MODULE_VERSION("0.1");

static void walk_page_table(struct task_struct *task) {
    struct mm_struct *mm = task->mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *ptep, pte;
    unsigned long address;
    int rss_count = 0, swap_count = 0;

    for (address = mm->mmap->vm_start; address < mm->mmap->vm_end; address += PAGE_SIZE) {
        pgd = pgd_offset(mm, address);
        if (pgd_none(*pgd) || pgd_bad(*pgd))
            continue;

        p4d = p4d_offset(pgd, address);
        if (p4d_none(*p4d) || p4d_bad(*p4d))
            continue;

        pud = pud_offset(p4d, address);
        if (pud_none(*pud) || pud_bad(*pud))
            continue;

        pmd = pmd_offset(pud, address);
        if (pmd_none(*pmd) || pmd_bad(*pmd))
            continue;

        ptep = pte_offset_map(pmd, address);
        if (!ptep)
            continue;

        pte = *ptep;
        if (pte_present(pte))
            rss_count++;
        else
            swap_count++;
    }

    printk(KERN_INFO "RSS pages: %d\n", rss_count);
    printk(KERN_INFO "SWAP pages: %d\n", swap_count);
}

static int __init page_walk_init(void) {
    printk(KERN_INFO "PageWalkModule: Loaded\n");
    walk_page_table(current);
    return 0;
}

static void __exit page_walk_exit(void) {
    printk(KERN_INFO "PageWalkModule: Removed\n");
}

module_init(page_walk_init);
module_exit(page_walk_exit);

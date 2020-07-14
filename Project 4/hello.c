#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/fdtable.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched/signal.h>
#include <linux/pagemap.h>
#include <linux/file.h>
#include <linux/fs_struct.h>
#include <linux/rcupdate.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/sched.h>
#include <linux/hugetlb.h>
static int pid;
module_param(pid, int, 0);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("BarisVePinar");
MODULE_DESCRIPTION("A Simple Hello World module");

void retrieve(struct task_struct *task) {
    //struct task_struct *ts = current;
    //struct list_head *list;

/*
    printk(KERN_INFO "Process Name%d", ts->pid);

    list_for_each(list, &current->children) {
        ts = list_entry(list, struct task_struct, sibling);
        retrieve(ts);
    }
*/
}

static int __init pcbinfo_init(void)
{
    struct fdtable* files_table;
    struct task_struct *node;
    struct path *root;
    char *pathname;
    char *tmp;
    int i = 0;
    //printk("hop");
    printk(KERN_INFO "Starting module...\n");
    
    for_each_process(node){
        if(node->pid == pid){
            
            printk("Task %s (pid = %d)\n",node->comm, node->pid);
            files_table = files_fdtable(node->files);
            while(files_table->fd[i] != NULL) { 
                //printk(KERN_ALERT "Open file with fd %ld", files_table->open_fds[i]);
                //printk(KERN_ALERT "Max fds %d", files_table->max_fds);

                
                
                loff_t pos;
                mode_t mode;
                char* name;
                struct file *file;
                struct path *path;
                
                struct inode* inode; 
                struct dentry *dentry;
                
                spin_lock(&node->files->file_lock);
                file = fcheck_files(node->files, i  );
                if (!file) {
                    spin_unlock(&node->files->file_lock);
                    return -ENOENT;
                }
                
                path = &file->f_path;
                //inode = &file->f_inode;
                pos = file->f_pos;
                mode = file->f_mode;
                dentry= file->f_path.dentry;
                inode = dentry->d_inode;
                name = dentry->d_name.name;
                
                path_get(path);
                spin_unlock(&node->files->file_lock);

                tmp = (char *)__get_free_page(GFP_KERNEL);

                if (!tmp) {
                    path_put(path);
                    return -ENOMEM;
                }

                pathname = d_path(path, tmp, PAGE_SIZE);
                path_put(path);

                if (IS_ERR(pathname)) {
                    free_page((unsigned long)tmp);
                    return PTR_ERR(pathname);
                }

                /* do something here with pathname */
                printk(KERN_ALERT "FD with Pathname: %d %s", i, pathname);
                printk(KERN_ALERT "Pointer ID: %llu", pos);
                printk(KERN_ALERT "User Id: %x", inode->i_uid.val);
                printk(KERN_ALERT "Access Mode: %d", mode);
                printk(KERN_ALERT "Name: %s", name);
                printk(KERN_ALERT "Inode Id: %lu", inode->i_ino);
                printk(KERN_ALERT "File size: %llu", inode->i_size);
                printk(KERN_ALERT "Number of Blocks: %lu", inode->i_blocks);
                printk(KERN_ALERT "Number of Cached Blocks: %lu", inode->i_mapping->nrpages);
                
                unsigned long address = node->mm->mmap_base;
                struct mm_struct *mm = node->mm;
                pgd_t *pgd = pgd_offset(mm, address);
                pud_t *pud = pud_offset(pgd, address);
                pmd_t *pmd = pmd_offset(pud, address);  
                pte_t *pte = pte_offset_map(pmd, address);  
                struct page *page = pte_page(pte);
                printk(KERN_ALERT "Usage Count: %u", page->_refcount);
                /*
                struct page *page;
                page = find_get_page(inode->i_mapping, 0);
                printk(KERN_ALERT "Number of Cached Blocks: %d  %u", i, page->_refcount);
                */
                free_page((unsigned long)tmp);
                printk(KERN_ALERT "---------------------------------------------------------------------------------");
                i++;
            }
            spin_lock(&node->fs->lock);
            root = &node->fs->pwd;

            path_get(root);
            spin_unlock(&node->fs->lock);

            tmp = (char *)__get_free_page(GFP_KERNEL);

            if (!tmp) {
                path_put(root);
                return -ENOMEM;
            }

            pathname = d_path(root, tmp, PAGE_SIZE);
            path_put(root);

            if (IS_ERR(pathname)) {
                free_page((unsigned long)tmp);
                return PTR_ERR(pathname);
            }
            printk(KERN_ALERT "Process Root: %s", pathname);
            free_page((unsigned long)tmp);
            
            
            printk(KERN_ALERT "Buffer Cache: %lu", node->mm->mmap->vm_start);
            
            printk(KERN_ALERT "---------------------------------------------------------------------------------");
            //get_fs_root(node->fs, root);
            //printk("Task File Struct: %s\n",root);
            //printk("Task Files: %d\n",node->files->count);
        }
    }
    
    //for(;node->pid != 1; node = node->parent)

    //retrieve(node);
    return 0;
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}


module_init(pcbinfo_init);
module_exit(hello_cleanup);

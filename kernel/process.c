#include <memory.h>
#include <printk.h>
#include <process.h>
#include <server.h>
#include <table.h>
#include <thread.h>

/// The process/thread table.
struct table all_processes;
struct list_head process_list;
/// The kernel process.
struct process *kernel_process;

/// Creates a new process. `name` is used for just debugging use.
struct process *process_create(const char *name) {
    int pid = table_alloc(&all_processes);
    if (!pid) {
        return NULL;
    }

    struct process *proc = kmalloc(&page_arena);
    if (!proc) {
        table_free(&all_processes, pid);
        return NULL;
    }

    if (table_init(&proc->channels) != OK) {
        kfree(&small_arena, proc);
        table_free(&all_processes, pid);
        return NULL;
    }

    proc->pid = pid;
    strcpy((char *) &proc->name, PROCESS_NAME_LEN_MAX, name);
    spin_lock_init(&proc->lock);
    list_init(&proc->vmareas);
    list_init(&proc->threads);
    page_table_init(&proc->page_table);

    table_set(&all_processes, pid, (void *) proc);
    list_push_back(&process_list, &proc->next);

    TRACE("new process: pid=%d, name=%s", pid, &proc->name);
    return proc;
}

/// Destroys a process.
void process_destroy(UNUSED struct process *process) {
    UNIMPLEMENTED();
}

/// Adds a new vm area.
error_t vmarea_add(struct process *process, vaddr_t start, vaddr_t end,
                   pager_t pager, void *pager_arg, int flags) {

    struct vmarea *vma = kmalloc(&page_arena);
    if (!vma) {
        return ERR_OUT_OF_MEMORY;
    }

    TRACE("new vmarea: vaddr=%p-%p", start, end);
    vma->start = start;
    vma->end = end;
    vma->pager = pager;
    vma->arg = pager_arg;
    vma->flags = flags;

    list_push_back(&process->vmareas, &vma->next);
    return OK;
}

/// Initializes the process subsystem.
void process_init(void) {
    list_init(&process_list);
    if (table_init(&all_processes) != OK) {
        PANIC("failed to initialize all_processes");
    }

    kernel_process = process_create("kernel");
    ASSERT(kernel_process->pid == KERNEL_PID);
}

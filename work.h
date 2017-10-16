#ifndef __WORK_H__
#define __WORK_H__

#include <stdbool.h>
#include <sys/eventfd.h>

#include "util.h"

struct work;

typedef void (*work_func_t)(struct work *);

struct work {
	struct list_node w_list;
	work_func_t fn;
	work_func_t done;
};

struct work_queue {
	int wq_state;
	struct list_head pending_list;
};

enum wq_thread_control {
	WQ_ORDERED, /* Only 1 thread created for work queue */
	WQ_DYNAMIC, /* # of threads proportional to nr_nodes created */
	WQ_FIXED, /* Fixed # of threads created */
};

static inline bool is_main_thread(void)
{
	return gettid() == getpid();
}

static inline bool is_worker_thread(void)
{
	return !is_main_thread();
}

/*
 * Helper macros to guard variables from being accessed out of the
 * main thread.  Note that we can use these only for pointers.
 */
#define main_thread(type) struct { type __val; }
#define main_thread_get(var)			\
({						\
	assert(is_main_thread());		\
	(var).__val;				\
})
#define main_thread_set(var, val)		\
({						\
	assert(is_main_thread());		\
	(var).__val = (val);			\
})

/*
 * 'get_nr_nodes' is the function to get the current number of nodes and used
 * for dynamic work queues.  'create_cb' will be called when worker threads are
 * created and 'destroy_cb' will be called when worker threads are destroyed.
 */
int init_work_queue(void);
struct work_queue *create_dynamic_work_queue(const char *name);
struct work_queue *create_ordered_work_queue(const char *name);
struct work_queue *create_fixed_work_queue(const char *name, int nr_threads);
void queue_work(struct work_queue *q, struct work *work);
bool work_queue_empty(struct work_queue *q);
int wq_trace_init(void);
void set_max_dynamic_threads(size_t nr_max);

#ifdef HAVE_TRACE
void suspend_worker_threads(void);
void resume_worker_threads(void);
#endif	/* HAVE_TRACE */

typedef pthread_t sd_thread_t;
int sd_thread_create(const char *, sd_thread_t *,
		     void *(*start_routine)(void *), void *);
int sd_thread_create_with_idx(const char *, sd_thread_t *,
		     void *(*start_routine)(void *), void *);
int sd_thread_join(sd_thread_t , void **);

#endif

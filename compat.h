#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#define CONFIG_64BIT
#define CONFIG_MAPLE_RCU_DISABLED

/* rcu*/
struct rcu_head {
    void *pad1;
    void *pad2;
};
#define __rcu
#define rcu_assign_pointer(x, y) (x) = (y)
#define rcu_read_lock()
#define rcu_read_unlock()

/* lock */
typedef struct {
    void *pad;
} spinlock_t;
#define __SPIN_LOCK_UNLOCKED(x) {}
#define spin_lock(x)
#define spin_unlock(x)
#define spin_lock_init(x)
#define lockdep_is_held(x) 1

/* linux */
#define BUG_ON(x) assert(!(x))
#define WARN_ON(...)

/* memory */
typedef int gfp_t;

static inline void *xa_mk_value(unsigned long v)
{
	WARN_ON((long)v < 0);
	return (void *)((v << 1) | 1);
}

static inline unsigned long xa_to_value(const void *entry)
{
	return (unsigned long)entry >> 1;
}
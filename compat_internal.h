#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define trace_ma_op(...)
#define trace_ma_write(...)
#define trace_ma_read(...)

/* rcu */
#define rcu_dereference_check(x, y) (x)
#define rcu_dereference_protected(x, y) (x)
#define call_rcu(...)
#define smp_wmb()

/* memory */
struct kmem_cache {};
#define kmem_cache_create(...) NULL
#define kmem_cache_shrink(...)
static inline void *kmem_cache_alloc(struct kmem_cache *, gfp_t) {
	void *ptr;
	if (posix_memalign(&ptr, sizeof(struct maple_node), sizeof(struct maple_node)) != 0)
		return NULL;
	return ptr;
}
static inline void kmem_cache_free(struct kmem_cache *, void *ptr) {
	free(ptr);
}
static inline void kmem_cache_free_bulk(struct kmem_cache *s, size_t size, void **ptr) {
	size_t i;
	for (i = 0; i < size; i++)
		free(ptr[i]);
}
static inline int kmem_cache_alloc_bulk(struct kmem_cache *s, gfp_t, size_t size, void **ptr) {
	size_t i;
	for (i = 0; i < size; i++)
		if (posix_memalign(&ptr[i], sizeof(struct maple_node), sizeof(struct maple_node)) != 0)
			goto error;
	return size;
error:
	kmem_cache_free_bulk(s, i, ptr);
	return 0;
}
#define __GFP_ZERO 0
#define GFP_NOWAIT 0
#define __GFP_NOWARN 0
#define GFP_KERNEL 0
#define __KERNEL__
#define gfpflags_allow_blocking(x) 1
#define WARN_ON_ONCE(x) (x)

/* linux */
#define __init
#define __must_hold(x)
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)
#define EXPORT_SYMBOL_GPL(x)
#define EXPORT_SYMBOL(x)
#define container_of(ptr, type, member) ({				\
	void *__mptr = (void *)(ptr);					\
	((type *)(__mptr - offsetof(type, member))); })


#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)
#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define __is_constexpr(x) \
	(sizeof(int) == sizeof(*(8 ? ((void *)((long)(x) * 0l)) : (int *)8)))

#define __typecheck(x, y) \
	(!!(sizeof((typeof(x) *)1 == (typeof(y) *)1)))

#define __no_side_effects(x, y) \
		(__is_constexpr(x) && __is_constexpr(y))

#define __safe_cmp(x, y) \
		(__typecheck(x, y) && __no_side_effects(x, y))

#define __cmp(x, y, op)	((x) op (y) ? (x) : (y))

#define __cmp_once(x, y, unique_x, unique_y, op) ({	\
		typeof(x) unique_x = (x);		\
		typeof(y) unique_y = (y);		\
		__cmp(unique_x, unique_y, op); })

#define __careful_cmp(x, y, op) \
	__builtin_choose_expr(__safe_cmp(x, y), \
		__cmp(x, y, op), \
		__cmp_once(x, y, __UNIQUE_ID(__x), __UNIQUE_ID(__y), op))

/**
 * min - return minimum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define min(x, y)	__careful_cmp(x, y, <)

/**
 * max - return maximum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define max(x, y)	__careful_cmp(x, y, >)

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* xarray */
#define MAX_ERRNO	4095
static inline bool xa_is_internal(const void *entry)
{
	return ((unsigned long)entry & 3) == 2;
}

static inline void *xa_mk_internal(unsigned long v)
{
	return (void *)((v << 2) | 2);
}

static inline bool xa_is_err(const void *entry)
{
	return unlikely(xa_is_internal(entry) &&
			entry >= xa_mk_internal(-MAX_ERRNO));
}

static inline bool xa_is_node(const void *entry)
{
	return xa_is_internal(entry) && (unsigned long)entry > 4096;
}

#define XA_RETRY_ENTRY		xa_mk_internal(256)
#define XA_ZERO_ENTRY		xa_mk_internal(257)

static inline bool xa_is_zero(const void *entry)
{
	return unlikely(entry == XA_ZERO_ENTRY);
}

static inline bool xa_is_advanced(const void *entry)
{
	return xa_is_internal(entry) && (entry <= XA_RETRY_ENTRY);
}

static inline int xa_err(void *entry)
{
	/* xa_to_internal() would not do sign extension. */
	if (xa_is_err(entry))
		return (long)entry >> 2;
	return 0;
}
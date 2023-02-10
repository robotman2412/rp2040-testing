
#include <abi_all.h>

// Declare main function.
extern int main();

// Reent variables used in a few stdlib things.
struct _reent *_impure_ptr __ATTRIBUTE_IMPURE_PTR__;
struct _reent *const _global_impure_ptr __ATTRIBUTE_IMPURE_PTR__;

// Initialisers list.
typedef void (*init_func_t)();
extern init_func_t __start_init_array[];
extern init_func_t __stop_init_array[];

// On exit callback function.
typedef void (*exit_func_t)(void *arg);

// List of on exit callbacks.
typedef struct {
	exit_func_t func;
	void       *arg;
} exit_entry_t;
static exit_entry_t *exit_entries = NULL;
static size_t num_exit_entries = 0;

// Register exit handlers.
int __aeabi_atexit(void *arg, exit_func_t callback, void *dso_handle) {
	num_exit_entries ++;
	void *mem = realloc(exit_entries, num_exit_entries * sizeof(exit_entry_t));
	// if (!mem) abort();
	exit_entries = mem;
	exit_entries[num_exit_entries-1] = (exit_entry_t) {
		callback, arg
	};
}

// Dummy definition so that __dso_handle exists.
int __dso_handle;

int _start() {
	// Despite the const, we set the reent pointers.
	_impure_ptr = __get_reent();
	(*(struct _reent **) &_global_impure_ptr) = __get_global_reent();
	
	// Run global constructors.
	for (init_func_t *i = __start_init_array; i != __stop_init_array; i++) {
		(*i)();
	}
	
	int retval = main();
	
	// Run global deconstructors.
	for (size_t i = 0; i < num_exit_entries; i++) {
		exit_entries[i].func(exit_entries[i].arg);
	}
	free(exit_entries);
	
	return retval;
}

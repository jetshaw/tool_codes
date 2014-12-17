#ifndef __RW_LOKCER_H__
#define __RW_LOKCER_H__

#include <pthread.h>

class xrw_locker {
public:
	xrw_locker() {
		pthread_rwlock_init(&rw_locker, NULL);
	}

	~xrw_locker() {
		pthread_rwlock_destroy(&rw_locker);
	}

	bool get_read_permission() {
		if (0 != pthread_rwlock_rdlock(&rw_locker)) {
			return false;
		}

		return true;
	
	}

	bool get_write_permission() {
		if (0 != pthread_rwlock_wrlock(&rw_locker)) {
			return false;
		}

		return true;
	
	}

	// unblocking mode.
	bool get_read_permission_unblock() {
		if (0 != pthread_rwlock_tryrdlock(&rw_locker)) {
			return false;
		}

		return true;
	}

	bool get_write_permission_unblock() {
		if (0 != pthread_rwlock_trywrlock(&rw_locker)) {
			return false;
		}

		return true;
	}

	bool release_permission() {
		pthread_rwlock_unlock(&rw_locker);

		return true;
	}

private:
	pthread_rwlock_t rw_locker; 
};

#endif // __RW_LOKCER_H__


#ifndef _SHARED_H_
#define _SHARED_H_

#include <stdbool.h>
#include <limits.h>


#include <libinput.h>


enum tools_backend {
	BACKEND_DEVICE,
	BACKEND_UDEV
};


struct libinput* tools_open_backend(enum tools_backend which,
				    const char *seat_or_device,
				    bool verbose,
				    bool *grab);

bool find_touchpad_device(char *path, size_t path_len);
bool is_touchpad_device(const char *devnode);

#endif

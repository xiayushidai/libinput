
#include "shared.h"
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <libudev.h>
#include <string.h>



#define EVIOCGRAB _IOW('E', 0x90, int)




static int
open_restricted(const char *path, int flags, void *user_data)
{
	bool *grab = user_data;
	int fd = open(path, flags);

	if (fd < 0)
		fprintf(stderr, "Failed to open %s (%s)\n",
			path, strerror(errno));
	else if (grab && *grab && ioctl(fd, EVIOCGRAB, (void*)1) == -1)
		fprintf(stderr, "Grab requested, but failed for %s (%s)\n",
			path, strerror(errno));

	return fd < 0 ? -errno : fd;
}

static void
close_restricted(int fd, void *user_data)
{
	close(fd);
}

static const struct libinput_interface interface = {
	.open_restricted = open_restricted,
	.close_restricted = close_restricted,
};

static struct libinput *
tools_open_udev(const char *path, bool verbose, bool *grab)
{
	struct libinput *li;
	struct udev *udev = udev_new();

	if (!udev) {
		fprintf(stderr, "Failed to initialize udev\n");
		return NULL;
	}

	li = libinput_udev_create_context(&interface, grab, udev);
	if (!li) {
		fprintf(stderr, "Failed to initialize context from udev\n");
		goto out;
	}


    if (libinput_udev_assign_seat(li, path)) {
		fprintf(stderr, "Failed to set seat\n");
		libinput_unref(li);
		li = NULL;
		goto out;
	}

out:
	udev_unref(udev);
	return li;
}

static struct libinput *
tools_open_device(const char *path, bool verbose, bool *grab)
{
	struct libinput_device *device;
	struct libinput *li;

	li = libinput_path_create_context(&interface, grab);
	if (!li) {
		fprintf(stderr, "Failed to initialize context from %s\n", path);
		return NULL;
	}


	device = libinput_path_add_device(li, path);
	if (!device) {
		fprintf(stderr, "Failed to initialized device %s\n", path);
		libinput_unref(li);
		li = NULL;
	}
    libinput_device_ref(device);
	return li;
}



struct libinput *
tools_open_backend(enum tools_backend which,
           const char *path,
		   bool verbose,
		   bool *grab)
{
	struct libinput *li;

	switch (which) {
	case BACKEND_UDEV:
        li = tools_open_udev(path, verbose, grab);
		break;
	case BACKEND_DEVICE:
        li = tools_open_device(path, verbose, grab);
		break;
	default:
		abort();
	}

	return li;
}



//static char*
//find_device(const char *udev_tag)
//{
//	struct udev *udev;
//	struct udev_enumerate *e;
//	struct udev_list_entry *entry = NULL;
//	struct udev_device *device;
//	const char *path, *sysname;
//	char *device_node = NULL;

//	udev = udev_new();
//	e = udev_enumerate_new(udev);
//	udev_enumerate_add_match_subsystem(e, "input");
//	udev_enumerate_scan_devices(e);

//	udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(e)) {
//		path = udev_list_entry_get_name(entry);
//		device = udev_device_new_from_syspath(udev, path);
//		if (!device)
//			continue;

//		sysname = udev_device_get_sysname(device);
//		if (strncmp("event", sysname, 5) != 0) {
//			udev_device_unref(device);
//			continue;
//		}

//		if (udev_device_get_property_value(device, udev_tag))
//            strcpy(device_node, udev_device_get_devnode(device));

//		udev_device_unref(device);

//		if (device_node)
//			break;
//	}
//	udev_enumerate_unref(e);
//	udev_unref(udev);

//	return device_node;
//}

//bool
//find_touchpad_device(char *path, size_t path_len)
//{
//	char *devnode = find_device("ID_INPUT_TOUCHPAD");

//	if (devnode) {
//		snprintf(path, path_len, "%s", devnode);
//		free(devnode);
//	}

//	return devnode != NULL;
//}

//bool
//is_touchpad_device(const char *devnode)
//{
//	struct udev *udev;
//	struct udev_device *dev = NULL;
//	struct stat st;
//	bool is_touchpad = false;

//	if (stat(devnode, &st) < 0)
//		return false;

//	udev = udev_new();
//	dev = udev_device_new_from_devnum(udev, 'c', st.st_rdev);
//	if (!dev)
//		goto out;

//	is_touchpad = udev_device_get_property_value(dev, "ID_INPUT_TOUCHPAD");
//out:
//	if (dev)
//		udev_device_unref(dev);
//	udev_unref(udev);

//	return is_touchpad;
//}


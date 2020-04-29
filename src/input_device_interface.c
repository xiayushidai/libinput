#include "input_device_interface.h"
#include "shared.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

typedef struct _device{
    struct libinput_device *dev;
    struct libinput *libinput;
}device;


struct libinput* uos_create_libiput_device_interface(char* device_node)
{
    bool grab = false;
    return tools_open_backend(BACKEND_DEVICE, device_node, false, &grab);
}


struct libinput_device* uos_find_device(char *device_node, struct libinput* li)
{
    struct libinput_device *dev = NULL;
    if (li) {
        struct libinput_event *ev = NULL;
        struct udev_device *udev_device;

        const char *devnode;

        libinput_dispatch(li);
        while ((ev = libinput_get_event(li))) {
           dev = libinput_event_get_device(ev);

           if (libinput_event_get_type(ev) == LIBINPUT_EVENT_DEVICE_ADDED){
               libinput_event_destroy(ev);
               udev_device = libinput_device_get_udev_device(dev);
               devnode = udev_device_get_devnode(udev_device);
               if(!strcmp(device_node, devnode)){
                   udev_device_unref(udev_device);
                   return dev;
               }
           }

           libinput_dispatch(li);
        }
    }

    return dev;
}



struct libinput_device* get_device(char* device_node, struct libinput* li)
{
    struct libinput_device *dev = NULL;
    struct libinput_event *ev = NULL;
    struct udev_device *udev_device;

    const char *devnode;

    libinput_dispatch(li);
    while ((ev = libinput_get_event(li))) {
       dev = libinput_event_get_device(ev);

       if (libinput_event_get_type(ev) == LIBINPUT_EVENT_DEVICE_ADDED){
           libinput_event_destroy(ev);
           udev_device = libinput_device_get_udev_device(dev);
           devnode = udev_device_get_devnode(udev_device);
           if(!strcmp(device_node, devnode)){
               udev_device_unref(udev_device);
               return dev;
           }
       }

       libinput_dispatch(li);
    }
    return dev;
}

void uos_drain_event(struct libinput* li)
{
    struct libinput_event *event;
    libinput_dispatch(li);
    while ((event = libinput_get_event(li))) {
        libinput_event_destroy(event);
        libinput_dispatch(li);
    }
}


char** uos_list_device(device_capability dev_cap)
{
    char** dev_nodes = NULL;
    dev_nodes = (char**)malloc(sizeof(char*) * MAX_DEV_NUM);
    for (int i = 0; i < MAX_DEV_NUM; i++) {
        dev_nodes[i] = NULL;
    }


    bool grab = false;
    struct libinput* li = tools_open_backend(BACKEND_UDEV, "seat0", false, &grab);
    if (!li) {
        return NULL;
    }

    struct libinput_event *ev = NULL;
    struct libinput_device *dev;
    struct udev_device *udev_device;
    const char *devnode;


    libinput_dispatch(li);
    int dev_count = 0;
    while ((ev = libinput_get_event(li))) {
        dev = libinput_event_get_device(ev);

        if (libinput_event_get_type(ev) == LIBINPUT_EVENT_DEVICE_ADDED){ 
            libinput_event_destroy(ev);
            udev_device = libinput_device_get_udev_device(dev);
            devnode = udev_device_get_devnode(udev_device);

            printf("device name is %s\n", libinput_device_get_name(dev));

            if (DEV_ALL == dev_cap) {
                dev_nodes[dev_count] = malloc(DEV_NODE_LEN);
                strcpy(dev_nodes[dev_count], devnode);
                dev_count++;
            }
            else if (DEV_KEYBOARD == dev_cap) {
                if (0 != libinput_device_has_capability(dev, LIBINPUT_DEVICE_CAP_KEYBOARD)) {
                    dev_nodes[dev_count] = malloc(DEV_NODE_LEN);
                    strcpy(dev_nodes[dev_count], devnode);
                    dev_count++;
                }
            }else if (DEV_POINTER == dev_cap) {
                if (0 != libinput_device_has_capability(dev, LIBINPUT_DEVICE_CAP_POINTER)) {
                    dev_nodes[dev_count] = malloc(DEV_NODE_LEN);
                    strcpy(dev_nodes[dev_count], devnode);
                    dev_count++;
                }
            }else if (DEV_TOUCH == dev_cap) {
                if (0 != libinput_device_has_capability(dev, LIBINPUT_DEVICE_CAP_TOUCH)) {
                    dev_nodes[dev_count] = malloc(DEV_NODE_LEN);
                    memcpy(dev_nodes[dev_count], devnode, strlen(devnode) + 1);
                    dev_count++;
                }
            }else if (DEV_TABLET_TOOL == dev_cap) {
                if (0 != libinput_device_has_capability(dev, LIBINPUT_DEVICE_CAP_TABLET_TOOL)) {
                    dev_nodes[dev_count] = malloc(DEV_NODE_LEN);
                    strcpy(dev_nodes[dev_count], devnode);
                    dev_count++;

                }
            }else if (DEV_TABLET_PAD == dev_cap) {
                if (0 != libinput_device_has_capability(dev, LIBINPUT_DEVICE_CAP_TABLET_PAD)) {
                    dev_nodes[dev_count] = malloc(DEV_NODE_LEN);
                    strcpy(dev_nodes[dev_count], devnode);
                    dev_count++;
                }
            }else if (DEV_GETSTURE == dev_cap) {
                if (0 != libinput_device_has_capability(dev, LIBINPUT_DEVICE_CAP_GESTURE)) {
                    dev_nodes[dev_count] = malloc(DEV_NODE_LEN);
                    strcpy(dev_nodes[dev_count], devnode);
                    dev_count++;

                }
            }else if (DEV_SWITCH == dev_cap) {
                if (0 != libinput_device_has_capability(dev, LIBINPUT_DEVICE_CAP_SWITCH)) {
                    dev_nodes[dev_count] = malloc(DEV_NODE_LEN);
                    strcpy(dev_nodes[dev_count], devnode);
                    dev_count++;
                }
            }

            udev_device_unref(udev_device);
            libinput_dispatch(li);
            if (dev_count > MAX_DEV_NUM) {
                break;
            }
        }
    }

    libinput_unref(li);
    if (dev_nodes[0] == NULL) {
        uos_free_dev_node(dev_nodes);
    }
    return dev_nodes;
}

void uos_free_dev_node(char** dev_nodes)
{
    if (dev_nodes != NULL) {
        for (int i = 0; i < MAX_DEV_NUM; ++i) {
            if (dev_nodes[i] != NULL) {
                 free(dev_nodes[i]);
                 dev_nodes[i] = NULL;
            }
        }
        free(dev_nodes);
        dev_nodes = NULL;
    }
}


int uos_set_left_handed(struct libinput_device *dev, int left_handed)
{
    return (int)(libinput_device_config_left_handed_set(dev, left_handed));

}

int uos_get_left_handed(struct libinput_device *dev)
{
    return libinput_device_config_left_handed_get(dev);
}

int uos_set_dwt_enable(struct libinput_device *dev, int enable)
{
    return (int)(libinput_device_config_dwt_set_enabled(dev, enable));
}

int uos_get_dwt_enable(struct libinput_device *dev)
{
    return (int)(libinput_device_config_dwt_get_enabled(dev));
}

double uos_get_accel_speed(struct libinput_device *dev)
{
    return libinput_device_config_accel_get_speed(dev);
}

int uos_set_accel_speed(struct libinput_device *dev, double speed)
{
     return (int)(libinput_device_config_accel_set_speed(dev, speed));
}

int uos_set_event_enable(struct libinput_device *dev, int enable)
{
    return (int)(libinput_device_config_send_events_set_mode(dev, enable));
}

int uos_set_natural_scroll_enable(struct libinput_device *dev, int enable)
{
    return (int)libinput_device_config_scroll_set_natural_scroll_enabled(dev, enable);
}

int uos_get_natural_scroll_enabled(struct libinput_device *dev)
{
    return (int)(libinput_device_config_scroll_get_natural_scroll_enabled(dev));
}

int uos_set_tap_enable(struct libinput_device *dev, int enable)
{
    return (int)(libinput_device_config_tap_set_enabled(dev, enable));
}

int uos_get_tap_enable(struct libinput_device *dev)
{
    return (int)libinput_device_config_tap_get_enabled(dev);
}

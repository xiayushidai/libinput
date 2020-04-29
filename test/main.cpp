#include "input_device_interface.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <poll.h>



void hand_event(libinput_event *event)
{
    auto ev_type = libinput_event_get_type(event);
    if (ev_type == LIBINPUT_EVENT_POINTER_BUTTON) {
        struct libinput_event_pointer *ptrev;
        ptrev = libinput_event_get_pointer_event(event);

        if (libinput_event_pointer_get_button(ptrev) == 0x110) { //BTN_LEFT
            std::cout << "BTN_LEFT" << std::endl;

        } else if (libinput_event_pointer_get_button(ptrev) == 0x111){ //BTN_RIGHT
            std::cout << "BTN_RIGHT" << std::endl;
        }
    }
}

void loop_event(struct libinput* li)
{
    struct libinput_event *event;
    struct pollfd fds;
    fds.fd = libinput_get_fd(li);
    fds.events = POLLIN;
    fds.events = 0;

    while (poll(&fds, 1, 1) > -1) {
        libinput_dispatch(li);
        while ((event = libinput_get_event(li)) != NULL) {
            hand_event(event);
            libinput_event_destroy(event);
            libinput_dispatch(li);
        }
    }

}


void print_dev_node(char** dev_node)
{
    if (dev_node != nullptr) {
        for (int i = 0; i < MAX_DEV_NUM; i++) {
            if (dev_node[i] != nullptr) {
                std::cout << std::string(dev_node[i]) << std::endl;
            } else {
                break;
            }

        }
    }
}

void set_left_hand()
{
    char** dev_nodes = uos_list_device(DEV_POINTER);
    struct libinput *li = NULL;
    if (dev_nodes != nullptr) {

        print_dev_node(dev_nodes);
        if (dev_nodes[2] != nullptr) {
            li = uos_create_libiput_device_interface("/dev/input/event5");
            struct libinput_device *dev = uos_find_device("/dev/input/event5", li);
            int rst = uos_set_left_handed(dev, 0);
            std::cout << "uos_set_left_handed: " << std::string(dev_nodes[2]) << " rst=" << rst << std::endl;
            std::cout << "uos_set_left_handed enable=" << uos_get_left_handed(dev) << std::endl;
            loop_event(li);
        }
    }

    uos_free_dev_node(dev_nodes);
}




void get_accel_speed()
{
    char** dev_nodes = uos_list_device(DEV_POINTER);

    if (dev_nodes != nullptr) {
        print_dev_node(dev_nodes);

    }
}

void set_accel_speed()
{
    char** dev_nodes = uos_list_device(DEV_POINTER);

    if (dev_nodes != nullptr) {

        print_dev_node(dev_nodes);
        if (dev_nodes[2] != nullptr) {
            struct libinput *li = uos_create_libiput_device_interface("/dev/input/event5");
            struct libinput_device *dev = uos_find_device("/dev/input/event5", li);

            int rst = uos_set_accel_speed(dev, 1.0);
            std::cout << "uos_set_accel_speed: "  << "rst=" << rst << std::endl;
            std::cout << "uos_get_accel_speed: current accel speed=" << uos_get_accel_speed(dev) << std::endl;
            loop_event(li);
        }
    }
    uos_free_dev_node(dev_nodes);
}

void set_dwt_enable()
{
    char** dev_nodes = uos_list_device(DEV_POINTER);

    if (dev_nodes != nullptr) {

        print_dev_node(dev_nodes);
        if (dev_nodes[2] != nullptr) {
            struct libinput *li = uos_create_libiput_device_interface("/dev/input/event2");
            struct libinput_device *dev = uos_find_device("/dev/input/event2", li);

            int rst = uos_set_dwt_enable(dev, 1);
            std::cout << "uos_set_dwt_enable: " << " rst=" << rst << std::endl;
            std::cout << "uos_get_dwt_enable:" << uos_get_dwt_enable(dev) << std::endl;
            loop_event(li);
        }
    }
}

void set_event_enable()
{
    char** dev_nodes = uos_list_device(DEV_POINTER);
    struct libinput *li = NULL;
    if (dev_nodes != nullptr) {

        print_dev_node(dev_nodes);
        if (dev_nodes[2] != nullptr) {
            li = uos_create_libiput_device_interface("/dev/input/event5");
            struct libinput_device *dev = uos_find_device("/dev/input/event5", li);
            int rst = uos_set_event_enable(dev, 0);
            std::cout << "uos_set_event_enable: " << " rst=" << rst << std::endl;
            loop_event(li);
        }
    }

    uos_free_dev_node(dev_nodes);
}

void  set_natural_scroll_enable()
{

    char** dev_nodes = uos_list_device(DEV_POINTER);
    struct libinput *li = NULL;
    if (dev_nodes != nullptr) {

        print_dev_node(dev_nodes);
        if (dev_nodes[2] != nullptr) {
            li = uos_create_libiput_device_interface("/dev/input/event5");
            struct libinput_device *dev = uos_find_device("/dev/input/event5", li);
            int rst = uos_set_natural_scroll_enable(dev, 1);
            std::cout << "uos_set_natural_scroll_enable: " << " rst=" << rst << std::endl;
            std::cout << "uos_get_natural_scroll_enable: " << uos_get_natural_scroll_enabled(dev) << std::endl;
            loop_event(li);
        }
    }

    uos_free_dev_node(dev_nodes);
}

void set_tap_enable()
{
    char** dev_nodes = uos_list_device(DEV_POINTER);
    struct libinput *li = NULL;
    if (dev_nodes != nullptr) {

        print_dev_node(dev_nodes);
        if (dev_nodes[2] != nullptr) {
            li = uos_create_libiput_device_interface("/dev/input/event2");
            struct libinput_device *dev = uos_find_device("/dev/input/event2", li);
            int rst = uos_set_tap_enable(dev, 0);
            std::cout << "uos_set_tap_enable: " << " rst=" << rst << std::endl;
            std::cout << "uos_get_tap_enable: " << uos_get_tap_enable(dev) << std::endl;
            loop_event(li);
        }
    }

    uos_free_dev_node(dev_nodes);
}

int main(int argc, char *argv[])
{

    //set_left_hand();
    //set_accel_speed();
    //set_dwt_enable();
    //set_event_enable();
    //set_natural_scroll_enable();
    set_tap_enable();
    return 0;
}

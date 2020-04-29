#include "input_device_interface.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <poll.h>


static struct libinput_device *G_dev[100];
static struct libinput *G_li[100];
char** dev_nodes;

void hand_event(libinput_event *event)
{
    auto ev_type = libinput_event_get_type(event);
    if (ev_type == LIBINPUT_EVENT_POINTER_BUTTON) {
        struct libinput_event_pointer *ptrev;
        ptrev = libinput_event_get_pointer_event(event);

        if (libinput_event_pointer_get_button(ptrev) == 0x110) { //BTN_LEFT
            if(libinput_event_pointer_get_button_state(ptrev)==0){
                std::cout << "BTN_LEFT : RELEASE" << std::endl;
            }else{
                std::cout << "BTN_LEFT : PRESSED" << std::endl;
            }

        } else if (libinput_event_pointer_get_button(ptrev) == 0x111){ //BTN_RIGHT
            if(libinput_event_pointer_get_button_state(ptrev)==0){
                std::cout << "BTN_RIGHT : RELEASE" << std::endl;
            }else{
                std::cout << "BTN_RIGHT : PRESSED" << std::endl;
            }
        }
    }
}

void loop_event(struct libinput** li)
{
    struct libinput_event *event;
    struct pollfd fds[100];
    int i=0;

    for(i=0;*(li+i)!=nullptr;i++){
        fds[i].fd = libinput_get_fd(li[i]);
        fds[i].events = POLLIN;
        fds[i].events = 0;
    }


    while (poll(fds, 1, 1) > -1) {
        for(i=0;*(li+i)!=nullptr;i++){
        libinput_dispatch(li[i]);
            while ((event = libinput_get_event(li[i])) != nullptr) {
                hand_event(event);
                libinput_event_destroy(event);
                libinput_dispatch(li[i]);
            }
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
    int i=0;
    for(i=0;G_dev[i]!=nullptr;i++){
        int rst = uos_set_left_handed(G_dev[i], 1);
        if(rst==0){
            std::cout << libinput_device_get_name(G_dev[i])<< "set : success"  << std::endl;
        }else{
            std::cout << libinput_device_get_name(G_dev[i])<< "set : fail"  << std::endl;
        }
    }
}

void get_left_hand(){
    int i=0;

    for(i=0;G_dev[i]!=nullptr;i++){
        int rst = uos_get_left_handed(G_dev[i]);
        if(rst==0){
            std::cout << libinput_device_get_name(G_dev[i])<< " : right mode"  << std::endl;
        }else{
            std::cout << libinput_device_get_name(G_dev[i])<< " : left mode"  << std::endl;
        }
    }
    loop_event(G_li);
}



void init(){
    dev_nodes = uos_list_device(DEV_POINTER);
    struct libinput *li = nullptr;
    int i=0;

    for(i=0;i<100;i++){
        G_dev[i]=nullptr;
        G_li[i]=nullptr;
    }

    if (dev_nodes != nullptr) {
        print_dev_node(dev_nodes);
        for(i=0;dev_nodes[i]!=nullptr;i++){
            li = uos_create_libiput_device_interface(dev_nodes[i]);
            struct libinput_device *dev = uos_find_device(dev_nodes[i], li);
            G_dev[i]=dev;
            G_li[i]=li;
        }
    }
}


int main(int argc, char *argv[])
{
    init();
    set_left_hand();
    get_left_hand();
    //set_accel_speed();
    //set_dwt_enable();
    //set_event_enable();
    //set_natural_scroll_enable();
    //set_tap_enable();
    uos_free_dev_node(dev_nodes);
    return 0;
}

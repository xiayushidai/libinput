#include "widget.h"
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
#include <unistd.h>
#include <poll.h>
#include <QDebug>
#include <QtDBus/QtDBus>
#include <QObject>
#include <QVBoxLayout>

#define EVIOCGRAB _IOW('E', 0x90, int)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    Init();
    SetLeftHanded(1);
}

Widget::~Widget()
{
    uos_free_dev_node(mDevNodes);
}

void Widget::Init(){
    char **dev_nodes = uos_list_device(DEV_POINTER);
    struct libinput *li = nullptr;
    int i=0;

    for(i=0;i<MAX_DEV_NUM;i++){
        mDevPointer[i]=nullptr;
        mli[i]=nullptr;
    }

    if (dev_nodes != nullptr) {
        for(i=0;dev_nodes[i]!=nullptr;i++){
            qDebug()<<"dev_count = "<<i;
            qDebug()<<"devnode = "<<dev_nodes[i];
            li = uos_create_libiput_device_interface(dev_nodes[i]);
            mDevPointer[i] = uos_find_device(dev_nodes[i], li);
            mli[i]=li;
        }
    }else{
        qDebug()<<"Can not get dev";
    }

    QWidget *widget=new QWidget();
    btntest=new QPushButton("test");
    setleft=new QPushButton("setleft");
    setright=new QPushButton("setright");

    QVBoxLayout *VLay=new QVBoxLayout;
    VLay->addWidget(setleft);
    VLay->addWidget(setright);
    VLay->addWidget(btntest);

    widget->setLayout(VLay);
    setBaseSize(100,200);

    connect(btntest,SIGNAL(clicked()),this,SLOT(test()));
    connect(setleft,SIGNAL(clicked()),this,SLOT(testleft()));
    connect(setright,SIGNAL(clicked()),this,SLOT(testright()));

    widget->show();

}
void Widget::test(){
    qDebug()<<"test";
    GetLeftHanded();
    qDebug()<<"";
}

void Widget::testright(){
    qDebug()<<"set rgiht";
    SetLeftHanded(0);
    qDebug()<<"";
}

void Widget::testleft(){
    qDebug()<<"set left";
    SetLeftHanded(1);
    qDebug()<<"";
}

char ** Widget::uos_list_device(device_capability dev_cap){
    char** dev_nodes = nullptr;
    dev_nodes = (char**)malloc(sizeof(char*) * MAX_DEV_NUM);
    for (int i = 0; i < MAX_DEV_NUM; i++) {
        dev_nodes[i] = nullptr;
    }

    bool grab = false;
    struct libinput* li = tools_open_backend(BACKEND_UDEV, "seat0", false, &grab);
    if (!li) {
        return nullptr;
    }

    struct libinput_event *ev = nullptr;
    struct libinput_device *dev;
    struct udev_device *udev_device;    //设备管理器
    const char *devnode;

    //事件调度函数
    libinput_dispatch(li);
    int dev_count = 0;
    while ((ev = libinput_get_event(li))) {  //通过li获取事件
        dev = libinput_event_get_device(ev); //通过事件获取设备。
        qDebug()<<"All device name=#####"<<libinput_device_get_name(dev);
        if (libinput_event_get_type(ev) == LIBINPUT_EVENT_DEVICE_ADDED){    //判断是否为新增事件
            libinput_event_destroy(ev);
            udev_device = libinput_device_get_udev_device(dev);
            devnode = udev_device_get_devnode(udev_device);

            if (DEV_POINTER == dev_cap) {
                if (0 != libinput_device_has_capability(dev, LIBINPUT_DEVICE_CAP_POINTER)) {  //判断设备是否为鼠标
                    dev_nodes[dev_count] = (char *)malloc(DEV_NODE_LEN);
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

    //没有获取到设备，free资源
    if (dev_nodes[0] == nullptr) {
        uos_free_dev_node(dev_nodes);
    }
    return dev_nodes;
}

struct libinput* Widget::tools_open_backend(enum tools_backend which,
                                                       const char *path,
                                                       bool verbose, bool *grab){

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

struct libinput * Widget::tools_open_device(const char *path, bool verbose, bool *grab)
{
    Q_UNUSED(verbose);
    struct libinput_device *device;
    struct libinput *li;

    //创建新的libinput上下文，可以添加删除设备
    li = libinput_path_create_context(&interface, grab);
    if (!li) {
        fprintf(stderr, "Failed to initialize context from %s\n", path);
        return nullptr;
    }

    //将检索到的设备添加到li
    device = libinput_path_add_device(li, path);
    if (!device) {
        fprintf(stderr, "Failed to initialized device %s\n", path);
        libinput_unref(li);
        li = nullptr;
    }
    libinput_device_ref(device);
    return li;
}

struct libinput * Widget::tools_open_udev(const char *path, bool verbose, bool *grab)
{
    Q_UNUSED(verbose);
    struct libinput *li;
    struct udev *udev = udev_new();

    if (!udev) {
        fprintf(stderr, "Failed to initialize udev\n");
        return nullptr;
    }

    //通过udev创建libinput上下文
    li = libinput_udev_create_context(&interface, grab, udev);
    if (!li) {
        fprintf(stderr, "Failed to initialize context from udev\n");
        goto out;
    }

    //给上下文分配seat，当执行libinput_dispatch时，
    //新增的设备和已经删除的设备可以通过event获取。
    if (libinput_udev_assign_seat(li, path)) {
        fprintf(stderr, "Failed to set seat\n");
        libinput_unref(li);
        li = nullptr;
        goto out;
    }

out:
    udev_unref(udev);
    return li;
}

int Widget::open_restricted(const char *path, int flags, void *user_data)
{
    bool *grab = (bool *)(user_data);
    int fd = open(path, flags);

    if (fd < 0)
        fprintf(stderr, "Failed to open %s (%s)\n",
            path, strerror(errno));
    else if (grab && *grab && ioctl(fd, EVIOCGRAB, (void*)1) == -1)
        fprintf(stderr, "Grab requested, but failed for %s (%s)\n",
            path, strerror(errno));

    return fd < 0 ? -errno : fd;
}

void Widget::close_restricted(int fd, void *user_data)
{
    Q_UNUSED(user_data);
    //close(fd);
}

void Widget::uos_free_dev_node(char** dev_nodes)
{
    if (dev_nodes != nullptr) {
        for (int i = 0; i < MAX_DEV_NUM; ++i) {
            if (dev_nodes[i] != nullptr) {
                 free(dev_nodes[i]);
                 dev_nodes[i] = nullptr;
            }
        }
        free(dev_nodes);
        dev_nodes = nullptr;
    }
}

struct libinput_device* Widget::uos_find_device(char *device_node, struct libinput* li)
{
    struct libinput_device *dev = nullptr;
    if (li) {
        struct libinput_event *ev = nullptr;
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

struct libinput* Widget::uos_create_libiput_device_interface(char* device_node)
{
    bool grab = false;
    return tools_open_backend(BACKEND_DEVICE, device_node, false, &grab);
}

void Widget::SetLeftHanded(int enable){
    int rst=0;
    int i=0;

    for(i=0;mDevPointer[i]!=nullptr;i++){
        rst = (libinput_device_config_left_handed_set(mDevPointer[i], enable));
        if(rst == LIBINPUT_CONFIG_STATUS_SUCCESS){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " SetLeftHanded : Sucess";
        }else if(rst == LIBINPUT_CONFIG_STATUS_UNSUPPORTED){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << "SetLeftHanded : Unsupport";
        }else if(rst == LIBINPUT_CONFIG_STATUS_INVALID){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) <<  "SetLeftHanded : Unknow error";
        }
    }
}

int Widget::GetLeftHanded(){
    int rst = 0;
    int i=0;

    for(i=0;mDevPointer[i]!=nullptr;i++){
        rst = (libinput_device_config_left_handed_get(mDevPointer[i]));
        if(rst == 0){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " Pointer Mode : Right Mode";
        }else{
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " Get Pointer Mode : Left Mode";
        }
    }
    return rst;
}

void Widget::SetAccelSpeed(double speed){
    int rst=0;
    int i=0;

    if(speed < -1.0 || speed > 1.0)
    {
        qDebug()<<"Invalid number,range -1.0 to 1.0";
        return;
    }
    for(i=0;mDevPointer[i]!=nullptr;i++){
        rst = libinput_device_config_accel_set_speed(mDevPointer[i], speed);
        if(rst == LIBINPUT_CONFIG_STATUS_SUCCESS){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " SetAccelSpeed : Sucess";
        }else if(rst == LIBINPUT_CONFIG_STATUS_UNSUPPORTED){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " SetAccelSpeed : Unsupport";
        }else if(rst == LIBINPUT_CONFIG_STATUS_INVALID){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " SetAccelSpeed : Unknow error";
        }
    }
}

double Widget::GetAccelSpeed(){
    double rst=0.0;
    int i=0;

    for(i=0;mDevPointer[i]!=nullptr;i++){
            rst = libinput_device_config_accel_get_speed(mDevPointer[i]);
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " current accel speed : " << rst;
    }
    return rst;
}


void Widget::SetNaturalScrollEnable(int enable){
    int rst = 0;
    int i;

    for(i=0;mDevPointer[i]!=nullptr;i++){
        rst = libinput_device_config_scroll_set_natural_scroll_enabled(mDevPointer[i], enable);
        if(rst == LIBINPUT_CONFIG_STATUS_SUCCESS){
            qDebug() << libinput_device_get_name(mDevPointer[i]) << " SetNaturalScrollEnable : Sucess";
        }else if(rst == LIBINPUT_CONFIG_STATUS_UNSUPPORTED){
            qDebug() << libinput_device_get_name(mDevPointer[i]) << " SetNaturalScrollEnable : Unsupport";
        }else if(rst == LIBINPUT_CONFIG_STATUS_INVALID){
            qDebug() << libinput_device_get_name(mDevPointer[i]) << " SetNaturalScrollEnable : Unknow error";
        }
     }
}

int Widget::GetNaturalScrollEnable(){
    int rst=0;
    int i;

    for(i=0;mDevPointer[i]!=nullptr;i++){
        rst = libinput_device_config_scroll_get_natural_scroll_enabled(mDevPointer[i]);
        if (rst == 0){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " Natural Scrolling : Disabled";
        }else {
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " Natural Scrolling : Enabled";
        }

    }
    return rst;
}

void Widget::SetPointerAcceleEnable(int enable){
    int rst = 0;
    int i;

    for(i=0;mDevPointer[i]!=nullptr;i++){
        if(enable == 0){
            rst = libinput_device_config_accel_set_profile(mDevPointer[i], LIBINPUT_CONFIG_ACCEL_PROFILE_NONE);
        } else{
            rst = libinput_device_config_accel_set_profile(mDevPointer[i], LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT);
        }

        if(rst == LIBINPUT_CONFIG_STATUS_SUCCESS){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " SetPointerAcceleEnable : Sucess";
        }else if(rst == LIBINPUT_CONFIG_STATUS_UNSUPPORTED){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " SetPointerAcceleEnable : Unsupport";
        }else if(rst == LIBINPUT_CONFIG_STATUS_INVALID){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " SetPointerAcceleEnable : Unknow error";
        }
    }
}

int Widget::GetPointerAcceleEnable(){
    int rst = 0;
    int i;

    for(i=0;mDevPointer[i]!=nullptr;i++){
        rst = libinput_device_config_accel_get_profile(mDevPointer[i]);

        if(rst == LIBINPUT_CONFIG_ACCEL_PROFILE_NONE){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " Pointer Status : LIBINPUT_CONFIG_ACCEL_PROFILE_NONE";
        }else if(rst == LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " Pointer Status : LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT";
        }else if(rst == LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE){
            qDebug()<< libinput_device_get_name(mDevPointer[i]) << " Pointer Status : LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE";
        }
    }

    return rst;
}

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QString>
#include <QPushButton>

#include <libudev.h>
#include <libinput.h>

#define MAX_DEV_NUM 100
#define DEV_NODE_LEN  64


typedef enum _device_capability {
        DEV_ALL = 0,
        DEV_KEYBOARD,
        DEV_POINTER,
        DEV_TOUCH,
        DEV_TABLET_TOOL,
        DEV_TABLET_PAD,
        DEV_GETSTURE,
        DEV_SWITCH
}device_capability;

enum tools_backend {
    BACKEND_DEVICE,
    BACKEND_UDEV
};



const QString POINTER_PATH = "/com/deepin/daemon/KWayland/Pointer";


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private:
    struct libinput_device *mDevPointer[MAX_DEV_NUM];
    char **mDevNodes;
    struct libinput *mli[100];

    QPushButton *setleft;
    QPushButton *setright;
    QPushButton *btntest;
private slots:
    void test();
    void testright();
    void testleft();

public:
    void Init();


    //静态函数，定义libinput接口
    static int open_restricted(const char *path, int flags, void *user_data);
    static void close_restricted(int fd, void *user_data);

private:
    //init function
    struct libinput* uos_create_libiput_device_interface(char* device_node);
    //find device
    struct libinput_device* uos_find_device(char *device_node, struct libinput* li);
    char** uos_list_device(device_capability dev_cap);
    void uos_free_dev_node(char** dev_nodes);

    struct libinput* tools_open_backend(enum tools_backend which,
                        const char *path,
                        bool verbose,
                        bool *grab);

    struct libinput * tools_open_device(const char *path, bool verbose, bool *grab);
    struct libinput * tools_open_udev(const char *path, bool verbose, bool *grab);

public Q_SLOTS:
    //左手模式
    void SetLeftHanded(int enable);
    int  GetLeftHanded();

    //鼠标移动速度
    void SetAccelSpeed(double speed);
    double GetAccelSpeed();

    //鼠标自然滚动
    void SetNaturalScrollEnable(int enable);
    int  GetNaturalScrollEnable();

    //鼠标加速
    void SetPointerAcceleEnable(int enable);
    int GetPointerAcceleEnable();

};

const struct libinput_interface interface = {
    .open_restricted = Widget::open_restricted,
    .close_restricted = Widget::close_restricted,

};

#endif // WIDGET_H

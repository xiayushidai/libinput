#ifndef INPUT_DEVICE_INTERFACE_H
#define INPUT_DEVICE_INTERFACE_H

#ifdef  __cplusplus
extern "C" {
#endif

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


/***************************************************************
    FunctionName: uos_create_libiput_device_interface
    Purpose: 创建libinput接口对象
    Parameter:	device_node：设备节点名称

    Return:	libinput接口对象
    Remark:
****************************************************************/
struct libinput* uos_create_libiput_device_interface(char* device_node);

/***************************************************************
    FunctionName: uos_find_device
    Purpose: 查找设备对象
    Parameter:	device_node：设备节点名称
                li： libinput接口对象
    Return:	设备对象
    Remark:
****************************************************************/
struct libinput_device* uos_find_device(char *device_node, struct libinput* li);

/***************************************************************
    FunctionName: uos_drain_event
    Purpose:  清空设备时间列表
    Parameter:	li：libinput接口对象

    Return:
    Remark:
****************************************************************/
void uos_drain_event(struct libinput* li);

/***************************************************************
    FunctionName: list_device
    Purpose:  列出指定设备类型的节点
    Parameter:	dev_cap：设备类型

    Return:	设备节点数组
    Remark: 当dev_cap为DEV_ALL表示获取所有设备节点，否则只返回指定类型的设备节点。
            在所有接口调用之前都必须先调用该方法获取设备节点
****************************************************************/
char** uos_list_device(device_capability dev_cap);

void uos_free_dev_node(char** dev_nodes);

/***************************************************************
    FunctionName: set_left_handed
    Purpose: 设置为左手模式
    Parameter:
            device_node：设备节点名称
            left_handed：0：关闭 1：开启

    Return:	0：设置成功 1：设备不支持 2：设置失败
    Remark: libinput_device_config_left_handed_is_available  libinput_device_config_left_handed_set
****************************************************************/
int uos_set_left_handed(struct libinput_device *dev, int left_handed);


/***************************************************************
    FunctionName: left_handed_get
    Purpose: 获取当前左手模式状态
    Parameter:	device_node：设备节点名称

    Return:	0：右手模式 1：左手模式
    Remark: libinput_device_config_left_handed_get
****************************************************************/
int  uos_get_left_handed(struct libinput_device *dev);


/***************************************************************
    FunctionName: set_dwt_enable
    Purpose: 当启用时，设备将在输入时和之后的短时间内禁用
    Parameter:	device_node:设备节点名称
                enable：0：关闭 1：开启
    Return:	0：设置成功 1：设备不支持 2：设置失败
    Remark:  libinput_device_config_dwt_is_available  libinput_device_config_dwt_set_enabled()
****************************************************************/

int uos_set_dwt_enable(struct libinput_device *dev, int enable);


/***************************************************************
    FunctionName: dwt_get_enable
    Purpose:  检查当前设备上是否启用了输入时禁用
    Parameter:	device_node：设备节点名称

    Return:	0：关闭 非0：开启
    Remark:
****************************************************************/
int uos_get_dwt_enable(struct libinput_device *dev);


/***************************************************************
    FunctionName: set_accel_speed
    Purpose:  设置鼠标/触摸板的指针加速度；
    Parameter:	device_node:设备节点名称
                speed：	设置在[- 1,1]范围内，其中0为该设备的默认加速度，-1为该设备上可用的最慢加速度，1为该设备上可用的最大加速度。
    Return:	 0：设置成功 ：设备不支持 2：设置失败
    Remark:  libinput_device_config_accel_is_available libinput_device_config_accel_set_speed()
****************************************************************/
int uos_set_accel_speed(struct libinput_device *dev, double speed);


/***************************************************************
    FunctionName: get_accel_speed
    Purpose:  获取设置鼠标/触摸板设备的当前指针加速设置
    Parameter:	device_node：设备节点名称

    Return:	 当前速度，范围-1到1
    Remark:  libinput_device_config_accel_get_speed
****************************************************************/
double uos_get_accel_speed(struct libinput_device *dev);


/***************************************************************
    FunctionName: set_event_enable
    Purpose:  启动/禁用设备事件上报
    Parameter:	device_node：设备节点名称
                enable: 0:启用 1：关闭
    Return:	 0：设置成功 1：设备不支持 2：设置失败
    Remark:   libinput_device_config_send_events_set_mode()
****************************************************************/
int uos_set_event_enable(struct libinput_device *dev, int enable);


/***************************************************************
    FunctionName: set_natural_scroll_enable
    Purpose: 设置自然滚动状态
    Parameter:	device_node:设备节点名称
                enable：0：关闭 1：开启
    Return:	  0：设置成功 1：设备不支持 2：设置失败
    Remark:  libinput_device_config_scroll_has_natural_scroll libinput_device_config_scroll_set_natural_scroll_enabled()
****************************************************************/
int uos_set_natural_scroll_enable(struct libinput_device *dev, int enable);

/***************************************************************
    FunctionName: set_natural_scroll_enable
    Purpose: 	获取自然滚动状态
    Parameter:	device_node：设备节点名称

    Return:	 0：关闭 1：开启
    Remark:  libinput_device_config_scroll_get_natural_scroll_enabled
****************************************************************/
int uos_get_natural_scroll_enabled(struct libinput_device *dev);


/***************************************************************
    FunctionName: set_tap_enable
    Purpose:  启用点击操作
    Parameter:	device_node：设备节点名称
                enable: 0:关闭 1：开启
    Return:	  0：设置成功 1：设备不支持 2：设置失败
    Remark:   libinput_device_config_tap_set_enabled()
****************************************************************/
int uos_set_tap_enable(struct libinput_device *dev, int enable);



/***************************************************************
    FunctionName: set_natural_scroll_enable
    Purpose: 	获取自然滚动状态
    Parameter:	device_node：设备节点名称

    Return:	 0：关闭 非0：开启
    Remark:  libinput_device_config_tap_get_enabled()
****************************************************************/
int uos_get_tap_enable(struct libinput_device *dev);

#ifdef  __cplusplus
}
#endif


#endif//INPUT_DEVICE_INTERFACE_H


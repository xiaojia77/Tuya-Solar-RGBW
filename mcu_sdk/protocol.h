/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2017, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: protocol.h
**描        述: 下发/上报数据处理函数
**使 用 说 明 :

                  *******非常重要，一定要看哦！！！********

** 1、用户在此文件中实现数据下发/上报功能
** 2、DP的ID/TYPE及数据处理函数都需要用户按照实际定义实现
** 3、当开始某些宏定义后需要用户实现代码的函数内部有#err提示,完成函数后请删除该#err
**
**--------------当前版本修订---------------------------------------------------
** 版  本: v1.0
** 日　期: 2017年5月3日
** 描　述: 1:创建涂鸦bluetooth对接MCU_SDK
**
**-----------------------------------------------------------------------------
******************************************************************************/
#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_



/******************************************************************************
                            用户相关信息配置
******************************************************************************/
/******************************************************************************
                            1:修改产品信息                
******************************************************************************/
#define PRODUCT_KEY "epcfhcyq"    //开发平台创建产品后生成的16位字符产品唯一标识


#define MCU_VER "1.0.0"                                 //用户的软件版本,用于MCU固件升级,MCU升级版本需修改
/******************************************************************************
                          2:MCU是否需要支固件升级                  
如需要支持MCU固件升级,请开启该宏
MCU可调用mcu_api.c文件内的mcu_firm_update_query()函数获取当前MCU固件更新情况
                        ********WARNING!!!**********
当前接收缓冲区为关闭固件更新功能的大小,固件升级包为256字节
如需要开启该功能,串口接收缓冲区会变大
******************************************************************************/
//#define         SUPPORT_MCU_FIRM_UPDATE                 //开启MCU固件升级功能(默认关闭)
/******************************************************************************
                         3:定义收发缓存:
                    如当前使用MCU的RAM不够,可修改为24
******************************************************************************/
#ifndef SUPPORT_MCU_FIRM_UPDATE
#define BT_UART_QUEUE_LMT             16 *10             //数据接收队列大小,如MCU的RAM不够,可缩小
#define BT_UART_RECV_BUF_LMT          24 *10             //根据用户DP数据大小量定,必须大于24
#else
#define BT_UART_QUEUE_LMT             128             //数据接收队列大小,如MCU的RAM不够,可缩小
#define BT_UART_RECV_BUF_LMT          300             //固件升级缓冲区,需大缓存,必须大于260
#endif

#define BT_UART_SEND_BUF_LMT         24              //根据用户DP数据大小量定,必须大于24
/******************************************************************************
                        4:定义模块工作方式
模块自处理:
          bt指示灯和bt复位按钮接在bt模块上(开启BT_CONTROL_SELF_MODE宏)
          并正确定义BT_STATE_KEY和BT_RESET_KEY
MCU自处理:
          bt指示灯和bt复位按钮接在MCU上(关闭BT_CONTROL_SELF_MODE宏)
          MCU在需要处理复位bt的地方调用mcu_api.c文件内的mcu_reset_bt()函数,并可调用mcu_get_reset_bt_flag()函数返回复位bt结果
          或调用设置bt模式mcu_api.c文件内的mcu_set_bt_mode(BT_CONFIG_E mode)函数,并可调用mcu_get_bt_work_state()函数返回设置bt结果
******************************************************************************/
//#define         BT_CONTROL_SELF_MODE                       //bt自处理按键及LED指示灯;如为MCU外界按键/LED指示灯请关闭该宏
#ifdef          BT_CONTROL_SELF_MODE                      //模块自处理
  #define     BT_STATE_KEY            14                    //bt模块状态指示按键，请根据实际GPIO管脚设置
  #define     BT_RESERT_KEY           0                     //bt模块重置按键，请根据实际GPIO管脚设置
#endif

/******************************************************************************
                      5:MCU是否需要支持校时功能                     
如需要请开启该宏,并在Protocol.c文件内mcu_write_rtctime实现代码
mcu_write_rtctime内部有#err提示,完成函数后请删除该#err
mcu在bt模块正确联网后可调用mcu_get_system_time()函数发起校时功能
******************************************************************************/
#define         SUPPORT_MCU_RTC_CHECK                //开启校时功能

/******************************************************************************
                        1:dp数据点序列号重新定义
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/
//开关(可下发可上报)
//备注:
#define DPID_SWITCH_LED 1
//模式(可下发可上报)
//备注:
#define DPID_WORK_MODE 2
//亮度值(可下发可上报)
//备注:
#define DPID_BRIGHT_VALUE 3
//冷暖值(可下发可上报)
//备注:
#define DPID_TEMP_VALUE 4
//彩光(可下发可上报)
//备注:
#define DPID_COLOUR_DATA 5
//场景(可下发可上报)
//备注:
#define DPID_SCENE_DATA 6
//倒计时(可下发可上报)
//备注:
#define DPID_COUNTDOWN 7
//音乐律动(只下发)
//备注:
#define DPID_MUSIC_DATA 8
//定时(可下发可上报)
//备注:定时开关，定时切换场景颜色功能
#define DPID_TIMER 9
//Rtc定时(可下发可上报)
//备注:
#define DPID_RTC_TIMER 37
//定时状态上报(只上报)
//备注:用于查询定时任务状态
#define DPID_TIMER_REPORT 38
//节能开关(可下发可上报)
//备注:节能模式开关
#define DPID_SWITCH_SAVE_ENERGY 51
//电池电量(可下发可上报)
//备注:剩余电池电量
#define DPID_BATTERY_PERCENTAGE 52
//设备模式(可下发可上报)
//备注:
#define DPID_DEVICE_MODE 53
//电池电量状态(只上报)
//备注:
#define DPID_BATTERY_STATE 62
//组内光控同步(可下发可上报)
//备注:
#define DPID_BRIGHTGROUP_SYNC 63



/*****************************************************************************
函数名称 : all_data_update
功能描述 : 系统所有dp点信息上传
输入参数 : 无
返回参数 : 无
使用说明 : MCU必须实现该函数内数据上报功能
*****************************************************************************/
void all_data_update(void);

#ifdef SUPPORT_MCU_FIRM_UPDATE
/*****************************************************************************
函数名称 : mcu_firm_update_handle
功能描述 : MCU进入固件升级模式
输入参数 : value:固件缓冲区
           position:当前数据包在于固件位置
           length:当前固件包长度(固件包长度为0时,表示固件包发送完成)
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length);
#endif

/*****************************************************************************
函数名称 : dp_download_handle
功能描述 : dp下发处理函数
输入参数 : dpid:DP序号
value:dp数据缓冲区地址
length:dp数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERRO
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length);
/*****************************************************************************
函数名称 : get_download_cmd_total
功能描述 : 获取所有dp命令总和
输入参数 : 无
返回参数 : 下发命令总和
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char get_download_cmd_total(void);

void Get_Time_info(void);

#endif


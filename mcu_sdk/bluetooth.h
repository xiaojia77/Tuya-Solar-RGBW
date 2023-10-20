/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2017, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: bluetooth.h
**描        述: bluetooth文件头定义
**使 用 说 明 : 用户无需关心该文件实现内容
**
**
**--------------当前版本修订---------------------------------------------------
** 版  本: v1.0
** 日　期: 2017年5月3日
** 描　述: 1:创建涂鸦bluetooth对接MCU_SDK
**
**-----------------------------------------------------------------------------
******************************************************************************/
#ifndef __BLUETOOTH_H_
#define __BLUETOOTH_H_

//#include "py32f0xx.h"
//=============================================================================
/*定义常量*/
//=============================================================================
#ifndef TRUE
#define         TRUE                1
#endif
//
#ifndef FALSE
#define         FALSE               0
#endif
//
#ifndef NULL
#define         NULL                ((void *) 0)
#endif

#ifndef SUCCESS
#define         BL_SUCCESS             1
#endif

#ifndef ERROR
#define         BL_ERROR               0
#endif

#ifndef INVALID
#define         BL_INVALID             0xFF
#endif

#ifndef ENABLE
#define         BL_ENABLE                1
#endif
//
#ifndef DISABLE
#define         BL_DISABLE               0
#endif
//=============================================================================
//dp数据点类型
//=============================================================================
#define         DP_TYPE_RAW                     0x00				//RAW型
#define         DP_TYPE_BOOL                    0x01	                        //布尔型
#define         DP_TYPE_VALUE                   0x02	                        //数值型
#define         DP_TYPE_STRING                  0x03				//字符串型
#define         DP_TYPE_ENUM                    0x04				//枚举型
#define         DP_TYPE_FAULT                   0x05				//故障型

//=============================================================================
//BT工作状态
//=============================================================================
#define         BT_UN_BIND                      0x00                            //蓝牙未绑定
#define         BT_NOT_CONNECTED                0x01                            //蓝牙已绑定未连接
#define         BT_CONNECTED                    0x02                            //蓝牙已绑定并已连接
#define         BT_SATE_UNKNOW                  0xff

//=============================================================================
//bt复位状态
//=============================================================================
#define         RESET_BT_ERROR                  0
#define         RESET_BT_SUCCESS                1

//=============================================================================
//MCU固件升级状态
//=============================================================================
#define         FIRM_STATE_UN_SUPPORT           0x00                            //不支持 MCU 升级
#define         FIRM_STATE_BT_UN_READY          0x01                            //模块未就绪
#define         FIRM_STATE_GET_ERROR            0x02                            //云端升级信息查询失败
#define         FIRM_STATE_NO                   0x03                            //无需升级（云端无更新版本）
#define         FIRM_STATE_START                0x04                            //需升级，等待模块发起升级操作

//=============================================================================
//BT和mcu的工作方式 
//=============================================================================
#define         UNION_WORK                      0x0                             //mcu模块与bt配合处理
#define         BT_ALONE                      0x1                             //bt模块自处理

//=============================================================================
//系统工作模式
//=============================================================================
#define         NORMAL_MODE             0x00                                    //正常工作状态
#define         FACTORY_MODE            0x01                                    //工厂模式	
#define         UPDATE_MODE             0X02                                    //升级模式	     

#include "protocol.h"
#include "system.h"
#include "mcu_api.h"


//=============================================================================
//下发命令
//=============================================================================
typedef struct {
  unsigned char dp_id;                        //dp序号
  unsigned char dp_type;                          //dp类型
} DOWNLOAD_CMD_S;

extern void enable_low_power(void);
extern void disable_low_power(void);

#endif

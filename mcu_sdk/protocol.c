/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2017, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: protocol.c
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


#include "bluetooth.h"
#include <stdlib.h>
#include "usart.h"
#include "bluetooth.h"
#include "led.h"
  
//extern TYPE_BUFFER_S FlashBuffer;

/******************************************************************************
                                移植须知:
1:MCU必须在while中直接调用mcu_api.c内的bt_uart_service()函数
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢失
3:请勿在中断/定时器中断内调用上报函数
******************************************************************************/

         
/******************************************************************************
                              第一步:初始化
1:在需要使用到bt相关文件的文件中include "bt.h"
2:在MCU初始化中调用mcu_api.c文件中的bt_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数内,并删除#error
4:在MCU串口接收函数中调用mcu_api.c文件内的uart_receive_input函数,并将接收到的字节作为参数传入
5:单片机进入while循环后调用mcu_api.c文件内的bt_uart_service()函数
******************************************************************************/

/******************************************************************************
                        1:dp数据点序列类型对照表
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/
const DOWNLOAD_CMD_S download_cmd[] =
{
  {DPID_SWITCH_LED, DP_TYPE_BOOL},
  {DPID_WORK_MODE, DP_TYPE_ENUM},
  {DPID_BRIGHT_VALUE, DP_TYPE_VALUE},
  {DPID_TEMP_VALUE, DP_TYPE_VALUE},
  {DPID_COLOUR_DATA, DP_TYPE_STRING},
  {DPID_SCENE_DATA, DP_TYPE_STRING},
  {DPID_COUNTDOWN, DP_TYPE_VALUE},
  {DPID_MUSIC_DATA, DP_TYPE_STRING},
  {DPID_TIMER, DP_TYPE_RAW},
  {DPID_RTC_TIMER, DP_TYPE_RAW},
  {DPID_TIMER_REPORT, DP_TYPE_RAW},
  {DPID_SWITCH_SAVE_ENERGY, DP_TYPE_BOOL},
  {DPID_BATTERY_PERCENTAGE, DP_TYPE_VALUE},
  {DPID_DEVICE_MODE, DP_TYPE_ENUM},
  {DPID_BATTERY_STATE, DP_TYPE_ENUM},
  {DPID_BRIGHTGROUP_SYNC, DP_TYPE_BOOL},
};
/******************************************************************************
                           2:串口单字节发送函数
请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
******************************************************************************/
/*****************************************************************************
函数名称 : uart_transmit_output
功能描述 : 发数据处理
输入参数 : value:串口收到字节数据
返回参数 : 无
使用说明 : 请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
*****************************************************************************/
void uart_transmit_output(unsigned char value)
{
	LL_USART_TransmitData8(DEBUG_USART, value);
	while (!LL_USART_IsActiveFlag_TC(DEBUG_USART));
	LL_USART_ClearFlag_TC(DEBUG_USART);
}
/******************************************************************************
                           第二步:实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处理
当前函数处理全部数据上报(包括可下发/可上报和只上报)
  需要用户按照实际情况实现:
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上报
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上报
******************************************************************************/

//自动化生成数据上报函数
/*****************************************************************************
函数名称 : all_data_update
功能描述 : 系统所有dp点信息上传,实现APP和muc数据同步
输入参数 : 无
返回参数 : 无
使用说明 : 此函数SDK内部需调用;
           MCU必须实现该函数内数据上报功能;包括只上报和可上报可下发型数据
*****************************************************************************/
void all_data_update(void)
{
 // #error "请在此处理可下发可上报数据及只上报数据示例,处理完成后删除该行"
  //此代码为平台自动生成，请按照实际数据修改每个可下发可上报函数和只上报函数
/*    mcu_dp_bool_update(DPID_SWITCH_LED,当前开关); //BOOL型数据上报;
    mcu_dp_enum_update(DPID_WORK_MODE,当前模式); //枚举型数据上报;
    mcu_dp_value_update(DPID_BRIGHT_VALUE,当前亮度值); //VALUE型数据上报;
    mcu_dp_value_update(DPID_TEMP_VALUE,当前冷暖值); //VALUE型数据上报;
    mcu_dp_string_update(DPID_COLOUR_DATA,当前彩光指针,当前彩光数据长度); //STRING型数据上报;
    mcu_dp_string_update(DPID_SCENE_DATA,当前场景指针,当前场景数据长度); //STRING型数据上报;
    mcu_dp_value_update(DPID_COUNTDOWN,当前倒计时); //VALUE型数据上报;
    mcu_dp_raw_update(DPID_TIMER,当前定时指针,当前定时数据长度); //RAW型数据上报;
    mcu_dp_raw_update(DPID_RTC_TIMER,当前Rtc定时指针,当前Rtc定时数据长度); //RAW型数据上报;
    mcu_dp_raw_update(DPID_TIMER_REPORT,当前定时状态上报指针,当前定时状态上报数据长度); //RAW型数据上报;
    mcu_dp_bool_update(DPID_SWITCH_SAVE_ENERGY,当前节能开关); //BOOL型数据上报;
    mcu_dp_value_update(DPID_BATTERY_PERCENTAGE,当前电池电量); //VALUE型数据上报;
    mcu_dp_enum_update(DPID_DEVICE_MODE,当前设备模式); //枚举型数据上报;
    mcu_dp_enum_update(DPID_BATTERY_STATE,当前电池电量状态); //枚举型数据上报;
    mcu_dp_bool_update(DPID_BRIGHTGROUP_SYNC,当前组内光控同步); //BOOL型数据上报;*/
	
	mcu_dp_bool_update(DPID_SWITCH_LED,RGB.OnFlag);
	mcu_dp_enum_update(DPID_WORK_MODE,TY.WorkMode);
	mcu_dp_value_update(DPID_BRIGHT_VALUE,TY.bright);
	mcu_dp_value_update(DPID_TEMP_VALUE,TY.TempValue); //VALUE型数据上报;
	mcu_dp_string_update(DPID_COLOUR_DATA,TY.ColourData,12);  //STRING型数据上报;
	mcu_dp_string_update(DPID_SCENE_DATA,TY.SceneData,2);   //STRING型数据上报;
//  mcu_dp_value_update(DPID_COUNTDOWN,当前倒计时); //VALUE型数据上报;
//  mcu_dp_raw_update(DPID_TIMER,当前定时指针,当前定时数据长度); //RAW型数据上报;
//  mcu_dp_raw_update(DPID_RTC_TIMER,当前Rtc定时指针,当前Rtc定时数据长度); //RAW型数据上报;
//  mcu_dp_raw_update(DPID_TIMER_REPORT,当前定时状态上报指针,当前定时状态上报数据长度); //RAW型数据上报
	mcu_dp_bool_update(DPID_SWITCH_SAVE_ENERGY,Bat.SaveEnergMode);
	mcu_dp_value_update(DPID_BATTERY_PERCENTAGE,Bat.Percent);
	mcu_dp_enum_update(DPID_DEVICE_MODE,Bat.SolarMode);
	mcu_dp_enum_update(DPID_BATTERY_STATE,Bat.Gera/2);
	mcu_dp_bool_update(DPID_BRIGHTGROUP_SYNC,TY.GroupSync); 
	

}
/******************************************************************************
                                WARNING!!!    
                            2:所有数据上报处理
自动化代码模板函数,具体请用户自行实现数据处理
******************************************************************************/

/*****************************************************************************
函数名称 : dp_download_switch_led_handle
功能描述 : 针对DPID_SWITCH_LED的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_switch_led_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为BOOL
    unsigned char ret;
    //0:off/1:on
    unsigned char switch_led = 0;
    
    switch_led = mcu_get_dp_download_bool(value,length);
    if(switch_led == 0 ) 
	{
        //bool off
		LED_RGB_Off_Handle();
		RGB.Command = TY_OFF_MODE;
		DEBUG_INFO("LEDOFF");
    }
	else 
	{
        //bool on
		LED_RGB_On_Handle();
		RGB.Command = TY_ON_MODE;
		DEBUG_INFO("LEDON");
    }
  
    //There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_SWITCH_LED,switch_led);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_work_mode_handle
功能描述 : 针对DPID_WORK_MODE的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_work_mode_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为ENUM
    unsigned char ret;
    unsigned char work_mode;
    
    work_mode = mcu_get_dp_download_enum(value,length);
	TY.WorkMode = work_mode ;
    switch(work_mode) {
        case 0:
        break;
        
        case 1:
        break;
        
        case 2:
        break;
        
        case 3:
        break;
        
        default:
    
        break;
    }
    
    //There should be a report after processing the DP
	
    ret = mcu_dp_enum_update(DPID_WORK_MODE, work_mode);

    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_bright_value_handle
功能描述 : 针对DPID_BRIGHT_VALUE的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_bright_value_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned int bright_value;
    
	TY.bright = mcu_get_dp_download_value(value,length);
    bright_value = 100 + ( TY.bright * 900 / 1000 ); 
	
	LED_RGB_To_OnStatus();
	LED_RGB_SetHSV(0,0,bright_value);

    //There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_BRIGHT_VALUE,TY.bright);
	
	RGB.Command = TY_bright_MODE;
	DEBUG_INFO("bright_value %d",bright_value);
	
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_temp_value_handle
功能描述 : 针对DPID_TEMP_VALUE的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_temp_value_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned long temp_value;
    
    temp_value = mcu_get_dp_download_value(value,length);
    TY.TempValue = temp_value;
	LED_RGB_To_OnStatus();
	LED_RGB_SetHSV(0,0,RGB.vTemp);	
	
    //There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_TEMP_VALUE,temp_value);
	RGB.Command = TY_temp_MODE;
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_colour_data_handle
功能描述 : 针对DPID_COLOUR_DATA的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_colour_data_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为STRING
    unsigned char i,ret;
	uint16_t h,s,v;
  
    char string_data[13];
    
	//打印得到的数据
	for(i=0;i<12;i++)
	{
		string_data[i] = value[i]; 
		TY.ColourData[i] = value[i]; 
	}
    string_data[12] = 0;
	DEBUG_INFO("%s ",string_data);
	
	string_data[0] = '0';
    string_data[1] = 'x';
    string_data[2] = value[0] ;
    string_data[3] = value[1] ;
    string_data[4] = value[2] ;
    string_data[5] = value[3] ;
    string_data[6] = 0 ;
	h  =  strtol(string_data, NULL, 16);
	
    string_data[2] = value[4] ;
    string_data[3] = value[5] ;
    string_data[4] = value[6] ;
    string_data[5] = value[7] ;
    string_data[6] = 0 ;
	s  =  strtol(string_data, NULL, 16);
	
    string_data[2] = value[8] ;
    string_data[3] = value[9] ;
    string_data[4] = value[10] ;
    string_data[5] = value[11] ;
    string_data[6] = 0 ;
	
	TY.bright = strtol(string_data, NULL, 16);
	v  = 100 + ( TY.bright * 900 / 1000);
	
	LED_RGB_To_OnStatus();
	LED_RGB_SetHSV(h,s,v);	
	

    //There should be a report after processing the DP
    ret = mcu_dp_string_update(DPID_COLOUR_DATA,value, length);

	RGB.Command = TY_colour_MODE;
	DEBUG_INFO("%d %d %d",h,s,v);
	DEBUG_INFO("%d %d %d",RGB.Rvalue,RGB.Gvalue,RGB.Bvalue);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_scene_data_handle
功能描述 : 针对DPID_SCENE_DATA的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_scene_data_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为STRING
    unsigned char ret;
    
    //STRING type data processing
    char string_data[3];
	
	uint16_t scence;
    
    string_data[0] = value[0];
    string_data[1] = value[1];
    string_data[2] = 0;

	TY.SceneData [0] = value[0];
	TY.SceneData [1] = value[1];
   
	
	scence = strtol(string_data,NULL, 10);
	
	LED_RGB_To_OnStatus();
	LED_RGB_SetDisplayHSV(0,0,0);
	switch(scence)
	{
		case 0:
			LED_RGB_SetHSV(58,763,RGB.vTemp);
			break;
		case 3:
			LED_RGB_SetHSV(0,0,RGB.vTemp);
			break;
		case 4:
			LED_RGB_SetHSV(120,1000,RGB.vTemp);
			break;
		case 5:
			LED_RGB_SetHSV(360,670,RGB.vTemp);
			break;
		case 6:
			LED_RGB_SetHSV(277,622,RGB.vTemp);
			break;
		case 7:
			LED_RGB_SetHSV(333,532,RGB.vTemp);
			break;
	}

	RGB.Command = TY_scene_MODE;
    DEBUG_INFO("scence %d ",scence);
    //There should be a report after processing the DP
	 DEBUG_INFO("scence Data %s length %d ",(char *)string_data,length);
    ret = mcu_dp_string_update(DPID_SCENE_DATA,value, length);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_countdown_handle
功能描述 : 针对DPID_COUNTDOWN的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_countdown_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned long countdown;
    
    countdown = mcu_get_dp_download_value(value,length);
    /*
    //VALUE type data processing
    
    */
    
    //There should be a report after processing the DP
	
    ret = mcu_dp_value_update(DPID_COUNTDOWN,countdown);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_music_data_handle
功能描述 : 针对DPID_MUSIC_DATA的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 只下发类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_music_data_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为STRING
      unsigned char i,ret;
	uint16_t h,s,v;
  
    char string_data[13];
    
	//打印得到的数据
	for(i=0;i<12;i++) string_data[i] = value[i]; 
    string_data[12] = 0;
	DEBUG_INFO("%s ",string_data);
	
	string_data[0] = '0';
    string_data[1] = 'x';
    string_data[2] = value[1] ;
    string_data[3] = value[2] ;
    string_data[4] = value[3] ;
    string_data[5] = value[4] ;
    string_data[6] = 0 ;
	h  =  strtol(string_data, NULL, 16);
	
    string_data[2] = value[5] ;
    string_data[3] = value[6] ;
    string_data[4] = value[7] ;
    string_data[5] = value[8] ;
    string_data[6] = 0 ;
	s  =  strtol(string_data, NULL, 16);
	
    string_data[2] = value[9] ;
    string_data[3] = value[10] ;
    string_data[4] = value[11] ;
    string_data[5] = value[12] ;
    string_data[6] = 0 ;
	v  =  strtol(string_data, NULL, 16);
	
	LED_RGB_To_OnStatus();
	
	LED_RGB_SetHSV(h,s,v);

	// 递增值 or 递减值
	//RGB.inc_h = RGB.set_h  - RGB.h / 150;
	//RGB.inc_s = RGB.set_s  - RGB.s / 150;
	//RGB.inc_v = RGB.set_v  - RGB.v / 150;
	
   //There should be a report after processing the DP
    ret = mcu_dp_string_update(DPID_MUSIC_DATA,value, length);

	RGB.Command = TY_MUSIC_MODE;
	DEBUG_INFO("%d %d %d",h,s,v);
	DEBUG_INFO("%d %d %d",RGB.Rvalue,RGB.Gvalue,RGB.Bvalue);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_timer_handle
功能描述 : 针对DPID_TIMER的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_timer_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为RAW
    unsigned char ret;
    /*
    //RAW type data processing
    
    */
    
    //There should be a report after processing the DP
    ret = mcu_dp_raw_update(DPID_TIMER,value,length);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_rtc_timer_handle
功能描述 : 针对DPID_RTC_TIMER的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_rtc_timer_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为RAW
    unsigned char ret;
    /*
    //RAW type data processing
    
    */
    
    //There should be a report after processing the DP
    ret = mcu_dp_raw_update(DPID_RTC_TIMER,value,length);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_switch_save_energy_handle
功能描述 : 针对DPID_SWITCH_SAVE_ENERGY的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_switch_save_energy_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为BOOL
    unsigned char ret;
    //0:off/1:on
    unsigned char switch_save_energy;
    
    switch_save_energy = mcu_get_dp_download_bool(value,length);
    if(switch_save_energy == 0) 
	{
        //bool off
		Bat.SaveEnergMode = 0;
    }
	else 
	{
		Bat.SaveEnergMode = 1;
        //bool on
    }
  
    //There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_SWITCH_SAVE_ENERGY,switch_save_energy);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_battery_percentage_handle
功能描述 : 针对DPID_BATTERY_PERCENTAGE的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_battery_percentage_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned long battery_percentage;
    
    battery_percentage = mcu_get_dp_download_value(value,length);
    /*
    //VALUE type data processing
    
    */
    
    //There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_BATTERY_PERCENTAGE,battery_percentage);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_device_mode_handle
功能描述 : 针对DPID_DEVICE_MODE的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_device_mode_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为ENUM
    unsigned char ret;
    unsigned char device_mode;
    device_mode = mcu_get_dp_download_enum(value,length);
    switch(device_mode) 
	{
        case 0:	
			Bat.SolarMode = 0; //太阳能模式
			break;
        case 1:
			Bat.SolarMode = 1; //太阳能模式
			if(BAT_CDS_RX)LED_RGB_Off_Handle(); //关灯
			break;
        default:
			break;
    }
    //There should be a report after processing the DP
    ret = mcu_dp_enum_update(DPID_DEVICE_MODE, device_mode);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_brightgroup_sync_handle
功能描述 : 针对DPID_BRIGHTGROUP_SYNC的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_brightgroup_sync_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为BOOL
    unsigned char ret;
    //0:off/1:on
    unsigned char brightgroup_sync;
    
    brightgroup_sync = mcu_get_dp_download_bool(value,length);
	TY.GroupSync = brightgroup_sync;
    if(brightgroup_sync == 0)
	{
        //bool off
    }
	else 
	{
        //bool on
    }
  
    //There should be a report after processing the DP
//    ret = mcu_dp_bool_update(DPID_BRIGHTGROUP_SYNC,brightgroup_sync);
    ret = mcu_dp_bool_update(DPID_BRIGHTGROUP_SYNC,brightgroup_sync);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}


/******************************************************************************
                                WARNING!!!                     
此代码为SDK内部调用,请按照实际dp数据实现函数内部数据
******************************************************************************/
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
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length)
{
  #error "请自行完成MCU固件升级代码,完成后请删除该行"
  unsigned long addr;
 
  if(length == 0)
  {
#ifdef ENABLE_BOOT
    //固件数据发送完成
    FlashBuffer.magic_code = FIREWARE_UPDATE_FLAG;
    
    if(Earse_Flash(PARA_ADDR) == ERROR)
      return ERROR;
    
    //写入升级标志
    if(Write_Flash(PARA_ADDR,(unsigned char *)&FlashBuffer,sizeof(FlashBuffer)) == ERROR)
      return ERROR;
    
    Reset();
#endif
  }
  else
  {
    //固件数据处理
    addr = FIREWARE_ADDR_H;
     
    if(position % 1024 == 0)
    {
      if(Earse_Flash(addr + position) == ERROR)
        return ERROR;
    }
    
    if(Write_Flash(addr + position,(unsigned char *)value,length) == ERROR)
      return ERROR;
  }

  return SUCCESS;
}
#endif
/******************************************************************************
                                WARNING!!!                     
以下函数用户请勿修改!!
******************************************************************************/

/*****************************************************************************
函数名称 : dp_download_handle
功能描述 : dp下发处理函数
输入参数 : dpid:DP序号
           value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERRO
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
  /*********************************
  当前函数处理可下发/可上报数据调用                    
  具体函数内需要实现下发数据处理
  完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
  ***********************************/
  unsigned char ret;
  switch(dpid)
  {
        case DPID_SWITCH_LED:
            //开关处理函数
            ret = dp_download_switch_led_handle(value,length);
        break;
        case DPID_WORK_MODE:
            //模式处理函数
            ret = dp_download_work_mode_handle(value,length);
        break;
        case DPID_BRIGHT_VALUE:
            //亮度值处理函数
            ret = dp_download_bright_value_handle(value,length);
        break;
        case DPID_TEMP_VALUE:
            //冷暖值处理函数
            ret = dp_download_temp_value_handle(value,length);
        break;
        case DPID_COLOUR_DATA:
            //彩光处理函数
            ret = dp_download_colour_data_handle(value,length);
        break;
        case DPID_SCENE_DATA:
            //场景处理函数
            ret = dp_download_scene_data_handle(value,length);
        break;
        case DPID_COUNTDOWN:
            //倒计时处理函数
            ret = dp_download_countdown_handle(value,length);
        break;
        case DPID_MUSIC_DATA:
            //音乐律动处理函数
            ret = dp_download_music_data_handle(value,length);
        break;
        case DPID_TIMER:
            //定时处理函数
            ret = dp_download_timer_handle(value,length);
        break;
        case DPID_RTC_TIMER:
            //Rtc定时处理函数
            ret = dp_download_rtc_timer_handle(value,length);
        break;
        case DPID_SWITCH_SAVE_ENERGY:
            //节能开关处理函数
            ret = dp_download_switch_save_energy_handle(value,length);
        break;
        case DPID_BATTERY_PERCENTAGE:
            //电池电量处理函数
            ret = dp_download_battery_percentage_handle(value,length);
        break;
        case DPID_DEVICE_MODE:
            //设备模式处理函数
            ret = dp_download_device_mode_handle(value,length);
        break;
        case DPID_BRIGHTGROUP_SYNC:
            //组内光控同步处理函数
            ret = dp_download_brightgroup_sync_handle(value,length);
        break;


  default:
    break;
  }
  DEBUG_INFO("RX");
  return ret;
}
/*****************************************************************************
函数名称 : get_download_cmd_total
功能描述 : 获取所有dp命令总和
输入参数 : 无
返回参数 : 下发命令总和
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char get_download_cmd_total(void)
{
  return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}
//static void bt_uart_write_data(unsigned char *in, unsigned short len)
//{
//  if((NULL == in) || (0 == len))
//  {
//    return;
//  }
//  
//  while(len --)
//  {
//    uart_transmit_output(*in);
//    in ++;
//  }
//}
//void Get_Time_info(void)
//{
////  unsigned char length = 0;
////  unsigned char data = 0x02;
////  
////  length = set_bt_uart_buffer(length,&data,1);
////  
////  bt_uart_write_frame(0xe1, 1);

//  unsigned char check_sum = 0;
//  
//  bt_uart_tx_buf[HEAD_FIRST] = 0x55;
//  bt_uart_tx_buf[HEAD_SECOND] = 0xaa;
//  bt_uart_tx_buf[PROTOCOL_VERSION] = 0x00;
//  bt_uart_tx_buf[FRAME_TYPE] = 0xe4;
//  bt_uart_tx_buf[LENGTH_HIGH] = 1 >> 8;
//  bt_uart_tx_buf[LENGTH_LOW] = 1 & 0xff;
//  bt_uart_tx_buf[LENGTH_LOW+1] = 0x01;
//  
// // len += PROTOCOL_HEAD;
//  check_sum = get_check_sum((unsigned char *)bt_uart_tx_buf, 8 - 1);
//  bt_uart_tx_buf[8 - 1] = check_sum;
//	
//  bt_uart_write_data(bt_uart_tx_buf,8);
//  //
//  //bt_uart_write_data((unsigned char *)bt_uart_tx_buf, len);

//}

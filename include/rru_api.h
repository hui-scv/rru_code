#ifndef RRU_API_H
#define RRU_API_H

#ifdef __cplusplus      
extern "C"{
#endif

	/******************************************************************************
	int watchDogConfig(int enable, int timeout)
	函数说明: 看门狗配置
	参   数: enable  - 0关闭 1开启
			timeout - 喂狗时间,单位-秒(second)
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int watchDogConfig(int enable, int timeout);

	/******************************************************************************
	int rfChannelEnable(int enable, int channel)
	函数说明: 射频通道使能
	参   数:  enable  - 0关闭 1开启
			 channel - 射频通道号
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int rfChannelEnable(int enable, int channel);

	/******************************************************************************
	int getRfChannelEnableState(int* enable, int channel)
	函数说明: 射频通道使能
	参   数:  enable  - 0关闭 1开启
			 channel - 射频通道号
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int getRfChannelEnableState(int* enable, int channel);

	/******************************************************************************
	int isAdcLocked(int* locked, int channel)
	函数说明: 检测2路ADC锁定状态
	参   数:  locked  - 0未锁定 1锁定
			  channel - 通道号
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int isAdcLocked(int* locked, int channel);

	/******************************************************************************
	int isDacLocked(int* locked, int channel)
	函数说明: 检测2路DAC锁定状态
	参   数:  locked  - 0未锁定 1锁定
	channel - 通道号
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int isDacLocked(int* locked, int channel);

	/******************************************************************************
	int getCWstate(int* state, int channel)
	函数说明:  检测载波状态
	参   数:  state  - 0/1
			 channel - 通道号
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int getCWstate(int* state, int channel);

	/******************************************************************************
	int getClockLockedState(void* state)
	函数说明:  检测时钟锁定状态fpga/serdes等
	参   数:  state  - 结构体待定
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int getClockLockedState(void* state);

	/******************************************************************************
	int getOpticalModuleInfo(void* info)
	函数说明:  获取光模块信息
	参   数:  state  - 结构体待定
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int getOpticalModuleInfo(void* info);

	/******************************************************************************
	int getBoradTemperatrue(double* t)
	函数说明:  获取板卡温度
	参   数:  t  - 温度
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int getBoradTemperatrue(double* t);

	/******************************************************************************
	*rtc 调用系统命令
	******************************************************************************/

	/******************************************************************************
	int getRfdBm(double* dBm, int channel)
	函数说明:  获取射频功率
	参   数:  dBm  - 功率
			 channel - 通道
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int getRfdBm(double* dBm, int channel);

	/******************************************************************************
	int getCPRIstate(void* state, int channel)
	函数说明:  获取CPRI状态
	参   数:  state  - 结构体待定
			 channel - 通道0~7
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int getCPRIstate(void* state, int channel);

	/******************************************************************************
	int setCPRIiq(void* configs, int channel)
	函数说明:  配置CPRI IQ通道
	参   数:   configs  - 结构体待定
			  channel  - 通道0~7
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int setCPRIiq(void* configs, int channel);

	/******************************************************************************
	int setCPRIvss(void* configs, int channel)
	函数说明:  配置CPRI vss通道
	参   数:   configs  - 结构体待定
			  channel  - 通道0~7
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int setCPRIvss(void* configs, int channel);

	/******************************************************************************
	int setCPRIcm(void* configs, int channel)
	函数说明:  配置CPRI c&m
	参   数:   configs  - 结构体待定
	channel  - 通道0~7
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int setCPRIcm(void* configs, int channel);

	/******************************************************************************
	int setCPRImanage(void* configs, int channel)
	函数说明:  配置CPRI 管理接口
	参   数:   configs  - 结构体待定
			  channel  - 通道0~7
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int setCPRImanage(void* configs, int channel);


	/******************************************************************************
	*以太网络操作-直接操作eth0~eth8
	******************************************************************************/

	/******************************************************************************
	int setfpgaLatencyCompensation(unsigned int ns, int direction, int channel);
	函数说明:  设置fpga延时补偿
	参   数:   ns  - 纳秒
			  direction - 0上行 1下行
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int setfpgaLatencyCompensation(unsigned int ns, int direction, int channel);

	/******************************************************************************
	int setRfGain(double gain, int channel, int direction);
	函数说明:  设置上行。下行各通道射频衰减
	参   数:   ns  - 纳秒
			  direction - 0上行 1下行
			  channel - 通道
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int setRfGain(double gain, int channel, int direction);

	/******************************************************************************
	int writeCwDataToRfChannel(int channel, char* buffer, int len);
	函数说明:  发送cpu载波数据到射频通道
	参   数:  channel - 通道
			 buffer - 数据缓冲区
			 len - 数据长度
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int writeCwDataToRfChannel(int channel, char* buffer, int len);

	/******************************************************************************
	int registerCwDataCallback(int channel, pfnCwDataCallback func);
	函数说明:  获取各个通道的载波数据
	参   数:  channel - 通道
			 func - 回调函数
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	typedef void(*pfnCwDataCallback)(int channel, char* buffer, int len);
	int registerCwDataCallback(int channel, pfnCwDataCallback func);

	/*取消注册*/
	int unRegisterCwDataCallback(int channel);

	/******************************************************************************
	int rfCWenable(int enable, int channel, int direction);
	函数说明:  设置载波使能
	参   数:     enable  - 0关闭 1开启
				direction - 0上行 1下行
				channel - 通道
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int rfCWenable(int enable, int channel, int direction);

	/******************************************************************************
	int fpageFirmwareWrite(int number, char* buf, int len);
	函数说明:    升级fpga固件
	参   数:    number fpga编号
			   buf - 固件内容
			   len - 固件长度
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int fpageFirmwareWrite(int number, char* buf, int len);

	/******************************************************************************
	int ADCconfig(void* configs, int channel)
	函数说明: 配置adc
	参   数:  configs  - 结构体待定
			 channel  - 通道号
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int ADCconfig(void* configs, int channel);

	/******************************************************************************
	int DACconfig(void* configs, int channel)
	函数说明: 配置dac
	参   数:  configs  - 结构体待定
			 channel  - 通道号
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int DACconfig(void* configs, int channel);

	/******************************************************************************
	int clockConfig(void* configs)
	函数说明: 配置时钟
	参   数:  configs  - 结构体待定
			 num - fpga编号
	返   回:  RET_OK-成功 RET_ERR-失败
	******************************************************************************/
	int clockConfig(void* configs,int num);

#ifdef __cplusplus
}
#endif

#endif

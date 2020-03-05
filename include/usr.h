#ifndef USR_H
#define USR_H

/*
#define RET_ERR (-1)
#define RET_OK 0
*/

/******************************************************************************
int device_write(int fpga, int dev, int offset, int val);
函数说明:	写设备寄存器
参    数:  
		fpga:
			0x00: fpga0
			0x01L fpga1
		dev:
			0x00: cpri0
			0x01: cpri1
			0x02: cpri2
			0x03: cpri3
			0x04: ad_pll7044
			0x05: ad
			0x06: da_pll7044
			0x07: da
		offset: 寄存器偏移值
		val: 	寄存器值
返    回:   RET_OK/RET_ERR
******************************************************************************/
int device_write(int fpga, int dev, int offset, int val);


/******************************************************************************
int device_read(int fpga, int dev, int offset, int val);
函数说明:	读设备寄存器
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		dev:
			0x00: cpri0
			0x01: cpri1
			0x02: cpri2
			0x03: cpri3
			0x04: ad_pll7044
			0x05: ad
			0x06: da_pll7044
			0x07: da
		offset: 寄存器偏移值
返    回:   RET_OK/RET_ERR
******************************************************************************/
int device_read(int fpga, int dev, int offset);


/******************************************************************************
int update_da_recall_data(int fpga, int channel,
							int child, char* buffer, int length);
函数说明:	更新da的回放数据
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		channel:
			0x00: da_ch0 da通道0
			0x01: da_ch1 da通道1
		child:
			0x00: da_child_ch0 da子信道0
			0x01: da_child_ch1 da子信道1
		buffer:
			数据起始地址
		length:
			数据长度
返    回:   RET_OK/RET_ERR
******************************************************************************/
int update_da_recall_data(int fpga, int channel, 
								int child, char* buffer, int length);


/******************************************************************************
int da_recall_mode_set(int fpga, int channel, int mode)
函数说明:	设置da回放的模式
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		channel:
			0x00: da_ch0 da通道0
			0x01: da_ch1 da通道1
		mode:
			0x00: 正常模式[数据源BBU]
			0x01: 模拟数据模式[测试数据]
返    回:   RET_OK/RET_ERR
******************************************************************************/
int da_recall_mode_set(int fpga, int channel, int mode);

/******************************************************************************
int da_recall_enable(int fpga, int channel, int enable);
函数说明:	设置da回放使能
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		channel:
			0x00: da_ch0 da通道0
			0x01: da_ch1 da通道1
		enable:
			0x00: 关闭
			0x01: 开启
返    回:   RET_OK/RET_ERR
******************************************************************************/
int da_recall_enable(int fpga, int channel, int enable);

/******************************************************************************
int da_set_da_gain(int fpga, int channel, int child, double db);
函数说明:	设置da回放增益
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		channel:
			0x00: da_ch0 da通道0
			0x01: da_ch1 da通道1
		child:
			0x00: da_child_ch0 da子信道0
			0x01: da_child_ch1 da子信道1
		db:
			增益值[~∞, 6.18]db
返    回:   RET_OK/RET_ERR
******************************************************************************/
int da_set_da_gain(int fpga, int channel, int child, double db);

/******************************************************************************
int cpri_status_read(int fpga, int cpri)
函数说明:	设置da回放增益
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		cpri:
			[0, 3]
		state:
			返回值
返    回:   RET_OK/RET_ERR
******************************************************************************/
typedef struct tagCPRI_STATUS_S
{
	short warning_report : 1;
	short state : 4;
	short lnk_speed : 11;
}CPRI_STATUS_S;

int cpri_status_read(int fpga, int cpri, CPRI_STATUS_S* state);

/******************************************************************************
int read_ad_stream_128k(int fpga, int channel, char buffer[]);
函数说明:	获取256k ad数据
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		channel:
			[0, 3]四通道
		buffer:
			读取的数据
返    回:   RET_OK/RET_ERR
******************************************************************************/
int read_ad_stream_128k(int fpga, int channel, char buffer[]);


/******************************************************************************
int fpga_clk_ref_set(int fpga, int rw, int ref);
函数说明:	参考钟设置
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		rw:
			0: 读
			1: 写
		ref:
			0x0:内参考
			0x1:外SMA参考
			0x2:外光纤参考
返    回:   RET_OK/RET_ERR
******************************************************************************/
int fpga_clk_ref_ctrl(int fpga, int rw, int *ref);


/******************************************************************************
int status_read(int fpga, STATUS_S* status);
函数说明:	fpga状态读取
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		status:
			读取的状态
返    回:   RET_OK/RET_ERR
******************************************************************************/
typedef struct tagSTATUS_S
{
	unsigned long long fpga_tempatrue;
	unsigned long long vccin_voltage;
	unsigned long long vccaux_voltage;
	char sys_clk_pll_state : 1;
	char ad_clk_pll_state : 1;
	char da_clk_pll_state : 1;
	char pcie_lnk_state : 1;
	char ad_work_state : 1;
	char da_work_state : 1;
	char none : 2;
}STATUS_S;
void status_read(int fpga, STATUS_S* status);


/******************************************************************************
int upstream_delay_ctrl(int fpga, int rw, unsigned int* us);
函数说明:	上行延时读写
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		rw:
			0: 读
			1: 写
		us:
			延时时间设置单位u秒.最大值33us
返    回:   RET_OK/RET_ERR
******************************************************************************/
int upstream_delay_ctrl(int fpga, int rw, unsigned int* us);

/******************************************************************************
int downstream_delay_ctrl(int fpga, int rw, unsigned int* us);
函数说明:	下行延时读写
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		rw:
			0: 读
			1: 写
		us:
			延时时间设置单位u秒.最大值33us
返    回:   RET_OK/RET_ERR
******************************************************************************/
int downstream_delay_ctrl(int fpga, int rw, unsigned int* us);


#endif

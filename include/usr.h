#ifndef USR_H
#define USR_H


#define RET_ERR (-1)
#define RET_OK 0

/******************************************************************************
int device_write(int fpga, int dev, int offset, int val);
函数说明:	写设备寄存器
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
			0x08:射频1-只写fpda0
			0x09:射频2-只写fpda0
			0x0a:射频3-只写
			0x0b:射频4-只写
			0x0c:射频5-只写
			0x0d:射频6-只写
			0x0e:射频7-只写
			0x0f:射频8-只写
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
			0x01: da_ch0 da通道0
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
			0x02: da_ch0 da通道0
		child:
			0x00: da_child_ch0 da子信道0
			0x01: da_child_ch1 da子信道1
		enable:
			0x00: 关闭
			0x01: 开启
返    回:   RET_OK/RET_ERR
******************************************************************************/
int da_recall_enable(int fpga, int channel, int child, int enable);


/******************************************************************************
int da_enable_read(int fpga, DA_ENABLE_S* enable);
函数说明:	回放使能读取
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		enable: 
			使能状态
返    回:   RET_OK/RET_ERR
******************************************************************************/
typedef struct tagDA_ENABLE_S
{
	int ch1_child1_en : 1;
	int ch1_child2_en : 1;
	int ch2_child1_en : 1;
	int ch2_child2_en : 1;
	int null	 : 28;
}DA_ENABLE_S;
int da_enable_read(int fpga, DA_ENABLE_S* enable);

/******************************************************************************
int ad_enable(int fpga, int channel, int enable);
函数说明:	设置ad使能
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		channel:
			0x01: 通道0
			0x02: 通道1
			0x03: 通道2
			0x04: 通道3
		enable:
			0x00: 关闭
			0x01: 开启
返    回:   RET_OK/RET_ERR
******************************************************************************/
int ad_enable(int fpga, int channel, int enable);


/******************************************************************************
int ad_enable_read(int fpga, AD_ENABLE_S* enable);
函数说明:	ad使能读取
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		enable: 
			使能状态
返    回:   RET_OK/RET_ERR
******************************************************************************/
typedef struct tagAD_ENABLE_S
{
	int ch1_en : 1;
	int ch2_en : 1;
	int ch3_en : 1;
	int ch4_en : 1;
	int null	 : 28;
}AD_ENABLE_S;
int ad_enable_read(int fpga, AD_ENABLE_S* enable);

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
int da_get_da_gain(int fpga, int channel, int child, double* db);
函数说明:	获取da回放增益
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
int da_get_da_gain(int fpga, int channel, int child, double* db);


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
int read_ad_stream_256k(int fpga, int channel, char buffer[]);
函数说明:	获取256k ad数据
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		channel:
			[0, 3]四通道
		buffer:
			读取的数据 256K buffer长度大于等于256K
返    回:   RET_OK/RET_ERR
******************************************************************************/
int read_ad_stream_256k(int fpga, int channel, char buffer[]);


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
	double fpga_tempatrue;
	double vccin_voltage;
	double vccaux_voltage;
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
			延时时间设置单位u秒.最大值33000ns
返    回:   RET_OK/RET_ERR
******************************************************************************/
int upstream_delay_ctrl(int fpga, int rw, unsigned int* ns); //修改成ns为单位

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
			延时时间设置单位u秒.最大值33000ns
返    回:   RET_OK/RET_ERR
******************************************************************************/
int downstream_delay_ctrl(int fpga, int rw, unsigned int* ns);

/******************************************************************************
int fpga_upgrade_firmware(int fpga, char* buffer, int len);
函数说明:	升级fpga固件
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		buffer: 固件内容
		len: 固件长度
返    回:   RET_OK/RET_ERR
******************************************************************************/
int fpga_upgrade_firmware(int fpga, char* buffer, int len);


/******************************************************************************
int vss_write(int fpga, int cpri, int child, char buf[16]);
函数说明:	cpri vss 写
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		cpri:
			[0, 3]
		chile:
			子信道[0,3]
		buf:
			128bit
			
返    回:   RET_OK/RET_ERR
******************************************************************************/
int vss_write(int fpga, int cpri, int child, char buf[16]);

/******************************************************************************
int vss_read(int fpga, int cpri, int child, char buf[16]);
函数说明:	cpri vss 读
参    数:  
		fpga:
			0x00: fpga0
			0x01: fpga1
		cpri:
			[0, 3]
		chile:
			子信道[0,3]
		buf:
			128bit
返    回:   RET_OK/RET_ERR
******************************************************************************/
int vss_read(int fpga, int cpri, int child, char buf[16]);



/*全局复位，调用这个函数写*/
//eblc_write(int fpga, 0, 0);

/*测试寄存器*/
int eblc_write(int fpga, int offset, int val);
int eblc_read(int fpga, int offset);
#endif

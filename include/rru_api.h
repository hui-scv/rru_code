#ifndef RRU_API_H
#define RRU_API_H

#ifdef __cplusplus      
extern "C"{
#endif

	/******************************************************************************
	int watchDogConfig(int enable, int timeout)
	����˵��: ���Ź�����
	��   ��: enable  - 0�ر� 1����
			timeout - ι��ʱ��,��λ-��(second)
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int watchDogConfig(int enable, int timeout);

	/******************************************************************************
	int rfChannelEnable(int enable, int channel)
	����˵��: ��Ƶͨ��ʹ��
	��   ��:  enable  - 0�ر� 1����
			 channel - ��Ƶͨ����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int rfChannelEnable(int enable, int channel);

	/******************************************************************************
	int getRfChannelEnableState(int* enable, int channel)
	����˵��: ��Ƶͨ��ʹ��
	��   ��:  enable  - 0�ر� 1����
			 channel - ��Ƶͨ����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int getRfChannelEnableState(int* enable, int channel);

	/******************************************************************************
	int isAdcLocked(int* locked, int channel)
	����˵��: ���2·ADC����״̬
	��   ��:  locked  - 0δ���� 1����
			  channel - ͨ����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int isAdcLocked(int* locked, int channel);

	/******************************************************************************
	int isDacLocked(int* locked, int channel)
	����˵��: ���2·DAC����״̬
	��   ��:  locked  - 0δ���� 1����
	channel - ͨ����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int isDacLocked(int* locked, int channel);

	/******************************************************************************
	int getCWstate(int* state, int channel)
	����˵��:  ����ز�״̬
	��   ��:  state  - 0/1
			 channel - ͨ����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int getCWstate(int* state, int channel);

	/******************************************************************************
	int getClockLockedState(void* state)
	����˵��:  ���ʱ������״̬fpga/serdes��
	��   ��:  state  - �ṹ�����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int getClockLockedState(void* state);

	/******************************************************************************
	int getOpticalModuleInfo(void* info)
	����˵��:  ��ȡ��ģ����Ϣ
	��   ��:  state  - �ṹ�����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int getOpticalModuleInfo(void* info);

	/******************************************************************************
	int getBoradTemperatrue(double* t)
	����˵��:  ��ȡ�忨�¶�
	��   ��:  t  - �¶�
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int getBoradTemperatrue(double* t);

	/******************************************************************************
	*rtc ����ϵͳ����
	******************************************************************************/

	/******************************************************************************
	int getRfdBm(double* dBm, int channel)
	����˵��:  ��ȡ��Ƶ����
	��   ��:  dBm  - ����
			 channel - ͨ��
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int getRfdBm(double* dBm, int channel);

	/******************************************************************************
	int getCPRIstate(void* state, int channel)
	����˵��:  ��ȡCPRI״̬
	��   ��:  state  - �ṹ�����
			 channel - ͨ��0~7
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int getCPRIstate(void* state, int channel);

	/******************************************************************************
	int setCPRIiq(void* configs, int channel)
	����˵��:  ����CPRI IQͨ��
	��   ��:   configs  - �ṹ�����
			  channel  - ͨ��0~7
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int setCPRIiq(void* configs, int channel);

	/******************************************************************************
	int setCPRIvss(void* configs, int channel)
	����˵��:  ����CPRI vssͨ��
	��   ��:   configs  - �ṹ�����
			  channel  - ͨ��0~7
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int setCPRIvss(void* configs, int channel);

	/******************************************************************************
	int setCPRIcm(void* configs, int channel)
	����˵��:  ����CPRI c&m
	��   ��:   configs  - �ṹ�����
	channel  - ͨ��0~7
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int setCPRIcm(void* configs, int channel);

	/******************************************************************************
	int setCPRImanage(void* configs, int channel)
	����˵��:  ����CPRI ����ӿ�
	��   ��:   configs  - �ṹ�����
			  channel  - ͨ��0~7
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int setCPRImanage(void* configs, int channel);


	/******************************************************************************
	*��̫�������-ֱ�Ӳ���eth0~eth8
	******************************************************************************/

	/******************************************************************************
	int setfpgaLatencyCompensation(unsigned int ns, int direction, int channel);
	����˵��:  ����fpga��ʱ����
	��   ��:   ns  - ����
			  direction - 0���� 1����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int setfpgaLatencyCompensation(unsigned int ns, int direction, int channel);

	/******************************************************************************
	int setRfGain(double gain, int channel, int direction);
	����˵��:  �������С����и�ͨ����Ƶ˥��
	��   ��:   ns  - ����
			  direction - 0���� 1����
			  channel - ͨ��
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int setRfGain(double gain, int channel, int direction);

	/******************************************************************************
	int writeCwDataToRfChannel(int channel, char* buffer, int len);
	����˵��:  ����cpu�ز����ݵ���Ƶͨ��
	��   ��:  channel - ͨ��
			 buffer - ���ݻ�����
			 len - ���ݳ���
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int writeCwDataToRfChannel(int channel, char* buffer, int len);

	/******************************************************************************
	int registerCwDataCallback(int channel, pfnCwDataCallback func);
	����˵��:  ��ȡ����ͨ�����ز�����
	��   ��:  channel - ͨ��
			 func - �ص�����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	typedef void(*pfnCwDataCallback)(int channel, char* buffer, int len);
	int registerCwDataCallback(int channel, pfnCwDataCallback func);

	/*ȡ��ע��*/
	int unRegisterCwDataCallback(int channel);

	/******************************************************************************
	int rfCWenable(int enable, int channel, int direction);
	����˵��:  �����ز�ʹ��
	��   ��:     enable  - 0�ر� 1����
				direction - 0���� 1����
				channel - ͨ��
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int rfCWenable(int enable, int channel, int direction);

	/******************************************************************************
	int fpageFirmwareWrite(int number, char* buf, int len);
	����˵��:    ����fpga�̼�
	��   ��:    number fpga���
			   buf - �̼�����
			   len - �̼�����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int fpageFirmwareWrite(int number, char* buf, int len);

	/******************************************************************************
	int ADCconfig(void* configs, int channel)
	����˵��: ����adc
	��   ��:  configs  - �ṹ�����
			 channel  - ͨ����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int ADCconfig(void* configs, int channel);

	/******************************************************************************
	int DACconfig(void* configs, int channel)
	����˵��: ����dac
	��   ��:  configs  - �ṹ�����
			 channel  - ͨ����
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int DACconfig(void* configs, int channel);

	/******************************************************************************
	int clockConfig(void* configs)
	����˵��: ����ʱ��
	��   ��:  configs  - �ṹ�����
			 num - fpga���
	��   ��:  RET_OK-�ɹ� RET_ERR-ʧ��
	******************************************************************************/
	int clockConfig(void* configs,int num);

#ifdef __cplusplus
}
#endif

#endif

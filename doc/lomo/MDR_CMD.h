//***********************************************
#ifndef __I2C_CMD_H
#define __I2C_CMD_H
//***********************************************
// ����� ���������
#define PACKET_START		0x1B
// ����� ��� ����� ������ ���� ���� (����� ������ ��� CRC)
// ����� ���� ������ ����������
// ����� ������� ��� ����������
// ��������� ���� ��� ����� ��� �������
// ��������� ����� CRC
/*
 ������ ����������� �����
 unsigned char crc;
 . . .
 . . .
 crc = 0;
 for(i=0;i<long_buf;i++) scrc(buf[i]);
 buf[long_buf] = crc;
 . . .
 . . .
void scrc(char inch)
{
	char b;
	char c_rc = crc;
	for(b=0;b<8;b++){
  		if((inch ^ c_rc) & 0x01) c_rc = 0x80 | ((c_rc^0x18) >> 1);
  		else 			 	 	 c_rc = c_rc >> 1;
  		inch >>= 1;
 	}
	crc = c_rc;
}
*/
//*************************************************
// �������������� ���������.
#define HUB_ID	    		0x10		        // ���                         	
#define MONO_ID	    		0x20		        // ������������
#define MIRROR_ID			0x22		        // ����� �������
#define FRAME_ID        	0x24                // ����� �����
#define FILTR_ID        	0x26                // ����� ��������
#define S256_MONO_ID		0x28		        // ������������ ��������� 256
#define SLIT_ID         	0x2A                // ����� ����
#define ADS1252_ID 			0x30				// ��� �� ������ ADS1252
#define PLM_ADS1250_ID 		0x32				// ��� + ��� �� ������ ADS1250
#define S256_ADC_ID		 	0x34				// ��� �� ������ ADS1252 ��� ��256
#define IK400_ADC_ID	 	0x36				// ��� �� ������ ADS1252 � ������������ �������� � ����������� 200-400��
#define SYNCHRO_ADS1252_ID      0x70		//��� �� ������ ADS1252 c ���������� ���������������
//***********************************************************************************************
// ����� ����
#define	HUB_ADDR		0x8A
// ��������� ����� ���������
#define	START_ADDR	0xFA
//***********************************************************************************************
// ������� ����.....
// ����� ������� � ������� �������
// ������� ������ ������� � ������� �������. ������� �� ����� HUB_ADR.
#define	CMD_HUB_RESET_SYSTEM	0x00
	// �� ��� ������� ������� �������� ����� �� ����
	#define	RET_HUB_HOME_FIND		0x00
	//�����, ��������� ������ RET_DEVICE_VERSION �� ������������ � ������� ���������
	// ����������� ������ 0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xA0,0xB0,0xD0,0xE0,0xF0
	// � ��������� �����
	#define	RET_HUB_END_FIND		0x01


// ��������� � ���� ������ ���������
#define	CMD_HUB_GET_VERSION		0x01
	// �������
	#define	RET_HUB_VERSION			0x02
	// RETURN PACK: HUB_ADDR,RET_HUB_VERSION,HUB_ID,VERSION,SUB_VERSION,LONG_SERIAL_NUMBER
#define LONG_SERIAL_NUMBER	8

	// ���������� �� ��������
	#define RET_ADDR_ERROR			0x03

	// ������ PC � ������ ���������
	#define CMP_PROGRAMM_OK			0x04
		// ����� �� ��
		#define RET_PC_OK		0x02

//***********************************************************************************************
// ����� ������� ��� ���� ���������

// ����� ������ ���������� ������������� START_ADR, � ��� ������� �� ����� ������
#define CMD_DEVICE_CHANGE_ADDR	    0x01
		// ������� � ���� ��� ����� �����, � ���� ���������� ������
		// PACK: DEVICE_ADR,CMD_CHANGE_ADR,NEW_ADR,0x55,0xAA
	// �� ��� ���������� �������� ����� ID � ����������������� I2C �����, ��� CMD_BAD
	#define RET_DEVICE_VERSION			0x01
		// RETURN PACK: DEVICE_ADR_I2C,DEVICE_VERSION,ID,VERSION,SUB_VERSION,+8byte �������� �����
	// ��� ���� ������ ������� �� ���������� ������� RET_DEVICE_CMD_BAD


// ��������� ������ ����������
#define CMD_DEVICE_GET_VERSION	0x02
		// PACK: DEVICE_ADDR,CMD_DEVICE_GET_VERSION
		// RETURN PACK: DEVICE_ADR,RET_DEVICE_VERSION,ID,VERSION,SUB_VERSION,+8byte �������� �����

// ������� ����������� �� ������
		#define RET_DEVICE_CMD_BAD	0xFF

// ������� ���������� �������� �����
// LONG_SERIAL_NUMBER ���� ANSI �������� �����
#define CMD_SERIAL_NUMBER		0x03
	// ����� �������
	#define	RET_OK                     0x02
	// ��� ���� ������ ������� �� ���������� ������� RET_DEVICE_CMD_BAD
//***********************************************************************************************
// ������� ��� �������������
//***********************************************************************************************

// ������� ������ �� "����"
// ���������� ���
#define CMD_MONO_SET_ZERO				0x10
	// ������� ����� ������ �� "����"
	#define RET_MONO_ZERO					0x10
	// ��� ���� �� 6 ������� �� ��������� � ����� �� ������� RET_MONO_BAD + CODE_ERROR(1 byte)


// ������� ������ �� ���
// 4 ����� - unsigned long ��������� ���
#define CMD_MONO_SET_STEP  				0x11
	// ������� ����� ������ �� ���������� ����
	#define RET_MONO_STEP					0x11
	// ��� ���� ������ ������� �� ���������� ������� RET_DEVICE_CMD_BAD
	// ��� ���� ���� ������ ������� ������� RET_MONO_NO_ZERO
	// ��� ������� RET_MONO_BAD + CODE_ERROR(1 byte)


// ������� �������� ������� ���
#define CMD_MONO_GET_CURRENT_STEP		0x12
	// ����� �������
	#define RET_MONO_CURRENT_STEP			0x12
	// � 4 ����� - unsigned long ������� ���


// ������� ������ �� ����� �����
// 4 ����� - float ��������� ����� �����
#define CMD_MONO_SET_LONGWAVE			0x13
	// ������� ����� ������ �� ��������� ����� �����
	#define RET_MONO_OK		  				0x13


// ������� �������� ������� ����� �����
#define CMD_MONO_GET_CURRENT_LONGWAVE	0x14
	// ����� �������
	#define RET_MONO_CURRENT_LONGWAVE		0x14
	// � 4 ����� - unsigned long ������� ����� �����


#define LONG_NAME_GRATE 10
// ������� ���������� ����������� �������������
// 1 ���� - ������ �������
// 4 ����� - float ��������� ����� �����
// 4 ����� - float ��������
// LONG_NAME_GRATE ���� ANSI ��� �������
#define CMD_MONO_SET_FACTOR				0x15
	// ����� ������� RET_MONO_OK


// ������� �������� ����������� �������������
// 1 ���� - ������ �������
#define CMD_MONO_GET_FACTOR 			0x16
	// ����� �������, ��� ������� ��������� �������
	#define RET_MONO_FACTOR 				0x15
	// 4 ����� - float ��������� ����� �����
	// 4 ����� - float ��������
	// LONG_NAME_GRATE ���� ANSI ��� �������


// ������� �������� ������ �������� �������
#define CMD_MONO_GET_CURRENT_FACTOR		0x17
	// ����� �������, ��� ������� ��������� �������
	#define RET_MONO_CURRENT_FACTOR			0x16
	// 1 ���� - ������ �������� �������

// ������� ���������� ������ �������� ������� �� ����� ��������� ������ �������� ������� ���������� � �������
// 1 ���� - ������ �������� �������
#define CMD_MONO_SET_CURRENT_FACTOR	 	0x18

// ������� ���������� �������� ������
// 2 ����� - word ����� ������ ���� (1��. = 16.0 ���.)
#define CMD_MONO_SET_SPEED				0x19
	// ����� ������� RET_MONO_OK


// ������� �������� ������������ ��������
#define CMD_MONO_GET_SPEED				0x1A
	// ����� �������, ��� ������� ��������� �������
	#define RET_MONO_SPEED					0x17
	// 2 ����� - word ����� ������ ���� (1��. = 16.0 ���.) ������������ �������� �� �������


// ������� ���������� ��� � ����� (��������� ������ �� ��������� ������� ��������� ���������, ����� ��� ���)
// 4 ����� - unsigned long ��������� ��� ����� ������� MONO ������� ������� RET_MONO_CURRENT_STEP
#define CMD_MONO_SET_ECHO_STEP			0x1B
	// ����� ������� RET_MONO_OK


// ������� ���������� ����� ��� (��������� ������ �� ��������� ������� ��������� ���������, ����� ����� ����)
// 1 ����� - ����� ���������� �� ������� MONO ������� ������� RET_MONO_CURRENT_STEP
#define CMD_MONO_SET_ECHO_ADDR			0x1C
	// ����� ������� RET_MONO_OK

// ������� ��������
#define CMD_MONO_STOP					0x1D
	// ����� �������� �������
	#define	RET_MONO_STOP					0x18

// ������� ���������� �������� �����
// LONG_SERIAL_NUMBER ���� ANSI �������� �����
#define CMD_MONO_SERIAL_NUMBER			0x1E
	// ����� ������� RET_MONO_OK

#define CMD_MONO_SAVE_STEP				0x1F


	#define RET_MONO_BAD	  				0x19
	#define RET_MONO_NO_ZERO  				0x1A

// ������ �������� � ������� �� ������� ���������, �� ���� �������� ��� �������
// 2 ����� - WORD ��������
#define CMD_MONO_SET_DEF_SPEED  		0x20
	// ����� ������� RET_MONO_OK
// �������� �������� � ������� �� ������� ���������, �� ���� �������� ��� �������
#define CMD_MONO_GET_DEF_SPEED  		0x21
	#define RET_MONO_DEF_SPEED 				0x1c
	// ������� RET_MONO_DEF_SPEED + 2 ����� WORD ��������

// ������ ���������� ����� �������
// 1 ����� - ���������� �����
#define CMD_MONO_SET_STEP_RAZG  		0x22
	// ����� ������� RET_MONO_OK
// �������� ���������� ����� �������
#define CMD_MONO_GET_STEP_RAZG  		0x23
	#define RET_MONO_STEP_RAZG 				0x1D
	// ������� RET_MONO_DEF_SPEED + 1 ����� - ���������� �����

// ���� �� MONO �������� ����� �� �� ����, MONO �������
 //� ���� RET_MONO_CURRENT_STEP � ����� �� ������������� ����� ECHO
// ���� ����� ECHO �� ����������, �� ��� ����� HUB_ADDR
//***********************************************************************************************
// ��������� ��� �������������
#define PARM_MONO_MIN_QUANTUMS_STEP		20    // ����������� ���������� ������� ��� ��������
#define PARM_MONO_MAX_QUANTUMS_STEP		65535   // ������������ ���������� ������� ��� ��������
#define PARM_MONO_QUANTUM_STEP			16.0  	// ����� ������ ������ �������� � ���
#define	PARM_MONO_MAX_STEP				73000L


//***********************************************************************************************
// ������� ��� ��� �� ������ ADS1252
//***********************************************************************************************

// ������� �������� N ���������
// ��������� 2 �����, word ������� ������� � ����� ���������
#define CMD_ADS1252_N_MEASURE			0x20
	// ������� ����� ������� N ���������
	#define RET_ADS1252_N_MEASURE			0x20
	// � ���������� float ����� N ���������, word N.


// ������� ��������� ���������
#define CMD_ADS1252_START_MEASURE		0x21
	// ����� �������
	#define	RET_ADS1252_OK					0x21

// ������� ���������� ���������
#define CMD_ADS1252_STOP_MEASURE		0x22
	// ����� �������
	#define	RET_ADS1252_STOP				0x22

// ������� ��������� ���������
// 4 ����� - unsigned long ��������� (ECHO �������)
#define CMD_ADS1252_START_ECHO			0x23
	// ������� �� ���������� N ECHO
	#define	RET_ADS1252_END_ECHO			0x23
// ������� ���������� ��� � N ��������� (��������� ���������)
// 2 ����� - word N ��������� ����� ������� ADS1252 ������� ������� RET_ADS1252_N_MEASURE
#define CMD_ADS1252_SET_ECHO_MEASURE	0x24
	// ����� ������� RET_ADS1252_OK


// ������� ���������� ����� ��� (��������� ������ �� ���������
// ������� CMD_ADS1252_START_MEASURE, ����� �� ���)
// 1 ����� - ����� ���������� �� ������� ADS1252 ������� ������� RET_ADS1252_N_MEASURE
#define CMD_ADS1252_SET_ECHO_ADDR		0x25
	// ����� ������� RET_ADS1252_OK

// ������� ��������� ����������� ��������� �� N ���������
// 1 ���� ���������� ��������� ��������� � ���������� �����
#define CMD_ADS1252_START_INCESSANT		0x26
	// ����� �������
	//#define	RET_ADS1252_OK					

// ������� �������� N ���������
#define CMD_ADS1252_GET_BUFFER			0x27
	// ����� �������
	#define RET_ADS1252_BUFFER				0x24
	// 1 ���� ���������� � ������ ���������, N long ���������.

//�������� ��� ��� ����������� � ������ ���������

//� ���� ������� ���� ������ ���-�� ��������� �� ������
//�������� 4 ���� - ���-�� �������� ���������
#define CMD_ADS1252_COUNT_MES_PERIOD   0x26
	#define RET_ADS1252_COUNT_MES_PERIOD   0x26
	// � ���������� float ����� N ���������, word N.

//��� ������� ��������� ���������� ������� �� ������
//�������� 4 ���� - ���-�� �������� ���������
#define CMD_ADS1252_AVERAGE_MES_PERIOD 0x27
	#define RET_ADS1252_AVERAGE_MES_PERIOD 0x27
	// � ���������� float ����� N ���������, word N.

//��� ������� ��������� �������� � ������ ����������������� ���������
//�������� 2 ���� - ���-�� �������� ���������
//�������� 1 ���� - ����� ����
//�������� 1 ���� - ���������� �� �����
#define CMD_ADS1252_SYNCHRO_DETECTOR   0x28
	#define RET_ADS1252_SYNCHRO_DETECTOR   0x28
	// � ���������� float ����� N ���������, word N.

// ���� �� ADS1252 �������� ����� �� �� ����, ADS1252 ������� � ����
// RET_ADS1252_N_MEASURE � ����� �� ������������� ����� ECHO
// ���� ����� ECHO �� ����������, �� ��� ����� HUB_ADDR

//***********************************************************************************************
// ������� ��� ��� �� ������ ADS1252  ��� ������������� ������� � ���������� �� 200-400��
//***********************************************************************************************
// ������ ����� ������ ��� ���������� �����. 1 ��������� ��� 3 ����� (24 ��������� ���)
#define LONG_BLOCK_MEAS400	16
// ������ �����. 1 ��������� ��� 3 ����� (24 ��������� ���) (15 ������)
#define MAX_FRAME_MEAS400	256
// ��������� ���� ������ �����...
// 1 ����� - ����� �����
#define CMD_IK400_ADC_GET_BLOCK			0x10
// �����
	#define RET_IK400_ADC_BLOCK				0x10
	// � LONG_BLOCK_MEAS400 * 3 ���� ������ ���������

// ��������� 1 ������
#define CMD_IK400_ADC_GET_GATES1			0x11
// �����
	#define RET_IK400_ADC_GATES1				0x11
	// � BYTE ������ � BYTE ����� ���������. � ������� ��������� 
// ������ 1 ������
//BYTE ������ � BYTE ����� ���������. � ������� ��������� 
#define CMD_IK400_ADC_SET_GATES1			0x12
// �����
	#define RET_IK400_ADC_OK					0x12

// ��������� 2 ������
#define CMD_IK400_ADC_GET_GATES2			0x13
// �����
	#define RET_IK400_ADC_GATES2				0x13
	// � BYTE ������ � BYTE ����� ���������. � ������� ��������� 
// ������ 2 ������
//BYTE ������ � BYTE ����� ���������. � ������� ��������� 
#define CMD_IK400_ADC_SET_GATES2			0x14
 //����� RET_S256_ADC_OK

// ��������� N ������ ���������
#define CMD_IK400_ADC_GET_N_LOOP			0x17
// �����
	#define RET_IK400_ADC_N_LOOP				0x15
	// � WORD ������ ��������� 
// ������ N ������ ���������
//WORD ������ ��������� 
#define CMD_IK400_ADC_SET_N_LOOP			0x18
// ����� RET_S256_ADC_OK

// ���������� ���������
#define CMD_IK400_ADC_GET_MEASURE 	    	0x19
// �����
	#define RET_IK400_ADC_MEASURE				0x16
	// unsigned INT64 �������������� ����� �� ������ �������
	// unsigned INT64 �������������� ����� �� ������ �������
	// char Statur - 0�01 ������ ����������, 0�02 ������ �������, 0�04 ������ ����� ����������

// ������ ���������
#define CMD_IK400_ADC_START_MEASURE 		0x1A
// ����� RET_S256_ADC_OK
// ����� ����� ���������, ����������� � ������ �� ������
	#define RET_IK400_ADC_N_MEASURE				0x17
	// unsigned long	���������� ������ ������ ����������
	// unsigned INT64 �������������� ����� �� ������ �������
	// unsigned INT64 �������������� ����� �� ������ �������
	// char Statur 0�01 ������ ����������, 0�02 ������ �������, 0�04 ������ ����� ����������

// ���������� �������� 
#define CMD_IK400_ADC_STOP_MEASURE 	    	0x1C
// ����� RET_S256_ADC_OK

// ���������� ��������� 
#define CMD_IK400_ADC_SLEEP_MEASURE 		0x1D
// ����� RET_S256_ADC_OK

// ��������� ��������� 
#define CMD_IK400_ADC_GO_MEASURE 		   0x1E
// ����� RET_S256_ADC_OK

	// ����� ���� ���� ������
	#define RET_IK400_ADC_ERROR					0x18
	// BYTE ��� ������
	#define CODE_ERROR_IK400_ADC_NO_MODULE			    0x01		//������ ���������� ������� ����������
	#define CODE_ERROR_IK400_ADC_NO_GATES_MODULE		0x02		//������ ������ ���������� ����� �� ����� ��� ����������
	#define CODE_ERROR_IK400_ADC_ERROR_SCALE			0x03		//������ ������ �������

// ��������� ������ ����������
#define CMD_IK400_ADC_GET_MODULE			0x1F
// �����
	#define RET_IK400_ADC_MODULE					0x19
	// WORD ������� ������ ����������
	// WORD ����������� ������ ����������, ������� ���������� ��������
	// WORD ������������ ������ ����������, ������� ���������� �������� 
	// WORD ����������� ������ ���������� ����������� ��� ������ �����
	// WORD ������������ ������ ���������� ����������� ��� ������ �����


//***********************************************
#endif
//***********************************************

#include "F28x_Project.h"
#include "../main.h"

//#include "..\\Phoenixon_GlobalFunc.c "
//#include "..\\Phoenixon_GlobalFunc.c "
//#include "..\\Phoenixon_GlobalFunc.c "
#include "PhoenixonControls_COMMUNICATION.h"
#include "F2837xD_Ipc_drivers.h"
#include "Phoenixon_can.h"


#define DSP_SCIA_MAX_FIFO_LENGTH                (16)
#define DSP_SCIA_START_FLAG                     (0xAAAA)
#define DSP_SCIA_START_FLAG_LENGTH              (3)
#define DSP_SCIA_START_CHEANNEL_INDEX_LENGTH    (2)
#define DSP_SCIA_HEADER_LENGTH                  (DSP_SCIA_START_FLAG_LENGTH + DSP_SCIA_START_CHEANNEL_INDEX_LENGTH)
#define DSP_SCIA_CRC16_LENGTH                   (1)

//#ifdef    DEVICE_CPU2
#define DSP_SCIA_TX_DATA_LENGTH                 (sizeof(S_COMMUNICATION_GSRAM_CPU2))
#define DSP_SCIA_RX_DATA_LENGTH                 (sizeof(S_COMMUNICATION_GSRAM_CPU1))
//#endif

#define DSP_SCIA_TX_TOTAL_LENGTH                (DSP_SCIA_HEADER_LENGTH + DSP_SCIA_TX_DATA_LENGTH + DSP_SCIA_CRC16_LENGTH)
#define DSP_SCIA_RX_TOTAL_LENGTH                (DSP_SCIA_HEADER_LENGTH + DSP_SCIA_RX_DATA_LENGTH + DSP_SCIA_CRC16_LENGTH)

#define DSP_SCIA_TX_DUMY                        (0xBBBB)
#define DSP_SCIA_TX_DUMY_LENGTH                 (DSP_SCIA_TX_TOTAL_LENGTH % DSP_SCIA_MAX_FIFO_LENGTH)
#define DSP_SCIA_RX_DUMY_LENGTH                 (DSP_SCIA_RX_TOTAL_LENGTH % DSP_SCIA_MAX_FIFO_LENGTH)

#define DSP_SCIA_TX_MAX_DATA_LENGTH             (DSP_SCIA_TX_TOTAL_LENGTH + DSP_SCIA_TX_DUMY_LENGTH)
#define DSP_SCIA_RX_MAX_DATA_LENGTH             (DSP_SCIA_RX_TOTAL_LENGTH + DSP_SCIA_RX_DUMY_LENGTH)

#define DSP_SCIA_TX_MAX_BUFFER_LENGTH           (DSP_SCIA_TX_MAX_DATA_LENGTH * 2)
#define DSP_SCIA_RX_MAX_BUFFER_LENGTH           (DSP_SCIA_RX_MAX_DATA_LENGTH * 2)




volatile S_COMMUNICATION_GSRAM_CPU1 GSRAM_STPE[COMMUNICATION_TX_DEVICE_BANK];
volatile S_COMMUNICATION_GSRAM_CPU1 GSRAM_STPE_Buf[COMMUNICATION_TX_DEVICE_BANK];
volatile S_COMMUNICATION_GSRAM_CPU2 GSRAM_DATA[COMMUNICATION_TX_DEVICE_BANK];
volatile Uint16 ui_UpdateflagCPU1 = 0;
volatile Uint16 ui_UpdateflagCPU2 = 0;

volatile Uint16 SCIATxBuff[DSP_SCIA_TX_MAX_BUFFER_LENGTH]   = {0,};
volatile Uint16 SCIARxBuff[DSP_SCIA_RX_MAX_BUFFER_LENGTH]   = {0,};
volatile Uint16 SCIARxBuff_sub[DSP_SCIA_RX_MAX_BUFFER_LENGTH]   = {0,};


volatile Uint16 share_pause_flag = 0;
volatile Uint16 share_can_data[8] = {0,};

volatile Uint16 share_vol_high = 0;
volatile Uint16 share_vol_low  = 0;
volatile int16 share_cur_high = 0;
volatile int16 share_cur_low  = 0;


volatile int32 share_int32_cur_data = 0;
volatile float share_float64_capacity = 0;

volatile Uint16 share_gpio_hw_sync = 0;

volatile Uint16 share_IGBT_error_code = 0;
volatile Uint16 share_Warning_error_code = 0;
volatile Uint16 share_Grid_error_code = 0;
volatile Uint16 share_DC_error_code = 0;
volatile Uint16 share_Fault_error_code = 0;
volatile Uint16 share_step_index = 0;


volatile Uint16 share_step_index_cpu2 = 0;
#pragma DATA_SECTION(share_step_index_cpu2,"SHARER_STEP_INDEX");


volatile Uint16 share_dbg_data[30] = {0,};
#pragma DATA_SECTION(share_dbg_data,"SHARER_DBG_DATA");

#pragma DATA_SECTION(GSRAM_STPE,"SHARER_STEP");
#pragma DATA_SECTION(GSRAM_STPE_Buf,"SHARER_STEP");
#pragma DATA_SECTION(Schedule_refresh_CPU2,"SHARER_STEP");

#pragma DATA_SECTION(GSRAM_DATA,"SHARERAMGS6");
#if 0
#pragma DATA_SECTION(Schedule_refresh_CPU1,"RAMGS_ref_DIS");
#else
#pragma DATA_SECTION(Schedule_refresh_CPU1,"SHARER_GS15_RAM_12");
#endif

#pragma DATA_SECTION(ui_UpdateflagCPU2,"SHARER_FLAG2");
#if 0 // 수정
#pragma DATA_SECTION(ui_UpdateflagCPU1,"SHARERAMGS6");
#else
#pragma DATA_SECTION(ui_UpdateflagCPU1,"SHARER_GS15_RAM_11");
#endif

#pragma DATA_SECTION(SCIARxBuff,"SHARER_RXBUF");
#pragma DATA_SECTION(SCIATxBuff,"SHARER_TXBUF");

//#pragma DATA_SECTION(Save_log_data_EN,"SHARER_ENFLAG");

#pragma DATA_SECTION(share_pause_flag,"SHARER_PAUSE_FLAG");
#pragma DATA_SECTION(share_can_data,"SHARER_CAN_DATA");


#pragma DATA_SECTION(share_vol_high,"SHARER_GS15_RAM");
#pragma DATA_SECTION(share_vol_low,"SHARER_GS15_RAM_2");
#pragma DATA_SECTION(share_cur_high,"SHARER_GS15_RAM_3");
#pragma DATA_SECTION(share_cur_low,"SHARER_GS15_RAM_4");
#pragma DATA_SECTION(share_Warning_error_code,"SHARER_GS15_RAM_5");
#pragma DATA_SECTION(share_Grid_error_code,"SHARER_GS15_RAM_6");
#pragma DATA_SECTION(share_DC_error_code,"SHARER_GS15_RAM_7");
#pragma DATA_SECTION(share_Fault_error_code,"SHARER_GS15_RAM_8");

#pragma DATA_SECTION(share_cmd_data,"SHARER_GS15_RAM_9");
#pragma DATA_SECTION(share_op_paracode_data,"SHARER_GS15_RAM_10");
#pragma DATA_SECTION(share_step_index,"SHARER_GS15_RAM_13");

#pragma DATA_SECTION(share_IGBT_error_code,"SHARER_GS15_RAM_14");
#pragma DATA_SECTION(share_gpio_hw_sync,   "SHARER_GS15_RAM_15");


#pragma DATA_SECTION(share_int32_cur_data,"SHARER_GS15_RAM_16");
#pragma DATA_SECTION(share_float64_capacity,"SHARER_GS15_RAM_17");


volatile Uint16 share_cmd_data = 0;
volatile Uint16 share_op_paracode_data = 0;


unsigned int Schedule_refresh_CPU1=0, Schedule_refresh_CPU2=0;


volatile Uint16 bank = 0;


Uint16 pause_flag = 0;
extern void STEP_READ_FUNCTION(unsigned int ui_address);
extern Uint16 START_FUNCTION(void);
extern Uint16 check_command_function(void);
extern Uint16 check_alarm(E_COMMUNICATION_STEP_INDEX * event_end_condition);
extern Uint16 check_jump_function(E_COMMUNICATION_STEP_INDEX * event_end_condition);
extern Uint16 check_pause_function(E_COMMUNICATION_STEP_INDEX * event_end_condition);
extern Uint16 endconditon_buffer_is_data(Uint16 * value);
extern Uint16 check_end_condition_state(E_COMMUNICATION_STEP_INDEX * data_end_condition);
extern void copy_GSRAM_DATA(void);
extern Uint16 check_loop_count(E_COMMUNICATION_STEP_INDEX * step_index);
extern Uint16 check_end_condition_state(E_COMMUNICATION_STEP_INDEX * data_end_condition);
extern Uint16 check_end_condition_state_2(E_COMMUNICATION_STEP_INDEX * data_end_condition);
extern void END_FUNCTION_2(void);
extern void push_endcondition_buffer(Uint16 value);
extern void clear_tx_can_buffer(void);
extern void push_tx_can_data(Uint16 * tx_buffer);
extern Uint16 get_current_COMMAND(void);
//--------------------------------------------------------------------------------------------
float End_Current_Condition  = 0;
float End_Voltage_condition  = 0;
float End_Watt_Condition	 = 0;
float End_Capacity_condition = 0;

long sc_End_time_condition = 0;
long sc_End_Voltage_condition = 0;
long sc_End_Current_condition = 0;
long sc_End_Capacity_condition = 0;
long sc_End_Watt_condition = 0;
long sc_Current_ref=0, sc_Voltage_ref=0;
//-------------------------------------------------------------------------------------------


Uint16 sc_Cycle_Loop_cnt = 0;
Uint16 Comm_Loop_Cnt = 1;
Uint16 sc_Sampling_time = 0;
Uint16 sc_STEP_SEND_TIME = 0;
Uint16 sc_STEP_INDEX = 0;
Uint16 prev_sc_STEP_INDEX = 0;
Uint16 sc_End_Time_Index = 0;

Uint16 Control_CMD = 0;
Uint16 MC_CMD = 0;
Uint16 Pause_CMD = 0;

Uint16 sc_Goto_Index = 0;
Uint16 sc_Return_Index = 0;
Uint16 sc_End_Voltage_Index = 0;
Uint16 Schedule_Start_EN = 0;
Uint16 SCI_Err_val_call = 0;
Uint16 prev_sc_Cycle_NO = 0;
Uint16 sc_Cycle_NO = 0;

unsigned int Now_step = 0;
unsigned int prev_Now_step = 0;
unsigned int Pause_Start_EN = 0;
Uint64 Step_End_Condition_val = 0;
Uint64 Time_Span = 0;
unsigned int Comp_Loop_Cnt = 1;
unsigned int prev_Comp_Loop_Cnt = 1;

extern volatile Uint16 index;

volatile Uint16 Step_End_val = 0;
static Uint16 prev_COMMAND = 0;
unsigned int Charge_cc_mode = 0, Discharge_cc_mode = 0, Charge_CCCP = 0, Discharge_CCCP=0;



float64 sum_VoltageAVG = 0;
float64 sum_CurrentAVG = 0;
float64 sum_WattAVG = 0;
float64 VoltageAVG = 0;
float64 CurrentAVG = 0;
float64 WattAVG = 0;




//float Watt_capacity = 0;
float Current_Capacity = 0;
float Watt_val = 0;
Uint64 Time_Span_cnt = 0;
float  Voltage_1ms_avg_data = 0., Current_1ms_avg_data = 0.;
Uint16 cmd_can_data[8] = {0,};
extern Uint16 tx_can_is_data(Uint16 * tx_buffer);
extern void set_current_COMMAND(Uint16 prev_command);
static Uint16 temp_tx_can_buffer[8] = {0,};
extern volatile Uint16 original_count;

volatile Uint16 stop_flag = 0;




void delay_loop(void);
void spi_xmit(Uint16 a);
void spi_fifo_init(void);
void spi_init(void);
void error(void);
void Display_Data_send();
void Display_Data_send_2(void);
//
// Main
//

#define m2048 0
#define m4096 1
#define m8129 0
#if (m2048 == 1)
    float sine_table[3072];
    float pt = -PI, table_div = _2PI / 2048.;
#elif (m4096 == 1)
    float sine_table[6144] = { 0.0, };
    float pt = -PI, table_div = _2PI / 4096.;
#elif (m8129 == 1)
    float sine_table[12288];
    float pt = -PI, table_div = _2PI / 8129.;
#endif

int i = 0;

//Display brd
unsigned int SCI_Data_Temp[6], SCI_Data_Send_Cnt = 0, Test_flag = 1;
float Voltage_3phase = 0., Voltage_dc = 0., Current_dc = 0., Watt_dc = 0.;
//Display brd

extern Uint16 rx_buffer_is_data(Uint16 * rxdata);
static Uint16 send_count = 0;

extern void check_command(Uint16 id,Uint16 * rx_buffer);
Uint16 g_debug_fan_on = 1;

Uint16 main_count = 0;
void main(void)
{
    InitSysCtrl();
    //InitSpiaGpio();

//#ifdef _FLASH
//   InitFlash();// Call Flash Initialization to setup flash waitstates This function must reside in RAM
//#endif
#if 1
   while( !((MemCfgRegs.GSxMSEL.bit.MSEL_GS9) &&
           (MemCfgRegs.GSxMSEL.bit.MSEL_GS10) &&
           (MemCfgRegs.GSxMSEL.bit.MSEL_GS11) &&
           (MemCfgRegs.GSxMSEL.bit.MSEL_GS12) &&
           (MemCfgRegs.GSxMSEL.bit.MSEL_GS13) &&
           (MemCfgRegs.GSxMSEL.bit.MSEL_GS14)))
      {
          EALLOW;
   //       MemCfgRegs.GSxMSEL.bit.MSEL_GS0 = 0;
   //       MemCfgRegs.GSxMSEL.bit.MSEL_GS1 = 1;
          MemCfgRegs.GSxMSEL.bit.MSEL_GS9 = 1;
          MemCfgRegs.GSxMSEL.bit.MSEL_GS10 = 1;
          MemCfgRegs.GSxMSEL.bit.MSEL_GS11 = 1;
          MemCfgRegs.GSxMSEL.bit.MSEL_GS12 = 1;
          MemCfgRegs.GSxMSEL.bit.MSEL_GS13 = 1;
           MemCfgRegs.GSxMSEL.bit.MSEL_GS14 = 1;

          //share_can_data[1] ++;
          EDIS;
      }
#endif

#if 0//
   // DATA의 주도권은 원래 CPU1이지만....CPU2로 임의로 줘서 데이타를 쓸수 있는 지 테스트 해보자!
   while( !(MemCfgRegs.GSxMSEL.bit.MSEL_GS6)) // DATA영역을 CPU2로 넘겨준다.. 그럼 끝!
   {
       EALLOW;
       MemCfgRegs.GSxMSEL.bit.MSEL_GS6 = 1;
       EDIS;
   }
   while( !(MemCfgRegs.GSxMSEL.bit.MSEL_GS7)) // DATA영역을 CPU2로 넘겨준다.. 그럼 끝!
   {
       EALLOW;
       MemCfgRegs.GSxMSEL.bit.MSEL_GS7 = 1;
       EDIS;
   }

   while( !(MemCfgRegs.GSxMSEL.bit.MSEL_GS8)) // DATA영역을 CPU2로 넘겨준다.. 그럼 끝!
   {
       EALLOW;
       MemCfgRegs.GSxMSEL.bit.MSEL_GS8 = 1;
       EDIS;
   }


#endif
    InitGpio();
    CANA_GPIO_INIT();

    Init_Pack_Data();

    Init_Global_Gpio();
    InitEPwmGpio();
    //Extern_ACD_INIT(GPIO_MUX_CPU1);
    EALLOW;
          DevCfgRegs.CPUSEL5.bit.SCI_D = 1;

          GPIO_SetupPinOptions(24, GPIO_OUTPUT, GPIO_PUSHPULL);
          GPIO_SetupPinMux(24, GPIO_MUX_CPU2, 0);
          GPIO_SetupPinOptions(25, GPIO_OUTPUT, GPIO_PUSHPULL);
          GPIO_SetupPinMux(25, GPIO_MUX_CPU2, 0);
          GPIO_SetupPinOptions(26, GPIO_OUTPUT, GPIO_PUSHPULL);
          GPIO_SetupPinMux(26, GPIO_MUX_CPU2, 0);



          // SCID setting CPU2
        GPIO_SetupPinMux(94, GPIO_MUX_CPU2, 6);
        GPIO_SetupPinOptions(94, GPIO_INPUT, GPIO_PUSHPULL);
        GPIO_SetupPinMux(93, GPIO_MUX_CPU2, 6);
        GPIO_SetupPinOptions(93, GPIO_OUTPUT, GPIO_ASYNC);




        // LED use cpu2 test
       // CPU2 에서 데이타 인에이블 핀으로 쓴다.
    //    GPIO_SetupPinMux(35, GPIO_MUX_CPU2,0); // normal gpio setti
    //
    //    GPIO_SetupPinOptions(35,GPIO_OUTPUT,GPIO_PUSHPULL);


        // Data EN pin..
    //    GPIO_SetupPinOptions(52,GPIO_OUTPUT,GPIO_PUSHPULL); // 순서가 상관있나?
    //    GPIO_SetupPinMux(52, GPIO_MUX_CPU2,0);

        GPIO_SetupPinOptions(54,GPIO_OUTPUT,GPIO_PUSHPULL); // 순서가 상관있나?
         GPIO_SetupPinMux(54, GPIO_MUX_CPU2,0);






        EDIS;
    DINT;
        InitPieCtrl();
        IER = 0x0000;
        IFR = 0x0000;
        InitPieVectTable();

        //Init_Ext_Bus();
        //spi_fifo_init();

        DSP_ADC_Init();
        Emif1_AD7679_Init();
        //I2CA_TCA9535_Init();

        Init_Unit_Setting();

        Init_adc_param_18bit();
        CANA_INIT_INIT();

    #if (m2048 == 1)
        for (i = 0; i < 3072; i++)
    #elif (m4096 == 1)
        for (i = 0; i < 6144; i++)
    #elif (m8129 == 1)
        for (i = 0; i < 12288; i++)
    #endif
        {//3072 -> 6144 -> 12288
            sine_table[i] = -sinf(pt);
            pt += table_div;

            if (pt > PI)    { pt -= _2PI; }
            if (pt < -PI)   { pt += _2PI; }
        }

        DELAY_US(100000);
        //while(!I2CA_TCA9535_WriteData(0x02, 0));
        //while(!I2CA_TCA9535_WriteData(0x03, 0));
        //while(!I2CA_TCA9535_WriteData(0x06, 0));
        //while(!I2CA_TCA9535_WriteData(0x07, 0));
        DELAY_US(100000);

        Init_Timer();//1s on/off led

        EALLOW;
            //0: EPWMCLK = SYSCLKOUT = 200MHz
            //1: EPWMCLK = SYSCLKOUT/2 = 100MHz (Default)
        ClkCfgRegs.LOSPCP.all = 0;// 200131 추가 kwk 보레이트를 맞출려면 이걸 넣어야 함! //추가..
        ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 0;
        EDIS;


        InitEPwm_ALL();

        //easyDSP_FlashBootCPU2();
        //easyDSP_SCIBootCPU2();
        easyDSP_SCI_Init();
        ScidSetup();


#define _STANDALONE  1

#if _STANDALONE
    IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);        //CPU2 boot
#endif


    EINT;//Enable Global interrupt INTM
    //ERTM;//Enable Global realtime interrupt DBGM   //Program write 후 '?' 뜰때 주석처리

    HWREG(CANA_BASE + CAN_O_CTL) |= CAN_CTL_TEST;
    HWREG(CANA_BASE + CAN_O_TEST) = 0; //CAN_TEST_EXL;				// Can loop back

    CANEnable(CANA_BASE);
    CANGlobalIntEnable(CANA_BASE, CAN_GLB_INT_CANINT0);
//=================================================================================	end setting


    while (1)
    {
//        if(g_debug_fan_on == 1) {
//            AC_HS_FAN_ON();
//        }

    	main_count ++;

        if (CAN_all_receive)
        {
            Write_Pack_Data();

            can_cnt = 0;
            CAN_all_receive = 0;

            send_count++;
            if (send_count >= 10)
            {
                send_count = 0;

//                Display_Data_send_2(); // 다이렉트로 보내는 대신에 카운트 필요.
            }
        }

        if (m_CanConnectCheck == 0)
        {
//            Display_Data_send();
        }


        static E_COMMUNICATION_STEP_INDEX event_end_condition = INDEX_END_NONE;// JUMP,PAUSE,
        static E_COMMUNICATION_STEP_INDEX step_end_condition = INDEX_END_NONE;//
        static E_COMMUNICATION_STEP_INDEX data_end_condition = INDEX_END_NONE; // To UI program.
        Uint16 bank_offset1 = 0;

#if 1
        if(Schedule_refresh_CPU1 != Schedule_refresh_CPU2)// 100ms 타이머 싱크
#else
        if( ui_UpdateflagCPU1 != ui_UpdateflagCPU2)
#endif
        {
       	stop_flag = 1;


#if 1
       	Schedule_refresh_CPU1 = Schedule_refresh_CPU2;
#else
       	ui_UpdateflagCPU1 = ui_UpdateflagCPU2;
#endif

       	Uint16 bank_offset = bank + 1;
       	index = 99;
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].VOLTAGEx16 = share_vol_high;
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].VOLTAGEx00 = share_vol_low;

			if( share_int32_cur_data < 0 ) share_int32_cur_data *= - 1;

			int32 temp_Cur_2 = share_int32_cur_data;


			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].CURRENTx16 = (((Uint32)temp_Cur_2) >> 16) & 0xFFFF;
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].CURRENTx00 = ((Uint16)temp_Cur_2 & 0xFFFF);

			share_float64_capacity += fabsf(Ibat_rms) * (float)(1.0/3600.0) * (float)(0.001);
			                            // Ibat_rms...
			Uint32 temp32 = (Uint32)(share_float64_capacity * MULTI);
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].CAPACITYx16 = (Uint16)( temp32 >> 16 ) & 0xFFFF;
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].CAPACITYx00 = (Uint16)( temp32 ) & 0xFFFF;

			GSRAM_DATA[bank_offset].CPU2_DATA.ChannelIndex = CH_NUMBER;//default;
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].COMMAND = get_current_COMMAND();
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].SAMPLING_TIME = 1;//default;

			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].STEP_INDEX = Now_step;

			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].OPERATIN = ((share_op_paracode_data >> 4 ) & 0xFF);
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].MODE = 1;
			GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].LOOP_COUNT = prev_Comp_Loop_Cnt - 1;

#if 1
			//if( display_flag == 1)
			//{
			VoltageAVG +=  Vpack_UI_rms ;
			CurrentAVG +=  Ibat_UI_rms ;
			WattAVG    += ( Vpack_UI_rms * Ibat_UI_rms);

			sum_VoltageAVG = VoltageAVG/(float32)(100.0);
			sum_CurrentAVG = CurrentAVG/(float32)(100.0);
			sum_WattAVG    = WattAVG   /(float32)(100.0);

			VoltageAVG = 0;
			CurrentAVG = 0;
			WattAVG = 0;
			//}
#endif




       	    event_end_condition = INDEX_END_NONE;// JUMP,PAUSE,
			step_end_condition = INDEX_END_NONE;//
			data_end_condition = INDEX_END_NONE;//

        	if( START_FUNCTION() > 0 ){}
        	if( check_command_function() > 0 ){}
        	if( check_alarm(&event_end_condition) > 0 ){
        		Uint16 temp_tx_buffer[8] = {0,};
        		temp_tx_buffer[0] = 0x86;
        		push_tx_can_data(temp_tx_buffer);
        		temp_tx_buffer[0] = 0x84;
        		push_tx_can_data(temp_tx_buffer);
        	}
        	if( check_jump_function(&event_end_condition) > 0 ){}
        	if( check_pause_function(&event_end_condition) > 0 ){}
        	if( endconditon_buffer_is_data(&event_end_condition) > 0 )
        	{
        		Uint16 bank_offset1 = bank + 1;
        		GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_STATE		= (Uint16)event_end_condition;
    			GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_VALUEx16	= 0;
    			GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_VALUEx00	= 0;

    		//	check_hear ++;
        	}
        	else
        	{
       // 		check_hear_2 ++;
        		if( pause_flag == 0 )
        		{
        			if( check_end_condition_state_2(&data_end_condition) > 0 )
        			{
        				bank_offset1 = bank + 1;
        				GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_STATE		=  (Uint16)data_end_condition;
						GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_VALUEx16  =  (Step_End_Condition_val >> 16) & 0x0000ffff;
						GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_VALUEx00	=   Step_End_Condition_val & 0x0000ffff;

						if( Schedule_Start_EN > 0 )
						{
							if ( check_loop_count(&step_end_condition) > 0 )
							{

								STEP_READ_FUNCTION((unsigned int)step_end_condition);// goto or return 값을 읽고,
								Now_step = sc_STEP_INDEX;
								share_step_index_cpu2 = Now_step;
							}
							else
							{
								STEP_READ_FUNCTION((unsigned int)data_end_condition);//일반 타임, 전류, 전압 용량, 와트로 고고
								Now_step = sc_STEP_INDEX;
								share_step_index_cpu2 = Now_step;
							}
						}
        			}
        			else
        			{
        				bank_offset1 = bank  + 1;
						GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_STATE		= 0;
						GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_VALUEx16	= 0;
						GSRAM_DATA[bank_offset1].CPU2_DATA.DATA[99].END_CONDITION_VALUEx00	= 0;
        			}
        		}
        		else if ( pause_flag == 1)
        		{

        		}
        	}
        	copy_GSRAM_DATA();
        	if( sc_STEP_INDEX == INDEX_STEP_END)// 0xFFFF
        	{
        		sc_STEP_INDEX = 0;
        		Now_step = 0;
        		Comp_Loop_Cnt = 1;
        		push_endcondition_buffer(INDEX_STEP_END);
        		GSRAM_DATA[0].CPU2_StepIndex = 0;
        		Schedule_Start_EN = 0;
        		clear_tx_can_buffer();
   			Uint16 temp_tx_data[8] = {0,};
				temp_tx_data[0] = 0x86;
				push_tx_can_data(&temp_tx_data[0]);
				temp_tx_data[0] = 0x84;
				push_tx_can_data(&temp_tx_data[0]);
        		END_FUNCTION_2();
        	}
        	else
        	{
        		if( Schedule_Start_EN > 0 )
        		{
        			GSRAM_DATA[0].CPU2_StepIndex = sc_STEP_INDEX;
        		}
        		else
        		{
        			GSRAM_DATA[0].CPU2_StepIndex = 0;
        			Comp_Loop_Cnt = 1;
        		}

        	}

        	Uint16 k = 0;
        	if( tx_can_is_data(&temp_tx_can_buffer[0]))
        	{
        		cmd_can_data[k] = temp_tx_can_buffer[k];
        	}
        	for( k = 0; k < 8; k ++)
        	{
        		cmd_can_data[k] = temp_tx_can_buffer[k];
        	}
        	prev_COMMAND = cmd_can_data[0];
        	set_current_COMMAND(prev_COMMAND);
        	check_command(0x100,&cmd_can_data[0]);
        	bank ++;
        	bank %= 3;
        	original_count = 0;// 타이머 싱크 맞추는것
        	index = 0;
        	stop_flag = 0;

        	//GSRAM_DATA[0].CPU2_Data ++;// 데이터 갱신용 무조건 넣으삼!

        }
    }
}



void error(void)
{//error - Error function that halts the debugger
    asm("     ESTOP0");
    // Test failed!! Stop!
    for (;;)
        ;
}


void spi_xmit(Uint16 a)
{//spi_xmit - Transmit value via SPI
    SpiaRegs.SPITXBUF = a;
}


void spi_fifo_init()
{//spi_fifo_init - Initialize SPIA FIFO
    //Initialize SPI FIFO registers
    SpiaRegs.SPIFFTX.all = 0xE040;
    SpiaRegs.SPIFFRX.all = 0x2044;
    SpiaRegs.SPIFFCT.all = 0x0;

    InitSpi();//Initialize core SPI registers
}



#if 0

void Display_Data_send_2(void)
{
    //Grid
    Voltage_3phase = Va_rms * 0.4222;//AC 380V -> 900V

    //Vpack
    if (Pack_sensing1_flag == 1)
    {
        Voltage_dc = fabsf(Vpack_UI_rms);
    }
    else
    {
        Voltage_dc = 0;
    }

    //Ibat
    if ((SCI_Charge_flag == 1) || (SCI_Discharge_flag == 1))
    {
        Current_dc = Ibat_UI_rms;
    }
    else
    {
        Current_dc = 0.;
    }

    //Watt
    Watt_dc = Voltage_dc * Current_dc * 0.001;

    //Grid
    ScidPut(0x40);
    FloatToArray(6, (float) (Voltage_3phase * 10), &SCI_Data_Temp[0]);
    ScidWirte(&SCI_Data_Temp[0], 6);

#if 0
    //Vpack
    FloatToArray(6, (float)(Voltage_dc * 10), &SCI_Data_Temp[0]);

    if(Vpack_UI_rms <= -0.001)
    {  //0.001
        SCI_Data_Temp[1] = 0x0A;
    }
    else
    {
        SCI_Data_Temp[1] = 0x0F;
    }

    ScidWirte(&SCI_Data_Temp[0], 6);

    //Ibat
    FloatToArray(6, (float)(Current_dc * 10), &SCI_Data_Temp[0]);

    if(SCI_Discharge_flag == 1)
    {
        SCI_Data_Temp[1] = 0x0A;
    }
    else
    {
        SCI_Data_Temp[1] = 0x0F;
    }

    ScidWirte(&SCI_Data_Temp[0], 6);
#else
    //Vpack
    FloatToArray(7, (float) (Voltage_dc * 10), &SCI_Data_Temp[0]);

    if (Vpack_UI_rms <= -0.001)
    {  //0.001
        SCI_Data_Temp[1] = 0x0A;
    }
    else
    {
        SCI_Data_Temp[1] = 0x0F;
    }

    ScidWirte(&SCI_Data_Temp[0], 7);

    //Ibat
    FloatToArray(7, (float) (Current_dc * 10), &SCI_Data_Temp[0]);

    if (SCI_Discharge_flag == 1)
    {
        SCI_Data_Temp[1] = 0x0A;
    }
    else
    {
        SCI_Data_Temp[1] = 0x0F;
    }

    ScidWirte(&SCI_Data_Temp[0], 7);
#endif

    //Watt
    FloatToArray(6, (float) (Watt_dc * 10), &SCI_Data_Temp[0]);

    if (SCI_Discharge_flag == 1)
    {
        SCI_Data_Temp[1] = 0x0A;
    }
    else
    {
        SCI_Data_Temp[1] = 0x0F;
    }

    ScidWirte(&SCI_Data_Temp[0], 6);

#if 1
    if (SCI_Parallel_flag == 1)
        ScidPut(0x01);
    else
        ScidPut(0x00);
#endif
    ScidPut(0x21);
}


void Display_Data_send()
{
    if (SCI_Data_Send_Cnt > 10000)
    {
        SCI_Data_Send_Cnt = 0;

        //Grid
        Voltage_3phase = Va_rms * 0.4222;		// AC 380V -> 900V

        //Vpack
        if (Pack_sensing1_flag == 1)
        {
            Voltage_dc = fabsf(Vpack_UI_rms);
        }
        else
        {
            Voltage_dc = 0;
        }

        //Ibat
        if ((SCI_Charge_flag == 1) || (SCI_Discharge_flag == 1))
        {
            Current_dc = Ibat_UI_rms;
        }
        else
        {
            Current_dc = 0.;
        }

        //Watt
        Watt_dc = Voltage_dc * Current_dc * 0.001;

        //Grid
        ScidPut(0x40);
        FloatToArray(6, (float) (Voltage_3phase * 10), &SCI_Data_Temp[0]);
        ScidWirte(&SCI_Data_Temp[0], 6);

#if 0
        //Vpack
        FloatToArray(6, (float)(Voltage_dc * 10), &SCI_Data_Temp[0]);

        if(Vpack_UI_rms <= -0.001)
        {  //0.001
            SCI_Data_Temp[1] = 0x0A;
        }
        else
        {
            SCI_Data_Temp[1] = 0x0F;
        }

        ScidWirte(&SCI_Data_Temp[0], 6);

        //Ibat
        FloatToArray(6, (float)(Current_dc * 10), &SCI_Data_Temp[0]);

        if(SCI_Discharge_flag == 1)
        {
            SCI_Data_Temp[1] = 0x0A;
        }
        else
        {
            SCI_Data_Temp[1] = 0x0F;
        }

        ScidWirte(&SCI_Data_Temp[0], 6);
#else
        //Vpack
        FloatToArray(7, (float) (Voltage_dc * 10), &SCI_Data_Temp[0]);

        if (Vpack_UI_rms <= -0.001)
        {  //0.001
            SCI_Data_Temp[1] = 0x0A;
        }
        else
        {
            SCI_Data_Temp[1] = 0x0F;
        }

        ScidWirte(&SCI_Data_Temp[0], 7);

        //Ibat
        FloatToArray(7, (float) (Current_dc * 10), &SCI_Data_Temp[0]);

        if (SCI_Discharge_flag == 1)
        {
            SCI_Data_Temp[1] = 0x0A;
        }
        else
        {
            SCI_Data_Temp[1] = 0x0F;
        }

        ScidWirte(&SCI_Data_Temp[0], 7);
#endif

        //Watt
        FloatToArray(6, (float) (Watt_dc * 10), &SCI_Data_Temp[0]);

        if (SCI_Discharge_flag == 1)
        {
            SCI_Data_Temp[1] = 0x0A;
        }
        else
        {
            SCI_Data_Temp[1] = 0x0F;
        }

        ScidWirte(&SCI_Data_Temp[0], 6);

#if 1
        if (SCI_Parallel_flag == 1)
            ScidPut(0x01);
        else
            ScidPut(0x00);
#endif
        ScidPut(0x21);
    }
}


#endif


#define MAX_SIZE  10
Uint16 end_condition_buffer[MAX_SIZE] = {0,};
Uint16 end_condition_buffer_head = 0;
Uint16 end_condition_buffer_tail = 0;
void push_endcondition_buffer(Uint16 value)
{
	end_condition_buffer[end_condition_buffer_head ++] = value;
	if( end_condition_buffer_head >= MAX_SIZE )
		end_condition_buffer_head = 0;
	if( end_condition_buffer_head == end_condition_buffer_tail ){
		end_condition_buffer_tail ++;
		if( end_condition_buffer_tail >= MAX_SIZE)
			end_condition_buffer_tail = 0;
	}
}
Uint16 endconditon_buffer_is_data(Uint16 * value)
{
	if( end_condition_buffer_head == end_condition_buffer_tail )
		return 0;
	* value = end_condition_buffer[end_condition_buffer_tail];
	end_condition_buffer_tail ++;
	end_condition_buffer_tail %= MAX_SIZE;
	return 1;
}
void clear_endconditon_buffer(void)
{
	end_condition_buffer_head = 0;
	end_condition_buffer_tail = 0;
}





#define MAX_TX_CAN_DATA_BUFFER_SIZE   10
static Uint16 tx_can_data_buffer[MAX_TX_CAN_DATA_BUFFER_SIZE][8] = {0,};
static Uint16 tx_can_data_head = 0;
static Uint16 tx_can_data_tail = 0;


void clear_tx_can_buffer(void)
{
	Uint16 k = 0;
	for( k = 0; k < 8 ; k ++) temp_tx_can_buffer[k] = 0;
	tx_can_data_head = 0;
	tx_can_data_tail = 0;
}

void clear_tx_can_data_buffer(void)
{
	tx_can_data_head = 0;
	tx_can_data_tail = 0;
}

Uint16 tx_can_is_data(Uint16 * tx_buffer)
{
	if( tx_can_data_head == tx_can_data_tail )
		return 0;

	Uint16 k = 0;
	for( k = 0; k < 8 ; k ++){
		tx_buffer[k] = tx_can_data_buffer[tx_can_data_tail][k];
	}
	tx_can_data_tail ++;
	tx_can_data_tail %= MAX_TX_CAN_DATA_BUFFER_SIZE;
	return 1;
}
void push_tx_can_data(Uint16 * tx_buffer)
{
	Uint16 k = 0;
	for( k = 0; k < 8 ; k ++){
		tx_can_data_buffer[tx_can_data_head][k] = tx_buffer[k];
	}
	tx_can_data_head ++;
	if( tx_can_data_head >= MAX_TX_CAN_DATA_BUFFER_SIZE){
		tx_can_data_head = 0;
	}
	if( tx_can_data_head == tx_can_data_tail ){
		tx_can_data_tail ++;
		if( tx_can_data_tail >= MAX_TX_CAN_DATA_BUFFER_SIZE){
			tx_can_data_tail = 0;
		}
	}
}




Uint16 check_end_condition_state_2(E_COMMUNICATION_STEP_INDEX * data_end_condition)
{
	Uint16 result = 0;
	Now_step = GSRAM_DATA[0].CPU2_StepIndex;

	Uint32 temp_Vol = (((Uint32)share_vol_high << 16 ) | share_vol_low);
	int32 temp_Cur = (((int32)share_cur_high  << 16 ) | share_cur_low);


	int32 temp_Cur_2 = share_int32_cur_data;


#if 0 // 자리수 맞추기 위한것.
	Voltage_1ms_avg_data = (float)temp_Vol * 0.001;
	//Current_1ms_avg_data = (float)temp_Cur * 0.001;// 100ms에 대한 평균을 내야하니???

	Current_1ms_avg_data = (float)temp_Cur_2 * 0.001;// 100ms에 대한 평균을 내야하니???
#else
	Voltage_1ms_avg_data = (float)temp_Vol * (1/MULTI);
	//Current_1ms_avg_data = (float)temp_Cur * 0.001;// 100ms에 대한 평균을 내야하니???

	Current_1ms_avg_data = (float)temp_Cur_2 *(1/MULTI);// 100ms에 대한 평균을 내야하니???

#endif




	//Uint16 bank_offset = bank + 1;
	//Uint16 index = 99; // 마지막 전류값 및 전압값을 다시 세팅 넣어준다. 그러면 가장 최근 센싱값이 들어가겠지?


	//GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].VOLTAGEx16                = share_vol_high;
	//GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].VOLTAGEx00                = share_vol_low;


	//전류 오케이~~
	//temp_Cur_2 = fabsf(share_int32_cur_data);
	//GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].CURRENTx16 = (((Uint32)temp_Cur_2) >> 16) & 0xFFFF;
	//GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].CURRENTx00 = ((Uint16)temp_Cur_2 & 0xFFFF);


//	Watt_capacity

    // CP일 경우
#if 0
	if(((((share_op_paracode_data >> 4 ) & 0xFF ) == 7 ) || (((share_op_paracode_data >> 4 ) & 0xFF ) == 8)))
	{
#if 0
		Watt_val = Voltage_1ms_avg_data * Current_1ms_avg_data; // 변수는 1ms이지만 실제론 100ms 데이타.
#else
		Watt_val = Voltage_1ms_avg_data * (fabsf(Current_1ms_avg_data));
#endif
#if 1// 100ms일때
		Watt_capacity += ((Watt_val/3600000)*100); // 1초를 다시 1/10(100ms)
#else // 50ms일땐
		Watt_capacity += ((Watt_val/3600000)*50); // 50
#endif

	}
	else
	{
		Watt_val = 0;
		Watt_capacity = 0;
	}
#endif




//=====================================================================================================
			//if(Schedule_Start_EN == 1 && Pause_CMD == 0 && Pause_Start_EN ==1 )
			if(Schedule_Start_EN > 0 )
			{
				//if(i==99)	Time_Span_cnt++;
				Time_Span_cnt ++;

			}
			else
			{
				Time_Span_cnt = Time_Span_cnt;
			}







			//sum_VoltageAVG = VoltageAVG / (float32)(100);
			//float sum_CurrentAVG = CurrentAVG / (float32)(100);
			//float sum_WattAVG

#if 0
			Watt_val = Voltage_1ms_avg_data * (fabsf(Current_1ms_avg_data));
#else
			Watt_val = sum_WattAVG;
#endif

			if(( End_Watt_Condition > 0 ) && ( Watt_val >= End_Watt_Condition))
			{
				* data_end_condition = INDEX_END_WATT;
				Step_End_val = INDEX_END_WATT;
				Step_End_Condition_val = Watt_val*1000;

			}
			else if(( End_Capacity_condition > 0 ) && ( fabsf(share_float64_capacity) >= End_Capacity_condition))//10
			{
				* data_end_condition = INDEX_END_CAPACITY;

				Step_End_val = INDEX_END_CAPACITY;
				Step_End_Condition_val = share_float64_capacity * 1000;//용량 계산하는 변수 필요.

			}
			else if((Time_Span_cnt >= sc_End_time_condition) &&  (sc_End_time_condition > 0))
			{
				* data_end_condition = INDEX_END_TIME;

				Step_End_val = INDEX_END_TIME;
				Step_End_Condition_val = Time_Span_cnt;
				Time_Span_cnt = 0;
				//Watt_capacity = 0;

				result = 1;

			}

			//share_vol_high 평균값을 비교해야한다.
#if 0
			else if( (SCI_Charge_flag == 1) && (Voltage_1ms_avg_data >= End_Voltage_condition) && ((Charge_cc_mode == 1) || (Charge_CCCP == 1)) )
#elif 0
			else if( (share_cmd_data == 0x92) && (Voltage_1ms_avg_data >= End_Voltage_condition) && ((Charge_cc_mode == 1) || (Charge_CCCP == 1)) )

#elif 1
#if 0
			else if( (share_cmd_data == 0x92) && (sum_VoltageAVG >= End_Voltage_condition) && ((Charge_cc_mode == 1) || (Charge_CCCP == 1)) )
#else
			else if( (share_cmd_data == 0x92) && (sum_VoltageAVG >= End_Voltage_condition) && ( End_Voltage_condition > 0 ) && ((Charge_cc_mode == 1) || (Charge_CCCP == 1)) )
#endif
#else
#endif
			{
				* data_end_condition = INDEX_END_VOLTAGE;


				Step_End_val = INDEX_END_VOLTAGE;
#if 0
				Step_End_Condition_val = Voltage_1ms_avg_data*1000;
#else
				Step_End_Condition_val = sum_VoltageAVG * 1000;
#endif
				Time_Span_cnt = 0;
				//Watt_capacity = 0;
				result = 1;
			}
#if 0
			else if( (SCI_Discharge_flag == 1) && (Voltage_1ms_avg_data <= End_Voltage_condition) && ((Discharge_cc_mode == 1) || (Discharge_CCCP == 1)) )
#else
#if 0
			else if( (share_cmd_data == 0x93 ) && (Voltage_1ms_avg_data <= End_Voltage_condition) && ((Discharge_cc_mode == 1) || (Discharge_CCCP == 1)) )
#else
			else if( (share_cmd_data == 0x93 ) && ( sum_VoltageAVG <= End_Voltage_condition) && ((Discharge_cc_mode == 1) || (Discharge_CCCP == 1)) )
#endif
#endif
			{
				* data_end_condition = INDEX_END_VOLTAGE;


				Step_End_val = INDEX_END_VOLTAGE;
#if 0
				Step_End_Condition_val = Voltage_1ms_avg_data*1000;
#else
				Step_End_Condition_val = sum_VoltageAVG * 1000;
#endif
				Time_Span_cnt = 0;
				//Watt_capacity = 0;

				result = 1;
			}
#if 0
#if 0
			else if( (Watt_capacity >= End_Capacity_condition)  &&  ((SCI_CCCV_CP_CR_flag ==7) || (SCI_CCCV_CP_CR_flag == 8)) && (End_Capacity_condition >= 1) )
#else
			else if( (Watt_capacity >= End_Capacity_condition)  &&  ((((share_op_paracode_data >> 4 ) & 0xFF ) == 7) || (((share_op_paracode_data >> 4 ) & 0xFF ) == 8)) && (End_Capacity_condition >= 1) )
#endif
			{

				* data_end_condition = INDEX_END_CAPACITY;

				Step_End_val = INDEX_END_CAPACITY;
				Step_End_Condition_val = Watt_capacity*1000;
				Time_Span_cnt = 0;
				Watt_capacity = 0;
				result = 1;
			}
#endif

			//  CV가 들어갔을때 상황 같음....아마도?
#if 1
#if 0
			else if( ((  SCI_TX_cccv !=3   ) || (SCI_TX_cccv !=5)) &&  (sys_mode > 5))
#else
			else if( ((  ((share_op_paracode_data >> 4 ) & 0xFF ) != 3) || (  ((share_op_paracode_data >> 4 ) & 0xFF ) != 5 ) ) &&   (((share_op_paracode_data >> 4 ) & 0xFF ) > 2) ) // 이거 맞는지 확인 필요
#endif
			{
				//if(i >= 50)
#if 0
				if( fabsf(Current_1ms_avg_data)  <=  End_Current_Condition)
				//if( fabsf(check_cv_cut_curr) <=  End_Current_Condition)
				{

					cutoff_count ++;
					share_dbg_data[17] = cutoff_count;

					if( share_cmd_data == 0x92 )
					{//충전charge) {
						if(cutoff_count >= 4)
						{
							cutoff_count = 0;

							share_dbg_data[19] = 7;



							* data_end_condition = INDEX_END_CURRENT;

							Step_End_val = INDEX_END_CURRENT;
							Step_End_Condition_val = Current_1ms_avg_data*1000;
							Time_Span_cnt = 0;
							Watt_capacity = 0;
							result = 1;
						}
					} else {
						if(cutoff_count >= 4)
						{
							cutoff_count = 0;

							share_dbg_data[19] = 7;



							* data_end_condition = INDEX_END_CURRENT;

							Step_End_val = INDEX_END_CURRENT;
							Step_End_Condition_val = Current_1ms_avg_data*1000;
							Time_Span_cnt = 0;
							Watt_capacity = 0;
							result = 1;
						}
					}
				}
#elif 0
					if( fabsf(Current_1ms_avg_data)  <=  End_Current_Condition)//이하로 걸렜음..
					{



						* data_end_condition = INDEX_END_CURRENT;

						Step_End_val = INDEX_END_CURRENT;
						Step_End_Condition_val = fabsf(Current_1ms_avg_data) *1000;
						Time_Span_cnt = 0;
						Watt_capacity = 0;
						result = 1;
					}
#elif 1



					//sum_VoltageAVG = VoltageAVG/(float32)(100);
					//sum_CurrentAVG = CurrentAVG/(float32)(100);
					//sum_WattAVG
#if 0
					if( fabsf(Current_1ms_avg_data)  <=  End_Current_Condition)//이하로 걸렜음..
#else
					if(fabsf(sum_CurrentAVG) < End_Current_Condition)

#endif
					{



						* data_end_condition = INDEX_END_CURRENT;

						Step_End_val = INDEX_END_CURRENT;
#if 0
						Step_End_Condition_val = fabsf(Current_1ms_avg_data) *1000;
#else
						Step_End_Condition_val = fabsf(sum_CurrentAVG) *1000;
#endif
						Time_Span_cnt = 0;
						//Watt_capacity = 0;
						result = 1;
					}

					//share_dbg_data[2] = 2;
					//share_dbg_data[4] ++;

					//share_dbg_data[15] ++;
					//shre_dbg_data[16] = 8;

#else
				if( share_cmd_data == 0x92 )//충전
				{
					if( Current_1ms_avg_data <=  End_Current_Condition)
					{

						* data_end_condition = INDEX_END_CURRENT;


						Step_End_val = INDEX_END_CURRENT;
						Step_End_Condition_val = Current_1ms_avg_data*1000;
						Time_Span_cnt = 0;
						Watt_capacity = 0;
						result = 1;


						//share_dbg_data[2] = 2;
						//share_dbg_data[4] ++;

						//share_dbg_data[0] ++;
						//share_dbg_data[1] = 7;
					}
				}
				else if( share_cmd_data == 0x93)
				{

					if( Current_1ms_avg_data >=  End_Current_Condition)
					{

						* data_end_condition = INDEX_END_CURRENT;


						Step_End_val = INDEX_END_CURRENT;
						Step_End_Condition_val = Current_1ms_avg_data*1000;
						Time_Span_cnt = 0;
						Watt_capacity = 0;
						result = 1;


						//share_dbg_data[2] = 2;
						//share_dbg_data[4] ++;

						//share_dbg_data[0] ++;
						//share_dbg_data[1] = 7;
					}

				}
#endif

			}
#endif
			else
			{
				Step_End_Condition_val = 0;
				 * data_end_condition = INDEX_END_NONE;
			}


		if( * data_end_condition > 0 )
		{
			result = 1;
		}
		else result = 0;

		//copy_Step_End_val = Step_End_val;

	return result;
}

void set_current_COMMAND(Uint16 prev_command)
{
	prev_COMMAND = prev_command;
}

Uint16 check_loop_count(E_COMMUNICATION_STEP_INDEX * step_index)
{
	Uint16 result = 0;

	if( (sc_Cycle_Loop_cnt > 0) && (Step_End_val))
	{
		if( Comp_Loop_Cnt >= sc_Cycle_Loop_cnt )
		{
			Comp_Loop_Cnt = 1;
			Step_End_val = 0;

			* step_index = INDEX_GOTO;
			result = 1;
		}
		else
		{
			Comp_Loop_Cnt ++;
			Step_End_val = 0;
			* step_index = INDEX_RETURN;
			result = 1;
		}
	}
    return result;
}
void copy_GSRAM_DATA(void)
{
	Uint16 bank_index = bank + 1;

//	static Uint16 bank_index_2 = 0;

	//bank_index_2++;
//	bank_index_2 %= 4;
	//Uint16 i;

	/*
	Uint16 bank_offset = bank + 1;
	for( index = 0; index < 100; index ++)
	{
		GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].COMMAND = get_current_COMMAND();
		GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].SAMPLING_TIME				=	1;
		GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].STEP_INDEX					=	Now_step;

		GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].OPERATIN					=	((share_op_paracode_data >> 4 ) & 0xFF );


		if((share_op_paracode_data & 0xFF ) == 1)						GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].MODE = 3;
		else															GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].MODE = 1;

		GSRAM_DATA[bank_offset].CPU2_DATA.DATA[index].LOOP_COUNT = prev_Comp_Loop_Cnt - 1;
	}
	*/

   //데이타 길이는 인터페이스 보드ㅔ서 계산해서줌..여기서 계산하느건 의미 없음!!
	GSRAM_DATA[bank_index].CPU2_DATA.Header = 0x4044;
#if LOOP_COUNT_ENABLE == 1
	GSRAM_DATA[bank_index].CPU2_DATA.DataCount = 0xC86 + (0x02 * 100);
#else
	GSRAM_DATA[0].CPU2_DATA.DataCount = 0xC86;
#endif
	GSRAM_DATA[bank_index].CPU2_DATA.ChannelIndex = CH_NUMBER;


	GSRAM_DATA[0].CPU2_Data ++;//무조건 증가//위에처럼 쉬프트하지말고 마지막 다이렉트로 보내도록 하자..
	//GSRAM_DATA[0].CPU2_DataBank = bank_index_2;//무조건 증가//위에처럼 쉬프트하지말고 마지막 다이렉트로 보내도록 하자..
	// 왜 제대로 안되는지 모르겠지만....

	GSRAM_DATA[0].CPU2_DATA = GSRAM_DATA[bank_index].CPU2_DATA;

	GSRAM_DATA[0].CPU2_Alarm = GSRAM_DATA[bank_index].CPU2_Alarm;// 세가지만 나머지는 모두 0번 인덱스로 다 처리한다.
	GSRAM_DATA[0].CPU2_End   = GSRAM_DATA[bank_index].CPU2_End;
	GSRAM_DATA[0].CPU2_Step  = GSRAM_DATA[bank_index].CPU2_Step;


	//Todo 중요

    GSRAM_DATA[0].CPU2_END   = GSRAM_DATA[bank_index].CPU2_END;
	GSRAM_DATA[0].CPU2_ALARM = GSRAM_DATA[bank_index].CPU2_ALARM;
	   // GSRAM_DATA[0].CPU2_Data ++;  // 내부안에 END프레임도 같이 올라간다...

	GSRAM_DATA[0].CPU2_DATA.Header       = GSRAM_DATA[ bank_index].CPU2_DATA.Header;
	GSRAM_DATA[0].CPU2_DATA.DataCount    = GSRAM_DATA[ bank_index].CPU2_DATA.DataCount;
	GSRAM_DATA[0].CPU2_DATA.ChannelIndex = GSRAM_DATA[ bank_index].CPU2_DATA.ChannelIndex;


	prev_Now_step = Now_step;
	prev_Comp_Loop_Cnt = Comp_Loop_Cnt;




}
Uint16 check_jump = 0;
Uint16 check_jump_function(E_COMMUNICATION_STEP_INDEX * event_end_condition)
{
	Uint16 result = 0;
	static Uint16 prev_state = 0;
	Uint16 curr_state = 0;



	if( GSRAM_DATA[0].CPU2_Jump != GSRAM_STPE[0].CPU1_Jump)
	{
		//share_dbg_data[5] ++;

		//curr_state = INDEX_JUMP;// 무조건 점프된다고 보고...점프 이벤트 보내고 다시들어와서 밑에 값이 맞으면 그것 진행..한번해보자. 밑에는 무조건 참일듯?


		//if(GSRAM_STPE[0].CPU1_STEP[0].STEP_INDEX == GSRAM_STPE[0].CPU1_JUMP.STEP_INDEX)
		if( GSRAM_DATA[0].CPU2_StepIndex == GSRAM_STPE[0].CPU1_JUMP.STEP_INDEX )
		  // (GSRAM_STPE[0].CPU1_STEP[0].STEP_INDEX == GSRAM_STPE[0].CPU1_JUMP.STEP_INDEX))// 이건 무조건 참이다...이건아니야..
		{

			//Stage_STPE_UPDATE(0);



#if 1
			if( prev_sc_Cycle_NO == sc_Cycle_NO)
			{
				if( prev_sc_STEP_INDEX  >  sc_STEP_INDEX )
				{
					check_jump ++;
					Comp_Loop_Cnt ++;// 같은 사이클에서 지금 스텝보다 이전으로 돌아가면 루푸카운트 플플
				}
			}


#endif
			clear_tx_can_buffer(); //버퍼도 초기화 하자..왜냐면  0x86레스트 나갔던게 있어서?
				if( Schedule_Start_EN > 0 )
				{
					STEP_READ_FUNCTION(0);
				}
				GSRAM_DATA[0].CPU2_Jump = GSRAM_STPE[0].CPU1_Jump;





			//share_dbg_data[6] ++;
			curr_state = 0;

		}
		else
		{
			GSRAM_DATA[0].CPU2_StepIndex = GSRAM_STPE[0].CPU1_JUMP.STEP_INDEX;
			sc_STEP_INDEX = GSRAM_STPE[0].CPU1_JUMP.STEP_INDEX;// 밑에서 읽어올때 다시 예전에 읽은 값을 덮어써버린다.여기에 바로 넣으며 끝..점프 끝

			//강제 세팅이다. 한번만 들어오게..
			//GSRAM_DATA[0].CPU2_Jump = GSRAM_STPE[0].CPU1_Jump;


		//	GSRAM_DATA[0].CPU2_Data ++;

			//Time_Span_cnt = 0;
			//Watt_capacity = 0; //시간과 용량은 초기화 해줘야 한다..다시 시작된 스텝이니깐..


			*event_end_condition = INDEX_JUMP;
			curr_state = INDEX_JUMP;
			result = 1;

			//share_dbg_data[7] ++;
			//share_dbg_data[11] = GSRAM_STPE[0].CPU1_JUMP.STEP_INDEX;

#if 0 //테스트 해보자....레스트 들어갔다가..다음 스텝읽어오고 가면 되니깐..
			// 대신 버퍼에 계속 넣으니깐 다른명령으로 못들어간다 한번만 보내도록 고쳐봅세..
			// 아래 부분에서 한번 만 보내고 준비할껏이다.. 테스트 고
			Uint16 temp_tx_data[8] = {0,};
			clear_tx_can_buffer();
			temp_tx_data[0] = 0x86;
			push_tx_can_data(&temp_tx_data[0]);
#endif

		}
	}
	if( prev_state != curr_state)
	{

		if( curr_state == INDEX_JUMP)
		{
			Uint16 temp_tx_data[8] = {0,};
			clear_tx_can_buffer();
			temp_tx_data[0] = 0x86;
			push_tx_can_data(&temp_tx_data[0]);


			push_endcondition_buffer(curr_state);

			//Time_Span_cnt = 0;
			//Watt_capacity = 0; //시간과 용량은 초기화 해줘야 한다..다시 시작된 스텝이니깐..

		}
		//share_dbg_data[8] ++;
		prev_state = curr_state;

	}

	return result;
}

Uint16 get_current_COMMAND(void)
{
#if 0
	return prev_COMMAND;

#elif 0
	if( prev_COMMAND == 0x86 ) return  0x81;
	else if( prev_COMMAND == 0x82 ) return 0x82;
	else if( prev_COMMAND == 0x83 ) return 0x83;
	else if( prev_COMMAND == 0x80 ) return 0x86;
	else if( prev_COMMAND == 0x81 ) return 0x88;// ok
	else if( prev_COMMAND == 0x84 ) return 0x87;//
	else if( prev_COMMAND == 0x85 ) return 0x89;
	else if( prev_COMMAND == 0x89 ) return 0x84;
	else return 0x81;

#elif 0

	if( Control_CMD == 0x81 ) return 0x81;
	else if( Control_CMD == 0x82) return 0x82;
	else if( Control_CMD == 0x83) return 0x83;
#else


	switch(share_cmd_data & 0xFF)
	{
	default: return 0x00;
	case ( 0x86 + 0x10): return 0x81;// REST
	case ( 0x80 + 0x10): return 0x86;//0x01; input relay on
	case ( 0x81 + 0x10): return 0x88;//0x02; output relay on
	case ( 0x82 + 0x10): return 0x82;// Charge
	case ( 0x83 + 0x10): return 0x83;// Discharge
	case ( 0x84 + 0x10): return 0x87;// Output relay off
	case ( 0x89 + 0x10): return 0x84;//Alarm Clear
	case ( 0x85 + 0x10): return 0x89;//All Relay Off
	}



#endif


}

Uint16 check_pause_function(E_COMMUNICATION_STEP_INDEX * event_end_condition)
{
	Uint16 result = 0;
	Uint16 temp_tx_data[8] = {0,};
	static Uint16 prev_state = 0;
	Uint16 curr_state = 0;

	static Uint16 prev_share_can_data[8] = {0,};
	Uint16 k = 0;

	if(GSRAM_DATA[0].CPU2_Pause != GSRAM_STPE[0].CPU1_Pause)
	{
		//switch(GSRAM_STPE[0].CPU1_PAUSE.COMMAND)
		switch(GSRAM_STPE[0].CPU1_PAUSE.COMMAND)
		{
		default:
			break;
		case PAUSE_STOP:
			curr_state = PAUSE_STOP;

			pause_flag = 0;

			break;
		case PAUSE_PAUSE://1
			//*end_condition_state = INDEX_PAUSE;
			* event_end_condition = INDEX_PAUSE_PAUSE;
			result = 1;

			curr_state = INDEX_PAUSE_PAUSE;


			pause_flag = 1;


			break;
		case PAUSE_COMPLETE://2
			* event_end_condition  = INDEX_PAUSE_COMPLETE;

			result = 1;



			GSRAM_DATA[0].CPU2_Pause = GSRAM_STPE[0].CPU1_Pause;

			GSRAM_DATA[0].CPU2_StepIndex = 0;

#if 1// 이부분이 몬가 이상하다 방전할때 안들어간ㄴㄷㅅ 한다. 일단 여긴 아님...

			Schedule_Start_EN = 0;
	//		Pause_Start_EN = 0;
			Comm_Loop_Cnt = 1;





			clear_tx_can_buffer();
			temp_tx_data[0] = 0x86;
			push_tx_can_data(&temp_tx_data[0]);
			temp_tx_data[0] = 0x84;
			push_tx_can_data(&temp_tx_data[0]);



			END_FUNCTION_2();


#if 1 //추가
			if( pause_flag == 0 )
			{
				GSRAM_DATA[0].CPU2_Start = GSRAM_STPE[0].CPU1_Start;

				GSRAM_DATA[0].CPU2_StepIndex = 0;
				//	STEP_READ_FUNCTION(0);/// 0 에서 2로 테스트 해볼까?
					Schedule_Start_EN = 0;
				//	Pause_Start_EN = 0;

				 // 얼리갔으면....대기 준비 버퍼를 초기화 해주자 이부분 중요...!!


				//	Time_Span_cnt = 0;
					sc_Cycle_Loop_cnt = 0;
					sc_STEP_INDEX = 0;
					Comm_Loop_Cnt = 1;
			}
			else
			{

				clear_endconditon_buffer();
			}

#endif


			//share_dbg_data[17] ++;


#if 0	// 추가해서 되는지 보자.
			sc_STEP_INDEX = 0;

			Now_step = 0;

			push_endcondition_buffer(INDEX_STEP_END);

			GSRAM_DATA[0].CPU2_StepIndex = 0;

			Schedule_Start_EN = 0;
			Pause_Start_EN = 0;

			clear_tx_can_buffer();

			Uint16 temp_tx_data[8] = {0,};
			temp_tx_data[0] = 0x86;
			push_tx_can_data(&temp_tx_data[0]);
			temp_tx_data[0] = 0x84;
			push_tx_can_data(&temp_tx_data[0]);
#endif

#endif
			//share_dbg_data[24] ++;

			//curr_state = PAUSE_COMPLETE;
			// 중요 UI끌때 이것 중요함..
			curr_state = INDEX_PAUSE_COMPLETE;//
			Now_step = 0;
			pause_flag = 0;

			break;
		case PAUSE_RESTART:

			//share_dbg_data[8] ++;
			//*end_condition_state = INDEX_PAUSE;
			GSRAM_DATA[0].CPU2_Pause = GSRAM_STPE[0].CPU1_Pause;

			curr_state = PAUSE_RESTART;// 이값때문에 용량이 나온거다..이거 확인하자.

			pause_flag = 0;
			break;
		}
	}

	if( prev_state != curr_state )// 한번 보내기 위한..버퍼를 만들자.
	{
		if( curr_state == INDEX_PAUSE_PAUSE)
		{
             //여러번 오기때문에 한번만 하기 위해 여기에 넣음..갱신을 안하기 때문..
			for ( k = 0; k < 8; k ++)
			{
				//prev_share_can_data[k] = share_can_data[k];
				prev_share_can_data[k] = temp_tx_can_buffer[k];
			}

			Uint16 temp_tx_data[8] = {0,};
			clear_tx_can_buffer();
			temp_tx_data[0] = 0x86;
			push_tx_can_data(&temp_tx_data[0]);

			push_endcondition_buffer(curr_state);
		}
		else if( curr_state == PAUSE_RESTART)
		{
			clear_tx_can_buffer();


			push_tx_can_data(&prev_share_can_data[0]);

			clear_endconditon_buffer();



		}
		else if( curr_state == INDEX_PAUSE_COMPLETE)
		{

			//if( prev_curr == )
		//	Uint16 temp_tx_data[8] = {0,};
		//	clear_tx_can_buffer();
		//	temp_tx_data[0] = 0x86;
		//	push_tx_can_data(&temp_tx_data[0]);

			//share_dbg_data[2] ++;
			push_endcondition_buffer(curr_state);


		}

	}
	prev_state = curr_state;

	if(( curr_state == INDEX_PAUSE_PAUSE) && ( pause_flag == 1))
	{
		// ??UI가 Pause신호를 계속보낸다....paracode 20 , cmd data 0x10 -> 00일때
		if(( prev_COMMAND != 0x00 ) || ( prev_COMMAND != 0x80 ) || ( prev_COMMAND != 0x81))
		{
			push_endcondition_buffer(INDEX_PAUSE_PAUSE);
		//	share_dbg_data[14] ++;
		}
		else
		{
			//share_dbg_data[15] ++;
		}
	}

	share_pause_flag = pause_flag;// renewal!!!!

	return result;
}

Uint16 check_alarm(E_COMMUNICATION_STEP_INDEX * event_end_condition)
{
	Uint16 result = 0;
	Uint16 bank_offset = bank + 1;
	static Uint16 prev_alarm_state = 0;
	Uint16 alarm_state = 0;

	//한번만 보내야 한다.. 어떻게 보낼까...

	GSRAM_DATA[ bank_offset].CPU2_ALARM.Header = 0x4041;
	GSRAM_DATA[ bank_offset].CPU2_ALARM.DataCount = 16;
	GSRAM_DATA[ bank_offset].CPU2_ALARM.ChannelIndex = CH_NUMBER;
	GSRAM_DATA[ bank_offset].CPU2_ALARM.StepIndx = 16;


	if(share_Warning_error_code | share_Grid_error_code | share_DC_error_code | share_Fault_error_code | share_IGBT_error_code)
	{

		 /*
		    //FAULT ERROR CODE
		    #define Emergency_Stop          0  //1      Emergency Stop
		    #define	Heat_Fan_Fault   		1  //2      Inverter heatsink overtemp
		    #define comm_chk_Fault         11  //800    PC-DSP Communication check Fault

		    //IGBT ERROR CODE
		    #define Drive_Fault_R          0   //1      Drive_Fault_1
		    #define Drive_Fault_S		   1   //2		Drive_Fault_2
		    #define Drive_Fault_T		   2   //4		Drive_Fault_3
		    #define Drive_Fault_SRCP	   3   //8		Drive_Fault_4
		    #define Drive_Fault_SRCS	   4   //10  	Drive_Fault_5
		    #define Drive_Fault_DCDC_T	   5   //20 	Drive_Fault_6
		    #define Drive_Fault_DCDC_B	   6   //40 	Drive_Fault_A
		*/





		GSRAM_DATA[ bank_offset].CPU2_ALARM.WARING.all =	share_Warning_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.GRID.all   =	share_Grid_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.DC.all 	   =	share_DC_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all  =	share_Fault_error_code;

		if( share_IGBT_error_code > 0 )
		{
			if( share_IGBT_error_code & 0x01)	GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all |= ( 1 << 2);
			if( share_IGBT_error_code & 0x02)	GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all |= ( 1 << 3);
			if( share_IGBT_error_code & 0x04)	GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all |= ( 1 << 4);
			if( share_IGBT_error_code & 0x08)	GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all |= ( 1 << 5);
			if( share_IGBT_error_code & 0x10)	GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all |= ( 1 << 6);
			if( share_IGBT_error_code & 0x20)	GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all |= ( 1 << 7);
			if( share_IGBT_error_code & 0x40)	GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all |= ( 1 << 8);

		}

#if 0// 일단 뺌
	    GSRAM_DATA[ bank_offset].CPU2_ALARM.IGBT_FAULT.all = share_IGBT_error_code;
#endif

		Schedule_Start_EN = 0;
		//Pause_Start_EN = 0;



		GSRAM_DATA[0].CPU2_StepIndex = 0;// initialize..
		GSRAM_DATA[ bank_offset].CPU2_StepIndex = 0;



		alarm_state = 1;




		GSRAM_DATA[ bank_offset].CPU2_Alarm ++;
		//share_dbg_data[11] = GSRAM_DATA[bank + 1].CPU2_Alarm;


		GSRAM_DATA[ bank_offset].CPU2_END.Header = 0x4045;
		GSRAM_DATA[ bank_offset].CPU2_END.DataCount = 0x06;
		GSRAM_DATA[ bank_offset].CPU2_END.ChannelIndex = CH_NUMBER;




		//GSRAM_DATA[ bank_offset].CPU2_End ++;

	//	* end_condition_state = INDEX_ALARM;

		*event_end_condition = INDEX_ALARM;
		result = 1;
	}
	else
	{

		GSRAM_DATA[ bank_offset].CPU2_ALARM.WARING.all  =	share_Warning_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.GRID.all 	=	share_Grid_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.DC.all 	    =	share_DC_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all 	=	share_Fault_error_code;
#if 0
		GSRAM_DATA[ bank_offset].CPU2_ALARM.IGBT_FAULT.all = share_IGBT_error_code;
#endif
		//GSRAM_DATA[ bank_offset].CPU2_Alarm ++; // 계속보내는지 확인하고 틀려질때만 보내도록 수정하자

		alarm_state = 0;

	}
	if(SCI_Err_val_call == 1)
	{
#if 1
		GSRAM_DATA[ bank_offset].CPU2_ALARM.WARING.all  =	share_Warning_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.GRID.all 	=	share_Grid_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.DC.all 	    =	share_DC_error_code;
		GSRAM_DATA[ bank_offset].CPU2_ALARM.FAULT.all 	=	share_Fault_error_code;
#if 0
		GSRAM_DATA[ bank_offset].CPU2_ALARM.IGBT_FAULT.all = share_IGBT_error_code;
#endif

		GSRAM_DATA[ bank_offset].CPU2_Alarm ++;
		//End Condition 넣어줘야함.

		result = 1;
#endif
	}


	if( prev_alarm_state != alarm_state)
	{
		if( alarm_state == 1)
		{
			push_endcondition_buffer(INDEX_ALARM);// index 10
		}
		else if( alarm_state == 0 )
		{
			push_endcondition_buffer(INDEX_PAUSE_COMPLETE); // 마지막에 에러클리어가 되면 컴피트 신호 보내기

		}
		prev_alarm_state = alarm_state;
	}
	return result;
}
Uint16 check_command_function(void)
{
	Uint16 result = 0;
	//Uint16 temp_can_data[8] = {0,};
	// Command 값 I/F에서 증가를 해줘야 갱신이 되도록 되어 있다. 확인 하도록..
	if(GSRAM_DATA[0].CPU2_Command != GSRAM_STPE[0].CPU1_Command)
	{

		GSRAM_DATA[0].CPU2_Command = GSRAM_STPE[0].CPU1_Command;


		switch(GSRAM_STPE[0].CPU1_COMMAND.COMMAD)
		{
		default:
			break;
		case 0x0:
			//sc_CMD1 = 0x00;
			break;
		case 0x81:// rest
			//sc_CMD1 = 0x81;
			break;
		case 0x82:// charge
			//sc_CMD1 = 0x82;
			break;
		case 0x83:// discharge // 위의 세가지 명령은 따로 실행하지 않고 스타트를 받으면 스텝정보를 가지고 움직인다. 필요없당..
			//sc_CMD1 = 0x83;
			break;
		case 0x84:// Alarm Clear

			//sc_CMD1 = 0x84;

			clear_tx_can_buffer();




			share_can_data[0] = 0x89;
			share_can_data[1] = 0;
			share_can_data[2] = 0;
			share_can_data[3] = 0;
			share_can_data[4] = 0;
			share_can_data[5] = 0;
			share_can_data[6] = 0;
			share_can_data[7] = 0;


			cmd_can_data[0] = 0x89;
			cmd_can_data[1] = 0;
			cmd_can_data[2] = 0;
			cmd_can_data[3] = 0;
			cmd_can_data[4] = 0;
			cmd_can_data[5] = 0;
			cmd_can_data[6] = 0;
			cmd_can_data[7] = 0;



			//Schedule_refresh_CPU2 ++;

			push_tx_can_data(&cmd_can_data[0]);
			check_command(0x100,&cmd_can_data[0]);


			//GSRAM_DATA[0].CPU2_Start = GSRAM_STPE[0].CPU1_Start;
			//GSRAM_DATA[0].CPU2_StepIndex = 0;


			break;
		case 0x85: // Input Relay Off
			//temp_can_data[0] = 0x85; // Pack on// 입력만끄는 신호는 따로 없다.
			//sc_CMD1 = 0x85;
			break;
		case 0x86: // input Relay On
			if((( share_op_paracode_data >> 4 ) & 0x0F ) < 2)
			{
			//	temp_can_data[0] = 0x80; // 0x81 혹은 0x80 일때 센싱을 킬수 있도록 여기서 AA값을 보내던가. 아니면 DSP cpu1에서 그냥 프래그 강제로 뛰우던가 아니면 데이타 canid AA로 보내던가.
			//	sc_CMD1 = 0x86;
			//	push_tx_can_data_2(&temp_can_data[0]);// 준비하는 버퍼에만 넣는다.
				result = 1;
			}
			break;
		case 0x87://  output relay off
			//temp_can_data[0] = 0x84;
			//sc_CMD1 = 0x87;
			break;
		case 0x88: // output relay on
			if((( share_op_paracode_data >> 4 ) & 0x0F ) < 2)
			{
			//	temp_can_data[0] = 0x81; // Pack on

			// 여기에 센싱하도록 AA도 날려줘야한다 오키?
			// 버퍼 만들까?
			//sc_CMD1 = 0x88;
			//push_tx_can_data_2(&temp_can_data[0]);// 준비하는 버퍼에만 넣는다.
			result = 1;

			}
			break;
		case 0x89: // All relay off
		//	temp_can_data[0] = 0x85; // Pack on
			//sc_CMD1 = 0x89;
			break;
		case 0x90: // Alarm Request
			//sc_CMD1 = 0x90;
			//SCI_Err_val_call = 1;
			break;
		case 0x91: // Mode Parallel
		//	sc_CMD1 = 0x90; // 문서에 없다.  최신문서 필요.
			break;
		case 0x92: // Mode Normal
		//	sc_CMD1 = 0x90;   // 위와 똑같은 데이타 확인 필요.
			break;
		case 0x98: // Charge CV
			break;

		}
		//push_tx_can_data_2(&temp_can_data[0]);// 준비하는 버퍼에만 넣는다.
		//Schedule_refresh_CPU2 ++;
		result = 1;//
	}

	return result;
}

//Uint16 check_start = 0;
//Uint16 check_start_2 = 0;

//Uint16 check_pass = 0;
//Uint16 check_pass_2 = 0;

//Uint16 check_start_3 = 0;

Uint16 START_FUNCTION(void)
{
	Uint16 result = 0;
	 //check_start = GSRAM_STPE[0].CPU1_Start;
	 //check_start_2 = GSRAM_DATA[0].CPU2_Start;

	// check_start_3 ++;
	if(GSRAM_DATA[0].CPU2_Start != GSRAM_STPE[0].CPU1_Start)
	{
		if((( share_op_paracode_data >> 4 ) & 0x0F ) == 0)
		{
			//0x80 때리기
			Uint16 temp_tx_can_data[8] = {0,};
			temp_tx_can_data[0] = 0x80;
			push_tx_can_data(temp_tx_can_data);

			GSRAM_DATA[0].CPU2_StepIndex = 0;
		//	check_pass ++;
		}
		else if((( share_op_paracode_data >> 4 ) & 0x0F ) == 1)
		{
			Uint16 temp_tx_can_data[8] = {0,};
			temp_tx_can_data[0] = 0x81;
			push_tx_can_data(temp_tx_can_data);

			GSRAM_DATA[0].CPU2_StepIndex = 0;
		//	check_pass_2 ++;
		}
		else if((( share_op_paracode_data >> 4 ) & 0x0F )  >= 2 )
		{
			clear_tx_can_buffer();

			GSRAM_DATA[0].CPU2_Start = GSRAM_STPE[0].CPU1_Start;

			GSRAM_DATA[0].CPU2_StepIndex = 0;
			STEP_READ_FUNCTION(0);/// 0 에서 2로 테스트 해볼까?
			Schedule_Start_EN = 1;
		//	Pause_Start_EN = 1;

		 // 얼리갔으면....대기 준비 버퍼를 초기화 해주자 이부분 중요...!!

		//	Time_Span_cnt = 0;
			sc_Cycle_Loop_cnt = 0;
			sc_STEP_INDEX = 0;
			Comm_Loop_Cnt = 1;
		}
	}

	return result;
}

//Uint16 check_cycle_change = 0;
void STEP_READ_FUNCTION(unsigned int ui_address)
{
	Control_CMD = GSRAM_STPE[0].CPU1_STEP[ui_address].COMMAND;
	Uint16 op_value = 0;

	if(GSRAM_STPE[0].CPU1_STEP[ui_address].OPERATIN == 1)// Charge CC  1 < - > 5
	{
		Charge_cc_mode = 1;
		Discharge_cc_mode = 0;

		Charge_CCCP = 0;
		Discharge_CCCP = 0;
#if 0
		SCI_CCCV_CP_CR_flag = 3;
#else
		op_value = 3;
#endif


	}
	else if(GSRAM_STPE[0].CPU1_STEP[ui_address].OPERATIN == 2)// Charge CV 2 <-> 6
	{
		Charge_cc_mode = 0;
		Discharge_cc_mode = 0;

		Charge_CCCP = 0;
		Discharge_CCCP = 0;
#if 0
		SCI_CCCV_CP_CR_flag = 3;
#else
		op_value = 3;
#endif
	}
	else if (GSRAM_STPE[0].CPU1_STEP[ui_address].OPERATIN == 5)//Discharge CC
	{
		Charge_cc_mode = 0;
		Discharge_cc_mode = 1;

		Charge_CCCP = 0;
		Discharge_CCCP = 0;
#if 0
		SCI_CCCV_CP_CR_flag = 5;
#else
		op_value = 5;
#endif
	}
	else if (GSRAM_STPE[0].CPU1_STEP[ui_address].OPERATIN == 6) //Discharge CV
	{
		Charge_cc_mode = 0;
		Discharge_cc_mode = 0;

		Charge_CCCP = 0;
		Discharge_CCCP = 0;
#if 0
		SCI_CCCV_CP_CR_flag = 5;
#else
		op_value = 5;
#endif
	}
	else if (GSRAM_STPE[0].CPU1_STEP[ui_address].OPERATIN == 3)//Charge CC CP  3 <- 7
	{
		Charge_cc_mode = 0;
		Discharge_cc_mode = 0;

		Charge_CCCP = 1;
		Discharge_CCCP = 0;
#if 0
		SCI_CCCV_CP_CR_flag = 7;
#else
		op_value = 7;
#endif
	}
	else if (GSRAM_STPE[0].CPU1_STEP[ui_address].OPERATIN == 4)//Charge CV CP
	{
		Charge_cc_mode = 0;
		Discharge_cc_mode = 0;
		Charge_CCCP = 0;
		Discharge_CCCP = 0;
#if 0
		SCI_CCCV_CP_CR_flag = 7;
#else
		op_value = 7;
#endif
	}
	else if (GSRAM_STPE[0].CPU1_STEP[ui_address].OPERATIN == 7)//Discharge CC CP  3 <-> 7
	{
		Charge_cc_mode = 0;
		Discharge_cc_mode = 0;

		Charge_CCCP = 0;
		Discharge_CCCP = 1;
#if 0
		SCI_CCCV_CP_CR_flag = 8;
#else
		op_value = 8;
#endif
	}
	else if (GSRAM_STPE[0].CPU1_STEP[ui_address].OPERATIN == 8)//Discharge CV CP
	{
		Charge_cc_mode = 0;
		Discharge_cc_mode = 0;

		Charge_CCCP = 0;
		Discharge_CCCP = 0;
#if 0
		SCI_CCCV_CP_CR_flag = 8;
#else
		op_value = 8;
#endif
	}

#if 0
	else	SCI_CCCV_CP_CR_flag = 0;
#else
	else   op_value = 0;
#endif

	sc_Sampling_time	=	GSRAM_STPE[0].CPU1_STEP[ui_address].SAMPLING_TIME;		//GSRAM Data 부분에서 처리
	sc_STEP_SEND_TIME 	=	GSRAM_STPE[0].CPU1_STEP[ui_address].SEND_TIME;

#if 1
	prev_sc_STEP_INDEX  =   sc_STEP_INDEX;// 전 스텝인덱스 복사.
	sc_STEP_INDEX		=	GSRAM_STPE[0].CPU1_STEP[ui_address].STEP_INDEX;
#endif


	sc_Cycle_Loop_cnt	=	GSRAM_STPE[0].CPU1_STEP[ui_address].CYCLE_LOOP_COUNT;

	sc_Goto_Index		=	GSRAM_STPE[0].CPU1_STEP[ui_address].GOTO_INDEX;
//			GSRAM_STPE[0].CPU1_STEP.RETURN_INDEX

#if 1// 추가함...

	sc_Return_Index    = GSRAM_STPE[0].CPU1_STEP[ui_address].RETURN_INDEX;

#endif


#if 1
	prev_sc_Cycle_NO = sc_Cycle_NO;// 20.6.16 추가
#endif
	sc_Cycle_NO = GSRAM_STPE[0].CPU1_STEP[ui_address].PATTERN_PRIOD;

#if 1
	if( prev_sc_Cycle_NO != sc_Cycle_NO)
	{
		Comp_Loop_Cnt = 1;//initialize...change cycle..


	//	check_cycle_change ++;
	}
#endif

	//share_dbg_data[29] = sc_Cycle_NO ;





    sc_Voltage_ref = GSRAM_STPE[0].CPU1_STEP[ui_address].VOLTAGE_REFx16;
    sc_Voltage_ref <<= 16;
    sc_Voltage_ref |= (GSRAM_STPE[0].CPU1_STEP[ui_address].VOLTAGE_REFx00);

    sc_End_Voltage_condition 	= GSRAM_STPE[0].CPU1_STEP[ui_address].END_VOLTAGE_CONDITIONx16;
    sc_End_Voltage_condition 	<<= 16;
    sc_End_Voltage_condition 	|= (GSRAM_STPE[0].CPU1_STEP[ui_address].END_VOLTAGE_CONDITIONx00);

    sc_End_Voltage_Index = 	GSRAM_STPE[0].CPU1_STEP[ui_address].END_VOLTAGE_INDEX;

	sc_Current_ref = GSRAM_STPE[0].CPU1_STEP[ui_address].CURRENT_REF_16;
	sc_Current_ref <<= 16;
	sc_Current_ref |= (GSRAM_STPE[0].CPU1_STEP[ui_address].CURRENT_REFx00);

	sc_End_time_condition = GSRAM_STPE[0].CPU1_STEP[ui_address].END_TIME_CONDITIONx16;
	sc_End_time_condition <<= 16;
	sc_End_time_condition |= (GSRAM_STPE[0].CPU1_STEP[ui_address].END_TIME_CONDITIONx00);


//			GSRAM_STPE[0].CPU1_STEP.PATTERN_PRIOD;
	sc_End_Time_Index = GSRAM_STPE[0].CPU1_STEP[ui_address].END_TIME_INDEX;

	sc_End_Current_condition = GSRAM_STPE[0].CPU1_STEP[ui_address].END_CURRENT_CONDITIONx16;
	sc_End_Current_condition <<= 16;
	sc_End_Current_condition |= (GSRAM_STPE[0].CPU1_STEP[ui_address].END_CURRENT_CONDITIONx00);
//			GSRAM_STPE[0].CPU1_STEP.END_CURRENT_INDEX;

	sc_End_Capacity_condition = GSRAM_STPE[0].CPU1_STEP[ui_address].END_CAPACITY_CONDITIONx16;
	sc_End_Capacity_condition <<= 16;
	sc_End_Capacity_condition |= (GSRAM_STPE[0].CPU1_STEP[ui_address].END_CAPACITY_CONDITIONx00);
//			GSRAM_STPE[0].CPU1_STEP.END_CAPACITY_CONDITIONx16;
//			GSRAM_STPE[0].CPU1_STEP.END_CAPACITY_CONDITIONx00;
//			GSRAM_STPE[0].CPU1_STEP.END_CAPACITY_INDEX;

	sc_End_Watt_condition = GSRAM_STPE[0].CPU1_STEP[ui_address].END_WATT_CONDITIONx16;
	sc_End_Watt_condition <<= 16;
	sc_End_Watt_condition |= (GSRAM_STPE[0].CPU1_STEP[ui_address].END_WATT_CONDITIONx00);
//			GSRAM_STPE[0].CPU1_STEP.END_WATT_INDEX;



#if 0 // 원래 소스 ,, 그냥 켄을 세팅하자. 엔드조건은 그냥 플래그로 세팅하고.
	SCI_Voltage_ref = (float)(sc_Voltage_ref * 0.001);
	SCI_Current_ref = (float)(sc_Current_ref * 0.001);
#endif
	End_Current_Condition = (float)(sc_End_Current_condition * 0.001);
	End_Voltage_condition = (float)(sc_End_Voltage_condition * 0.001);
	//End_Watt_Condition	  = (float)(sc_End_Watt_condition * 0.001);
	End_Watt_Condition	  = (float)(sc_End_Watt_condition * 1);
	End_Capacity_condition = (float)(sc_End_Capacity_condition * 0.001);



	if( sc_STEP_INDEX == INDEX_STEP_END) return; // 버퍼에 넣지 말아주세요. 끝 나중에 처리...버퍼로 정지시키는것..그리고 엔디컨디션

	// 0. 실제 세팅
	Uint16 temp_can_data[8] = {0,};


	//0. CMD

	if( Control_CMD == 0x81)       temp_can_data[0] = 0x86; // REST  standby
	else if( Control_CMD == 0x82 ) temp_can_data[0] = 0x82; // Charge
	else if( Control_CMD == 0x83 ) temp_can_data[0] = 0x83; // Discharge


	// 1. OPcode
	if( op_value == 3 )       temp_can_data[1] |= 0x30; // CC
	else if( op_value == 5 )  temp_can_data[1] |= 0x50; // 방전 cC
	else if( op_value == 7 )  temp_can_data[1] |= 0x70; // CC
	else if( op_value == 8 )  temp_can_data[1] |= 0x80; // CP




	//3. Voltage

	if( sc_Voltage_ref >= 0 )
	{
		temp_can_data[2] = (sc_Voltage_ref >> 16 ) & 0xFF;
		temp_can_data[3] = (sc_Voltage_ref >> 8  ) & 0xFF;
		temp_can_data[4] = (sc_Voltage_ref >> 0 ) & 0xFF;

	    //share_dbg_data[0] ++;
	}
	//4. Current
    if( sc_Current_ref >= 0 )
    {
    	sc_Current_ref = fabsf(sc_Current_ref);

    	temp_can_data[5] = ( sc_Current_ref >> 16 ) & 0xFF;
    	temp_can_data[6] = ( sc_Current_ref >> 8 ) & 0xFF;
    	temp_can_data[7] = ( sc_Current_ref >> 0 ) & 0xFF;

        //share_dbg_data[1 ] ++;
    }
    else
    {

    }

    // 5.마지막 세팅 고고~~


    //share_can_id = sc_Voltage_ref & 0xFFFF;
/*
    share_dbg_data[0] = temp_can_data[0];
    share_dbg_data[1] = temp_can_data[1];
    share_dbg_data[2] = temp_can_data[2];
    share_dbg_data[3] = temp_can_data[3];
    share_dbg_data[4] = temp_can_data[4];
    share_dbg_data[5] = temp_can_data[5];
    share_dbg_data[6] = temp_can_data[6];
    share_dbg_data[7] = temp_can_data[7];

*/
    // 버퍼에 넣자 그게 맞는것 같다 그래야. 100ms에 하나씩 빼서 보낼수 있다.

    clear_tx_can_data_buffer(); //버퍼 초기화 부분 넣자.

    push_tx_can_data(&temp_can_data[0]);


  //  memcpy((void*)share_can_data,(void*)temp_can_data,8);


	GSRAM_DATA[0].CPU2_Step	= GSRAM_STPE[0].CPU1_Step;


}

void END_FUNCTION_2(void)
{

	Uint16 bank_offset = bank + 1;

	GSRAM_DATA[bank_offset].CPU2_END.Header = 0x4045;//@E
	GSRAM_DATA[bank_offset].CPU2_END.DataCount = 0x06;
	GSRAM_DATA[bank_offset].CPU2_END.ChannelIndex = CH_NUMBER;
	GSRAM_DATA[bank_offset].CPU2_End ++;

	//share_dbg_data[15] ++;
}











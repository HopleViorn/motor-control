#define GLOBALS
#include "VerInfo.h"
#include "EDBclm.h"         // define and function declare
#include "globalvar.h"      //
#include "AD2S.h"
#include "SPI4.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"
Uint16 mm;
extern unsigned char reg[10];
Uint16 myerror,myerror2;
extern int32 MasterPulse;
extern int32 SlavePulse;
extern int32 syncOut;
extern Uint16 CountSpeedLoop;  //5次速度环（10K)做一次同步
extern int32 SyncSpeed;
extern int16 speed3;
extern int32 e_speed;  //第三轴速度;相加/2
extern int32 Delspeed;
extern int32 Masterspeed;
extern char SlaveDataerror;
extern char SlaveDataerror;
extern int32 IError;

extern int32 EncodrIError;

unsigned char A1210Faultdata;
extern Uint16 Pos485;
extern const Uint16 uvwtab[];
extern const Uint16 uvwtab_sumtak[];
extern const int sintab[];
extern const Uint16 MotorPar4[][4][14];
extern void Main_Test(void);
extern void Init_TestData(void);
extern volatile Uint32 Position_Numerator;  //Numerator
extern volatile Uint32 Position_Divisor;    //Feed constant
extern volatile Uint32 Home_Offset;
void WriteFlash(unsigned int pt,unsigned int val);

// Prototype statements for functions found within this file.
#define MotorStopSel_DB         (((memhex00.hex.hex00 & 0x0001) == 0))                      //((memBusSel != 3) && ((memhex00.hex.hex00 & 0x0001) == 0))
#define MotorStopSel_BrakeOFF   (((memhex00.hex.hex00 == 2) || (memhex00.hex.hex00 == 3)))  //((memBusSel != 3) && ((memhex00.hex.hex00 == 2) || (memhex00.hex.hex00 == 3)))
#define MotorStopSel_BrakeZP    (((memhex00.hex.hex00 == 4) || (memhex00.hex.hex00 == 5)))  //((memBusSel != 3) && ((memhex00.hex.hex00 == 4) || (memhex00.hex.hex00 == 5)))
extern int16 PositionCurrentError;
Uint16 sst;
void main(void)
{Uint16 tmp;
 int32 maintemp;
////////////////////////////////////////////////////////////////////////////////////////
// Step 1. Initialize System Control: PLL, WatchDog, enable Peripheral Clocks.
////////////////////////////////////////////////////////////////////////////////////////
    InitSysCtrl();
   
////////////////////////////////////////////////////////////////////////////////////////
// Step 2. Initalize GPIO: 8
////////////////////////////////////////////////////////////////////////////////////////
    InitGpio();
    InitMcbspbGpioOK();
////////////////////////////////////////////////////////////////////////////////////////
// Step 3. Clear all interrupts and initialize PIE vector table: Disable CPU interrupts 
////////////////////////////////////////////////////////////////////////////////////////
    DINT;   // Disable Global Interrupts

    // Initialize PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags are cleared.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in DSP281x_DefaultIsr.c.
    InitPieVectTable();

    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    EALLOW;
    PieVectTable.EPWM1_INT = &SPEED_Loop_ISR;
    //PieVectTable.T4PINT = &SPEED_T4PINT_ISR;      //T4PINT_ISR;
//  PieVectTable.T3UFINT= &CURRENT_T3UFINT_ISR; //T3UFINT_ISR;
    PieVectTable.XINT1  = &ENCODER_PGC_ISR;  //编码器Z外部中断
    PieVectTable.SCIRXINTC = &SCI_RX_ISR;
    PieVectTable.ECAN1INTB = &ECAN_ISR; //ECAN1INTB
    //-------------------------SPI4 ISR------------
    PieVectTable.MRINTB= &Mcbspb_RxINTB_ISR;
    PieVectTable.MXINTB= &Mcbspb_TxINTB_ISR;
    //----------------------------------------------
    EDIS;

    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    MemCopy(&Ramfuncs1LoadStart, &Ramfuncs1LoadEnd, &Ramfuncs1RunStart);
    MemCopy(&Ramfuncs2LoadStart, &Ramfuncs2LoadEnd, &Ramfuncs2RunStart);
    InitFlash();
    InitXintf();
    out_state.bit.RstDCM = 1;
    *REG_SPD = out_state.all;   // RESET FPGA DCM
////////////////////////////////////////////////////////////////////////////////////////
// Step 4. Initialize all the Device Peripherals:
////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------


    InitSpi();

   InitEPwm();

    InitAdc();

    out_state.bit.RstDCM = 0;
    *REG_SPD = out_state.all;   // SET FPGA DCM
//---------------------------------------------------------------------------------
//   ʼ
//---------------------------------------------------------------------------------
    InitVar();
    Pos485=0;
    PositionCurrentError=0;
    for (CheckSum=0,tmp=0;tmp<SumAddr;tmp++)
    {
        CheckSum += Pn[tmp];
    }
//  ⲿ     ߺ͵ ַ ߲
    //for(;;)
    //{
    XRAMerr = 0;
    XRAMerr += XRAMtest((Uint16 *)SAMPLE_BUF,2000);
    XRAMerr += XRAMtest((Uint16 *)TX_BUF,400);
    XRAMerr += XRAMtest((Uint16 *)SX_BUF,400);
    XRAMerr += XRAMtest((Uint16 *)RX_BUF,400);
    XRAMerr += DatalineTest();
    //}
//CANͨѶ  ʼ
    if(memBusSel == 3)
    {
        CO_ResetComm();
        membit00.bit.bit00 = 0;
        Position_Numerator = Pn[Agear1ADD];
        Position_Divisor = Pn[BgearADD];
        Bgear = 1;
        memAgear1 = 1;
        memAgear2 = 1;
    }
//---------------------------------------------------------------------------------
//   ʼ  MACHXO
//---------------------------------------------------------------------------------

    MpInB = QPCNT;

    tmp = memhex00.hex.hex03;       //
    tmp |= (memhex00.hex.hex02<<2); //
    tmp <<= 5;  //2.07 tmp <<= 4;
    tmp |= memCurLoop00.hex.hex00;  //
    tmp |= (memhex02.hex.hex03<<3); // Pulse clk sel 2.07 tmp |= ((memhex02.hex.hex03 ^ 1)<<3);
    *REG_PLS = (tmp<<6);    //2.07 *REG_PLS = (tmp<<7);

    Pg[1] = *GPCNT;
    Pf[1] = QPCNT;
    PGCNT[1] = QPCNT;

    cacuload(Iqn,IPMImax);
    factory_load = cacuload_temp;
    abl_load = cacuload_temp1;



    /////////////////////////////////////

    PieCtrlRegs.PIEIER6.bit.INTx3=1;     // Enable PIE Group 6, INT 5  开mcbpsB中断
    PieCtrlRegs.PIEIER6.bit.INTx4=1;     // Enable PIE Group 6, INT 6

    //-------------------------SPI4 ------------
    SPISLAVEOK();  //32bit
    //---------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////
// Step 5. User specific code, enable interrupts:
////////////////////////////////////////////////////////////////////////////////////////

    UVWsector = EncElect/12 + 25;       //  30
    // FIND ORIGINAL PLACE.
    if(memCurLoop00.hex.hex00 == Encoder_tABCUVW)
    {
        inUVW = (*PG_IN) & 0x0007;
        maintemp = uvwtab[inUVW - 1];
        maintemp *= EncElect;
        encoder = maintemp / 360;
    }
    else if(memCurLoop00.hex.hex00 == Encoder_Resolver)
    {
        readRES_POS();
        state_flag2.bit.RES_read = 1;
    }

//  PIE  ʹ ܸ  ж
// Enable XINT1_INTn in the PIE: Group 1 interrupt 4
   PieCtrlRegs.PIEIER1.bit.INTx4 = 1;   //XINT-1
// Enable EPWM1_INTn in the PIE: Group 3 interrupt 1
   PieCtrlRegs.PIEIER3.bit.INTx1 = 1;   //ePWM-1
// Enable SCIC_INTn in the PIE: Group 8 interrupt 5
   PieCtrlRegs.PIEIER8.bit.INTx5 = 1;   //SCI-C
// Enable CANA_INTn in the PIE: Group 9 interrupt 8
   PieCtrlRegs.PIEIER9.bit.INTx8 = 1;   // CAN-A

// Clear Peripherals Int Flags
   EPwm1Regs.ETCLR.bit.INT = 1;
   PieCtrlRegs.PIEACK.all = 0x0FFF;

//ʹ  CPU ж  INT1,INT3,INT5,INT8,INT12:
    IER |= M_INT1;
    IER |= M_INT3;
    IER |= M_INT8;
    IER |= M_INT9;
    IER=IER |0x20;                            // Enable CPU INT6    MCBPS 中断
  
    XIntruptRegs.XNMICR.bit.POLARITY = 0;   // falling edge  // DPRAM
    XIntruptRegs.XNMICR.bit.SELECT = 1;     // XNMI--> INT13 // DPRAM
    XIntruptRegs.XNMICR.bit.ENABLE = 0;     // Enable INT13  // DPRAM
    //cofigure xint1
    XIntruptRegs.XINT1CR.bit.POLARITY = 0;  //falling edge
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;    //enable xint1
    XIntruptRegs.XINT2CR.bit.POLARITY = 0;  //falling edge
    XIntruptRegs.XINT2CR.bit.ENABLE = 0;    //enable xint2

    CO_DrvState = SwitchOnDisabledState;
    CO_Statusword.bit.SwitchednOnDisabled = 1;

    EINT;//Enable Global interrupt INTM
    ERTM;//Enable Global realtime interrupt DBGM                            // Enable Global Interrupts
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;  // Start all the timers synced
    EDIS;
   //EvbRegs.T3CON.bit.TENABLE = 1;     // start GP timer 3

    /*Au5561[0] = *AU5561_1;
    Au5561[1] = *AU5561_2;
    Au5561[2] = *AU5561_3;
    Au5561[3] = *AU5561_4;
    Au5561[4] = *AU5561_5;
    *AU5561_0 = 3; // Request ID = 3*/
    //PowerUpInitHandshake();       // DPRAM
    DPRAM_init();
    //test inti
    Init_TestData();
//--------------------------------------------------------
//wxw 6.5536ms  λ   // open dog
//--------------------------------------------------------
#if 1
        SPI3Init();   //for1210 IO
        A1210_INIT();
  //      reg[9]= ReadA1210_Fault(); //清除故障代码
    #endif

    EALLOW;
    SysCtrlRegs.WDCR= 0x0028;
    EDIS;
////////////////////////////////////////////////////////////////////////////////////////
// Step 6. IDLE loop. Just sit and loop forever:
////////////////////////////////////////////////////////////////////////////////////////

    for (;;)
    {Uint16 pg_in,cn1_in;

  // myerror2=NewAD2S1210_READ(0xff);
   // McbspbRegs.DXR1.all=0xCCDD;
   // McbspbRegs.DXR2.all=0xAABB;
       if(!state_flag2.bit.Son)
         {
           PositionCurrentError=0;
               // IError=0;
              //  syncOut=0;
              //  speed3=0;
             //  Masterspeed=0;
            }
        if(int_flagx.bit.int_time1 && int_flagx.bit.int_time2)
        {
            KickDog();              // feed dog
            asm("   SETC    INTM");
            int_flagx.bit.int_time1 = 0;
            int_flagx.bit.int_time2 = 0;
            asm("   CLRC    INTM");
        }
        if(memBusSel == 3)
        {
            CO_ProcessMain();
        }
        DPRAM_main_process();
        SCI_COM_Process();
        if(rehome_start_flag==1)        //        ֵ      ʱ          Ϣ
        {
           rehome_start_flag = 0;
           WriteFlash(HOMED_FLAG,0);
        }
        if(already_homed_flag==1)
        {
            already_homed_flag = 0;
            //save the saved position when home end
            Pn[SumAddr] -= (Pn[SinglePosLatZERO] + Pn[SinglePosHatZERO]);
            CheckSum    -= (Pn[SinglePosLatZERO] + Pn[SinglePosHatZERO]);
            Pn[SinglePosLatZERO] = PosL ;
            Pn[SinglePosHatZERO] = PosH ;
            Pn[SumAddr] += (Pn[SinglePosLatZERO] + Pn[SinglePosHatZERO]);
            CheckSum    += (Pn[SinglePosLatZERO] + Pn[SinglePosHatZERO]);

            //save current home offset
            Pn[SumAddr] -= (Pn[HomeOffsetL] + Pn[HomeOffsetH]);
            CheckSum    -= (Pn[HomeOffsetL] + Pn[HomeOffsetH]);
            Pn[HomeOffsetL] = Home_Offset & 0x0000FFFF;
            Pn[HomeOffsetH] = Home_Offset >> 16;
            Pn[SumAddr] += (Pn[HomeOffsetL] + Pn[HomeOffsetH]);
            CheckSum    += (Pn[HomeOffsetL] + Pn[HomeOffsetH]);
            //save Position_Numerator
            Pn[SumAddr] -= (Pn[PosNumL] + Pn[PosNumH]);
            CheckSum    -= (Pn[PosNumL] + Pn[PosNumH]);
            Pn[PosNumL] = Position_Numerator & 0x0000FFFF;
            Pn[PosNumH] = Position_Numerator >> 16;
            Pn[SumAddr] += (Pn[PosNumL] + Pn[PosNumH]);
            CheckSum    += (Pn[PosNumL] + Pn[PosNumH]);
            //save Position_Divisor
            Pn[SumAddr] -= (Pn[PosDivL] + Pn[PosDivH]);
            CheckSum    -= (Pn[PosDivL] + Pn[PosDivH]);
            Pn[PosDivL] = Position_Divisor & 0x0000FFFF;
            Pn[PosDivH] = Position_Divisor >> 16;
            Pn[SumAddr] += (Pn[PosDivL] + Pn[PosDivH]);
            CheckSum    += (Pn[PosDivL] + Pn[PosDivH]);

            //save home done flag
            Pn[SumAddr] -= Pn[HOMED_FLAG];
            CheckSum    -= Pn[HOMED_FLAG];
            Pn[HOMED_FLAG] = 1;
            Pn[SumAddr] += 1;
            CheckSum    += 1;
            SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
            SPI_WriteFRAM(SinglePosLatZERO,&Pn[SinglePosLatZERO],1);
            SPI_WriteFRAM(SinglePosHatZERO,&Pn[SinglePosHatZERO],1);
            SPI_WriteFRAM(HomeOffsetL,&Pn[HomeOffsetL],1);
            SPI_WriteFRAM(HomeOffsetH,&Pn[HomeOffsetH],1);
            SPI_WriteFRAM(PosNumL,&Pn[PosNumL],1);
            SPI_WriteFRAM(PosNumH,&Pn[PosNumH],1);
            SPI_WriteFRAM(PosDivL,&Pn[PosDivL],1);
            SPI_WriteFRAM(PosDivH,&Pn[PosDivH],1);
            SPI_WriteFRAM(HOMED_FLAG,&Pn[HOMED_FLAG],1);
        }
//-------------------------------------------------------
        if(pbus_stw.bit.Alm_Rst && ((memBusSel == 1) || (memBusSel == 2)))          //   DATA       ǰ
        {
            state_flag2.bit.DispAlm = 0;
            state_flag2.bit.HaveAlm = 0;
            state_flag2.bit.ClrAlm = 1;
            pbus_stw.bit.Alm_Rst = 0;
            asm("   SETC    INTM");
            profibus_buf.pbus_STW &= 0xFF7F;
            asm("   CLRC    INTM");
        }
        pbus_stw.all = profibus_buf.pbus_STW;           //     ߿
        pbus_zsw.bit.SVReady1 = state_flag2.bit.SvReady;
        pbus_zsw.bit.SVReady1 = state_flag2.bit.SvReady;
        pbus_zsw.bit.Son = state_flag2.bit.Son;
        pbus_zsw.bit.Son1= state_flag2.bit.Son;
        pbus_zsw.bit.Son2= state_flag2.bit.Son;
        pbus_zsw.bit.COIN = pos_flag4.bit.VCMP_COIN;
        pbus_zsw.bit.CtrlReq = 1;
        pbus_zsw.bit.Alm = state_flag2.bit.DispAlm;
        pbus_zsw.bit.CLT = int_flag3.bit.limitIqr;
        profibus_buf.pbus_ZSW = pbus_zsw.all;
        pg_in = *PG_IN;
        if (!(pg_in & 0x0800))
        {
            state_flag2.bit.haveReadUVW = 0;
        }
        else if(memCurLoop00.hex.hex00 == Encoder_tABC && !state_flag2.bit.haveReadUVW)
        {
            inUVW = (pg_in >> 8) & 0x0007;
            state_flag2.bit.haveReadUVW = 1;
            asm("   SETC    INTM");
            encoder = (int32)uvwtab[inUVW - 1];
            encoder *= EncElect;
            encoder /= 360;
            asm("   CLRC    INTM");
        }
        else if(memCurLoop00.hex.hex00 == Encoder_sABC && !state_flag2.bit.haveReadUVW)
        {
            inUVW = (pg_in >> 8) & 0x0007;
            state_flag2.bit.haveReadUVW = 1;
            asm("   SETC    INTM");
            encoder = (int32)uvwtab_sumtak[inUVW - 1];
            encoder *= EncElect;
            encoder /= 360;
            asm("   CLRC    INTM");
        }

        if(!state_flag2.bit.RES_read & !RES_FAULT && memCurLoop00.hex.hex00 == Encoder_Resolver)
        {
            readRES_POS();
            state_flag2.bit.RES_read = 1;
        }

        tmp = in_filter_out[1];
        tmp ^= (Pn[InputLSBADD] & 0x000F);
        tmp ^= ((Pn[InputMSBADD] << 4) & 0x00F0);
        cn1_in = tmp;
        asm("   SETC    INTM");
        def_input(cn1_in,memCN1DEF14,memCN1DEF58);
        asm("   CLRC    INTM");

        alm_state.all = ((almin_filter_out[1] & 0x0FFF) ^ 0x0FFF);          // xor, if=1 ,alm
        if(Pn[simPowerADD])
        {
            alm_state.bit.LOSRS = 1;
        }
        if(input_state.bit.AlMRST == 0) // I/O         ź
        {
            //
            state_flag2.bit.HaveAlm = 0;
            //    (  ʾ      ־)
            state_flag2.bit.DispAlm = 0;
            //     (        ı ־)
            state_flag2.bit.ClrAlm = 1;
        }
        JudgeAlarm();
        //       ʾ      ж
        if(state_flag2.bit.HaveAlm && (state_flag2.bit.ClrAlm == 0))
        {
            state_flag2.bit.DispAlm = 1;
        }
        if(state_flag2.bit.DispAlm)
        {
            out_state.bit.ALM = 0;
            servo_state.bit.alm = 1;
        }
        else
        {
            out_state.bit.ALM = 1;
            servo_state.bit.alm = 0;
        }
        out_state.bit.ALM ^= ((Pn[OutputNegADD] & 0x0002)>>1);  //1CN_7,8
        *OUTP = out_state.all;
        // PWM7
# if 1
        if((alm_state.bit.PUMP) && (Timers.Pumpcnt < Time_PUMP) && (alarmno != 16))
        {
            #if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
            if (!VoltAlmDisBit.bit.bit00)       //  й ŵ   ʱ Ŵ򿪵 7·
            {
                out_state.bit.PWM7 = 1;
                *DRIVE = out_state.all;
            }
            #else
            out_state.bit.PWM7 = 1;
            *DRIVE = out_state.all;
            #endif
        }
        else
        {
            if((int_flag3.bit.PowerOn == 0) && (int_flag3.bit.HavePower))
            {
                #if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
                if (!VoltAlmDisBit.bit.bit00)
                {
                    out_state.bit.PWM7 = 1;     //   й ŵ   ,    · µ  ŵ  ݵĵ
                }
                #else
                out_state.bit.PWM7 = 1;     //     · µ  ŵ  ݵĵ
                #endif
            }
            else
            {
                out_state.bit.PWM7 = 0;
            }
            *DRIVE = out_state.all;
        }
#endif
        // p_ot, n_ot
        if(input_state.bit.POT && (membit00.bit.bit01 == 0))
        {
            servo_state.bit.p_ot = 1;
            state_flag2.bit.Pot = 1;
        }
        else
        {
            servo_state.bit.p_ot = 0;
            state_flag2.bit.Pot = 0;
        }
        if(input_state.bit.NOT && (membit00.bit.bit02 == 0))
        {
            servo_state.bit.n_ot = 1;
            state_flag2.bit.Not = 1;
        }
        else
        {
            servo_state.bit.n_ot = 0;
            state_flag2.bit.Not = 0;
        }
    //  ϵ ̵
        #if AC_VOLT == AC_400V || (AC_VOLT == AC_200V && DRV_TYPE == DRV_5KW)
        if(int_flag3.bit.PowerFinsih)
        {
            out_state.bit.RYCTL = 1;
            *DRIVE = out_state.all;
        }
        else
        {
            out_state.bit.RYCTL = 0;
            *DRIVE = out_state.all;
        }
        #endif

        // judge sv ready      //DBCTL        û  ʹ
        if(state_flag2.bit.DispAlm || (int_flag3.bit.PowerFinsih == 0)) // б      ϵ û
        {
            state_flag2.bit.SvReady = 0;
            if(state_flag2.bit.PWM_HIZ)     // PWM  Ч  (  PWM     Ժ     ͷż̵   )
            {
                #if AC_VOLT == AC_200V && (DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW)
                out_state.bit.RYCTL = 0;
                #endif
                out_state.bit.DBCTL = 0;
                *DRIVE = out_state.all;
            }
        }
        else if(state_flag2.bit.SvReady == 0)
        {
            state_flag2.bit.SvReady = 1;
            #if AC_VOLT == AC_200V && (DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW)
            out_state.bit.RYCTL = 1;
            #endif
            out_state.bit.DBCTL = 1;
            *DRIVE = out_state.all;
        }
        // PN_OT
        VCMDBUF = CMDref;   //   VCMDBUFȥ ж   Ϊ ˷ ֹVCMD ж ǰ     ֵ  һ
        if(state_flag2.bit.HandPhase || key_flag1.bit.JInertia)
        {// DoneHandPhase
            if ( state_flag2.bit.SvReady
                && Timers.cputon >= Reycle_delay && int_flag3.bit.adjCurZero)
            {
                state_flag2.bit.enSON = 1;
                state_flag2.bit.INorJOG_SOFF = 0;
                if(state_flag2.bit.HaveAlm)
                {
                    state_flag2.bit.Son = 0;
                    state_flag2.bit.enSON = 0;
                    servo_state.bit.svstate = 0;
                    state_flag2.bit.HandPhase = 0;
                    key_flag1.bit.JInertia = 0;
                    asm("   SETC    INTM");
                    int_flag3.bit.J_initia = 0;
                    int_flag3.bit.J_End = 0;
                    asm("   CLRC    INTM");
                    if(input_state.bit.AlMRST)
                    {
                        state_flag2.bit.DispAlm = 1;
                        state_flag2.bit.ClrAlm = 0;
                    }
                }
            }
            else
            {
                state_flag2.bit.Son = 0;
                state_flag2.bit.enSON = 0;
                servo_state.bit.svstate = 0;
            }
        }
        else if(sci_oper_flag.bit.sciStartInertia)
        {
            if(int_flag3.bit.J_End)
            {
                sci_oper_flag.bit.sciStartInertia = 0;
                sci_oper_flag.bit.sciPhaseSON = 0;
                sci_oper_status.bit.sciInertiaStatus = 2;
                state_flag2.bit.Son = 0;
                state_flag2.bit.enSON = 0;
                servo_state.bit.svstate = 0;
                asm("   SETC INTM");
                int_flag3.bit.J_initia = 0;
                int_flag3.bit.J_End = 0;
                asm("   CLRC INTM");
            }
            else if ( state_flag2.bit.SvReady
                   && Timers.cputon >= Reycle_delay && int_flag3.bit.adjCurZero)
            {
                state_flag2.bit.enSON = 1;
                state_flag2.bit.INorJOG_SOFF = 0;
                if(state_flag2.bit.HaveAlm)
                {
                    state_flag2.bit.Son = 0;
                    state_flag2.bit.enSON = 0;
                    servo_state.bit.svstate = 0;
                    state_flag2.bit.HandPhase = 0;
                    key_flag1.bit.JInertia = 0;
                    asm("   SETC    INTM");
                    int_flag3.bit.J_initia = 0;
                    int_flag3.bit.J_End = 0;
                    asm("   CLRC    INTM");
                    if(input_state.bit.AlMRST)
                    {
                        state_flag2.bit.DispAlm = 1;
                        state_flag2.bit.ClrAlm = 0;
                    }
                }
            }
            else
            {
                state_flag2.bit.Son = 0;
                state_flag2.bit.enSON = 0;
                servo_state.bit.svstate = 0;
            }
        }
        else if(sci_oper_flag.bit.sciPhaseSON)
        {
            if(int_flagx.bit.SciFindInitPhase)
            {
                sci_oper_status.bit.sciPhaseStatus = 2;
                sci_oper_flag.bit.sciPhaseSON = 0;
                state_flag2.bit.Son = 0;
                state_flag2.bit.enSON = 0;
                servo_state.bit.svstate = 0;
                asm("   SETC INTM");
                int_flag3.bit.HaveFindInitPos = 0;
                int_flag3.bit.pass360 = 0;
                asm("   CLRC INTM");
            }
            else if(state_flag2.bit.SvReady && (!state_flag2.bit.HaveAlm) && (Timers.cputon >= Reycle_delay))
            {

                state_flag2.bit.enSON = 1;
                state_flag2.bit.INorJOG_SOFF = 0;
            }
            else
            {
                state_flag2.bit.Son = 0;
                state_flag2.bit.enSON = 0;
                servo_state.bit.svstate = 0;
            }
        }
        else if(key_flag1.bit.JOGrun || sci_oper_flag.bit.sciJogson)
        {// JOGServo
                if(state_flag2.bit.SvReady == 0 || Timers.cputon < Reycle_delay
                    || !int_flag3.bit.adjCurZero)
                {
                    state_flag2.bit.Son = 0;
                    state_flag2.bit.enSON = 0;
                    servo_state.bit.svstate = 0;
                }
                else
                {
                    if(state_flag2.bit.JOGsv_on || sci_oper_flag.bit.sciJogson)
                    {
                        if(state_flag2.bit.HaveAlm)
                        {
                            state_flag2.bit.Son = 0;
                            state_flag2.bit.enSON = 0;
                            servo_state.bit.svstate = 0;
                            if(input_state.bit.AlMRST)
                            {
                                state_flag2.bit.DispAlm = 1;
                                state_flag2.bit.ClrAlm = 0;
                            }
                        }
                        else
                        {
                            state_flag2.bit.INorJOG_SOFF = 0; //   JOG_S-OFF  ־
                            state_flag2.bit.enSON = 1;
                        }
                    }
                    else if(key_flag1.bit.JOGrun)
                    {
                        state_flag2.bit.INorJOG_SOFF = 1;
                    }
                    else
                    {
                        state_flag2.bit.INorJOG_SOFF = 1;
                    }
                }
        }
        else
        {
            if(state_flag2.bit.Pot && state_flag2.bit.Not)
            {// PN_all
                state_flag2.bit.PotNotvalid = 1;
                state_flag2.bit.POTvalid = 1;
                state_flag2.bit.NOTvalid = 1;
            }
            else if(state_flag2.bit.Pot && pos_flag4.bit.PCONzcl && int_flag3.bit.ZCL && (VCMDBUF >= 0))
            {// N_OT1
                state_flag2.bit.PotNotvalid = 1;
                state_flag2.bit.POTvalid = 1;
                state_flag2.bit.NOTvalid = 0;
            }
            else if(state_flag2.bit.Pot && (((pos_flag4.bit.PCONzcl==0) || (int_flag3.bit.ZCL==0)))
                   &&(((Vfact>0) || ((Vfact + VCMDBUF) == 0))))
            {// N_OT1
                state_flag2.bit.PotNotvalid = 1;
                state_flag2.bit.POTvalid = 1;
                state_flag2.bit.NOTvalid = 0;
            }
            else if(state_flag2.bit.Pot
                   && (((pos_flag4.bit.PCONzcl==0) || (int_flag3.bit.ZCL==0)
                       )
                     &&((Vfact<=0) && (Vfact + VCMDBUF) && (state_flag2.bit.Son == 0) && (VCMDBUF >= 0)
                       )
                      )
                   )
            {// N_OT1
                state_flag2.bit.PotNotvalid = 1;
                state_flag2.bit.POTvalid = 1;
                state_flag2.bit.NOTvalid = 0;
            }
            else if(state_flag2.bit.Not && pos_flag4.bit.PCONzcl && int_flag3.bit.ZCL && (VCMDBUF <= 0))
            {// nextN_OT1
                state_flag2.bit.PotNotvalid = 1;
                state_flag2.bit.POTvalid = 0;
                state_flag2.bit.NOTvalid = 1;
            }
            else if(state_flag2.bit.Not && (((pos_flag4.bit.PCONzcl==0) || (int_flag3.bit.ZCL==0)))
                   &&(((Vfact<0) || ((Vfact + VCMDBUF) == 0))))
            {// nextN_OT1
                state_flag2.bit.PotNotvalid = 1;
                state_flag2.bit.POTvalid = 0;
                state_flag2.bit.NOTvalid = 1;
            }
            else if(state_flag2.bit.Not
                   && (((pos_flag4.bit.PCONzcl==0) || (int_flag3.bit.ZCL==0)
                       )
                     &&((Vfact>=0) && (Vfact + VCMDBUF) && (state_flag2.bit.Son == 0) && (VCMDBUF <= 0)
                       )
                      )
                   )
            {// nextN_OT1
                state_flag2.bit.PotNotvalid = 1;
                state_flag2.bit.POTvalid = 0;
                state_flag2.bit.NOTvalid = 1;
            }
            else
            {// nextN_OT
                state_flag2.bit.PotNotvalid = 0;    //    PotNot  ־
                state_flag2.bit.nBRKsoff = 0;       //         ƶ   ־
                if(state_flag2.bit.POTvalid == 0)
                {// n_otwork
                    if(state_flag2.bit.Not == 0)
                    {// ClrZp1
                        state_flag2.bit.NOTvalid = 0;   //    N-OT     ñ ־
                        state_flag2.bit.nBRKzcl = 0;    //         ƶ      ǯλ  ־
                    }
                    else if(VCMDBUF>0)
                    {// ClrZp1
                        state_flag2.bit.NOTvalid = 0;   //    N-OT     ñ ־
                        state_flag2.bit.nBRKzcl = 0;    //         ƶ      ǯλ  ־
                    }
                }
                else if(state_flag2.bit.Pot == 0)
                {// ClrZp0
                    state_flag2.bit.POTvalid = 0;   //    P-OT     ñ ־
                    state_flag2.bit.nBRKzcl = 0;    //         ƶ      ǯλ  ־
                }
                else if(VCMDBUF<0)
                {// ClrZp0
                    state_flag2.bit.POTvalid = 0;   //    P-OT     ñ ־
                    state_flag2.bit.nBRKzcl = 0;    //         ƶ      ǯλ  ־
                }
            }

            //  ŷ ͨ ж
            if((state_flag2.bit.HaveDB == 0) && (state_flag2.bit.DB == 0))
            {
                if(state_flag2.bit.SvReady == 0)
                {   // no_on
                    state_flag2.bit.Son = 0;
                    state_flag2.bit.enSON = 0;
                    servo_state.bit.svstate = 0;
                    // OFFstate
                    if(MotorStopSel_DB && (!state_flag2.bit.PWM_HIZ) && (labs(speed) > 15*SpeedUnit))// PWM ر ʱ      DB ƶ   ־
                    {
                        state_flag2.bit.DB = 1;
                    }
                    PN_OTrelay();
                }
                else if(state_flag2.bit.PotNotvalid == 0)
                {   // NoPNOT
                    NoPNOT();
                }
                else if(MotorStopSel_BrakeOFF)
                {// Onlybrake
                    if(labs(speed) > 15*SpeedUnit)
                    {// SetBreak
                        if(state_flag2.bit.nBRKfinish)
                        {
                            // PN_OTrelay
                            PN_OTrelay();
                        }
                        else
                        {
                            state_flag2.bit.nBRKsoff = 1;
                            // NoPNOT
                            NoPNOT();
                        }
                    }
                    else
                    {
                        state_flag2.bit.nBRKfinish = 1;
                        state_flag2.bit.nBRKsoff = 0;
                        state_flag2.bit.Son = 0;
                        state_flag2.bit.enSON = 0;
                        servo_state.bit.svstate = 0;
                        // PN_OTrelay
                        PN_OTrelay();
                    }
                }
                else if(MotorStopSel_BrakeZP)
                {// ZPbrake
                    state_flag2.bit.nBRKzcl = 1;
                    NoPNOT();
                }
                else
                {// no_on
                    state_flag2.bit.Son = 0;
                    state_flag2.bit.enSON = 0;
                    servo_state.bit.svstate = 0;
                    // OFFstate
                    if(MotorStopSel_DB && (!state_flag2.bit.PWM_HIZ) && (labs(speed) > 15*SpeedUnit))// PWM ر ʱ      DB ƶ   ־
                    {
                        state_flag2.bit.DB = 1;
                    }
                    PN_OTrelay();
                }
            }
            else
            {// PN_OTrelay
                PN_OTrelay();
            }
        }// P-OT,N-OT  DB


        // S-OFF   ƶ     ʱ
        // BRstate
        BRstate();

        // S-ON
        // SON_dowith
        if(state_flag2.bit.enSON)           // S-ONԤ
        {
            if((int16)Pn[SONWAITIMADD] >= 0)
            {
                if(state_flag2.bit.HaveBRKout && (state_flag2.bit.INorJOG_SOFF ==0))    //      ⲿ      S-OFF  е   ƶ    ź      ?
                {
                    state_flag2.bit.BRKon = 0;  //  ƶ   ON ͷ
                    if(Timers.SONWAIcnt >= (abs((int16)Pn[SONWAITIMADD])<<1))
                    {
                        #if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
                        if(!int_flag3.bit.Volt_Reached)             //15V  δ        ҪPWM
                        {
                            int_flag3.bit.PWM_Charge = 1;
                            state_flag2.bit.IGBTOFF = 0;
                        }
                        else
                        {
                            asm("   SETC INTM");
                            int_flag3.bit.PWM_Charge = 0;           //15V      󣬳   PWM
                            Timers.ChargeTimer = 0;
                            if (!state_flag2.bit.IGBTOFF)
                            {
                                //EvbRegs.COMCONB.bit.FCMPOE = 0;       //HIZ
                                EALLOW;
                                EPwm1Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM1B HIZ
                                EPwm2Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM2B HIZ
                                EPwm3Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM3B HIZ
                                EDIS;
                                DELAY_US(5);        //  ʱ5us
                                state_flag2.bit.IGBTOFF = 1;        //     ɺ IGBT ѹضϹ
                            }
                            state_flag2.bit.Son = 1;
                            servo_state.bit.svstate = 1;
                            asm("   CLRC INTM");
                        }
                        #else
                        state_flag2.bit.Son = 1;
                        servo_state.bit.svstate = 1;
                        #endif
                    }
                }
                else
                {// DirectSON
                    #if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
                    if(!int_flag3.bit.Volt_Reached)
                    {
                        int_flag3.bit.PWM_Charge = 1;           //15V  δ        ҪPWM
                        state_flag2.bit.IGBTOFF = 0;
                    }
                    else
                    {
                        asm("   SETC INTM");
                        int_flag3.bit.PWM_Charge = 0;           //15V      󣬳   PWM
                        Timers.ChargeTimer = 0;
                        if (!state_flag2.bit.IGBTOFF)
                        {
                            //EvbRegs.COMCONB.bit.FCMPOE = 0;       //HIZ
                            EALLOW;
                            EPwm1Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM1B HIZ
                            EPwm2Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM2B HIZ
                            EPwm3Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM3B HIZ
                            EDIS;
                            DELAY_US(5);        //  ʱ5us
                            state_flag2.bit.IGBTOFF = 1;        //     ɺ IGBT ѹضϹ
                        }
                        state_flag2.bit.Son = 1;
                        servo_state.bit.svstate = 1;
                        asm("   CLRC INTM");
                    }
                    #else
                    state_flag2.bit.Son = 1;
                    servo_state.bit.svstate = 1;
                    #endif
                }
            }
            else
            {// DirectSON
                #if AC_VOLT == AC_200V && DRV_TYPE == DRV_400W
                if(!int_flag3.bit.Volt_Reached)
                {
                    int_flag3.bit.PWM_Charge = 1;           //15V  δ        ҪPWM
                    state_flag2.bit.IGBTOFF = 0;
                }
                else
                {
                    asm("   SETC INTM");
                    int_flag3.bit.PWM_Charge = 0;           //15V      󣬳   PWM
                    Timers.ChargeTimer = 0;
                    if (!state_flag2.bit.IGBTOFF)
                    {
                        //EvbRegs.COMCONB.bit.FCMPOE = 0;       //HIZ
                        EALLOW;
                        EPwm1Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM1B HIZ
                        EPwm2Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM2B HIZ
                        EPwm3Regs.TZCTL.bit.TZB = TZ_HIZ;   //Force EPWM3B HIZ
                        EDIS;
                        DELAY_US(5);        //  ʱ5us
                        state_flag2.bit.IGBTOFF = 1;        //     ɺ IGBT ѹضϹ
                    }
                    state_flag2.bit.Son = 1;
                    servo_state.bit.svstate = 1;
                    asm("   CLRC INTM");
                }
                #else
                state_flag2.bit.Son = 1;
                servo_state.bit.svstate = 1;
                #endif

                if(state_flag2.bit.HaveBRKout && (state_flag2.bit.INorJOG_SOFF ==0))    //      ⲿ      S-OFF  е   ƶ    ź      ?
                {
                    if(Timers.SONWAIcnt >= (abs((int16)Pn[SONWAITIMADD])<<1))
                    {
                        state_flag2.bit.BRKon = 0;  //  ƶ   ON ͷ
                    }
                }
            }
        }

        // bit display
        //     ·  Դ  ʾ
        if(int_flag3.bit.PowerOn)
        {
            //StateBit[0] |= StateBit0_MPower;
            SetBit(StateBit[0],StateBit0_MPower);
            servo_state.bit.mainpower = 1;
        }
        else
        {
            ClrBit(StateBit[0],StateBit0_MPower);
            servo_state.bit.mainpower = 0;
        }
        //   λ  ʾ
        if(pos_flag4.bit.VCMP_COIN)
        {
            SetBit(StateBit[1],StateBit1_VCMP_COIN);
            servo_state.bit.coin = 1;
        }
        else
        {
            ClrBit(StateBit[1],StateBit1_VCMP_COIN);
            servo_state.bit.coin = 0;
        }
        // λ á  ٶȡ    ظ     ʾ
        if((ColBlock == 0) || (ColBlock == 4))
        {// Pcoldisp
            if(Pgerr)
            {
                SetBit(StateBit[0],StateBit0_InPuls);
            }
            else
            {
                ClrBit(StateBit[0],StateBit0_InPuls);
            }
            if(input_state.bit.CLR)
            {
                ClrBit(StateBit[0],StateBit0_InCLR);
            }
            else
            {
                SetBit(StateBit[0],StateBit0_InCLR);
            }
        }
        else
        {
            if((ColBlock == 1) || (ColBlock == 2))
            {// Vcoldisp
                if(labs(VCMD) >= SpeedUnit*Pn[TGONSPDADD])
                {
                    SetBit(StateBit[0],StateBit0_InVref);
                }
                else
                {
                    ClrBit(StateBit[0],StateBit0_InVref);
                }
            }
            // Tcrdisp
            if(abs(Tn) >= 10)
            {
                SetBit(StateBit[0],StateBit0_InTcr);
            }
            else
            {
                ClrBit(StateBit[0],StateBit0_InTcr);
            }
        }
        // TGON źŴ
        if(labs(Unspd) >= SpeedUnit*Pn[TGONSPDADD])
        {
            state_flag2.bit.TGONvalid = 1;
            SetBit(StateBit[0],StateBit0_TGON);
        }
        else
        {
            state_flag2.bit.TGONvalid = 0;
            ClrBit(StateBit[0],StateBit0_TGON);
        }
        //    COIN/VCMP,TGON,S-RDY,CLT,BRK
        // 1CN_78
        SingleOut(memCN1OUTDEF.hex.hex00,0);
        // 1CN_9A
        SingleOut(memCN1OUTDEF.hex.hex01,1);
        // 1CN_BC
        SingleOut(memCN1OUTDEF.hex.hex02,2);

        // S-ON    PWMʹ  , Լ PWMʹ ܺ ĳ   λ
        if(state_flag2.bit.Son)
        {// servoon
            ClrBit(StateBit[1],StateBit1_SVREADY);// {    ʾ}  ŷ ͨ
            if(state_flag2.bit.PWM_HIZ)
            {

                asm("   SETC    INTM");
                state_flag2.bit.enPWM = 1;  //       PWMʹ
                asm("   CLRC    INTM");
                // FIND ORIGINAL PLACE.
                if (memCurLoop00.hex.hex00 == Encoder_tABCUVW)
                {
                    asm("   SETC    INTM");
                    inUVW = pg_in & 0x0007;
                    encoder = (int32)uvwtab[inUVW - 1];
                    encoder *= EncElect;
                    encoder /= 360;
                    UVWalmflag = 0;             // CLRB UVWalmflag,bit0
                    asm("   CLRC    INTM");
                }
            }
        }
        else
        {
            SetBit(StateBit[1],StateBit1_SVREADY); //{  ʾ}  ŷ
            state_flag2.bit.enPWM = 0;
        }

        EncStatus = pg_in;
        Un[Un_Unspd] = Unspd/SpeedUnit;                     // ƽ   ķ    ٶ
        Un[Un_UnVCMD] = UnVCMD/SpeedUnit;                   // ƽ    ģ         ٶ
        if(ColBlock != 6)
        {
            Un[Un_TCMD] = TCMD;                             //
        }
        Un[Un_Tn] = Tn;                                     //  ڲ ת ظ
        Un[Un_Input] = (~cn1_in) & 0x00FF;                  //
        Un[Un_PGUVWABC] = EncStatus & 0x00FF;               // PGUVW&PGERR&ABCBR&UVWBR
        Un[Un_OutST] = (out_state.all & 0x000F) | ((EncStatus>>4) & 0x00F0);                //         ź
        Un[Un_UnEkLSB] = (Uint16)(UnEk & 0xFFFF);           // ƫ       L
        Un[Un_UnEkMSB] = (Uint16)((UnEk>>16) & 0xFFFF);     // ƫ       H
        // 100msˢ  һ
        if(Timers.Un100ms >= 200)
        {
            asm("   SETC    INTM");
            Timers.Un100ms = 0;
            asm("   CLRC    INTM");
            Un[Un_Encoder] = encoder;//Position;//encoder;  //
            if (memCurLoop00.hex.hex00 == Encoder_Resolver)
            {
                switch(memhex02.hex.hex01)
                {
                    case 0:
                        Un[Un_Encoder] >>= 6;
                        break;
                    case 1:
                        Un[Un_Encoder] >>= 4;
                        break;
                    case 2:
                        Un[Un_Encoder] >>= 2;
                        break;
                    default:
                        break;
                }
            }

            if(ColBlock==6)
            {
                Un[Un_TCMD] = AnPressFeed_out / 100;            //   λ  bar
              //Un[Un_Pgspd] = An3display;
                Un[Un_Pgspd] = AnPressBack_out / 100;
            }
            else
            {
                Un[Un_Pgspd] = PgSpd;                       //         ٶ
            }

            if(memBusSel == 3)
            {
                Un[Un_CurPosLSB] = co_abs_count;            //   ǰλ  L
                Un[Un_CurPosMSB] = co_abs_roate;            //   ǰλ  H
            }
            else
            {
                Un[Un_CurPosLSB] = Pfcount;                 //   ǰλ  L
                Un[Un_CurPosMSB] = Pfroate;                 //   ǰλ  H
            }
            Un[Un_PgLSB] = UnPg_count;                      // λ ø   L;
            Un[Un_PgMSB] = UnPg_roate;                      // λ ø   H;
            #if AC_VOLT == AC_400V && DRV_TYPE == DRV_15KW
            Un[17] = Motor_Tem;                         //         ¶
            #endif
            Un[20] = RES_FAULT_LATCH;                       //        汨  λ
            Un[21] = RES_FAULT;                             //    䵱ǰ    λ
        }

        Un[15] = perJload;
        Un[16] = (x_SQRTiq >> 16) * 100 / 18000;
        Un[18] = Iu;
        Un[19] = Iv;

        //Key & Display
        Menu();

        //test
        Main_Test();

    }

}

void JudgeAlarm(void)
{
 Uint16 pg_in,judgeUVW,exio;
 //Uint16 TIMEOUT;
    pg_in = *PG_IN;
    judgeUVW = (pg_in>>8) & 0x0007;
    exio = *EXIO;
    exio &= 0x0002;     //bit1,ADCת
    //TIMEOUT = (* PGSTA_ALM) & 0x0400;
#if 1
    if(mm<100)mm++;
        else
      {
          mm=0;
          //A1210_INIT();
           reg[9]= ReadA1210_Fault();
                myerror2=reg[9]&0x02;//40;  //只检查插没插或断线

                if(myerror2==2)// if((myerror2!=0x40)&&(myerror2!=0x10))
                    {
                        alarmno = 10;
                        state_flag2.bit.HaveAlm = 1;
                       return;
                   }
        }

#endif
    if(SlaveDataerror>=5)
    {
        alarmno = 81;
        state_flag2.bit.HaveAlm = 1;
        return;
    }
    if(XRAMerr)
    {// !=0 :  ⲿ     ߻  ַ ߻ RAM 쳣
        alarmno = 80;
        state_flag2.bit.HaveAlm = 1;
        state_flag2.bit.DispAlm = 1;
        return;
    }
    if((memBusSel == 1) || (memBusSel == 2))
    {// profibus
        if((Timers.DPRAM_HS_Delay > TIME_100MS) && (ModuleStatus.bit.DPRAMerr))
        {// >100ms DPRAM    쳣
            alarmno = 60;
            state_flag2.bit.HaveAlm = 1;
            return;
        }
        if(ModuleStatus.bit.HSerr)
        {// ͨ Ű  쳣---      ֲ  ɹ
            alarmno = 61;
            state_flag2.bit.HaveAlm = 1;
            //state_flag2.bit.DispAlm = 1;
            return;
        }
        if(ModuleStatus.bit.VPC3err)
        {// ͨ Ű  쳣---VPC3
            alarmno = 62;
            state_flag2.bit.HaveAlm = 1;
            ModuleStatus.bit.SCANerr = 0;
            return;
        }
        if(ModuleStatus.bit.DPRAM_EX_Dataerr)
        {// ͨ Ű  쳣---DPRAM   ݽ
            alarmno = 63;
            state_flag2.bit.HaveAlm = 1;
            FieldbusStatus.bit.DPRAM_EX_Dataerr = 0;
            return;
        }
        if(ModuleStatus.bit.DPcommbreakerr)
        {// ͨ Ű  쳣---ͨ Ű
            alarmno = 64;
            state_flag2.bit.HaveAlm = 1;
            return;
        }
    }
    if(memBusSel == 3)
    {// canopen
        if(CANerr_REG.all)
        {// CANͨѶ 쳣
            alarmno = 66;
            state_flag2.bit.HaveAlm = 1;
            return;
        }
        if(CO_HBcons_error_bit)
        {// CAN master heartbeat over time
            alarmno = 67;
            state_flag2.bit.HaveAlm = 1;
            return;
        }
        if(SyncError_TimeEarly)
        {// IP time early
            alarmno = 68;
            state_flag2.bit.HaveAlm = 1;
            return;
        }
        if(SyncError_TimeOut)
        {// IP time out
            alarmno = 69;
            state_flag2.bit.HaveAlm = 1;
            return;
        }
    }
    //
    if((x_SQRTiq>>16) > 18000)
    {
        alarmno = 04;
        state_flag2.bit.HaveAlm = 1;
        return;
    }
    if(memhex01.hex.hex01==14)          //        servo-pumpģʽ  EXADC
    {
        if(state_flag2.bit.EXrignt)
        {
            if(exio)
            {
                alarmno = 40;           //     ģ  ADC
                state_flag2.bit.HaveAlm = 1;
                return;
            }
        }
        else
        {
            alarmno = 41;               //    ģ     ִ
            state_flag2.bit.HaveAlm = 1;
            return;
        }
    }
    // checksum
//  if (CheckSum!=Pn[SumAddr])
//  {
//      alarmno = 1;
//      state_flag2.bit.HaveAlm = 1;
//      return;
//  }
    // over speed
    if(labs(Unspd) >= (500*SpeedUnit + SPEEDlmt))
    {
        alarmno = 3;
        state_flag2.bit.HaveAlm = 1;
        return;
    }
    /*if(!alm_state.bit.ADCErr)
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 2;
        return;
    }*/
    // ƫ
    if(labs(Ek) >= 0x40000000)
    {
        alarmno = 5;
        state_flag2.bit.HaveAlm = 1;
        return;
    }
    //    ӳ  ֱ
    if(pos_flag4.bit.GearErr)
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 7;
        return;
    }
    if(labs(Scur[0]>>11)>400)   // е ƫ
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 8;
        return;
    }
    if(labs(Scur[1]>>11)>400)
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 9;
        return;
    }
    //
    if(pos_flag4.bit.PosErrOValm)
    {
        alarmno = 6;
        state_flag2.bit.HaveAlm = 1;
        return;
    }
    if(Pn[MotorTab]==0 && MotorPar4[memDriveSel][memMotorSel][10]==0)
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 42;
        return;
    }
    // PG ABC
    if(alm_state.bit.ABCBR == 0 && memCurLoop00.hex.hex00 != Encoder_Abs17bit
       && memCurLoop00.hex.hex00 != Encoder_Inc17bit && memCurLoop00.hex.hex00 != Encoder_Resolver
       && Timers.lessEncoderTime >= 1200)
    {
        state_flag2.bit.HaveAlm = 1;
        state_flag2.bit.DispAlm = 1;
        alarmno = 10;
        return;
    }
    // PG UVW
    if(alm_state.bit.UVWBR == 0 && memCurLoop00.hex.hex00 == Encoder_tABCUVW)
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 11;
        return;
    }

//   ӱ        Ͳ ƥ 䱨     qdd by 20110527
    if (memABS_RESOLVER == 0 && memPgSel != 3 && memPgSel != 4) //ABS
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 43;
        return;
    }

    if (memABS_RESOLVER == 1 && memPgSel != 5)  //RESOLVER
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 43;
        return;
    }

    if (memABS_RESOLVER == 2 && memPgSel != 6) //ʡ  ʽ2500
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 43;
        return;
    }

    if((memCurLoop00.hex.hex00 == Encoder_Abs17bit || memCurLoop00.hex.hex00 == Encoder_Inc17bit) && !rst5v)
    {
        if(alm_absenc.bit.TIMEOT)
        {
            if(state_flag2.bit.Son || absALMdelay>=sEncoderALMdelay)
            {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 50;
                return;
            }
        }
        if(memCurLoop00.hex.hex00 == Encoder_Abs17bit && !membit02.bit.bit02)
        {
            if(alm_absenc.bit.OS)
            {
                    state_flag2.bit.HaveAlm = 1;
                    alarmno = 51;       //
                    return;
            }
            if(alm_absenc.bit.ME)
            {
                    state_flag2.bit.HaveAlm = 1;
                    alarmno = 45;       //   Ȧ  Ϣ
                    return;
            }
            if(alm_absenc.bit.OF)
            {
                    state_flag2.bit.HaveAlm = 1;
                    alarmno = 46;       // counter overflow
                    return;
            }
            if(alm_absenc.bit.BE)
            {
                    state_flag2.bit.HaveAlm = 1;
                    alarmno = 47;       //   ص ѹ    2.5v  Ȧ  Ϣ Ѵ
                    return;
            }
            if(alm_absenc.bit.BA)
            {
                    state_flag2.bit.HaveAlm = 1;
                    alarmno = 48;       //   ص ѹ    3.1v  Ȧ  Ϣ  ȷ
                    return;
            }
        }
        /*if(alm_absenc.bit.OS)
        {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 51;       //
                return;
        }*/
        if((alm_absenc.bit.FS) && (memCurLoop00.hex.hex00 == Encoder_Abs17bit))
        {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 52;       // full abs status
                return;
        }
        if(alm_absenc.bit.CE)
        {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 53;       // counting err
                return;
        }
        /*if(alm_absenc.bit.OF)
        {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 53;       // counter overflow
                return;
        }*/
        if(alm_absenc.bit.CONTE && int_flag3.bit.PowerOn)
        {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 54;       // CONTE
                return;
        }
        if(alm_absenc.bit.CRCE && int_flag3.bit.PowerOn)
        {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 55;       // CPLD CRC ERR
                return;
        }
        if(alm_absenc.bit.FOME && int_flag3.bit.PowerOn)
        {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 56;       // FOME
                return;
        }
        if(alm_absenc.bit.SFOME && int_flag3.bit.PowerOn)
        {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 57;       // SFOME
                return;
        }
        if(Timers.wEncTimer>=15000)
        {
            state_flag2.bit.HaveAlm = 1;
            alarmno = 44;       // д        ʱ
            asm("   SETC    INTM");
            Timers.wEncTimer = 0;
            asm("   CLRC    INTM");
            return;
        }
        if(int_flagx.bit.rFshROM && !Pn[MotorTab] && (backcode != 9) && (!alm_absenc.bit.TIMEOT))
        {
            if(sEncROM == 0)
            {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 58;       //     Ϊ
                return;
            }

            if(checkROM != rEncData[EncDataNum - 1] || rEncData[EncDataNum - 2] != 0xAA)
            {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 59;       //    ݸ ʽ
                return;
            }
            if(memDriveSel != rEncData[0] || memMotorSel != rEncData[1] || memPgSel != rEncData[4])
            {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 43;       //                   Ͳ
                return;
            }
        }
    }
    //if((memCurLoop00.hex.hex00 == Encoder_Resolver) && !rst5v)
#if 0
    if(!rst5v)
    {
        //adRES_FAULT();
        A1210Faultdata=ReadA1210_Fault;
        if ( A1210Faultdata)
            RES_FAULT=A1210Faultdata;
            RES_FAULT_LATCH = RES_FAULT;

        //if((RES_FAULT || Timers.RESTimer1 < resolverNoALMdelay) && (Timers.RESTimer >= resolverALMdelay || int_flag3.bit.PowerFinsih))
            if(RES_FAULT && (!int_flag3.bit.PowerFinsih))
            {
            state_flag2.bit.HaveAlm = 1;
            alarmno = 17;
            state_flag2.bit.RES_read = 0;
            return;
        }
    }
#endif
    //
    if(alm_state.bit.OCURR)
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 12;
        return;
    }

    // IGBT
    if (alm_state.bit.OHEAT && !OTEnableBit.bit.bit00)  //IGBT   ȱ
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 18;
        return;
    }

    //  ƶ   ·
    if(alm_state.bit.OCURR7)
    {
        state_flag2.bit.HaveAlm = 1;
        alarmno = 23;
        return;
    }

    //
    #if AC_VOLT == AC_400V && DRV_TYPE == DRV_15KW
    if (!OTEnableBit.bit.bit01)                         //      ȱ
    {
        if (Motor_Tem == 9999)                          //2CNδ ӻ   PT-1000
        {
             state_flag2.bit.HaveAlm = 1;
             alarmno = 22;                              //PT-1000   ߱   ,  2CNδ ӱ        ߱     ʾ
             return;
        }
        else if (Motor_Tem > (int16)Pn[MotorTemLevelADD])
        {
            state_flag2.bit.HaveAlm = 1;
            alarmno = 19;                   //      ȱ
            return;
        }
    }
    #endif

    //        뱨  ʹ
    if(membit03.bit.bit00 &&
        ((memCurLoop00.hex.hex00 == Encoder_tABC || memCurLoop00.hex.hex00 == Encoder_sABC) && (judgeUVW == 0 || judgeUVW == 7) && (pg_in & 0x0800) ||
         memCurLoop00.hex.hex00 == Encoder_tABCUVW && ((pg_in & 0x0007) == 0 || (pg_in & 0x0007) == 7 )))
    {
        // Feifacode
        state_flag2.bit.HaveAlm = 1;
        alarmno = 30;
        return;
    }
    if((memCurLoop00.hex.hex00 == Encoder_tABCUVW) &&
        membit03.bit.bit00 && ((state_flag2.bit.DB == 0) && (state_flag2.bit.Son == 0)))
    {// nextFeifacode
        Uint32 tmp2;
        inUVW = pg_in & 0x0007;     //  õ       ֵ
        UVWencoder = uvwtab[inUVW - 1];
        tmp2 = EncElect;
        tmp2 *= UVWencoder;
        tmp2 /= 360;
        UVWencoder = (int16)tmp2;
        UVWcomm = abs((int)encoder - UVWencoder);   // UVW õ         A  B         Ƚ
        if((UVWcomm<<1) >= EncElect)
        {
            UVWcomm -= EncElect;
        }
        if((labs(speed) <= 300*SpeedUnit) && (UVWalmflag & 0x0001) && (UVWcomm >= UVWsector))
        {
            state_flag2.bit.HaveAlm = 1;
            alarmno = 31;
            return;
        }
        // noUVWALM
        if((UVWalmflag & 0x0002))
        {
            state_flag2.bit.HaveAlm = 1;
            alarmno = 32;
            asm("   SETC INTM");
            UVWalmflag &= 0x0001;
            asm("   CLRC INTM");
            return;
        }
        if(abs(Encoder4time) >= ((EncElect<<2)+1000))
        {// noCpulsAlm
            state_flag2.bit.HaveAlm = 1;
            alarmno = 33;
            return;
        }
    }
    // ģ   ϵ
    if(!Pn[simPowerADD] && int_flag3.bit.PowerOn)
    {
        #if AC_VOLT == AC_400V && DRV_TYPE == DRV_15KW
        if(!SoftAlmEnaBit.bit.bit00 && alm_state.bit.OVOLT && !VoltAlmDisBit.bit.bit02)
        {
            state_flag2.bit.HaveAlm = 1;
            state_flag2.bit.DispAlm = 1;    //     ·
            alarmno = 13;
            return;
        }
        else if (SoftAlmEnaBit.bit.bit00 && !VoltAlmDisBit.bit.bit02)
        {
            if (DC_Volt > 800)
            {
                OV_cnt++;
                if (OV_cnt > 5) //      ⵽5
                {
                    OV_cnt = 0;
                    state_flag2.bit.HaveAlm = 1;
                    state_flag2.bit.DispAlm = 1;    //     ·
                    alarmno = 13;
                    return;
                }
            }
            else
            {
                OV_cnt = 0;
            }
        }
        #else
        if(alm_state.bit.OVOLT && !VoltAlmDisBit.bit.bit02)
        {
            state_flag2.bit.HaveAlm = 1;
            state_flag2.bit.DispAlm = 1;    //     ·
            alarmno = 13;
            return;
        }
        #endif

        if(int_flag3.bit.PowerFinsih)
        {//       Ҫ      ·     ټ  ı

            #if AC_VOLT == AC_400V && DRV_TYPE == DRV_15KW
            if(!SoftAlmEnaBit.bit.bit00 && alm_state.bit.UVOLT && !VoltAlmDisBit.bit.bit01)
            {
                state_flag2.bit.HaveAlm = 1;
                state_flag2.bit.DispAlm = 1;    //     ·
                alarmno = 14;
                return;
            }
            else if (SoftAlmEnaBit.bit.bit00 && !VoltAlmDisBit.bit.bit01)
            {
                if (DC_Volt < 375)
                {
                    UV_cnt++;
                    if (UV_cnt > 10)    //      ⵽10
                    {
                        UV_cnt = 0;
                        state_flag2.bit.HaveAlm = 1;
                        state_flag2.bit.DispAlm = 1;    //     ·
                        alarmno = 14;
                        return;
                    }
                }
                else
                {
                    UV_cnt = 0;
                }
            }
            #else
            //(alm_state.bit.UVOLT && !VoltAlmDisBit.bit.bit01) // Ƿѹ
            //
            //tate_flag2.bit.HaveAlm = 1;
            //tate_flag2.bit.DispAlm = 1;   //
            //larmno = 14;
            //eturn;
            //
            #endif

            if(int_flag3.bit.LOSTRS && !VoltAlmDisBit.bit.bit03)    // ȱ   ־
            {
                state_flag2.bit.HaveAlm = 1;
                state_flag2.bit.DispAlm = 1;    //
                alarmno = 20;
                return;
            }

            if((membit00.bit.bit03) && (int_flag3.bit.ZQDD))        // ˲ͣ
            {
                state_flag2.bit.HaveAlm = 1;
                alarmno = 21;
                return;
            }

        //  if((Timers.Rescnt>=1100) && !VoltAlmDisBit.bit.bit00)
        //  {
        //      state_flag2.bit.HaveAlm = 1;
        //      state_flag2.bit.DispAlm = 1;    //
        //      alarmno = 15;
        //      return;
        //  }

            //if (pumpflag || (Timers.Pumpcnt >= Time_PUMP))
            //{//      쳣
            //  state_flag2.bit.HaveAlm = 1;
            //  state_flag2.bit.DispAlm = 1;    //
            //  alarmno = 16;
            //  return;
            //}
        }
    }
    state_flag2.bit.HaveAlm = 0;
    state_flag2.bit.ClrAlm = 0;
}

void NoPNOT(void)
{
// NoPNOT
    if(Timers.cputon < Reycle_delay || !int_flag3.bit.adjCurZero)
    {   // no_on
        state_flag2.bit.Son = 0;
        state_flag2.bit.enSON = 0;
        servo_state.bit.svstate = 0;
        // OFFstate
        if(MotorStopSel_DB && !state_flag2.bit.PWM_HIZ && (labs(speed) > 15*SpeedUnit))// PWM ر ʱ      DB ƶ   ־
        {
            state_flag2.bit.DB = 1;
        }
        PN_OTrelay();
    }
    else if(membit00.bit.bit00 == 0)// Mem000.0=0
    {   // memsvoff
        if(((input_state.bit.SON == 0) && (memBusSel != 3)) //IO
         ||(pbus_stw.bit.Son && ((memBusSel == 1) || (memBusSel == 2)))// DP
         ||(CO_Statusword.bit.OperEnabled && ((CO_DrvState == OperationEnableState) || (CO_DrvState == QuickStopActiveState)) && (memBusSel == 3)))// CANopen
        {// SON
            state_flag2.bit.INorJOG_SOFF = 0;
            state_flag2.bit.enSON = 1;
            state_flag2.bit.nBRKfinish = 0;
            if(state_flag2.bit.HaveAlm)
            {// almdisp
                state_flag2.bit.Son = 0;
                state_flag2.bit.enSON = 0;
                servo_state.bit.svstate = 0;
                if(input_state.bit.AlMRST)
                {
                    state_flag2.bit.DispAlm = 1;
                    state_flag2.bit.ClrAlm = 0;
                }
            }
            PN_OTrelay();
        }
        else
        {// in_OFF
            state_flag2.bit.INorJOG_SOFF = 1;
            // OFFstate
            if(MotorStopSel_DB && !state_flag2.bit.PWM_HIZ && (labs(speed) > 15*SpeedUnit))// PWM ر ʱ      DB ƶ   ־
            {
                state_flag2.bit.DB = 1;
            }
            PN_OTrelay();
        }
    }
    else if(state_flag2.bit.HaveAlm)
    {   // almdisp
        state_flag2.bit.Son = 0;
        state_flag2.bit.enSON = 0;
        servo_state.bit.svstate = 0;
        if(input_state.bit.AlMRST)
        {
            state_flag2.bit.DispAlm = 1;
            state_flag2.bit.ClrAlm = 0;
        }
    }
    else
    {
        state_flag2.bit.enSON = 1;
        state_flag2.bit.nBRKfinish = 0;
        PN_OTrelay();
    }
}

void PN_OTrelay(void)
{
        #if DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW
        if(Timers.cntRELAY < (Pn[DBTimeADD]<<1))    //   DB״̬         ״̬    ʱ      ?
        #elif DRV_TYPE == DRV_5KW || DRV_TYPE == DRV_15KW
        if(Timers.cntRELAY < (Pn[DBTimeADD]<<1) || speed != 0)  //   DB״̬         ״̬    ʱ      ?
        #endif
        {
            if(state_flag2.bit.DB && (state_flag2.bit.Son == 0))
            {// PNOT    ʱ:OT=1:flag2.0 = 0 int2:PWM ر  OT=0:flag2.0 = 1 COMCON.9=0
                if(state_flag2.bit.PWM_HIZ)
                {
                    #if AC_VOLT == AC_200V && (DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW)
                    out_state.bit.RYCTL = 0;
                    #endif
                    out_state.bit.DBCTL  = 0;
                    *DRIVE = out_state.all;
                    state_flag2.bit.HaveDB = 1;
                    state_flag2.bit.DB = 0;         //          ù DB ƶ
                }
            }
        }
        else
        {
            #if AC_VOLT == AC_200V && (DRV_TYPE == DRV_400W || DRV_TYPE == DRV_1KW)
            out_state.bit.RYCTL = 1;
            #endif
            out_state.bit.DBCTL  = 1;
            *DRIVE = out_state.all;
            state_flag2.bit.HaveDB = 0;         // ȡ     ƶ   ״̬
        }
}

void BRstate(void)
{
        if(state_flag2.bit.HaveBRKout)
        {
            if(state_flag2.bit.INorJOG_SOFF)
            {
                if(labs(speed) > 15*SpeedUnit)
                {// ServoffBR0               ٶȸ ʱ  S-OFF
                    state_flag2.bit.Son = 0;
                    state_flag2.bit.enSON = 0;
                //  state_flag2.bit.INorJOG_SOFF = 0;
                    servo_state.bit.svstate = 0;
                }
                else
                {
                    state_flag2.bit.BRKon = 1;      //  ƶ   ON  ־[ ƶ   Ч]
                    if(Timers.BRKcnt >= (Pn[BRKTIMADD]*20)) //   λ10ms
                    {// ServoffBR0
                        state_flag2.bit.Son = 0;
                        state_flag2.bit.enSON = 0;
                     // state_flag2.bit.INorJOG_SOFF = 0;
                        servo_state.bit.svstate = 0;
                    }
                }
            }
            // ServoffBR
            if((Pn[BRKSPDADD]*SpeedUnit >= labs(speed)) || ((Timers.BRKWAIcnt >= Pn[BRKWAITIMADD]*20))) //   λ  10ms
            {// ON_BRK
                state_flag2.bit.BRKon = 1;
            }
        }
        else
        {// noBREAKout
            if(state_flag2.bit.INorJOG_SOFF)
            {
                state_flag2.bit.Son = 0;
                state_flag2.bit.enSON = 0;
                servo_state.bit.svstate = 0;
            }
        }
}
////////////////////////////////////////////////////////////////////////////////////////
// Step 7. Insert all local Interrupt Service Routines (ISRs) and functions here:
////////////////////////////////////////////////////////////////////////////////////////
Uint16 DatalineTest(void)
{Uint16 i;
 Uint16 * XRAMindex;
    // data line
    XRAMindex = (Uint16 *)SAMPLE_BUF;
    for(i = 0; i <= 15; i++)
    {
        *XRAMindex++ = 1<<i;
    }
    XRAMindex = (Uint16 *)SAMPLE_BUF;
    for(i = 0; i <= 15; i++)
    {
        if(*XRAMindex++ != 1<<i)
        {
            return 1;
        }
    }
    // address line
    XRAMindex = (Uint16 *)SAMPLE_BUF;
    for(i = 0; i <= 11; i++)
    {
        *(XRAMindex + (1<<i)) = 1<<i;
    }
    XRAMindex = (Uint16 *)SAMPLE_BUF;
    for(i = 0; i <= 11; i++)
    {
        if(*(XRAMindex + (1<<i)) != 1<<i)
        {
            return 1;
        }
    }
    return 0;
}

Uint16 XRAMtest(Uint16 * XRAM_FirstIndex,Uint16 length)
{Uint16 * XRAMindex;
 Uint16 i= 0;
    // WR 0x5555
    XRAMindex = XRAM_FirstIndex;
    for(i = 0; i <= length; i++)
    {
        *XRAMindex++ = 0x5555;
    }
    // RD 0x5555
    XRAMindex = XRAM_FirstIndex;
    for(i = 0; i <= length; i++)
    {
       // RDX=*XRAMindex++;
        if(*XRAMindex++ != 0x5555)
        //if(RDX != 0x5555)
        {
            return 1;
        }
    }
    // WR 0xAAAA
    XRAMindex = XRAM_FirstIndex;
    for(i = 0; i <= length; i++)
    {

        *XRAMindex++ = 0xAAAA;
    }
    // RD 0xAAAA
    XRAMindex = XRAM_FirstIndex;
    for(i = 0; i <= length; i++)
    {
        //RDX=*XRAMindex++;
        if(*XRAMindex++ != 0xAAAA)
        //if(RDX != 0xAAAA)
        {
            return 1;
        }
    }
    // WR i
    XRAMindex = XRAM_FirstIndex;
    for(i = 0; i <= length; i++)
    {
        *XRAMindex++ = i;
    }
    // RD i
    XRAMindex = XRAM_FirstIndex;
    for(i = 0; i <= length; i++)
    {
        //RDX=*XRAMindex++;
        if(*XRAMindex != i)
        //if(RDX != i)
        {
            return 1;
        }
        // Clear
        *XRAMindex++ = 0;
    }
    return 0;
}
void error(void)
{
    asm("     ESTOP0");                     // Test failed!! Stop!
    for (;;);
}
void WriteFlash(unsigned int pt,unsigned int val)
{
    Pn[SumAddr] -= Pn[pt];
    CheckSum -= Pn[pt];
    Pn[pt] = val;
    Pn[SumAddr] += val;
    CheckSum += val;
    SPI_WriteFRAM(SumAddr,&Pn[SumAddr],1);
    SPI_WriteFRAM(pt,&Pn[pt],1);
}
//===========================================================================
// No more.
//===========================================================================

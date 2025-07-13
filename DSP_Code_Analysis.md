# DSP 代码分析报告

本报告对当前项目的DSP代码进行了全面分析，该项目是一个功能完善的电机运动控制器，实现了基于FOC（磁场定向控制）的电流环、速度环和位置环，支持多种编码器类型、多种通信协议和多种运行模式。

## 1. 核心特点和功能模块

### 1.1 实时控制
通过 [`DSP/master+position/src/User_Isr.c`](DSP/master+position/src/User_Isr.c) 中的中断服务例程（如 [`SPEED_Loop_ISR()`](DSP/master+position/src/User_Isr.c:688) 和 [`ENCODER_PGC_ISR()`](DSP/master+position/src/User_Isr.c:1122)）实现高频率的实时控制，确保了系统对电机状态的快速响应。

### 1.2 磁场定向控制 (FOC)
*   **电流环**: [`runcur_process()`](DSP/master+position/src/User_Isr.c:1757) 函数实现了Clarke和Park变换，将三相电流转换为d-q轴电流，并使用PI控制器计算d-q轴电压。
*   **空间矢量脉宽调制 (SVPWM)**: [`SVPWM()`](DSP/master+position/src/User_Isr.c:2119) 函数根据d-q轴电压生成三相PWM占空比，驱动电机。

### 1.3 多环控制与输出

DSP代码实现了经典的三闭环控制结构：位置环、速度环和电流环，最终通过PWM信号驱动电机。

1.  **位置环 (如果启用)**:
    *   在 [`DSP/master+position/src/User_subFunc.c`](DSP/master+position/src/User_subFunc.c:1880) 的 `PControl()` 函数中，根据给定脉冲 (`Pgerr`) 和实际反馈脉冲 (`Pferr`) 计算位置偏差 `Ek`。
    *   `Ek` 经过位置环PI调节（在 `notclrEk()` 函数中，通过 `Kpos` 增益），生成一个速度指令 `SPEED`。这个 `SPEED` 作为速度环的输入。

2.  **速度环**:
    *   在 [`DSP/master+position/src/User_Isr.c`](DSP/master+position/src/User_Isr.c:2364) 的 `speed_loop_process()` 函数中，计算速度误差 `e_speed = SPEED - speed` (指令速度 - 实际速度)。
    *   在Slave DSP中，这个误差还会被来自STM32的PID同步校正值 (`PositionSpeed`) 调整：`e_speed = SPEED - speed - PositionSpeed;`。
    *   在 [`DSP/master+position/src/User_subFunc.c`](DSP/master+position/src/User_subFunc.c:3151) 的 `SModule2()` 函数中，`e_speed` 经过速度PI调节（通过 `Kpspeed` 和 `Kispeed` 增益），生成一个电流参考值 `Iqrbuf`。
    *   `SModule2()` 还包含了惯量补偿 (`ACCFF`)、摩擦力补偿 (`Tf`) 和阻尼补偿 (`Damp`)，以及积分器抗饱和等功能，以优化速度环的性能。
    *   `Iqrbuf` 还会经过陷波滤波 (`dowith_Notch()`)。

3.  **电流环**:
    *   在 [`DSP/master+position/src/User_subFunc.c`](DSP/master+position/src/User_subFunc.c:3127) 的 `SModule1()` 函数中，`Iqrbuf` 经过转矩滤波（通过 `Tiqr` 时间常数），得到最终的电流指令 `Iqr`。
    *   在 [`DSP/master+position/src/User_Isr.c`](DSP/master+position/src/User_Isr.c:1757) 的 `runcur_process()` 函数中，实际电流 `Iu`, `Iv` 经过Clarke和Park变换得到 `Id`, `Iq`。
    *   然后，`Id` 和 `Iq` 与电流指令 `Idr` (通常为0) 和 `Iqr` 进行比较，计算电流误差 `e_id`, `e_iq`。
    *   这些误差经过电流PI调节（通过 `Kpcurrent` 和 `Kicurrent` 增益），生成d-q轴电压指令 `Ud` 和 `Uq`。
    *   `runcur_process()` 还包含了反电动势补偿、电流前馈等。

4.  **PWM生成**:
    *   在 [`DSP/master+position/src/User_Isr.c`](DSP/master+position/src/User_Isr.c:2119) 的 `SVPWM()` 函数中，d-q轴电压指令 `Ud` 和 `Uq` 经过反Park变换（或直接空间矢量调制），计算出三相电压分量 `Ua`, `Ub`, `Uc`。
    *   `SVPWM()` 根据这些电压分量和扇区信息，计算出PWM的比较值 (`taon`, `tbon`, `tcon`)。
    *   这些比较值直接写入ePWM模块的比较寄存器 (`EPwm1Regs.CMPA.half.CMPA` 等)。

### 1.3.1 输出到电机的具体内容

最终输出到电机的是**PWM（脉冲宽度调制）信号**。

*   这些PWM信号由DSP的ePWM模块生成。
*   每个PWM信号控制一个H桥臂的开关状态（通常是IGBT或MOSFET）。
*   三相PWM信号（U相、V相、W相）通过功率模块（如IGBT模块）驱动电机绕组，产生旋转磁场，从而控制电机的转速和转矩。
*   PWM信号的占空比决定了施加到电机绕组上的平均电压，而PWM的频率通常是固定的。通过改变占空比，DSP可以精确控制电机的电流和电压，从而实现对电机转速和转矩的精确控制。

简而言之，多环控制的输出是一个层层递进的过程：
位置误差 -> 速度指令 -> 速度误差 -> 电流指令 -> 电流误差 -> 电压指令 -> PWM占空比 -> 驱动电机。

### 1.3.2 `Iqrbuf` 与电机转速及力矩的关系

*   **`Iqrbuf` 的作用**: `Iqrbuf` 是速度环的输出，也是电流环的输入，它代表了**转矩电流指令**。在FOC（磁场定向控制）中，q轴电流 (`Iq`) 直接控制电机的转矩。因此，`Iqrbuf` 实际上是DSP期望电机产生的转矩大小的指令。
*   **与转速的关系**: `Iqrbuf` **不是直接与电机转速变化成正比**，而是与**电机转矩**成正比。它更像是控制电机转矩的“油门”，通过控制转矩来间接控制转速的变化。当电机需要加速时，速度环会输出一个较大的 `Iqrbuf`（正值），产生正向转矩，使电机加速；当电机需要减速时，速度环会输出一个负值的 `Iqrbuf`，产生反向转矩（制动转矩），使电机减速。当电机稳定运行时，如果负载不变，`Iqrbuf` 会维持在一个恒定值，以产生与负载转矩平衡的转矩。

### 1.3.3 电机力矩的决定因素

电机的力矩（转矩）主要由以下因素决定：

*   **q轴电流 (`Iq`)**: 在FOC控制的永磁同步电机中，电机的电磁转矩与q轴电流 (`Iq`) 成正比。DSP通过电流环精确控制 `Iq`，从而控制电机转矩。
    *   在代码中，[`DSP/master+position/src/User_Isr.c`](DSP/master+position/src/User_Isr.c:1807) 的 `Iq` 变量就是实际的q轴电流。
    *   [`DSP/master+position/src/User_subFunc.c`](DSP/master+position/src/User_subFunc.c:3062) 中的 `Torque` 变量计算方式为 `Torque = ((int32)Iqr * varFactualTn)/Iqn;` 或 `Torque = ((int32)Iqr * MotorPar1[memDriveSel][memMotorSel][3])/Iqn;;`，其中 `Iqr` 是电流环的指令，`varFactualTn` 或 `MotorPar1[][][3]` 是额定转矩相关的参数，`Iqn` 是额定电流。这明确表示转矩与 `Iqr`（即q轴电流指令）成正比。
*   **电机参数**: 电机的物理参数，如磁链、极对数、电感等，决定了电流与转矩之间的转换关系。这些参数通常在电机模型中被考虑。
*   **磁链 (d轴电流 `Id`)**: 虽然在永磁同步电机中，d轴电流通常被控制为0以最大化转矩效率，但在某些弱磁控制或磁阻电机中，d轴电流也会影响转矩。

因此，可以总结为：**电机的力矩主要由q轴电流 (`Iq`) 决定，而 `Iq` 又是由速度环的输出 `Iqrbuf` 经过滤波和限幅后得到的指令。**

### 1.3.4 `Iqrbuf` 的限幅机制与上限

`Iqrbuf` 的限幅主要在 [`DSP/master+position/src/User_subFunc.c`](DSP/master+position/src/User_subFunc.c:3035) 的 `limitIqr()` 函数中实现，并赋值给 `Iqrref`。其限幅逻辑是动态的，并受到多重因素的限制：

1.  **根据方向的限幅 (`Iqrmax` 和 `Iqrmin`)**:
    *   `limitIqr()` 函数首先根据 `Iqrbuf` 的正负方向（或通过 `membit01.bit.bit00` 判断的正反向）与 `Iqrmax` 或 `Iqrmin` 进行比较。
    *   `Iqrmax` 和 `Iqrmin` 的值在 [`DSP/master+position/src/User_subFunc.c`](DSP/master+position/src/User_subFunc.c:3620) 的 `SModule3()` 函数中计算。它们可以由以下参数决定：
        *   **模拟量转矩指令**: 如果 `membit01.bit.bit02` 和 `memhex01.hex.hex00` 被设置，`Iqrmax` 和 `Iqrmin` 会根据 `TCMD` (模拟量转矩指令) 和 `Iqn` (额定电流) 计算：`Iqrmax = (labs(TCMD)*Iqn/100); Iqrmin = Iqrmax;`。
        *   **软件参数配置**: 否则，`Iqrmax` 和 `Iqrmin` 由 `Pn[P_TCR_inLmtADD]` (正转转矩内部限幅) 和 `Pn[N_TCR_inLmtADD]` (反转转矩内部限幅) 或 `Pn[P_TCR_outLmtADD]` (正转转矩外部限幅) 和 `Pn[N_TCR_outLmtADD]` (反转转矩外部限幅) 决定。这些参数通常在 [`EDBclm.h`](DSP/master+position/include/EDBclm.h:405) 中定义。
        *   **刹车/零夹紧状态**: 在刹车或零夹紧状态 (`state_flag2.bit.nBRKzcl || state_flag2.bit.nBRKsoff`) 下，限幅值会根据 `Pn[nBRKStopTCRADD]` 重新计算。

2.  **最大电流限幅 (`IPMImax`)**:
    *   在 `limitIqr()` 函数的末尾，还有一个基于 `IPMImax` 的绝对最大电流限幅。
    *   根据 `DRV_TYPE` (驱动器类型，如 `DRV_400W`, `DRV_1KW`, `DRV_5KW`, `DRV_15KW`)，`Iqrref` 会被进一步限幅。例如，对于 `DRV_400W` 或 `DRV_1KW`，最大限幅为 `IPMImax * 400`。
    *   `IPMImax` 是模块电流相关的参数，它定义了驱动器能够输出的理论最大电流。这个限幅是硬件层面的保护，防止电流过大损坏驱动器或电机。

**`Iqrbuf` 的上限总结**:

`Iqrbuf` 的上限是存在的，并且是动态的，它受到以下因素的综合限制：

*   **软件参数配置**: 通过 `Pn` 数组中的各种转矩限幅参数进行配置。
*   **模拟量指令**: 在模拟量转矩控制模式下，模拟量指令本身也会作为限幅的依据。
*   **驱动器硬件能力**: 由 `IPMImax` 定义的驱动器能够承受的绝对最大电流，这是最终的硬性上限。

### 1.4 编码器支持
支持多种编码器类型（绝对式、增量式、旋转变压器），并在 [`abs_Encoder()`](DSP/master+position/src/User_Isr.c:1351) 和 [`pulsEncoder()`](DSP/master+position/src/User_subFunc.c:252) 等函数中进行数据读取和处理。

### 1.5 多种运动模式
*   **JOG模式**: [`SJDControl()`](DSP/master+position/src/User_subFunc.c:1131) 和 [`JDPosCol()`](DSP/master+position/src/User_subFunc.c:1943) 处理JOG运行。
*   **参数速度控制**: [`ParSpd()`](DSP/master+position/src/User_subFunc.c:1207) 实现基于参数的速度控制。
*   **模拟量控制**: [`AnalogSpdCol()`](DSP/master+position/src/User_subFunc.c:1279) 和 [`AnalogTcrCol()`](DSP/master+position/src/User_subFunc.c:1334) 处理模拟量速度和转矩指令。
*   **回零操作**: 一系列函数（[`Home_State_Position()`](DSP/master+position/src/User_subFunc.c:1439), [`Ref_Zero()`](DSP/master+position/src/User_subFunc.c:1496), [`Pos_Zero()`](DSP/master+position/src/User_subFunc.c:1563), [`PD_Zero()`](DSP/master+position/src/User_subFunc.c:1677), [`Home_Operation()`](DSP/master+position/src/User_subFunc.c:1698)）实现了复杂的回零状态机。
*   **惯量识别**: [`Jdynamic()`](DSP/master+position/src/User_subFunc.c:718) 函数实现了实时惯量识别功能。
*   **伺服泵控制**: [`ServoPump()`](DSP/master+position/src/User_subFunc.c:160) 和 [`PressControl()`](DSP/master+position/src/User_subFunc.c:71) 用于伺服泵应用。

### 1.6 滤波与限幅
*   **中值滤波**: [`MedianFilter()`](DSP/master+position/src/User_Isr.c:595) 用于ADC采样数据。
*   **陷波滤波**: [`Notch()`](DSP/master+position/src/User_subFunc.c:448) 和 [`dowith_Notch()`](DSP/master+position/src/User_subFunc.c:493) 用于滤除振动。
*   **脉冲滤波**: [`pulsefilter()`](DSP/master+position/src/User_subFunc.c:3669) 用于平滑脉冲输入。
*   **电流限幅**: [`limitIqr()`](DSP/master+position/src/User_subFunc.c:3035) 对电流指令进行限幅。
*   **速度限幅**: [`LmtSpd()`](DSP/master+position/src/User_subFunc.c:3844) 对速度指令进行限幅。

### 1.7 系统管理与诊断
*   **系统初始化**: [`main()`](DSP/master+position/main.c:43) 函数中包含PLL、GPIO、中断和外设的初始化。
*   **报警处理**: [`JudgeAlarm()`](DSP/master+position/main.c:1164) 函数负责判断和处理各种报警情况。
*   **参数配置**: 大量控制参数通过 [`Pn[]`](DSP/master+position/include/globalvar.h:208) 数组进行配置，并通过 [`EDBclm.h`](DSP/master+position/include/EDBclm.h) 中的宏进行索引。
*   **内存测试**: [`DatalineTest()`](DSP/master+position/main.c:1818) 和 [`XRAMtest()`](DSP/master+position/main.c:1852) 用于测试外部RAM。

### 1.8 通信协议
支持CANopen、Profibus和SCI等多种工业通信协议，相关处理函数分布在 [`main.c`](DSP/master+position/main.c:293), [`User_Isr.c`](DSP/master+position/src/User_Isr.c:70), [`globalvar.h`](DSP/master+position/include/globalvar.h:709) 和 [`EDBclm.h`](DSP/master+position/include/EDBclm.h:914) 中定义的结构体和函数原型中。

## 2. 文件结构总结

*   [`DSP/master+position/main.c`](DSP/master+position/main.c): 程序入口，系统初始化，主循环。
*   [`DSP/master+position/include/globalvar.h`](DSP/master+position/include/globalvar.h): 全局变量和结构体定义。
*   [`DSP/master+position/include/EDBclm.h`](DSP/master+position/include/EDBclm.h): 硬件寄存器宏、常量、位域结构体和函数原型。
*   [`My/pid.c`](My/pid.c): 编码器和功率速度的PID控制器。
*   [`DSP/master+position/src/User_Isr.c`](DSP/master+position/src/User_Isr.c): 主要中断服务例程和核心控制函数（电流环、SVPWM、ADC、PWM控制）。
*   [`DSP/master+position/src/User_subFunc.c`](DSP/master+position/src/User_subFunc.c): 大量子函数，实现各种控制模式的细节、滤波、限幅、加减速、回零、惯量识别等。

该项目代码量较大，功能复杂，体现了嵌入式运动控制系统在实时性、鲁棒性和多功能性方面的要求。
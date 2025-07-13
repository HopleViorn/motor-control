# STM32与DSP交互分析报告

本报告基于对 `Core/Src/stm32f1xx_it.c`、[`DSP/master+position/src/SPI4.c`](DSP/master+position/src/SPI4.c)、[`DSP/Slave/src/SPI4.c`](DSP/Slave/src/SPI4.c) 和 [`DSP/Slave/src/User_subFunc.c`](DSP/Slave/src/User_subFunc.c) 文件的分析，解释了STM32与DSP之间关于电机同步校正和速度调整的交互机制。

## 1. 测量角度/相位偏差与PID同步校正

在 [`Core/Src/stm32f1xx_it.c`](Core/Src/stm32f1xx_it.c) 文件的 [`HAL_TIM_IC_CaptureCallback()`](Core/Src/stm32f1xx_it.c:612) 函数中，当TIM8的通道1中断触发时，执行以下操作来测量两个电机之间的角度/相位偏差并进行PID同步校正：

1.  **偏差测量**:
    *   `FactError = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);` 读取TIM8捕获的计数值，该值代表两个电机编码器信号的相位差（时间差）。
    *   `BaiFenShu = FactError * 1000 / CCH2;` 将 `FactError` 归一化，其中 `CCH2` 是由TIM4测得的单个电机转动一个齿轮齿所需的时间周期，这有助于消除速度变化对偏差测量的影响。
    *   `NowError = BaiFenShu - syncConst;` 计算最终的误差，`syncConst` 可能是同步的参考值或偏移量。

2.  **PID计算**:
    *   `PIDout = PIDencodr(ToCheckError);` 调用 [`My/pid.c`](My/pid.c) 中定义的 `PIDencodr` 函数，对 `NowError`（即 `ToCheckError`）执行PID计算，得到修正量 `PIDout`。
    *   `PidSpi = PIDout;` 将PID计算结果赋值给 `PidSpi`。
    *   `if(PidSpi>10)PidSpi =10; else if( PidSpi <-10)PidSpi=-10;` 对 `PidSpi` 进行限幅，将其值限制在 -10 到 10 之间。

3.  **PID值送入DSP的哪个部分？**
    *   限幅后的 `PidSpi` 值被放入SPI发送缓冲区：`SPITxBuffer[0] = PidSpi;`。
    *   通过SPI接口发送给Slave DSP：`HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t * )&SPITxBuffer,(uint8_t * )&SPIRxBuffer,1);`。
    *   在Slave DSP的 [`DSP/Slave/src/SPI4.c`](DSP/Slave/src/SPI4.c:126) 中的 `Mcbspb_RxINTB_ISR` 中断服务例程中，STM32发送的PID值被接收并存储在 `PositionCurrentError` 变量中：
        ```c
        135 | PositionCurrentError=((int16)RXtemp)>>8;
        137 | if( PositionCurrentError>50)PositionCurrentError=50;
        138 | if( PositionCurrentError<-50)PositionCurrentError=-50;
        ```
        `PositionCurrentError` 在此被限幅在 -50 到 50 之间。
    *   **PID值具体调整DSP代码的方式**: 在Slave DSP的 [`DSP/Slave/src/User_subFunc.c`](DSP/Slave/src/User_subFunc.c:3307) 中的 `speed_loop_process` 函数中，`PositionCurrentError` 的值（经过赋值给 `PositionSpeed` 并再次限幅）被直接用于调整速度误差 `e_speed`：
        ```c
        3303 | PositionSpeed=PositionCurrentError;
        3304 | if(PositionSpeed>3000)PositionSpeed=3000;
        3307 | e_speed = SPEED - speed - PositionSpeed;
        ```
        这意味着 `PositionSpeed` 作为对速度误差的直接补偿项。如果 `PositionSpeed` 是正值，它会减小 `e_speed`，从而减小速度环的输出，使电机减速。如果 `PositionSpeed` 是负值，它会增大 `e_speed`，从而增大速度环的输出，使电机加速。通过这种方式，STM32发送的PID值直接作用于Slave DSP的速度环，实现两个电机之间的同步校正。

## 2. `HAL_SPI_TransmitReceive_IT` 与 `AdjSpeed` 如何作用于DSP

### 2.1 `HAL_SPI_TransmitReceive_IT` 的作用
*   `HAL_SPI_TransmitReceive_IT` 是STM32通过SPI与DSP进行数据交换的底层函数。它负责将计算出的PID同步校正值 (`PidSpi`) 从 `SPITxBuffer` 发送给Slave DSP。
*   Slave DSP通过其配置的SPI接收中断服务例程（`Mcbspb_RxINTB_ISR`）来读取这些接收到的数据，并将其存储在 `PositionCurrentError` 变量中，进而影响速度环的误差计算。

### 2.2 `AdjSpeed` 的作用
*   在 [`Core/Src/stm32f1xx_it.c`](Core/Src/stm32f1xx_it.c:369) 的 `HAL_TIM_PeriodElapsedCallback` 函数中，`AdjSpeed(0,NowCommandSPEED);` 被调用。
*   `AdjSpeed` 函数（其具体实现不在当前分析的文件中，但很可能在 `Function.h` 或 `Function.c` 中定义）负责将新的速度指令 `NowCommandSPEED` 发送给DSP。
*   这个速度指令通常通过SPI或其他串行通信协议（如Modbus RTU，因为代码中提到了 `Pc485RtuReg`）传输给DSP。
*   在Master DSP中，`NowCommandSPEED` 会更新DSP内部的速度给定值 (`SPEEDINS`)。在 [`DSP/master+position/src/User_subFunc.c`](DSP/master+position/src/User_subFunc.c) 中，`SPEEDINS` 被用于多种控制模式（JOG、参数速度、模拟量速度等）的计算，并通过 `SModule4()` (软件加减速) 和 `SModule3()` (限幅) 等函数处理后，最终影响 `SPEED` 变量，进而驱动电机。
*   对于Slave DSP，虽然没有直接看到 `NowCommandSPEED` 的使用，但通常Master DSP会将其速度指令传递给Slave DSP，或者Slave DSP有自己的速度指令来源，并以类似的方式影响其速度控制。

综上所述，STM32作为主控器，通过SPI将计算出的同步校正量和速度指令发送给DSP，DSP则根据这些指令调整其内部的电机控制算法，实现双电机同步和速度调节。
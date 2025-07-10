# 中断列表与功能代码对应表

## 1. 中断列表

### A. Cortex-M3 处理器异常 (System Exceptions)

这些是ARM Cortex-M内核自带的异常，用于处理系统级的错误或特殊请求。

*   `NMI_Handler`: 不可屏蔽中断。
*   `HardFault_Handler`: 硬件错误中断，通常由程序错误引起。
*   `MemManage_Handler`: 内存管理单元（MPU）错误。
*   `BusFault_Handler`: 总线错误。
*   `UsageFault_Handler`: 用法错误（如执行未定义指令）。
*   `SVC_Handler`: 系统服务调用，通过`SVC`指令触发。
*   `DebugMon_Handler`: 调试监视器中断。
*   `PendSV_Handler`: 可悬起系统服务调用，常用于RTOS的任务切换。
*   `SysTick_Handler`: 系统滴答定时器中断，由HAL库用于提供时间基准（`HAL_IncTick()`）。

### B. STM32F1xx 外设中断 (Peripheral Interrupts)

这些中断由STM32微控制器的特定外设触发。

*   `TIM2_IRQn` (`HAL_TIM_PeriodElapsedCallback`): **定时器2全局中断**。这是最核心的周期性中断，每2.5ms触发一次，用于：
    *   喂狗（刷新独立看门狗）。
    *   统计系统运行时间。
    *   计算功率。
    *   调用 `SaftyCheck()` 进行安全检查和速度调整决策。
    *   调度DSP数据读取（力矩、速度）和速度指令写入。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `TIM8_UP_IRQn`: **定时器8更新中断**。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `TIM8_TRG_COM_IRQn`: **定时器8触发/通信中断**。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `TIM8_CC_IRQn` (`HAL_TIM_IC_CaptureCallback`): **定时器8捕获/比较中断**。由编码器信号触发，用于：
    *   捕获编码器脉冲，计算实际误差 `FactError`。
    *   执行PID计算 (`PIDencodr`)。
    *   通过SPI发送PID输出给电机驱动器。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `USART1_IRQn`: **USART1全局中断**。用于PC485通信，处理来自PC的Modbus数据。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `USART2_IRQn`: **USART2全局中断**。用于DSP485通信，处理来自DSP的Modbus数据（如力矩、速度响应）。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `EXTI9_5_IRQn` (`HAL_GPIO_EXTI_Callback`): **外部中断线9-5**。由GPIO_PIN_9触发，可能连接到紧急停止按钮或故障指示。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `EXTI15_10_IRQn` (`HAL_GPIO_EXTI_Callback`): **外部中断线15-10**。由GPIO_PIN_10触发，可能连接到另一个紧急停止按钮或故障指示。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `SPI1_IRQn` (`HAL_SPI_TxCpltCallback`, `HAL_SPI_RxCpltCallback`, `HAL_SPI_TxRxCpltCallback`): **SPI1全局中断**。处理SPI数据传输完成事件。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `TIM3_IRQn`: **定时器3全局中断**。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `TIM6_IRQn`: **定时器6全局中断**。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`
*   `UART4_IRQn`: **UART4全局中断**。
    *   **代码位置**: `Core/Src/stm32f1xx_it.c`

## 2. 功能与代码对应列表

### 1. 电机控制 (Motor Control)

*   **系统启动与停止逻辑**:
    *   `main.c`: 主循环中根据 `Pc485RtuReg[3]` 和 `Pc485RtuReg[4]` 判断运行状态，调用 `BeginSystemSyncProcess()` 和 `StopMoto()`。
    *   `My/Function/Function.c`:
        *   `StopMoto()`: 实现电机停止的具体逻辑，包括关闭GPIO、复位标志位、记录运行时间到EEPROM等。
        *   `BeginSystemSyncProcess()`: 实现电机启动的具体逻辑，包括开启GPIO、初始化标志位、等待电机达到初始速度等。
*   **速度调节 (指令速度下发)**:
    *   `stm32f1xx_it.c`: 在 `HAL_TIM_PeriodElapsedCallback` (TIM2中断) 中，根据 `NowCommandSPEED` 的变化，调度 `AdjSpeed()` 函数的调用。
    *   `My/MODbus/MasterRTU.c`:
        *   `AdjSpeed(uint8_t MotorNum, uint16_t Speed)`: 通过Modbus RTU协议向指定电机（或所有电机）的DSP发送速度指令。
        *   `SendDSPCommand(uint8_t MotorNum, uint16_t RegAdd, uint16_t Data)`: Modbus RTU写入命令的底层实现。
*   **加减速时间管理**:
    *   `My/Function/Function.c`: 定义了 `ADDTime` (加速时间) 和 `DecTime` (减速时间) 变量，并在 `WriteBengType()` 和 `BengParaCheck()` 中根据泵类型设置这些参数。
    *   `My/MODbus/MasterRTU.c`:
        *   `AdjAddTime(uint8_t MotorNum, uint16_t AddTime)`: 设置DSP的加速时间。
        *   `AdjDecTime(uint8_t MotorNum, uint16_t DecTime)`: 设置DSP的减速时间。

### 2. 双电机同步 (Dual-Motor Synchronization)

*   **实时位置/角度偏差测量**:
    *   `stm32f1xx_it.c`:
        *   `TIM8_CC_IRQHandler` (TIM8捕获/比较中断): 捕获编码器信号，计算 `FactError` (实际误差) 和 `CCH2` (周期)。
        *   `NowError`: 全局变量，存储计算出的角度偏差。
*   **PID计算**:
    *   `stm32f1xx_it.c`: 在 `TIM8_CC_IRQHandler` 中调用 `PIDencodr(ToCheckError)`。
    *   `My/pid.c`:
        *   `PIDencodr(int16_t error)`: PID控制算法的具体实现，根据输入误差计算输出。
        *   `PID_Init()`: PID控制器的初始化函数（可能在 `main.c` 或其他初始化函数中调用）。
*   **SPI修正量传输**:
    *   `stm32f1xx_it.c`:
        *   `TIM8_CC_IRQHandler`: 将PID输出 `PidSpi` 通过SPI发送给DSP。
        *   `HAL_SPI_TxRxCpltCallback()`: SPI传输完成回调函数，用于处理SPI通信的完成事件。
    *   `My/SPI.c`: 包含SPI通信的底层驱动函数（如 `SPI_ReadWriteByte` 等，虽然代码中直接使用了HAL库的SPI函数）。

### 3. 系统安全与故障保护 (System Safety & Fault Protection)

*   **力矩与速度监控**:
    *   `stm32f1xx_it.c`: 在 `HAL_TIM_PeriodElapsedCallback` (TIM2中断) 中，通过 `ReadDsp1RegITReiver()` 周期性读取DSP的力矩 (`0x0809`) 和速度 (`0x0806`)，并更新到 `Pc485RtuReg[22]` 和 `Pc485RtuReg[23]`。
    *   `My/Function/Function.c`:
        *   `SaftyCheck()`: 根据力矩、速度、角度偏差等参数判断系统安全状态，并进行相应的速度调整或错误处理。
*   **角度偏差监控**:
    *   `stm32f1xx_it.c`: 在 `TIM8_CC_IRQHandler` 中计算 `NowError`，并在 `SaftyCheck()` 中使用。
*   **板载温度监控**:
    *   `main.c`:
        *   `adcGetBatteryVoltage()`: 读取ADC值。
        *   `KTY81()`: 将ADC值转换为温度。
        *   主循环中根据温度判断是否过热，并触发停机。
*   **错误处理与显示**:
    *   `main.c`: 主循环中检查 `BigError` 和 `HaveError` 标志，调用 `SelfCheck()` 和 `DisSpeed()`。
    *   `My/Function/Function.c`:
        *   `SelfCheck()`: 启动时和运行时检查DSP报警、电机型号匹配等，如果存在错误则进入死循环或复位。
        *   `DisError(uint8_t A, uint16_t Code)`: 在TM1650显示屏上显示错误代码，并进入死循环。
        *   `ClearALMcom()`: 清除DSP报警。
        *   `armReset()`: 软件复位STM32。
*   **看门狗 (Watchdog)**:
    *   `main.c`: `MX_IWDG_Init()` 初始化独立看门狗。
    *   `stm32f1xx_it.c`: 在 `HAL_TIM_PeriodElapsedCallback` (TIM2中断) 中周期性调用 `HAL_IWDG_Refresh(&hiwdg)` 喂狗。

### 4. 通信与人机交互 (Communication & HMI)

*   **Modbus RTU (主站)**:
    *   `My/MODbus/MasterRTU.c`:
        *   `ReadDsp1Reg(uint8_t MotorNum, uint16_t RegAdd)`: 同步读取DSP寄存器。
        *   `ReadDsp1RegITReiver(uint8_t MotorNum, uint16_t RegAdd)`: 异步读取DSP寄存器（通过中断接收）。
        *   `SendDSPCommand(uint8_t MotorNum, uint16_t RegAdd, uint16_t Data)`: 写入DSP寄存器。
        *   `AdjSpeed()`, `AdjAddTime()`, `AdjDecTime()`: 高层Modbus写入函数。
    *   `Core/Src/stm32f1xx_it.c`: `USART2_IRQHandler` 处理DSP485（Modbus）的接收中断。
    *   `Core/Src/main.c`: `MX_USART2_UART_Init()` 初始化DSP485 UART。
*   **Modbus RTU (从站)**:
    *   `My/MODbus/pc485slave.c`: 如果系统作为从站与PC通信，则该文件包含从站的Modbus协议处理逻辑。
    *   `Core/Src/stm32f1xx_it.c`: `USART1_IRQHandler` 处理PC485的接收中断。
    *   `Core/Src/main.c`: `MX_USART1_UART_Init()` 初始化PC485 UART。
*   **SPI通信**:
    *   `My/SPI.c`: 包含SPI的底层驱动函数（如 `SPI_ReadWriteByte` 等）。
    *   `Core/Src/stm32f1xx_it.c`: `SPI1_IRQHandler` 处理SPI中断，`HAL_SPI_TxRxCpltCallback` 处理传输完成回调。
    *   `Core/Src/main.c`: `MX_SPI1_Init()` 初始化SPI1。
*   **EEPROM (24C64) 数据存储**:
    *   `My/IIC_24C64/24cxx.c`: 24C64 EEPROM的驱动函数，如 `eeprom_WriteBytes()`, `eeprom_ReadBytes()`, `AT24C64_Check()`。
    *   `My/IIC/myiic.c`: I2C总线的底层驱动函数，如 `bsp_InitI2c()`。
    *   `main.c`: 在初始化阶段读取EEPROM数据到 `Pc485RtuReg`，在 `StopMoto()` 中写入运行时间。
*   **TM1650 显示**:
    *   `My/TM1650/tm1650.c`: TM1650显示驱动函数，如 `TM1650_Init()`, `TM1650_Set()`。
    *   `main.c`: 在初始化阶段和运行时调用 `TM1650_Set()` 显示状态和错误信息。

### 5. 系统自检与校准 (System Self-Check & Calibration)

*   **启动自检**:
    *   `My/Function/Function.c`:
        *   `SelfCheck()`: 启动时和运行时检查DSP报警、电机型号匹配等。
        *   `CheckBeforeRun()`: 检查电机是否停止、是否有报警、速度是否一致等启动前条件。
*   **初始角度查找与保存**:
    *   `My/Function/Function.c`:
        *   `DoFingInitAngVal(uint8_t MotorNum)`: 触发DSP查找电机初始角度，并返回结果。
        *   `FindSaveInitAngle()`: 调用 `DoFingInitAngVal()` 查找并保存两个电机的初始角度到EEPROM。
        *   `InitAngIfRight(uint8_t MotorNum)`: 检查当前查找的初始角度与存储的角度是否匹配。
        *   `AllZdFind()`: 执行中点检测（可能与初始角度查找相关）。
*   **泵参数检查**:
    *   `My/Function/Function.c`:
        *   `BengParaCheck(uint8_t BengType)`: 根据泵类型设置电机参数（如PID参数、加减速时间），并检查DSP中对应的参数是否一致。
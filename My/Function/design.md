控速环 -> 速度环 -> 电流环


主电机： 设定速度 -> 控速环 -> final_target_speed -> 速度环(DSP)-> 主电机转矩 -> 电流环 -> 输出电流

从电机： 主电机转矩 + sync_pid(position_err) -> 电流环 -> 输出电流







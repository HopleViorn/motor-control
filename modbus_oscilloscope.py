import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import time
from collections import deque
from modbus_client import connect_modbus_client, read_registers, send_power_on_command

# 配置示波器参数
MAX_POINTS = 200  # 示波器上显示的最大数据点数
UPDATE_INTERVAL_MS = 100  # 更新间隔，毫秒
TIME_WINDOW = 20  # 时间窗口，秒

# Modbus 读取配置
START_ADDRESS = 50
COUNT = 3  # 读取1个寄存器

class ModbusOscilloscope:
    def __init__(self):
        self.client = None
        self.fig, self.axes = plt.subplots(COUNT, 1, figsize=(12, 4 * COUNT), sharex=True)
        if COUNT == 1:
            self.axes = [self.axes] # 确保self.axes始终是可迭代的列表
        
        # 数据存储
        self.data_queues = [deque(maxlen=MAX_POINTS) for _ in range(COUNT)]
        self.time_queue = deque(maxlen=MAX_POINTS)
        self.start_time = time.time()
        
        # 绘图线条
        self.lines = []
        colors = ['blue', 'red', 'green', 'orange', 'purple', 'brown', 'pink', 'gray', 'olive', 'cyan']
        for i in range(COUNT):
            line, = self.axes[i].plot([], [],
                                      label=f'Register {START_ADDRESS + i}',
                                      color=colors[i % len(colors)],
                                      linewidth=2)
            self.lines.append(line)
        
        # 设置图形
        self.setup_plots() # 修改方法名为setup_plots
        
        # 连接计数器，用于处理连接失败
        self.connection_fail_count = 0
        self.max_fail_count = 10
        
    def setup_plots(self):
        """设置绘图参数"""
        for i, ax in enumerate(self.axes):
            ax.set_title(f'Modbus Register {START_ADDRESS + i} Oscilloscope', fontsize=14, fontweight='bold')
            ax.set_xlabel('Time (s)', fontsize=12)
            ax.set_ylabel('Register Value', fontsize=12)
            ax.grid(True, alpha=0.3)
            ax.legend(loc='upper right')
            
            # 设置初始轴范围
            ax.set_xlim(0, TIME_WINDOW)
            ax.set_ylim(0, 100)  # 初始Y轴范围，会动态调整
        
    def init_modbus_client(self):
        """初始化Modbus客户端"""
        self.client = connect_modbus_client()
        if self.client:
            print("正在发送开机指令...")
            send_power_on_command(self.client)
            return True
        return False
    
    def update_data(self, frame):
        """动画更新函数"""
        if not self.client or not self.client.connected:
            print(f"连接失败次数: {self.connection_fail_count}")
            self.connection_fail_count += 1
            if self.connection_fail_count >= self.max_fail_count:
                print("连接失败次数过多，停止尝试")
                return self.lines
            return self.lines
        
        # 重置连接失败计数
        self.connection_fail_count = 0
        
        # 读取数据
        registers = read_registers(self.client, START_ADDRESS, COUNT)
        if registers is not None:
            current_time = time.time() - self.start_time
            
            # 添加新数据
            self.time_queue.append(current_time)
            for i, reg_value in enumerate(registers):
                if i < COUNT:
                    # 将uint16转换为int16 (处理有符号数据)
                    if reg_value > 32767:
                        signed_value = reg_value - 65536
                    else:
                        signed_value = reg_value
                    self.data_queues[i].append(float(signed_value))
            
            # 更新绘图数据
            self.update_plot_data()
        
        return self.lines
    
    def update_plot_data(self):
        """更新绘图数据和坐标轴"""
        if len(self.time_queue) == 0:
            return
            
        # 转换为列表用于绘图
        time_list = list(self.time_queue)
        
        # 更新所有线条数据
        for i, line in enumerate(self.lines):
            if i < len(self.data_queues):
                data_list = list(self.data_queues[i])
                # 确保时间和数据长度一致
                min_len = min(len(time_list), len(data_list))
                if min_len > 0:
                    line.set_data(time_list[-min_len:], data_list[-min_len:])
        
        # 动态调整X轴（滚动窗口），所有子图共享X轴
        if time_list:
            current_time = time_list[-1]
            for ax in self.axes:
                if current_time > TIME_WINDOW:
                    ax.set_xlim(current_time - TIME_WINDOW, current_time)
                else:
                    ax.set_xlim(0, TIME_WINDOW)
        
        # 动态调整Y轴
        self.update_y_limits()
    
    def update_y_limits(self):
        """智能调整Y轴范围"""
        for i, ax in enumerate(self.axes):
            if i < len(self.data_queues):
                current_data = list(self.data_queues[i])
                if not current_data:
                    continue

                min_val = min(current_data)
                max_val = max(current_data)

                # 如果所有值都相同，设置一个合理的范围
                if min_val == max_val:
                    center = min_val
                    margin = max(abs(center) * 0.1, 10)  # 10%的边距或最小10个单位
                    min_val = center - margin
                    max_val = center + margin
                else:
                    # 添加10%的边距
                    margin = (max_val - min_val) * 0.1
                    min_val -= margin
                    max_val += margin

                # 平滑调整Y轴，避免频繁跳动
                current_ylim = ax.get_ylim()
                new_min = min(current_ylim[0], min_val)
                new_max = max(current_ylim[1], max_val)

                # 只有在变化较大时才调整
                if abs(new_min - current_ylim[0]) > abs(current_ylim[1] - current_ylim[0]) * 0.05 or \
                   abs(new_max - current_ylim[1]) > abs(current_ylim[1] - current_ylim[0]) * 0.05:
                    ax.set_ylim(new_min, new_max)
    
    def start(self):
        """启动示波器"""
        print("正在初始化Modbus连接...")
        if not self.init_modbus_client():
            print("Modbus客户端连接失败，无法启动示波器。")
            return
        
        print("示波器启动成功！按Ctrl+C停止...")
        
        # 创建动画
        self.ani = animation.FuncAnimation(
            self.fig, 
            self.update_data,
            interval=UPDATE_INTERVAL_MS,
            blit=False,  # 设置为False以确保坐标轴正确更新
            cache_frame_data=False,
            repeat=True
        )
        
        # 设置窗口关闭事件
        self.fig.canvas.mpl_connect('close_event', self.on_closing)
        
        try:
            plt.tight_layout()
            plt.show()
        except KeyboardInterrupt:
            self.cleanup()
        
    def on_closing(self, event):
        """窗口关闭事件处理"""
        print("正在关闭示波器...")
        self.cleanup()
    
    def cleanup(self):
        """清理资源"""
        if hasattr(self, 'ani'):
            self.ani.event_source.stop()
        
        if self.client and self.client.connected:
            self.client.close()
            print("Modbus客户端已关闭。")

def main():
    """主函数"""
    print("Modbus 示波器")
    print("=============")
    print("请选择示波器版本:")
    print("1. Matplotlib 版本 (传统界面)")
    print("2. Web 版本 (高性能，现代界面)")
    print()
    
    while True:
        try:
            choice = input("请输入选择 (1 或 2): ").strip()
            if choice == '1':
                print("正在启动 Matplotlib 版本...")
                oscilloscope = ModbusOscilloscope()
                oscilloscope.start()
                break
            elif choice == '2':
                print("正在启动 Web 版本...")
                try:
                    from modbus_web_oscilloscope import ModbusWebOscilloscope
                    web_oscilloscope = ModbusWebOscilloscope()
                    web_oscilloscope.run()
                except ImportError as e:
                    print("错误: 无法导入Web版本依赖库")
                    print("请安装以下依赖:")
                    print("pip install dash plotly")
                    print(f"详细错误: {e}")
                    print("将使用 Matplotlib 版本...")
                    oscilloscope = ModbusOscilloscope()
                    oscilloscope.start()
                break
            else:
                print("无效选择，请输入 1 或 2")
        except KeyboardInterrupt:
            print("\n程序已退出")
            break
        except Exception as e:
            print(f"发生错误: {e}")
            print("将使用默认的 Matplotlib 版本...")
            oscilloscope = ModbusOscilloscope()
            oscilloscope.start()
            break

if __name__ == "__main__":
    main()
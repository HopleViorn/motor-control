import dash
from dash import dcc, html, Input, Output, callback
import dash.dependencies
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import numpy as np
import time
from collections import deque
from threading import Thread, Event
import logging
from modbus_client import (connect_modbus_client, read_registers, send_power_on_command, 
                           get_available_ports, connect_modbus_client_by_port, 
                           send_power_off_command, send_speed_command)

# 配置示波器参数
MAX_POINTS = 300  # Web版可以显示更多数据点
UPDATE_INTERVAL_MS = 100  # 更新间隔，毫秒
TIME_WINDOW = 30  # 时间窗口，秒

# Modbus 读取配置
START_ADDRESS = 50
COUNT = 4  # 读取3个寄存器

# 配置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class ModbusWebOscilloscope:
    def __init__(self):
        self.client = None
        self.app = dash.Dash(__name__)
        
        # 数据存储
        self.data_queues = [deque(maxlen=MAX_POINTS) for _ in range(COUNT)]
        self.time_queue = deque(maxlen=MAX_POINTS)
        self.start_time = time.time()
        
        # 线程控制
        self.stop_event = Event()
        self.data_thread = None
        
        # 连接状态
        self.connection_status = "未连接"
        self.connection_fail_count = 0
        self.max_fail_count = 10
        self.selected_port = None
        
        # 设置布局
        self.setup_layout()
        
        # 设置回调
        self.setup_callbacks()
        
    def setup_layout(self):
        """设置Web界面布局"""
        self.app.layout = html.Div([
            html.H1("Modbus 实时示波器 (Web版)", 
                   style={'textAlign': 'center', 'color': '#2c3e50', 'marginBottom': '30px'}),
            
            # 连接设置面板
            html.Div([
                html.H3("连接设置", style={'color': '#34495e', 'marginBottom': '15px'}),
                html.Div([
                    html.Div([
                        html.Label("串口选择:", style={'marginRight': '10px', 'fontWeight': 'bold'}),
                        dcc.Dropdown(
                            id='port-dropdown',
                            placeholder="请选择串口",
                            style={'width': '300px', 'display': 'inline-block'}
                        ),
                        html.Button('刷新串口', id='refresh-ports-btn', n_clicks=0,
                                   style={'backgroundColor': '#3498db', 'color': 'white', 'marginLeft': '10px'}),
                        html.Button('连接', id='connect-btn', n_clicks=0,
                                   style={'backgroundColor': '#27ae60', 'color': 'white', 'marginLeft': '10px'}),
                        html.Button('断开', id='disconnect-btn', n_clicks=0,
                                   style={'backgroundColor': '#e74c3c', 'color': 'white', 'marginLeft': '10px'}),
                    ], style={'marginBottom': '15px'}),
                ]),
            ], style={'marginBottom': '20px', 'padding': '20px', 'backgroundColor': '#ecf0f1', 'borderRadius': '10px'}),
            
            # 控制面板
            html.Div([
                html.Div([
                    html.H3("数据采集控制", style={'color': '#34495e'}),
                    html.Button('开始采集', id='start-btn', n_clicks=0,
                               style={'backgroundColor': '#27ae60', 'color': 'white', 'marginRight': '10px'}),
                    html.Button('停止采集', id='stop-btn', n_clicks=0,
                               style={'backgroundColor': '#e74c3c', 'color': 'white', 'marginRight': '10px'}),
                    html.Button('清除数据', id='clear-btn', n_clicks=0,
                               style={'backgroundColor': '#f39c12', 'color': 'white'}),
                ], style={'width': '48%', 'display': 'inline-block'}),
                
                html.Div([
                    html.H3("设备控制", style={'color': '#34495e'}),
                    html.Button('开机', id='power-on-btn', n_clicks=0,
                               style={'backgroundColor': '#2ecc71', 'color': 'white', 'marginRight': '10px'}),
                    html.Button('关机', id='power-off-btn', n_clicks=0,
                               style={'backgroundColor': '#e67e22', 'color': 'white', 'marginRight': '15px'}),
                    html.Div([
                        dcc.Input(
                            id='speed-input',
                            type='number',
                            placeholder='转速(RPM)',
                            value=3000,
                            min=0,
                            max=30000,
                            style={'width': '120px', 'marginRight': '10px'}
                        ),
                        html.Button('设置转速', id='set-speed-btn', n_clicks=0,
                                   style={'backgroundColor': '#9b59b6', 'color': 'white'}),
                    ], style={'display': 'inline-block'}),
                ], style={'width': '48%', 'float': 'right', 'display': 'inline-block'}),
            ], style={'marginBottom': '20px', 'padding': '20px', 'backgroundColor': '#ecf0f1', 'borderRadius': '10px'}),
            
            # 状态显示面板
            html.Div([
                html.Div([
                    html.H3("连接状态", style={'color': '#34495e'}),
                    html.Div(id='connection-status', children=f"状态: {self.connection_status}",
                            style={'fontSize': '16px', 'color': '#e74c3c'}),
                    html.Div(id='data-count', children="数据点数: 0",
                            style={'fontSize': '14px', 'color': '#7f8c8d'}),
                ], style={'width': '48%', 'display': 'inline-block'}),
                
                html.Div([
                    html.H3("操作消息", style={'color': '#34495e'}),
                    html.Div(id='operation-message', children="等待操作...",
                            style={'fontSize': '14px', 'color': '#7f8c8d'}),
                ], style={'width': '48%', 'float': 'right', 'display': 'inline-block'}),
            ], style={'marginBottom': '30px', 'padding': '20px', 'backgroundColor': '#f8f9fa', 'borderRadius': '10px'}),
            
            # 图表显示
            dcc.Graph(id='live-graph', style={'height': '60vh'}),
            
            # 自动更新组件
            dcc.Interval(
                id='interval-component',
                interval=UPDATE_INTERVAL_MS,
                n_intervals=0,
                disabled=True  # 初始状态为禁用
            ),
            
            # 存储组件
            dcc.Store(id='data-store'),
            dcc.Store(id='ports-store', data=[]),
        ], style={'padding': '20px', 'fontFamily': 'Arial, sans-serif'})
    
    def setup_callbacks(self):
        """设置回调函数"""
        
        # 串口相关回调 - 合并初始化和刷新功能
        @self.app.callback(
            [Output('port-dropdown', 'options'),
             Output('ports-store', 'data')],
            [Input('refresh-ports-btn', 'n_clicks'),
             Input('port-dropdown', 'id')]
        )
        def refresh_ports(n_clicks, _):
            """刷新串口列表（包含页面初始化）"""
            ports = get_available_ports()
            options = [{'label': f"{p['device']} - {p['description']}", 'value': p['device']} for p in ports]
            return options, ports
        
        @self.app.callback(
            [Output('connection-status', 'children'),
             Output('connection-status', 'style')],
            [Input('connect-btn', 'n_clicks'),
             Input('disconnect-btn', 'n_clicks')],
            [dash.dependencies.State('port-dropdown', 'value')]
        )
        def handle_connection(connect_clicks, disconnect_clicks, selected_port):
            """处理连接和断开"""
            ctx = dash.callback_context
            if not ctx.triggered:
                return f"状态: {self.connection_status}", {'fontSize': '16px', 'color': '#e74c3c'}
            
            button_id = ctx.triggered[0]['prop_id'].split('.')[0]
            
            if button_id == 'connect-btn' and connect_clicks > 0:
                if not selected_port:
                    return "状态: 请选择串口", {'fontSize': '16px', 'color': '#e74c3c'}
                
                if self.client and self.client.connected:
                    self.client.close()
                
                self.client = connect_modbus_client_by_port(selected_port)
                if self.client:
                    self.selected_port = selected_port
                    self.connection_status = "已连接"
                    return "状态: 已连接", {'fontSize': '16px', 'color': '#27ae60'}
                else:
                    self.connection_status = "连接失败"
                    return "状态: 连接失败", {'fontSize': '16px', 'color': '#e74c3c'}
            
            elif button_id == 'disconnect-btn' and disconnect_clicks > 0:
                self.stop_data_collection()
                if self.client and self.client.connected:
                    self.client.close()
                self.connection_status = "已断开"
                return "状态: 已断开", {'fontSize': '16px', 'color': '#f39c12'}
            
            return f"状态: {self.connection_status}", {'fontSize': '16px', 'color': '#e74c3c'}
        
        # 数据采集控制回调
        @self.app.callback(
            [Output('interval-component', 'disabled')],
            [Input('start-btn', 'n_clicks'),
             Input('stop-btn', 'n_clicks')]
        )
        def control_data_collection(start_clicks, stop_clicks):
            """控制数据采集的开始和停止"""
            ctx = dash.callback_context
            if not ctx.triggered:
                return [True]
            
            button_id = ctx.triggered[0]['prop_id'].split('.')[0]
            
            if button_id == 'start-btn' and start_clicks > 0:
                if not self.client or not self.client.connected:
                    return [True]
                success = self.start_data_collection_only()
                return [not success]
            elif button_id == 'stop-btn' and stop_clicks > 0:
                self.stop_data_collection_only()
                return [True]
            
            return [True]
        
        # 设备控制回调
        @self.app.callback(
            Output('operation-message', 'children'),
            [Input('power-on-btn', 'n_clicks'),
             Input('power-off-btn', 'n_clicks'),
             Input('set-speed-btn', 'n_clicks'),
             Input('connect-btn', 'n_clicks'),
             Input('disconnect-btn', 'n_clicks')],
            [dash.dependencies.State('speed-input', 'value'),
             dash.dependencies.State('port-dropdown', 'value')],
            prevent_initial_call=True
        )
        def handle_device_control(power_on_clicks, power_off_clicks, speed_clicks, connect_clicks, disconnect_clicks, speed_value, selected_port):
            """处理所有设备操作命令"""
            ctx = dash.callback_context
            if not ctx.triggered:
                return "等待操作..."
            
            button_id = ctx.triggered[0]['prop_id'].split('.')[0]
            
            # 处理连接相关操作
            if button_id == 'connect-btn' and connect_clicks > 0:
                if not selected_port:
                    return "错误: 未选择串口"
                return f"成功连接到 {selected_port}" if self.client and self.client.connected else f"连接失败: {selected_port}"
            
            elif button_id == 'disconnect-btn' and disconnect_clicks > 0:
                return "已断开连接"
            
            # 处理设备控制操作（需要连接）
            if not self.client or not self.client.connected:
                return "错误: 设备未连接"
            
            if button_id == 'power-on-btn' and power_on_clicks > 0:
                success = send_power_on_command(self.client)
                return "开机指令发送成功" if success else "开机指令发送失败"
            
            elif button_id == 'power-off-btn' and power_off_clicks > 0:
                success = send_power_off_command(self.client)
                return "关机指令发送成功" if success else "关机指令发送失败"
            
            elif button_id == 'set-speed-btn' and speed_clicks > 0:
                if speed_value is None or speed_value < 0 or speed_value > 30000:
                    return "错误: 转速范围 0-30000 RPM"
                success = send_speed_command(self.client, int(speed_value))
                return f"设置转速 {speed_value} RPM 成功" if success else f"设置转速 {speed_value} RPM 失败"
            
            return "等待操作..."
        
        @self.app.callback(
            Output('data-store', 'data'),
            [Input('clear-btn', 'n_clicks')]
        )
        def clear_data(n_clicks):
            """清除数据"""
            if n_clicks > 0:
                self.clear_all_data()
            return {}
        
        @self.app.callback(
            [Output('live-graph', 'figure'),
             Output('data-count', 'children')],
            [Input('interval-component', 'n_intervals'),
             Input('data-store', 'data')]
        )
        def update_graph(n, data_store):
            """更新图表"""
            return self.create_figure(), f"数据点数: {len(self.time_queue)}"
    
    def create_figure(self):
        """创建Plotly图表"""
        # 创建子图
        fig = make_subplots(
            rows=COUNT, cols=1,
            shared_xaxes=True,
            subplot_titles=[f'Register {START_ADDRESS + i}' for i in range(COUNT)],
            vertical_spacing=0.08
        )
        
        # 颜色配置
        colors = ['#3498db', '#e74c3c', '#2ecc71', '#f39c12', '#9b59b6', '#34495e', '#e91e63', '#ff9800']
        
        if len(self.time_queue) > 0:
            time_list = list(self.time_queue)
            
            for i in range(COUNT):
                if i < len(self.data_queues):
                    data_list = list(self.data_queues[i])
                    
                    if len(data_list) > 0:
                        fig.add_trace(
                            go.Scatter(
                                x=time_list,
                                y=data_list,
                                mode='lines',
                                name=f'Register {START_ADDRESS + i}',
                                line=dict(color=colors[i % len(colors)], width=2),
                                hovertemplate='<b>%{fullData.name}</b><br>时间: %{x:.2f}s<br>值: %{y}<extra></extra>'
                            ),
                            row=i+1, col=1
                        )
        
        # 更新布局
        fig.update_layout(
            height=600,
            title_text="Modbus 寄存器实时监控",
            title_x=0.5,
            showlegend=True,
            legend=dict(x=1.02, y=1),
            margin=dict(l=50, r=150, t=80, b=50),
            plot_bgcolor='white',
            paper_bgcolor='white'
        )
        
        # 设置X轴
        if len(self.time_queue) > 0:
            current_time = list(self.time_queue)[-1]
            if current_time > TIME_WINDOW:
                x_range = [current_time - TIME_WINDOW, current_time]
            else:
                x_range = [0, TIME_WINDOW]
        else:
            x_range = [0, TIME_WINDOW]
            
        fig.update_xaxes(
            title_text="时间 (秒)",
            range=x_range,
            gridcolor='lightgray',
            row=COUNT, col=1
        )
        
        # 设置Y轴
        for i in range(COUNT):
            fig.update_yaxes(
                title_text="寄存器值",
                gridcolor='lightgray',
                row=i+1, col=1
            )
        
        return fig
    
    def init_modbus_client(self):
        """初始化Modbus客户端"""
        self.client = connect_modbus_client()
        if self.client:
            logger.info("正在发送开机指令...")
            send_power_on_command(self.client)
            self.connection_status = "已连接"
            return True
        self.connection_status = "连接失败"
        return False
    
    def data_collection_thread(self):
        """数据采集线程"""
        while not self.stop_event.is_set():
            if not self.client or not self.client.connected:
                logger.warning(f"连接失败次数: {self.connection_fail_count}")
                self.connection_fail_count += 1
                if self.connection_fail_count >= self.max_fail_count:
                    logger.error("连接失败次数过多，停止尝试")
                    break
                time.sleep(1)
                continue
            
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
                        if reg_value > 32767:
                            signed_value = reg_value - 65536
                        else:
                            signed_value = reg_value
                        if i==3:
                            signed_value = registers[1]*registers[2]
                        self.data_queues[i].append(float(signed_value))
            
            # 控制采集频率
            time.sleep(UPDATE_INTERVAL_MS / 1000.0)
    
    def start_data_collection(self):
        """开始数据采集（旧版本，包含连接）"""
        if not self.init_modbus_client():
            return False
        
        self.stop_event.clear()
        self.start_time = time.time()
        self.data_thread = Thread(target=self.data_collection_thread)
        self.data_thread.daemon = True
        self.data_thread.start()
        
        logger.info("数据采集已开始")
        return True
    
    def start_data_collection_only(self):
        """仅开始数据采集（不包含连接）"""
        if not self.client or not self.client.connected:
            return False
        
        self.stop_event.clear()
        self.start_time = time.time()
        self.data_thread = Thread(target=self.data_collection_thread)
        self.data_thread.daemon = True
        self.data_thread.start()
        
        logger.info("数据采集已开始")
        return True
    
    def stop_data_collection(self):
        """停止数据采集并关闭连接"""
        self.stop_event.set()
        if self.data_thread and self.data_thread.is_alive():
            self.data_thread.join(timeout=2)
        
        if self.client and self.client.connected:
            self.client.close()
            logger.info("Modbus客户端已关闭")
        
        self.connection_status = "已停止"
        logger.info("数据采集已停止")
    
    def stop_data_collection_only(self):
        """仅停止数据采集（不关闭连接）"""
        self.stop_event.set()
        if self.data_thread and self.data_thread.is_alive():
            self.data_thread.join(timeout=2)
        
        logger.info("数据采集已停止")
    
    def clear_all_data(self):
        """清除所有数据"""
        self.time_queue.clear()
        for queue in self.data_queues:
            queue.clear()
        self.start_time = time.time()
        logger.info("数据已清除")
    
    def run(self, debug=False, host='127.0.0.1', port=8050):
        """运行Web应用"""
        try:
            print(f"正在启动Web示波器...")
            print(f"请在浏览器中打开: http://{host}:{port}")
            self.app.run_server(debug=debug, host=host, port=port)
        except KeyboardInterrupt:
            print("\n正在关闭Web示波器...")
        finally:
            self.stop_data_collection()

def main():
    """主函数"""
    print("Modbus Web 示波器")
    print("================")
    print("这是一个基于Web的高性能实时示波器")
    print("功能特点:")
    print("- 更好的性能和响应速度")
    print("- 现代化的用户界面")
    print("- 支持更多数据点显示")
    print("- 可以在浏览器中运行")
    print()
    
    oscilloscope = ModbusWebOscilloscope()
    oscilloscope.run(debug=False)

if __name__ == "__main__":
    main() 
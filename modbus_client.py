import serial
from pymodbus.client import ModbusSerialClient
from pymodbus.exceptions import ModbusException
import time
import serial.tools.list_ports

# 串口配置
BAUDRATE = 115200
TIMEOUT = 1  # 秒

# Modbus Slave 地址
SLAVE_ADDRESS = 0x01

def connect_modbus_client():
    """连接Modbus RTU客户端"""
    ports = serial.tools.list_ports.comports()
    if not ports:
        print("未找到可用串口。请检查串口连接。")
        return None

    print("可用串口:")
    for i, p in enumerate(ports):
        print(f"{i+1}. {p.device} - {p.description}")

    selected_port = None
    while selected_port is None:
        try:
            choice = input("请选择串口号 (输入序号): ")
            index = int(choice) - 1
            if 0 <= index < len(ports):
                selected_port = ports[index].device
            else:
                print("无效的序号，请重新输入。")
        except ValueError:
            print("输入无效，请输入数字序号。")

    try:
        client = ModbusSerialClient(
            port=selected_port,
            baudrate=BAUDRATE,
            bytesize=8,
            parity='N',
            stopbits=1,
            timeout=TIMEOUT
        )
        if client.connect():
            print(f"成功连接到串口 {selected_port}，波特率 {BAUDRATE}")
            return client
        else:
            print(f"无法连接到串口 {selected_port}。请检查串口是否被占用或配置是否正确。")
            return None
    except serial.SerialException as e:
        print(f"串口错误: {e}")
        return None
    except Exception as e:
        print(f"连接Modbus客户端时发生未知错误: {e}")
        return None

def get_available_ports():
    """获取可用串口列表"""
    ports = serial.tools.list_ports.comports()
    port_list = []
    for p in ports:
        port_info = {
            'device': p.device,
            'description': p.description,
            'hwid': p.hwid
        }
        port_list.append(port_info)
    return port_list

def connect_modbus_client_by_port(port_name):
    """连接指定串口的Modbus RTU客户端"""
    try:
        client = ModbusSerialClient(
            port=port_name,
            baudrate=BAUDRATE,
            bytesize=8,
            parity='N',
            stopbits=1,
            timeout=TIMEOUT
        )
        if client.connect():
            print(f"成功连接到串口 {port_name}，波特率 {BAUDRATE}")
            return client
        else:
            print(f"无法连接到串口 {port_name}。请检查串口是否被占用或配置是否正确。")
            return None
    except serial.SerialException as e:
        print(f"串口错误: {e}")
        return None
    except Exception as e:
        print(f"连接Modbus客户端时发生未知错误: {e}")
        return None

def send_power_on_command(client):
    """发送开机指令: 01 06 00 03 00 01 B8 0A (写单个寄存器，地址0x0003，值0x0001)"""
    if not client or not client.connected:
        print("Modbus客户端未连接。")
        return False
    
    register_address = 0x0003  # 寄存器地址
    value_to_write = 0x0001    # 写入的值
    
    print(f"正在发送开机指令: 写寄存器地址 {register_address}，值 {value_to_write}")
    try:
        # ModbusSerialClient的write_register方法会自动计算CRC
        response = client.write_register(address=register_address, value=value_to_write, slave=SLAVE_ADDRESS)
        
        if isinstance(response, ModbusException):
            print(f"发送开机指令失败: Modbus异常 - {response}")
            return False
        elif response is None:
            print("发送开机指令失败: 未收到响应。")
            return False
        else:
            print("开机指令发送成功。")
            return True
    except ModbusException as e:
        print(f"发送开机指令时发生Modbus协议错误: {e}")
        return False
    except Exception as e:
        print(f"发送开机指令时发生未知错误: {e}")
        return False

def send_power_off_command(client):
    """发送关机指令: 01 06 00 03 00 00 79 CA (写单个寄存器，地址0x0003，值0x0000)"""
    if not client or not client.connected:
        print("Modbus客户端未连接。")
        return False
    
    register_address = 0x0003  # 寄存器地址
    value_to_write = 0x0000    # 写入的值
    
    print(f"正在发送关机指令: 写寄存器地址 {register_address}，值 {value_to_write}")
    try:
        response = client.write_register(address=register_address, value=value_to_write, slave=SLAVE_ADDRESS)
        
        if isinstance(response, ModbusException):
            print(f"发送关机指令失败: Modbus异常 - {response}")
            return False
        elif response is None:
            print("发送关机指令失败: 未收到响应。")
            return False
        else:
            print("关机指令发送成功。")
            return True
    except ModbusException as e:
        print(f"发送关机指令时发生Modbus协议错误: {e}")
        return False
    except Exception as e:
        print(f"发送关机指令时发生未知错误: {e}")
        return False

def send_speed_command(client, speed_rpm):
    """发送调速指令: 01 06 00 02 XXXX YYYY (写单个寄存器，地址0x0002，值为转速)"""
    if not client or not client.connected:
        print("Modbus客户端未连接。")
        return False
    
    register_address = 0x0002  # 寄存器地址
    
    # 限制速度范围（根据实际需要调整）
    if speed_rpm < 0:
        speed_rpm = 0
    elif speed_rpm > 30000:
        speed_rpm = 30000
    
    print(f"正在发送调速指令: 写寄存器地址 {register_address}，速度 {speed_rpm} RPM")
    try:
        response = client.write_register(address=register_address, value=speed_rpm, slave=SLAVE_ADDRESS)
        
        if isinstance(response, ModbusException):
            print(f"发送调速指令失败: Modbus异常 - {response}")
            return False
        elif response is None:
            print("发送调速指令失败: 未收到响应。")
            return False
        else:
            print(f"调速指令发送成功，设置速度: {speed_rpm} RPM")
            return True
    except ModbusException as e:
        print(f"发送调速指令时发生Modbus协议错误: {e}")
        return False
    except Exception as e:
        print(f"发送调速指令时发生未知错误: {e}")
        return False

def send_emergency_stop_command(client):
    """发送紧急停机指令: 01 06 00 28 00 01 C8 02 (写单个寄存器，地址0x0028，值0x0001)"""
    if not client or not client.connected:
        print("Modbus客户端未连接。")
        return False
    
    register_address = 0x0028  # 寄存器地址
    value_to_write = 0x0001    # 写入的值
    
    print(f"正在发送紧急停机指令: 写寄存器地址 {register_address}，值 {value_to_write}")
    try:
        response = client.write_register(address=register_address, value=value_to_write, slave=SLAVE_ADDRESS)
        
        if isinstance(response, ModbusException):
            print(f"发送紧急停机指令失败: Modbus异常 - {response}")
            return False
        elif response is None:
            print("发送紧急停机指令失败: 未收到响应。")
            return False
        else:
            print("紧急停机指令发送成功。")
            return True
    except ModbusException as e:
        print(f"发送紧急停机指令时发生Modbus协议错误: {e}")
        return False
    except Exception as e:
        print(f"发送紧急停机指令时发生未知错误: {e}")
        return False

def send_clear_error_command(client):
    """发送清除错误代码指令: 01 06 00 1B 00 01 38 0D (写单个寄存器，地址0x001B，值0x0001)"""
    if not client or not client.connected:
        print("Modbus客户端未连接。")
        return False
    
    register_address = 0x001B  # 寄存器地址
    value_to_write = 0x0001    # 写入的值
    
    print(f"正在发送清除错误代码指令: 写寄存器地址 {register_address}，值 {value_to_write}")
    try:
        response = client.write_register(address=register_address, value=value_to_write, slave=SLAVE_ADDRESS)
        
        if isinstance(response, ModbusException):
            print(f"发送清除错误代码指令失败: Modbus异常 - {response}")
            return False
        elif response is None:
            print("发送清除错误代码指令失败: 未收到响应。")
            return False
        else:
            print("清除错误代码指令发送成功。")
            return True
    except ModbusException as e:
        print(f"发送清除错误代码时发生Modbus协议错误: {e}")
        return False
    except Exception as e:
        print(f"发送清除错误代码时发生未知错误: {e}")
        return False

def read_registers(client, start_address, count):
    """
    读取指定范围的寄存器。
    :param client: Modbus客户端实例
    :param start_address: 起始寄存器地址
    :param count: 读取的寄存器数量
    :return: 寄存器值列表或None
    """
    if not client or not client.connected:
        print("Modbus客户端未连接。")
        return None
    
    # print(f"正在读取寄存器，起始地址: {start_address}, 数量: {count}")
    try:
        # ModbusSerialClient的read_holding_registers方法会自动计算CRC
        response = client.read_holding_registers(address=start_address, count=count, slave=SLAVE_ADDRESS)
        
        if isinstance(response, ModbusException):
            print(f"读取寄存器失败: Modbus异常 - {response}")
            return None
        elif response is None:
            print("读取寄存器失败: 未收到响应。")
            return False
        else:
            # print(f"读取成功。寄存器值: {response.registers}")
            return response.registers
    except ModbusException as e:
        print(f"读取寄存器时发生Modbus协议错误: {e}")
        return None
    except Exception as e:
        print(f"读取寄存器时发生未知错误: {e}")
        return None

def main():
    client = None
    try:
        client = connect_modbus_client()
        if client:
            # 尝试发送开机指令
            send_power_on_command(client)
            
            # 不间断读取寄存器并动态展示
            start_address = 50  # 示例：从地址 0x0000 开始
            count = 1              # 示例：读取 10 个寄存器
            
            print(f"\n开始不间断读取寄存器，起始地址: {start_address}, 数量: {count}")
            print("按 Ctrl+C 退出。")
            
            try:
                while True:
                    registers = read_registers(client, start_address, count)
                    # if registers is not None:
                        # 动态展示，可以根据需要格式化输出
                        # print(f"当前时间: {time.strftime('%Y-%m-%d %H:%M:%S')}, 寄存器值: {registers}")
                    time.sleep(0.01) # 可以调整读取频率
            except KeyboardInterrupt:
                print("\n用户中断，退出程序。")

    finally:
        if client and client.connected:
            client.close()
            print("Modbus客户端已关闭。")

if __name__ == "__main__":
    # 在运行前，请确保已安装 pyserial 和 pymodbus 库:
    # pip install pyserial pymodbus
    main()
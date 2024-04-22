#include<iostream>
#include<modbus.h>

#include "/VS_programing/pi/Relay/Relay/shm.h"

int main(int argc, char *argv[])
{
    modbus_t *ctx = nullptr;
    modbus_mapping_t *mb_mapping=nullptr;
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

    // 1. TCP SERVER ON
    ctx = modbus_new_tcp("192.168.219.110",502);
    if(ctx == nullptr){
        std::cerr<<"Error to make the libmodbus [Context]"<<std::endl;
        return -1;
    }
    
    // 2. Mapping
    mb_mapping = modbus_mapping_new(0, 0, 100, 0);
    // 2-E. Mapping Error
    if(mb_mapping == nullptr){
        std::cerr << "Failed to allocate the mapping : " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx);
        return -1;
    }

    // 초기 GPIO제어 레지스터 값 설정
    mb_mapping->tab_registers[2] = 0;

    // 3. Socket 생성
    int server_socket = modbus_tcp_listen(ctx, 1);
    std::cout << "Socket " << std::endl;
    if (server_socket == -1) {
        std::cerr << "Unable to listen TCP Connection" << std::endl;
        modbus_free(ctx);
        modbus_mapping_free(mb_mapping);
        return -1;
    }

    // 4. Socket Connection accept
    int rc = modbus_tcp_accept(ctx, &server_socket);
    std::cout << "Socket Connection" << std::endl;
    if (rc == -1) {
        std::cerr << "Accept failed" << std::endl;
        modbus_close(ctx);
        modbus_free(ctx);
        modbus_mapping_free(mb_mapping);
        return -1;
    }
    
    // 공유 메모리 매니저 초기화
    SharedMemoryManager shm(L"Temp_SHM", sizeof(int) * 2);

    

    for(;;){
        int rc = modbus_receive(ctx, query);
        if(rc>0){
            int humid = mb_mapping->tab_registers[0]; // 습도 값은 레지스터 0번에 저장되어 있다고 가정
            
            // 습도 값을 공유 메모리(윈도우내)에 저장
            shm.write(0, mb_mapping->tab_registers[0]);
            std::cout << "Data Received [ Humid : " << humid << " Gpio code is " << mb_mapping->tab_registers[2] << " ]" << std::endl;

            if (shm.read(2)==1) {
                mb_mapping->tab_registers[2] = 1; // 제어 코드 1 설정
                std::cout << "Data Send [ Relay Operate Code : 1 ] " << std::endl;
            }
            else
            {
                mb_mapping->tab_registers[2] = 0; // 제어 코드 0 설정
            }
            modbus_reply(ctx, query, rc, mb_mapping);
        }
        else if(rc == -1){
            break;
        }
  
    }

    std::cout << "Closing the server. \n";

    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
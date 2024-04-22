#include "shm.h"

int main()
{
    SharedMemoryManager shm(L"Temp_SHM", sizeof(int) * 2);
    int count = 0;

    while (1)
    {
        // ���� �޸𸮿��� �µ� ���� �б� [0 : humid , 1 : temp , 2 : Relay run code ] 
        int humid = shm.read(0);
        std::cout << "Data Recieved [" << count << "] Humid : " << humid << std::endl;

        // 60�� �̻��̸� GPIO ���� �ڵ� ������
        if (humid >= 65) {
            shm.write(2, 1);
            std::cout << "[Humid is Over 65, GPIO set to 1.]" << std::endl;
        
        }
        else {
            shm.write(2, 0);
        }
        count++;
        Sleep(1000);
    }

    return 0;
}
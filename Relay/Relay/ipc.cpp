#include <windows.h>
#include <iostream>

int main() {
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(int) * 2, // 온도 값과 GPIO 값의 저장 공간
        L"SharedMemoryExample");

    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
        return 1;
    }

    int* pBuf = (int*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(int) * 2);

    if (pBuf == NULL) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // 공유 메모리에 온도 값을 설정
    pBuf[0] = 25; // 예시 온도 값

    // 다른 프로세스가 변경을 감지할 때까지 대기
    std::cout << "Temperature set in shared memory. Press Enter to exit..." << std::endl;
    std::cin.get();

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}
#pragma once

#include <windows.h>
#include <iostream>

class SharedMemoryManager {
private:
    HANDLE hMapFile;
    int* pBuf;

public:
    SharedMemoryManager(const wchar_t* name, size_t size) : hMapFile(NULL), pBuf(NULL) {
        hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            size,
            name);

        if (hMapFile == NULL) {
            std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
            throw std::runtime_error("Failed to create file mapping");
        }

        pBuf = (int*)MapViewOfFile(
            hMapFile,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            size);

        if (pBuf == NULL) {
            CloseHandle(hMapFile);
            std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
            throw std::runtime_error("Failed to map view of file");
        }
    }

    ~SharedMemoryManager() {
        if (pBuf != NULL) {
            UnmapViewOfFile(pBuf);
        }
        if (hMapFile != NULL) {
            CloseHandle(hMapFile);
        }
    }

    void write(int index, int value) {
        pBuf[index] = value;
    }

    int read(int index) {
        return pBuf[index];
    }
};
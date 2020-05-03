#if !defined(WIN32_PLATFORM_H)
#define WIN32_PLATFORM_H

typedef struct
{

    void *memory_storage;
    uint64 memory_storage_size;
    GLuint vao;
    HDC device_context;
    bool32 is_running;

} Win32PlatformState;

#endif

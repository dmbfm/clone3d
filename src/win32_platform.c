#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <wingdi.h>
#include <gl/gl.h>
#include "main.h"
#include "win32_platform.h"
#include "win32_opengl.h"

#include "main.c"
#include "win32_opengl.c"

#define OutputDebugStringFmt(STR, ...) \
    { char buf[256]; StringCbPrintfA(buf, 256, STR, __VA_ARGS__); OutputDebugStringA(buf); }

global const int window_width = 1024;
global const int window_height = 768;
global const int framebuffer_width = 1024;
global const int framebuffer_height = 768;
global const int target_fps = 60;

LRESULT CALLBACK win32_window_procedure(HWND   hwnd,
					UINT   uMsg,
					WPARAM wParam,
					LPARAM lParam)
{
    switch (uMsg)
    {
	case WM_CLOSE:
	{
	    PostQuitMessage(0);
	    return 0;
	}break;
    
	default:
	{
	    return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}break;
    }
}

bool32 read_entire_file(const char *filename, ReadFileResult *result)
{
    HANDLE file_handle = CreateFileA(
	filename,
	GENERIC_READ,
	FILE_SHARE_READ,
	0,
	OPEN_EXISTING,
	FILE_ATTRIBUTE_NORMAL,
	0);

    if (file_handle == INVALID_HANDLE_VALUE)
    {
	OutputDebugStringA("Error opening file!");
	return;
    }

    LARGE_INTEGER file_size = {0};
    GetFileSizeEx(file_handle, &file_size);

    OutputDebugStringFmt("File size = %lld\n", file_size.QuadPart);

    void *buffer = (void *) malloc(file_size.QuadPart);

    if (!ReadFile(file_handle,
	     buffer,
	     file_size.QuadPart,
	     0,
		  0)) {
	return false;
    }

    result->buffer = buffer;
    result->size = (uint64) file_size.QuadPart;

    return true;
}

void process_messages(Win32PlatformState *win32_platform, InputState *input)
{
    MSG message;
    MouseState *mouse = &input->mouse;
    
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
	switch(message.message)
	{
	    case WM_QUIT:
	    {
		OutputDebugStringA("WM_QUIT\n");
		win32_platform->is_running = 0;
	    } break;

	    case WM_MOUSEMOVE:
	    {
		mouse->x_pos = GET_X_LPARAM(message.lParam);
		mouse->y_pos = GET_Y_LPARAM(message.lParam);
	    } break;

	    case WM_SYSKEYDOWN:
	    case WM_SYSKEYUP:
	    case WM_KEYDOWN:
	    case WM_KEYUP:
	    {
		WPARAM vk = message.wParam;
		//uint16 repeat_count = (uint16) message.lParam;
		//uint8 scan_code = (uint8) (message.lParam >> 16 );

		bool32 was_down = (message.lParam & (1 << 30)) != 0;
		bool32 is_down = (message.lParam & (1 << 31)) == 0;

		if (is_down != was_down)
		{
		    switch(vk)
		    {
			case VK_UP:
			case 'W':
			{
			    key_state_set(&input->key_up, is_down, was_down);
			} break;

			case VK_DOWN:
			case 'S':
			{
			    key_state_set(&input->key_down, is_down, was_down);
			} break;
			    
			case VK_LEFT:
			{
			    key_state_set(&input->key_left, is_down, was_down);
			} break;
			    
			case VK_RIGHT:
			{
			    key_state_set(&input->key_right, is_down, was_down);
			} break;

			case VK_ESCAPE:
			{
			    key_state_set(&input->key_esc, is_down, was_down);
			}
				
			default: {} break;
		    }
		}
		    
	    }break;
		
	    default:
	    {
		TranslateMessage(&message);
		DispatchMessage(&message);
	    } break;
	}
    }
}

void present_framebuffer(Win32PlatformState *state, Video *video)
{
    glTexSubImage2D(GL_TEXTURE_2D,
		    0,
		    0,
		    0,
		    framebuffer_width,
		    framebuffer_height,
		    GL_RGBA,
		    GL_UNSIGNED_BYTE,
		    video->framebuffer);
    
    glBindVertexArray(state->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    
    SwapBuffers(state->device_context);
}

int APIENTRY WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR     lpCmdLine,
		     int       nShowCmd)
{

    WNDCLASSEXA window_class = {0};
    window_class.cbSize = sizeof(WNDCLASSEXA);
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = win32_window_procedure;
    window_class.hInstance = hInstance;
    window_class.hCursor = LoadCursor(hInstance, IDC_ARROW);
    window_class.lpszClassName = "Raster";

    if (!RegisterClassExA(&window_class))
    {
	printf("[win32]: Failed to register window class!");
	return -1;
    }
    
    HWND window_handler;
    if (!(window_handler = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW,
					   window_class.lpszClassName,
					   "SoftRast",
					   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					   CW_USEDEFAULT,
					   CW_USEDEFAULT,
					   window_width,
					   window_height,
					   0,
					   0,
					   hInstance,
					   0)))
    {
	printf("[win32]: Failed to create window!");
	return -1;
    }

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cRedBits = 8;
    pfd.cDepthBits = 16;
    pfd.cStencilBits = 0;
    pfd.bReserved = PFD_MAIN_PLANE;

    HDC device_context = GetDC(window_handler);
    int pixel_format = ChoosePixelFormat(device_context, &pfd);

    if (!SetPixelFormat(device_context, pixel_format, &pfd))
    {
	printf("[raster] Error: failed to set pixel format.");
	return -1;
    }

    HGLRC gl_context;
    if (!(gl_context = wglCreateContext(device_context)))
    {
	printf("[raster] Error: failed to create OpengGL context.");
	return -1;
    }

    if (!wglMakeCurrent(device_context, gl_context))
    {
	printf("[raster] Error: failed to make current context.");
	return -1;
    }

    ReleaseDC(window_handler, device_context);

    //  Allocate memory
    Win32PlatformState win32_platform = {0};
    win32_platform.device_context = device_context;
	
    win32_platform.memory_storage_size = GB(1);
    win32_platform.memory_storage = VirtualAlloc(0,
						 win32_platform.memory_storage_size,
						 MEM_RESERVE|MEM_COMMIT,
						 PAGE_READWRITE);
    
    if (!win32_platform.memory_storage)
    {
	printf("Error: memory allocation failure.");
	return(-1);
    }

    Video video = {0};
    video.framebuffer = (uint32 *) win32_platform.memory_storage;
    video.width = framebuffer_width;
    video.height = framebuffer_height;
    video.bytes_per_pixel = 4;
    video.total_size_in_bytes = 4 * video.width * video.height;

    Memory memory = {0};
    memory.storage = ((char *) win32_platform.memory_storage) + video.total_size_in_bytes;
    memory.size = win32_platform.memory_storage_size - video.total_size_in_bytes;

    InputState input = {0};
    
    // Initialize OpenGL Functions
    init_opengl_function_pointers();

    // Create our basic shader program
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertex_shader_source[] = {
	"#version 330 core\n",
	"layout(location = 0) in vec3 v_position;\n",
	"layout(location = 1) in vec2 v_uv;\n",
	"out vec2 uv;"
	"void main() { gl_Position.xyz = v_position; gl_Position.w = 1.0; uv = v_uv; }\n"
    };
    glShaderSource(vertex_shader, sizeof(vertex_shader_source) / sizeof(char *), vertex_shader_source, 0);
    glCompileShader(vertex_shader);

    GLint compiled_ok = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled_ok);
    if (compiled_ok == GL_FALSE)
    {
	GLsizei length = 0;
	GLchar log[512] = {0};
	glGetShaderInfoLog(vertex_shader, 512, &length, log);
	OutputDebugStringA("Error compiling vertex shader");
	OutputDebugStringA(log);
	
	return(-1);
    }

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *frag_shader_source[] = {
	"#version 330 core\n",
	"out vec4 color;\n",
	"in vec2 uv;\n",
	"uniform sampler2D u_texture;\n",
	"void main() {  color = vec4(texture(u_texture, uv).rgb, 1.0);}"
    };
    glShaderSource(frag_shader, sizeof(frag_shader_source) / sizeof(char *), frag_shader_source, 0);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &compiled_ok);
    if (compiled_ok == GL_FALSE)
    {
	GLsizei length = 0;
	GLchar log[512] = {0};
	glGetShaderInfoLog(frag_shader, 512, &length, log);
	OutputDebugStringA("Error compiling fragment shader");
	OutputDebugStringA(log);
	
	return(-1);
    }


    GLuint shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);

    GLint linked_ok = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &linked_ok);
    if (linked_ok == GL_FALSE)
    {
	GLsizei length = 0;
	GLchar log[512] = {0};
	glGetProgramInfoLog(frag_shader, 512, &length, log);
	OutputDebugStringA("Error linking shader program");
	OutputDebugStringA(log);
	
	return(-1);
    }

    const real32 vertices[] = {
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f,
	
	1.0f, -1.0f, 0.0f,
	1.0f, 0.0f,
	
	-1.0f, -1.0f, 0.0f,
	0.0f, 0.0f,
	
	-1.0f, 1.0f, 0.0f,
	0.0f, 1.0f
    };

    const uint16 indices[] = {
	0, 2, 1,
	0, 3, 2
    };

    GLuint vbo;
    GLuint vao;
    GLuint ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint fb_tex;
    glGenTextures(1, &fb_tex);
    glBindTexture(GL_TEXTURE_2D, fb_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, framebuffer_width, framebuffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, video.framebuffer);
    glActiveTexture(GL_TEXTURE0);

    PlatformLayer win32_platform_layer = {0};
    win32_platform_layer.print_string = OutputDebugStringA;
    win32_platform_layer.read_entire_file = read_entire_file;
    
    glUseProgram(shader_program);
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glUniform1i(glGetUniformLocation(shader_program, "u_texture"), 0);

    win32_platform.vao = vao;
    
    ShowWindow(window_handler, nShowCmd);

    LARGE_INTEGER p_freq, p_start, p_end, p_ellapsed_ns;
    QueryPerformanceFrequency(&p_freq);

    int target_ns = (int) ((1.0f / target_fps) * 1000000.0f);

    win32_platform.is_running = true;

    SetCapture(window_handler);
    ShowCursor(false);

    ReadFileResult file = {0};
    
    while(win32_platform.is_running)
    {
	QueryPerformanceCounter(&p_start);
	
	glClear(GL_COLOR_BUFFER_BIT);

	process_messages(&win32_platform, &input);

	if (input.key_esc.is_down)
	{
	    win32_platform.is_running = false;
	}

	POINT center = { window_width / 2, window_height / 2 };
	ClientToScreen(window_handler, &center);
	SetCursorPos(center.x, center.y);
	

	MouseState *mouse = &input.mouse;
	mouse->delta_x = mouse->x_pos - (window_width / 2);
	mouse->delta_y = mouse->y_pos - mouse->last_y_pos;
	mouse->last_x_pos = mouse->x_pos;
	mouse->last_y_pos = mouse->y_pos;

	Update(&video, &win32_platform_layer, &memory, &input);

	present_framebuffer(&win32_platform, &video);

	QueryPerformanceCounter(&p_end);

	p_ellapsed_ns.QuadPart = p_end.QuadPart - p_start.QuadPart;
	p_ellapsed_ns.QuadPart *= 1000000;
	p_ellapsed_ns.QuadPart /= p_freq.QuadPart;

	if (p_ellapsed_ns.QuadPart < target_ns)
	{
	    int sleep_time = (int) (target_ns - p_ellapsed_ns.QuadPart);
	    sleep_time /= 1000;
	    Sleep((DWORD) sleep_time);
	}
    }

    ReleaseCapture();
    ReleaseDC(window_handler, device_context);
    return 0;
}

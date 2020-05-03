cd bin

set BASE=..\src
set FILES=%BASE%\win32_platform.c

REM set FILES=%BASE%\win32_platform.c %BASE%\main.c %BASE%\win32_opengl.c

cl /Zi /FC /Wall /wd4459 /wd4820  /wd4255 /wd4668 /wd4100 /wd4710 /wd4706 %FILES% User32.lib Gdi32.lib OpenGL32.lib /I..\deps\include  /link /LIBPATH:..\deps\lib   /out:clone3d.exe
cd ..

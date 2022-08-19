#pragma once

#define _WIN32_WINNT 0x600

#include "Utils/targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� ����
// Windows header file:
#include <windows.h>

// C runtime header files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//DirectX headers
#include "Utils/DxDefine.h"
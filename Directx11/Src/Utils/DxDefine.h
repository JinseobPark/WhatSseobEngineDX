#pragma once


/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dsound.lib") //Direct Sound
#pragma comment(lib, "dxguid.lib") //Direct Sound
#pragma comment(lib, "winmm.lib") //Direct Sound
#pragma comment(lib, "dinput8.lib") //Direct Input
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <mmsystem.h>				 //Direct Sound
#include <dsound.h>					 //Direct Sound
#include <dinput.h>						 //Direct Input
#include <d3dcompiler.h>
#include <directxmath.h>
#include "Component/Sprite/DDSTextureLoader.h"
using namespace DirectX;


///////////////////////////
//  warning C4316   //
///////////////////////////
#include "AlignedAllocationPolicy.h"
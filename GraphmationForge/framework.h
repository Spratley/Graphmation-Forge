#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define NODE_WIDTH 200
#define NODE_HEIGHT 150
#define NODE_SIZE Size nodeSize(NODE_WIDTH, NODE_HEIGHT)
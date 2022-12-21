#pragma once


#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace PackedVector;

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")


#include "SimpleMath.h"
using namespace SimpleMath;

#include <cassert>

#include <string>
using std::string;
using std::wstring;

#include <vector>
using std::vector;

#include <list>
using std::list;

#include <map>
using std::map;
using std::make_pair;

#include <deque>
#include <sstream>
#include <fstream>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <type_traits>
#include <bitset>
#include <random>
#include <chrono>
#include <array>

//engine specific
#include "define.h"
#include "struct.h"
#include "extern.h"
#include "singleton.h"
#include "func.h"

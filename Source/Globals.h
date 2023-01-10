#pragma once

// --- WARNING DISABLED ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#ifndef _HAS_EXCEPTIONS // Disable STL exceptions
#define _HAS_EXCEPTIONS 0
#endif
#define _STATIC_CPPLIB

// --- BUILD ---
#ifdef DAVOS_BUILD_DLL
#define DAVOS_API __declspec(dllexport)
#else
#define DAVOS_API __declspec(dllimport)
#endif

// --- NULL REDEFINITION ---
#ifdef NULL
#undef NULL
#endif
#define NULL 0
#define NULLRECT {0,0,0,0}

// --- LOG ---
/* CHARS AT END INDICATES THE TYPE OF LOG TO ENABLE FILTERS
* Characters must be written as string type %s
* i - internal (engine)
* g - geometry (rendering)
* d - debug    (game)
* e - error
* w - warning
* Ex: LOG("My log string: %s, my number: %d","My string", 23,'v')
*/
#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);
void log(const char file[], int line, const char* format, ...);

// --- ERROR HANDLING ---
#define DAVOS_ASSERT(x, ...) if(!x) { LOG(__VA_ARGS__); __debugbreak(); }

// --- HELPERS ---
#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n) // Keep a value between 0.0f and 1.0f
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))
#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

// --- MEMORY RELEASE ---
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// --- TYPEDEFS ---
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

// --- PRECALCULATED VALUES ---
static constexpr float DEGTORAD = 0.0174532925199432957f;
static constexpr float RADTODEG = 57.295779513082320876f;
static constexpr float PI = 3.14159265359f;
static constexpr float TWO_PI = 6.28318530717958647692f;
static constexpr float HALF_PI = 1.57079632679489661923f;
static constexpr float QUARTER_PI = 0.78539816339744830961f;
static constexpr float INV_PI = 0.31830988618379067154f;
static constexpr float INV_TWO_PI = 0.15915494309189533576f;

// --- CONFIGURATION ---
static constexpr char TITLE[8] = "Davos2D";
static constexpr char VERSION[5] = "v1.0";

static constexpr char ASSETS_FOLDER[8] = "Assets/";
static constexpr char SETTINGS_FOLDER[10] = "Settings/";
static constexpr char LIBRARY_FOLDER[9] = "Library/";
static constexpr char BUILDS_FOLDER[8] = "Builds/";
static constexpr char GAME_EXE[12] = "Davos2D.exe";

static constexpr char LIBRARY_SCENE_FOLDER[16] = "Library/Scenes/";
static constexpr char LIBRARY_PREFAB_FOLDER[17] = "Library/Prefabs/";
static constexpr char LIBRARY_TEXTURE_FOLDER[18] = "Library/Textures/";
static constexpr char LIBRARY_MATERIAL_FOLDER[19] = "Library/Materials/";
static constexpr char LIBRARY_ANIMATION_FOLDER[20] = "Library/Animations/";
static constexpr char LIBRARY_AUDIO_FOLDER[16] = "Library/Audios/";
static constexpr char LIBRARY_TILEMAP_FOLDER[18] = "Library/Tilemaps/";
static constexpr char LIBRARY_SCRIPT_FOLDER[17] = "Library/Scripts/";
static constexpr char LIBRARY_SHADER_FOLDER[17] = "Library/Shaders/";

static constexpr char EXTENSION_FOLDER[1] = "";
static constexpr char EXTENSION_SCENE[7] = ".scene";
static constexpr char EXTENSION_PREFAB[8] = ".prefab";
static constexpr char EXTENSION_TEXTURE[5] = ".tex";
static constexpr char EXTENSION_MATERIAL[10] = ".material";
static constexpr char EXTENSION_ANIMATION[6] = ".anim";
static constexpr char EXTENSION_TILEMAP[5] = ".map";
static constexpr char EXTENSION_AUDIO[7] = ".audio";
static constexpr char EXTENSION_SCRIPT[8] = ".script";
static constexpr char EXTENSION_SHADER[8] = ".shader";
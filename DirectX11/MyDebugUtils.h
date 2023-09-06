#ifndef MYDEBUGUTILS_H
#define MYDEBUGUTILS_H
#include "globals.h"
#include <string>
#include <d3d11.h> // Include the necessary Direct3D 11 headers.
std::string GetDebugObjectName(ID3D11DeviceChild* resource);
wchar_t* StringToWchar(const std::string& str);
wchar_t* CombineWideStrings(const wchar_t* baseWstr, const wchar_t* appendString);
ID3D11DeviceChild* FindShaderByValue(const ShaderMap map, UINT64 value);
ID3D11Resource* FindBufByValue(const ResourceMap map, UINT value);
std::string WcharToString(const wchar_t* wstr);
std::string MakeValidFilename(const std::string& filename);
#endif // MYDEBUGUTILS_H

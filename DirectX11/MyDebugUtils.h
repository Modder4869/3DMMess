#ifndef MYDEBUGUTILS_H
#define MYDEBUGUTILS_H

#include <string>
#include <d3d11.h> // Include the necessary Direct3D 11 headers.

std::string GetDebugObjectName(ID3D11Resource* resource);
wchar_t* StringToWchar(const std::string& str);
wchar_t* CombineWideStrings(const wchar_t* baseWstr, const wchar_t* appendString);
#endif // MYDEBUGUTILS_H

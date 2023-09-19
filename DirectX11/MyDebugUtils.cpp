#include <iostream>
#include <string>
#include <windows.h>
#include <d3d11.h>
#include <locale>
#include <codecvt>
#include "globals.h"
#include <algorithm>
#include <cctype>
#include "IniHandler.h"
std::string GetDebugObjectName(ID3D11DeviceChild* resource)
{
    UINT dataSize = 0;

    // First, get the size of the private data (if any) by passing nullptr as the third parameter.
    if (GetIniBool(L"Logging", L"debug_names", false, NULL) == 0) return "" ;
    try {
        HRESULT hr = resource->GetPrivateData(WKPDID_D3DDebugObjectName, &dataSize, nullptr);
    

        if (hr == S_OK)
        {
            // Allocate memory to store the private data.
            char* data = new char[dataSize];

            // Now, retrieve the private data into the allocated buffer.
            hr = resource->GetPrivateData(WKPDID_D3DDebugObjectName, &dataSize, data);

            if (hr == S_OK)
            {
                // Copy the binary data into a string
                std::string result(data, dataSize);

                // Release the memory.
                delete[] data;

                return result;
            }

            // If retrieval fails, release the memory and return an empty string.
            delete[] data;
        }
    }

    catch (const std::exception& ex)
    {
        // Handle the exception here, log it, or take appropriate action.
        // You can also rethrow it if needed.
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return ""; // Failed to get the private data or convert it to a string.
}
wchar_t* StringToWchar(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wideStr = converter.from_bytes(str);
    wchar_t* wideChars = new wchar_t[wideStr.size() + 1];
    wcscpy_s(wideChars, wideStr.size() + 1, wideStr.c_str());
    return wideChars;
}
std::string WcharToString(const wchar_t* wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}
wchar_t* CombineWideStrings(const wchar_t* baseWstr, const wchar_t* appendString) {
    // Convert the base and append wide-character strings to regular strings
    std::string baseStr = WcharToString(baseWstr);
    std::string appendStr = WcharToString(appendString);

    // Find the position of ".XXX" in the base string
    size_t pos = baseStr.find_last_of(L'.');

    if (pos != std::string::npos) {
        // Split the base string into two parts: before and after ".XXX"
        std::string part1 = baseStr.substr(0, pos);
        std::string part2 = baseStr.substr(pos);

        // Combine the parts with the append string in between
        std::string combinedStr = part1 + "-n=" + appendStr + part2;

        // Convert the combined regular string back to a wide-character string
        wchar_t* combinedWstr = StringToWchar(combinedStr);

        return combinedWstr;
    }
    else {
        // Handle the case where ".XXX" was not found in the base string
        // Return a copy of the base string with the append string appended
        std::string combinedStr = baseStr + "-n=" + appendStr;
        wchar_t* combinedWstr = StringToWchar(combinedStr);

        return combinedWstr;
    }
  
}
ID3D11Resource* FindBufByValue(const ResourceMap map, UINT value) {
    for (auto& pair : map) {
        if (pair.second.hash == value) {
            return pair.first;
        }
    }
    return nullptr; // Return nullptr if not found.
}
ID3D11DeviceChild* FindShaderByValue(const ShaderMap map, UINT64 value) {
    for (auto& pair : map) {
        if (pair.second == value) {
            return pair.first;
        }
    }
    return nullptr; // Return nullptr if not found.
}
std::string MakeValidFilename(const std::string& filename) {
    std::string validFilename = filename;

    // Replace invalid characters with underscores
    const std::string invalidChars = "\\/:*?\"<>|";
    for (char& c : validFilename) {
        if (std::find(invalidChars.begin(), invalidChars.end(), c) != invalidChars.end()) {
            c = '_';
        }
    }

    // Check for control characters and leading/trailing spaces
    validFilename.erase(
        std::remove_if(validFilename.begin(), validFilename.end(),
            [](char c) { return std::iscntrl(c) || std::isspace(c); }
        ),
        validFilename.end()
    );

    return validFilename;
}
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
#include "HackerContext.h"
std::map<UINT64, std::string> debugData;

// Function to retrieve the debug object name and hash
int64_t ConvertInt32toInt64(int32_t value) {
    return static_cast<int64_t>(value);  // Widening conversion to int64_t
}
bool debugNames() {
    return (GetIniBool(L"Logging", L"debug_names_overlay", false, NULL) == 1);
}
bool debugNamesUsages() {
    return (GetIniBool(L"Logging", L"debug_names_usages", false, NULL) == 1);
}
bool debugNamesFrame() {
    return (GetIniBool(L"Logging", L"debug_names_frame", false, NULL) == 1);
}
bool overlayLogs() {
    return (GetIniBool(L"Logging", L"overlay_logs", false, NULL) == 1);
}
std::pair<std::string, UINT64> GetDebugData(UINT64 hash)
{
    auto it = debugData.find(hash);
    if (it != debugData.end())
    {
        return { it->second, it->first }; // Return the debug name and the hash
    }
    else
    {
        return { "", 0 }; // Return empty string and 0 if not found
    }
}
void StoreDebugData(const std::string& debugName, UINT64 hash)
{
    if (hash != 0) {
        debugData[hash] = debugName;
    }
}

std::string GetDebugObjectName(ID3D11DeviceChild* resource)
{
    UINT dataSize = 0;
    UINT64 hash = 0;
    // First, get the size of the private data (if any) by passing nullptr as the third parameter.
    if (!debugNames()) return "";

    try {
        // Check if the data is already stored
        //auto debugData = GetDebugData(resource);
        //if (!debugData.first.empty())
        //{
        //    return debugData.first; // Debug name is already stored, return it
        //}

        // Cast ID3D11DeviceChild* to ID3D11Resource*
        ID3D11Resource* resourceAsResource = nullptr;
        resource->QueryInterface(__uuidof(ID3D11Resource), (void**)&resourceAsResource);

        auto shadersIt = G->mShaders.find(resource);
        if (shadersIt != G->mShaders.end()) {
            hash = shadersIt->second;
        }
        else {
            auto resourcesIt = G->mResources.find(resourceAsResource);
            if (resourcesIt != G->mResources.end()) {
                hash = ConvertInt32toInt64(resourcesIt->second.hash);
            }
        }
        std::pair<std::string, UINT64> debugData = GetDebugData(hash);

        if (!debugData.first.empty()) {
            return debugData.first;
        }
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
                std::string debugName(data, dataSize);

                // Store the debug name and hash
                StoreDebugData(debugName, hash);

                // Release the memory.
                delete[] data;

                return debugName;
            }

            // If retrieval fails, release the memory and return an empty string.
            delete[] data;
        }
    }
    catch (const std::exception& ex)
    {
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
ID3D11Resource* FindBufByValueTest(const ResourceMap map, UINT value)
{
    auto it = std::find_if(map.begin(), map.end(),
        [value](const auto& pair) { return pair.second.hash == value; });

    if (it != map.end()) {
        return it->first;
    }

    return nullptr; // Return nullptr if not found.
}
ID3D11DeviceChild* FindShaderByValueTest(const ShaderMap map, UINT64 value)
{
    auto it = std::find_if(map.begin(), map.end(),
        [value](const auto& pair) { return pair.second == value; });

    if (it != map.end()) {
        return it->first;
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



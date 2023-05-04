#include "RPC_Obj.h"
#include "DataObj.h"
#include <tchar.h>
#include <iostream>
#include <string>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <windows.h>
#include <Psapi.h>
#include "StringConversion.h"

std::string RPC_Obj::listPrcs() {
    std::string result = "";
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return "";

    cProcesses = cbNeeded / sizeof(DWORD);

    for (unsigned int i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

            if (hProcess)
            {
                WCHAR processName[MAX_PATH];
                DWORD len = MAX_PATH;
                if (QueryFullProcessImageName(hProcess, 0, processName, &len))
                {
                    std::wstring fullPath(processName);
                    std::wstring fileName = fullPath.substr(fullPath.find_last_of(L"\\") + 1);
                    result += StringConversion::ws2s(fileName);
                    result += "\n";
                }
                CloseHandle(hProcess);
            }
        }
    }

    // Return the list of installed applications
    return result;
}

// A funtion to deal with processes I dont know
HANDLE OpenProcessByName(const std::wstring& name)
{
    HANDLE hProcess = NULL;
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        std::wcout << L"Failed to enumerate processes" << std::endl;
        return NULL;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    bool foundProcess = false;
    for (unsigned int i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            // Use a different access level when calling OpenProcess
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, aProcesses[i]);

            if (hProcess)
            {
                WCHAR processName[MAX_PATH];
                DWORD len = MAX_PATH;
                if (QueryFullProcessImageName(hProcess, 0, processName, &len))
                {
                    std::wstring fullPath(processName);
                    std::wstring fileName = fullPath.substr(fullPath.find_last_of(L"\\") + 1);
                    std::wcout << L"Found process: " << fileName << std::endl;
                    if (fileName == name + L".exe")
                    {
                        foundProcess = true;
                        CloseHandle(hProcess);
                        hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, aProcesses[i]);
                        if (hProcess == NULL)
                        {
                            std::wcout << L"Failed to open process with terminate access for " << name << std::endl;
                        }
                        return hProcess;
                    }
                }
                CloseHandle(hProcess);
            }
        }
    }

    if (!foundProcess)
    {
        std::wcout << L"Failed to find process for " << name << std::endl;
    }

    return NULL;
}

std::string RPC_Obj::runPrc(std::string Name)
{
    std::string res = "";
    WCHAR szPath[MAX_PATH];
    std::wstring name = StringConversion::s2ws(Name);
    if (SearchPath(NULL, name.c_str(), L".exe", MAX_PATH, szPath, NULL) == 0)
    {
        res = "Failed to find executable file for " + Name ;
        res += "\n";
        return res;
    }

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    if (!CreateProcessW(szPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        res = "Failed to create process for " + Name;
        res += "\n";
        return res;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    res = "Process run successfully.\n";
    return res;
}


std::string RPC_Obj::killPrc(std::string Name)
{
    std::string result = "";
    std::wstring name = StringConversion::s2ws(Name);
    HANDLE hProcess = OpenProcessByName(name);
    if (hProcess == NULL)
    {
        result = "Failed to open process for " + Name;
        result += "\n";
        return result;
    }

    DWORD dwExitCode;
    GetExitCodeProcess(hProcess, &dwExitCode);
    if (!TerminateProcess(hProcess, dwExitCode))
    {
        result = "Failed to terminate process for " + Name;
        result += "\n";
        return result;

    }

    CloseHandle(hProcess);

    return result;
}

std::string RPC_Obj::toString()
{
    std::stringstream ss;

    // Convert the data to a string
    for (char c : _data)
    {
        ss << c;
    }

    return ss.str();
}

std::string RPC_Obj::toFile(std::string filename)
{
    std::string result = "";

    // Open the file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        return "Failed to open file.";
    }

    // Write the data to the file
    file.write(_data.data(), _data.size());

    // Close the file
    file.close();

    result = "Data written to file successfully.";
    return result;
}
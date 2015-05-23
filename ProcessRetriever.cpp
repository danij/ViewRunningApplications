#include "stdafx.h"
#include "ProcessRetriever.h"

using namespace std;

ProcessRetriever::ProcessRetriever()
{
}

ProcessRetriever::~ProcessRetriever()
{
}

vector<ProcessInstance> ProcessRetriever::GetInstances()
{
    vector<ProcessInstance> result;

    DWORD pids[65536];
    DWORD usedPids;
    if ( ! EnumProcesses(pids, sizeof(pids), &usedPids))
    {
        return result;
    }
    //EnumProcesses returns the memory used by the returned pids
    usedPids /= sizeof(pids[0]);

    for (size_t i = 0; i < usedPids; ++i)
    {
        if (pids[i] > 0)
        {
            result.push_back(ReadInstanceInfo(pids[i]));
        }
    }

    sort(begin(result), end(result), ProcessInstance::CompareByPID);

    return result;
}

ProcessInstance ProcessRetriever::ReadInstanceInfo(DWORD pid)
{
    ProcessInstance result;
    result.Pid = pid;
    result.PrivateBytes = 0;

    //PrivilegeEscalator escalator;

    auto handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (NULL == handle)
    {
        return result;
    }

    result.Process = ReadProcessInfo(handle);
    ReadProcessMemoryInformation(handle, result);

    return result;
}

shared_ptr<Process> ProcessRetriever::ReadProcessInfo(HANDLE handle)
{
    auto result = make_shared<Process>();

    HMODULE modules[1024];
    DWORD usedModules;

    TCHAR currentPath[1024];
    DWORD currentPathSize = sizeof(currentPath) / sizeof(TCHAR);

    if ( ! QueryFullProcessImageName(handle, 0, currentPath, &currentPathSize))
    {
        return shared_ptr<Process>();
    }

    result->Name = (_tcsrchr(currentPath, '\\') + 1);
    result->ExecutablePath = currentPath;

    if ( ! EnumProcessModulesEx(handle, modules, sizeof(modules), &usedModules, LIST_MODULES_ALL))
    {
        return shared_ptr<Process>();
    }
    usedModules /= sizeof(modules[0]);

    for (size_t i = 1; i < usedModules; i++)
    {
        if (GetModuleFileName(modules[i], currentPath, sizeof(currentPath) / sizeof(TCHAR)))
        {
            result->Modules.push_back(currentPath);
        }        
    }

    ReadExecutableInformation(*result);

    return result;
}

void ProcessRetriever::ReadExecutableInformation(Process& process)
{
    DWORD length;
    if ((length = GetFileVersionInfoSize(process.ExecutablePath, &length)) == 0)
    {
        return;
    }

    unique_ptr<char> buffer(new char[length]);

    if ( ! GetFileVersionInfo(process.ExecutablePath, 0, length, buffer.get()))
    {
        return;
    }

    VS_FIXEDFILEINFO* info;
    UINT infoLength;
    VerQueryValue(buffer.get(), _T("\\"), (LPVOID*)&info, &infoLength);

    process.Version = GetVersionString(info);
    process.Company = GetCompanyName(buffer.get());
}

CString ProcessRetriever::GetVersionString(VS_FIXEDFILEINFO* fileInfo)
{
    CString result;

    if (0xfeef04bd != fileInfo->dwSignature)
    {
        return result;
    }

    result.Format(_T("%d.%d"),
        (fileInfo->dwProductVersionMS >> 16) & 0xFFFF,
        (fileInfo->dwProductVersionMS >> 0) & 0xFFFF);

    return result;
}

struct LANGANDCODEPAGE {
    WORD wLanguage;
    WORD wCodePage;
};

CString ProcessRetriever::GetCompanyName(LPCVOID buffer)
{
    CString result;
    LANGANDCODEPAGE *langAndCodePage;
    UINT infoLength;
    VerQueryValue(buffer, _T("\\VarFileInfo\\Translation"), (LPVOID*)&langAndCodePage, &infoLength);

    if (infoLength < 1)
    {
        return result;
    }

    CString request;
    request.Format(_T("\\StringFileInfo\\%04x%04x\\CompanyName"), 
        langAndCodePage[0].wLanguage, langAndCodePage[0].wCodePage);

    TCHAR* companyName;
    VerQueryValue(buffer, request, (LPVOID*)&companyName, &infoLength);

    if (infoLength)
    {
        result += companyName;
    }

    return result;
}

void ProcessRetriever::ReadProcessMemoryInformation(HANDLE handle, ProcessInstance& process)
{
    PROCESS_MEMORY_COUNTERS_EX info;
    if (!GetProcessMemoryInfo(handle, (PPROCESS_MEMORY_COUNTERS)&info, sizeof(info)))
    {
        return;
    }
    if (info.cb < sizeof(PROCESS_MEMORY_COUNTERS_EX))
    {
        info.PrivateUsage = 0;
    }

    process.WorkingSet = info.WorkingSetSize;
    process.PrivateBytes = info.PrivateUsage;
}

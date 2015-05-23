#pragma once

#include "ProcessInfo.h"

class ProcessRetriever
{
public:
    ProcessRetriever();
    virtual ~ProcessRetriever();

    std::vector<ProcessInstance> GetInstances();
protected:
    ProcessInstance ReadInstanceInfo(DWORD pid);
    std::shared_ptr<Process> ReadProcessInfo(HANDLE handle);
    void ReadExecutableInformation(Process& process);
    CString GetVersionString(VS_FIXEDFILEINFO* fileInfo);
    CString GetCompanyName(LPCVOID buffer);
    void ReadProcessMemoryInformation(HANDLE handle, ProcessInstance& process);

};


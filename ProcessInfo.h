#include "stdafx.h"

struct Process
{
    CString Name;
    CString ExecutablePath;
    std::vector<CString> Modules;
    CString Version;
    CString Company;
};

struct ProcessInstance
{
    std::shared_ptr<Process> Process;
    DWORD Pid;
    DWORD64 WorkingSet;
    DWORD64 PrivateBytes;

    ProcessInstance();

    static bool CompareByPID(const ProcessInstance& first, const ProcessInstance& second);
    static bool CompareByName(const ProcessInstance& first, const ProcessInstance& second);
    static bool CompareByWorkingSetDesc(const ProcessInstance& first, const ProcessInstance& second);
    static bool CompareByPrivateBytesDesc(const ProcessInstance& first, const ProcessInstance& second);
};

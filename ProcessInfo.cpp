#include "stdafx.h"
#include "ProcessInfo.h"

ProcessInstance::ProcessInstance() : Pid(0), WorkingSet(0), PrivateBytes(0)
{

}

bool ProcessInstance::CompareByPID(const ProcessInstance& first, const ProcessInstance& second)
{
    return first.Pid < second.Pid;
}

bool ProcessInstance::CompareByName(const ProcessInstance& first, const ProcessInstance& second)
{
    if ( ! first.Process && ! second.Process)
    {
        return CompareByPID(first, second);
    }
    if ( ! first.Process && second.Process)
    {
        return false;
    }
    if (first.Process && ! second.Process)
    {
        return true;
    }
    return first.Process->Name.CompareNoCase(second.Process->Name) < 0;
}

bool ProcessInstance::CompareByWorkingSetDesc(const ProcessInstance& first, const ProcessInstance& second)
{
    return first.WorkingSet > second.WorkingSet;
}

bool ProcessInstance::CompareByPrivateBytesDesc(const ProcessInstance& first, const ProcessInstance& second)
{
    return first.PrivateBytes > second.PrivateBytes;
}


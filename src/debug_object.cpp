#include <Windows.h>
extern "C" {
	#include <dbgktypes.h>
	#include <dbgkfuncs.h>
	#include <obfuncs.h>
}
#include "windows_error.hpp"
#include "debug_object.hpp"

using namespace winc;

namespace {

HANDLE create_debug_object(bool kill_on_close)
{
	HANDLE result;
	NTSTATUS status = NtCreateDebugObject(&result,
		DEBUG_OBJECT_ALL_ACCESS, NULL, kill_on_close ? TRUE : FALSE);
	if (!NT_SUCCESS(status))
		throw winnt_error(status);
	return result;
}

}

debug_object::debug_object(bool kill_on_close)
	: _handle(create_debug_object(kill_on_close))
{}

debug_object::~debug_object()
{
	NtClose(_handle);
}

void debug_object::attach(HANDLE process_handle)
{
	NTSTATUS status = NtDebugActiveProcess(process_handle, _handle);
	if (!NT_SUCCESS(status))
		throw winnt_error(status);
}

void debug_object::detach(HANDLE process_handle)
{
	NTSTATUS status = NtRemoveProcessDebug(process_handle, _handle);
	if (!NT_SUCCESS(status))
		throw winnt_error(status);
}

bool debug_object::wait(PDBGUI_WAIT_STATE_CHANGE state_change)
{
	NTSTATUS status = NtWaitForDebugEvent(_handle, FALSE, NULL, state_change);
	if (!NT_SUCCESS(status)) {
		if (status == STATUS_DEBUGGER_INACTIVE)
			return false;
		else
			throw winnt_error(status);
	}
	return true;
}

void debug_object::debug_continue(PCLIENT_ID client_id, NTSTATUS continue_status)
{
	NTSTATUS status = NtDebugContinue(_handle, client_id, continue_status);
	if (!NT_SUCCESS(status))
		throw winnt_error(status);
}
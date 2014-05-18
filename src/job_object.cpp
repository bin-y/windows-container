#include <cstdint>
#include <windows.h>
#include "job_object.hpp"
#include "process.hpp"
#include "windows_error.hpp"

using namespace std;
using namespace winc;

namespace {

HANDLE create_job_object()
{
	HANDLE result = CreateJobObject(NULL, NULL);
	if (!result) {
		throw windows_error(GetLastError());
	}
	return result;
}

}

job_object::job_object()
	: _handle(create_job_object())
{}

job_object::~job_object()
{
	CloseHandle(_handle);
}

void job_object::assign(HANDLE process_handle)
{
	BOOL result = AssignProcessToJobObject(_handle, process_handle);
	if (!result) {
		throw windows_error(GetLastError());
	}
}

void job_object::terminate(int32_t exit_code)
{
	BOOL result = TerminateJobObject(_handle, static_cast<UINT>(exit_code));
	if (!result) {
		throw windows_error(GetLastError());
	}
}

HANDLE job_object::handle()
{
	return _handle;
}

job_object::limits_info job_object::limits()
{
	return limits_info(*this);
}

job_object::ui_restrictions_info job_object::ui_restrictions()
{
	return ui_restrictions_info(*this);
}

job_object::limits_info::limits_info(job_object &job)
	: job_(job)
{
	update();
}

void job_object::limits_info::update()
{
	BOOL result = QueryInformationJobObject(
		job_.handle(),
		JobObjectExtendedLimitInformation,
		static_cast<JOBOBJECT_EXTENDED_LIMIT_INFORMATION *>(this),
		sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION),
		NULL);
	if (!result) {
		throw windows_error(GetLastError());
	}
}

void job_object::limits_info::commit()
{
	BOOL result = SetInformationJobObject(
		job_.handle(),
		JobObjectExtendedLimitInformation,
		static_cast<JOBOBJECT_EXTENDED_LIMIT_INFORMATION *>(this),
		sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));
	if (!result) {
		throw windows_error(GetLastError());
	}
}

job_object::ui_restrictions_info::ui_restrictions_info(job_object &job)
	: job_(job)
{
	update();
}

void job_object::ui_restrictions_info::update()
{
	BOOL result = QueryInformationJobObject(
		job_.handle(),
		JobObjectBasicUIRestrictions,
		static_cast<JOBOBJECT_BASIC_UI_RESTRICTIONS *>(this),
		sizeof(JOBOBJECT_BASIC_UI_RESTRICTIONS),
		NULL);
	if (!result) {
		throw windows_error(GetLastError());
	}
}

void job_object::ui_restrictions_info::commit()
{
	BOOL result = SetInformationJobObject(
		job_.handle(),
		JobObjectBasicUIRestrictions,
		static_cast<JOBOBJECT_BASIC_UI_RESTRICTIONS *>(this),
		sizeof(JOBOBJECT_BASIC_UI_RESTRICTIONS));
	if (!result) {
		throw windows_error(GetLastError());
	}
}
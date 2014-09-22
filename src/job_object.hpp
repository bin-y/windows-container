#ifndef JOB_OBJECT_HPP
#define JOB_OBJECT_HPP

#include <cstdint>
#include <Windows.h>
#include "safe_handle.hpp"

namespace winc {

class job_object : protected safe_handle {
public:
	class limits_info : public ::JOBOBJECT_EXTENDED_LIMIT_INFORMATION {
	public:
		limits_info(job_object &job);
		void update();
		void commit();
	private:
		job_object &job_;
	};

	class ui_restrictions_info : public ::JOBOBJECT_BASIC_UI_RESTRICTIONS {
	public:
		ui_restrictions_info(job_object &job);
		void update();
		void commit();
	private:
		job_object &job_;
	};

public:
	job_object();
	void assign(HANDLE process_handle);
	void terminate(std::int32_t exit_code);
	limits_info limits();
	ui_restrictions_info ui_restrictions();
};

}

#endif
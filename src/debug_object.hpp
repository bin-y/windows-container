#ifndef DEBUG_OBJECT_HPP
#define DEBUG_OBJECT_HPP

#include <Windows.h>
extern "C" {
	#include <dbgktypes.h>
}
#include "non_copyable.hpp"

namespace winc {

class debug_object : non_copyable {
public:
	explicit debug_object(bool kill_on_close = true);
	~debug_object();
	void attach(HANDLE process_handle);
	bool wait(PDBGUI_WAIT_STATE_CHANGE state_change);
	void debug_continue(PCLIENT_ID client_id, NTSTATUS continue_status);
private:
	HANDLE _handle;
};


}

#endif

#include "logicprocessing.h"
#include "term.h"
#include "ncs.h"
#include "ncs_ring.h"
#include "missed_call.h"
#include "dialed_call.h"
#include "received_call.h"
#include "system_info.h"
#include "ncs_queue.h"
#include "guibroadcast.h"


void logicDamodule(void)
{
	TYPE_MSG_STRUCT TmpMsg;
	struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;

	if(OutQueue(term->queue, &TmpMsg))
	{
		if(TmpMsg.MsgCmd == ENUM_ENTER_REV_BROADCAST)
		{
			guibroadcast_newscreen(system_info.main_hwnd, get_system_task());
			bc->ncs_wav_broadcast_task_start(bc, g_SysStatue.mutil_ip);
		}
	}
}


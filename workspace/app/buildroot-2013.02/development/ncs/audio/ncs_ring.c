#include <stdio.h>
#include <stdlib.h>

#include "ncs_ring.h"
#include "ncs.h"
#include "term.h"
#include "NAS8532.h"

void ncs_ring_stop(void)
{
	struct audio_manager *audio_manager = term->audio_manager;
	
	audio_manager->ring_stop(audio_manager);
	audio_manager->bc_ring_stop(audio_manager);
	audio_manager->file_play_stop(audio_manager);
}

void ncs_ring_start(ring_t t, uint16_t target_id)
{
	char file_path[128] = {0};
	int interval = -1;
	struct audio_manager *audio_manager = term->audio_manager;
	char *path = NULL;

	ncs_ring_stop();

	/*audio_manager->set_ring_volume(audio_manager, -1);*/

	switch (t)
	{
		case OUTGOING:
			sprintf(file_path, "%s%s", RING_FILE_PATH, "out_calling.wav");
			if (access(file_path, F_OK) != 0)
				strcpy(file_path, REMOTE_RING);
			interval = 1000;
			break;
		case INCOMING:
			sprintf(file_path, "%s%s", RING_FILE_PATH, "in_calling.wav");
			if (access(file_path, F_OK) != 0)
				strcpy(file_path, LOCAL_RING);
			interval = 1000;
			break;
		case TARGET_BUSY:
			sprintf(file_path, "%s%s", RING_FILE_PATH, "targetbusy.wav");
			break;
		case TARGET_OFFLINE:
			sprintf(file_path, "%s%s", RING_FILE_PATH, "targetoffline.wav");
			break;
		case TARGET_ONHOOK:
			sprintf(file_path, "%s%s", RING_FILE_PATH, "targetonhook.wav");
			break;
		default:
			break;
	}
	
	if (interval == -1)
		audio_manager->file_play_start(audio_manager, file_path);
	else
		audio_manager->ring_start(audio_manager, file_path, interval);
}

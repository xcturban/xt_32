
#include "obj_vol_dev_src_select.h"

int gpio_plugin_array[3];

TYPE_VOL_DEV_SRC_CTRL obj_vol_dev_src_array[SIZE_VOL_DEV_SRC];




void obj_vol_dev_src_init(void)
{
	obj_vol_dev_src_array[0].dev_src = VOICE_IN_HTG;
	obj_vol_dev_src_array[0].flag = TRUE;

	obj_vol_dev_src_array[1].dev_src = VOICE_IN_LINE_IN;
	obj_vol_dev_src_array[1].flag = FALSE;

	obj_vol_dev_src_array[2].dev_src = VOICE_IN_HEAD_IN;
	obj_vol_dev_src_array[2].flag = FALSE;
}


void obj_vol_dev_src_set_line_in(bool_t flag)
{
	obj_vol_dev_src_array[1].flag = flag;
}

bool_t obj_vol_dev_src_get_line_in(void)
{
	return obj_vol_dev_src_array[1].flag;
}

void obj_vol_dev_src_set_head_in(bool_t flag)
{
	obj_vol_dev_src_array[2].flag = flag;
}

bool_t obj_vol_dev_src_get_head_in(void)
{
	return obj_vol_dev_src_array[2].flag;
}

bool_t is_obj_vol_dev_src_plugin_head_in(void)
{

	if(obj_vol_dev_src_array[2].flag)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool_t is_obj_vol_dev_src_plugin_line_in(void)
{

	if(obj_vol_dev_src_array[1].flag)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void Turn_obj_vol_dev_src(void)
{
    struct call_manager *call_manager = term->call_manager;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;
    struct audio_manager *audio_manager = term->audio_manager;
	//插上耳机ｍｉｃ，则打开耳机ｍｉｃ的开关
	if(is_obj_vol_dev_src_plugin_head_in())
	{
		audio_manager->set_rt3261regEx(audio_manager, 0x3e, 4, 0);
		audio_manager->set_rt3261regEx(audio_manager, 0x3e, 2, 1);
		audio_manager->set_rt3261reg(audio_manager, 0xc2, 0x0006);
		/*audio_manager->mic_set(audio_manager, FALSE);*/
	}
	//拔掉耳机ｍｉｃ，则关闭耳机ｍｉｃ的开关, 否则敲击机壳会有声音传到远端
	else
	{
		audio_manager->set_rt3261regEx(audio_manager, 0x3e, 4, 1);
		if(is_obj_vol_dev_src_plugin_line_in())
		{
			audio_manager->set_rt3261regEx(audio_manager, 0x3e, 2, 0);
			audio_manager->set_rt3261reg(audio_manager, 0xc2, 0x0006);
		}
		else
		{
			audio_manager->set_rt3261regEx(audio_manager, 0x3e, 2, 1);
			if(call_manager->calls_total_get(call_manager) || bc->bc_outgoing(bc))
			{
				/*audio_manager->set_rt3261reg(audio_manager, 0xc2, 0x0030);*/
				audio_manager->mic_set(audio_manager, TRUE);
			}
			else
			{
				/*audio_manager->set_rt3261reg(audio_manager, 0xc2, 0x0006);*/
				audio_manager->mic_set(audio_manager, FALSE);
			}

		}
	}
}

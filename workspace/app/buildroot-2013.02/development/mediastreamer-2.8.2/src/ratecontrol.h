//------------------------------------------
//   frame layer Rate control header file
//				Richard Wang		
//				03/16/2006
//------------------------------------------
#ifndef _RATECONTROL_H_
#define _RATECONTROL_H_


typedef long long int64;
#define rc_MAX_QUANT  52
#define rc_MIN_QUANT  0



typedef struct
{
	int rtn_quant;
	//long long frames;
	int64 frames;
	//long long total_size;
	double total_size;
	double framerate;
	int target_rate;
	short max_quant;
	short min_quant;
	//long long last_change;
	//long long quant_sum;
	int64 last_change;
	int64 quant_sum;
//	double quant_error[32];
	double quant_error[rc_MAX_QUANT];
	double avg_framesize;
	double target_framesize;
	double sequence_quality;
	int averaging_period;
	int reaction_delay_factor;
	int buffer;
	unsigned int u32IPInterval;
    unsigned int IPIntervalCnt;
	int pre_rtn_quant;
}H264RateControl;

//#define RC_DELAY_FACTOR         16
#define RC_DELAY_FACTOR         4
#define RC_AVERAGING_PERIOD     100
#define RC_BUFFER_SIZE_QUALITY  100 //quality sensitive, recommended
#define RC_BUFFER_SIZE_BITRATE  10  //bit rate sensitive, not recommended

#define quality_const (double)((double)2/(double)rc_MAX_QUANT)

void H264RateControlInit(H264RateControl *rate_control,
				unsigned int target_rate,
				unsigned int reaction_delay_factor,
				unsigned int averaging_period,
				unsigned int buffer,
				float framerate,
				int max_quant,
				int min_quant,
				unsigned int initq,
				unsigned int u32IPInterval);
				
void H264RateControlUpdate(H264RateControl *rate_control,
				  short quant,
				  int frame_size,
				  int keyframe);


void H264RateControlInit(H264RateControl * rate_control,
				unsigned int target_rate,
				unsigned int reaction_delay_factor,
				unsigned int averaging_period,
				unsigned int buffer,
				float framerate,
				int max_quant,
				int min_quant,
				unsigned int initq,
				unsigned int u32IPInterval)
{
	int i;
  /*
	if(buffer <= 50){
		rate_control->frames = 0;
		rate_control->total_size = 0;
	}else{
		rate_control->frames = framerate * 1;
		rate_control->total_size = target_rate * 1 / 2;
	}
  */
// 	printf("=======>%d - %d - %d - %d -%f  \n",target_rate,reaction_delay_factor,averaging_period,buffer,framerate);
//	printf("=======>%d - %d - %d - %d  \n",max_quant,min_quant,initq,u32IPInterval);
	rate_control->frames = 0;
	rate_control->total_size = 0;

	//rate_control->framerate = (double) framerate / 1000.0;
	rate_control->framerate = (double) framerate;
	rate_control->u32IPInterval= u32IPInterval;
	rate_control->IPIntervalCnt= 0;
	rate_control->target_rate = (int) target_rate;
//	rate_control->rtn_quant = get_initial_quant(target_rate);
	rate_control->rtn_quant = initq;
	rate_control->pre_rtn_quant = initq;
	rate_control->max_quant = (short) max_quant;
	rate_control->min_quant = (short) min_quant;
	for (i = 0; i < rc_MAX_QUANT; ++i) {
		rate_control->quant_error[i] = 0.0;
	}
	rate_control->target_framesize =
		(double) target_rate / 8.0 / rate_control->framerate;
	rate_control->sequence_quality = 2.0 / (double) rate_control->rtn_quant;
	rate_control->avg_framesize = rate_control->target_framesize;
	rate_control->reaction_delay_factor = (int) reaction_delay_factor;
	rate_control->averaging_period = (int) averaging_period;
	rate_control->buffer = (int) buffer;
}


void H264RateControlUpdate(H264RateControl *rate_control,
				  short quant,
				  int frame_size,
				  int keyframe)
{
	//long long deviation;
//	printf("=======>Enter %s-%d\n",__func__,__LINE__);
	int64 deviation;
	double overflow, averaging_period, reaction_delay_factor;
	double quality_scale, base_quality, target_quality;
	int rtn_quant;

	rate_control->rtn_quant = rate_control->pre_rtn_quant;
	if ((quant == rate_control->min_quant) && (frame_size < rate_control->target_framesize))
		goto skip_integrate_err;
	else if ((quant == rate_control->max_quant) && (frame_size > rate_control->target_framesize))
		goto skip_integrate_err;

	rate_control->frames++;
	rate_control->total_size += frame_size;

skip_integrate_err:
	deviation =
		/*(long long)*/(int64) ((double) rate_control->total_size -
				   ((double) ((double) rate_control->target_rate / 8.0 /
					 (double) rate_control->framerate) * (double) rate_control->frames));

	if (rate_control->rtn_quant >= 2) {
		averaging_period = (double) rate_control->averaging_period;
		rate_control->sequence_quality -=
			rate_control->sequence_quality / averaging_period;
		rate_control->sequence_quality +=
			2.0 / (double) rate_control->rtn_quant / averaging_period;
		if (rate_control->sequence_quality < 0.1)
			rate_control->sequence_quality = 0.1;
		if (!keyframe) {
			reaction_delay_factor =
				(double) rate_control->reaction_delay_factor;
			rate_control->avg_framesize -=
				rate_control->avg_framesize / reaction_delay_factor;
			rate_control->avg_framesize += frame_size / reaction_delay_factor;
		}
	}

	quality_scale =
		rate_control->target_framesize / rate_control->avg_framesize *
		rate_control->target_framesize / rate_control->avg_framesize;

	base_quality = rate_control->sequence_quality;
	if (quality_scale >= 1.0) {
		base_quality = 1.0 - (1.0 - base_quality) / quality_scale;
	} else {
		//base_quality = 0.06452 + (base_quality - 0.06452) * quality_scale;
		base_quality = quality_const + (base_quality - quality_const) * quality_scale;
	}
	overflow = -((double) deviation / (double) rate_control->buffer);
  /*
	target_quality =
		base_quality + (base_quality -
						0.06452) * overflow / rate_control->target_framesize;*/
	target_quality =
		base_quality + (base_quality - quality_const) * overflow / rate_control->target_framesize;
	/*
	if (target_quality > 2.0)
		target_quality = 2.0;
	else if (target_quality < 0.06452)
		target_quality = 0.06452;*/
	if (target_quality > 2.0)
		target_quality = 2.0;
	else if (target_quality < quality_const)
		target_quality = quality_const;

	rtn_quant = (int) (2.0 / target_quality);
	if (rtn_quant < rc_MAX_QUANT) {
		rate_control->quant_error[rtn_quant] +=
			2.0 / target_quality - rtn_quant;
		if (rate_control->quant_error[rtn_quant] >= 1.0) {
			rate_control->quant_error[rtn_quant] -= 1.0;
			rtn_quant++;
		}
	}
	if(rate_control->framerate<=10) {
		if (rtn_quant > rate_control->rtn_quant + 3)
			rtn_quant = rate_control->rtn_quant + 3;
		else if (rtn_quant < rate_control->rtn_quant - 3)
			rtn_quant = rate_control->rtn_quant - 3;
	}else{
		if (rtn_quant > rate_control->rtn_quant + 1)
			rtn_quant = rate_control->rtn_quant + 1;
		else if (rtn_quant < rate_control->rtn_quant - 1)
			rtn_quant = rate_control->rtn_quant - 1;
	}
		 
	if (rtn_quant > rate_control->max_quant)
		rtn_quant = rate_control->max_quant;
	else if (rtn_quant < rate_control->min_quant)
		rtn_quant = rate_control->min_quant;
	rate_control->pre_rtn_quant = rtn_quant;
	if(rate_control->framerate <= 10 && rate_control->target_rate <= 128000) {
		if(++rate_control->IPIntervalCnt % rate_control->u32IPInterval == 0) {
			rtn_quant -= 5;
			if(rtn_quant <= 25){
				rtn_quant = 25;
			}
			//rate_control->IPIntervalCnt =0;
		}
	}
	if(keyframe == 1)
		rate_control->IPIntervalCnt=1; 
	rate_control->rtn_quant = rtn_quant;
}




#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

void main(){
	printf("Content-type: text/html \n\n");
	cgi_to_browser_by_xml("../c2h/AudioCfg.c2h", "../xml/audio_cfg.xml");
}

#ifndef __COMMON_LIB_H__
#define __COMMON_LIB_H__

#include "checksum.h"
#include "net.h"
#include "list.h"
#include "timer.h"
#include "encoding_convert.h"
#include "sponlog.h"

#undef bool_t
typedef unsigned char bool_t;

#undef FALSE
#undef TRUE
#define FALSE 0
#define TRUE 1

#ifdef DEBUG
#define ENTER_DEBUG(fmt,arg...) do{printf("==>Enter [%s:%d] "fmt,__FUNCTION__,__LINE__,##arg);}while(0)
#else
#define ENTER_DEBUG(fmt,arg...)
#endif


#endif

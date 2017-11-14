
#include  "phonebook.h"

TYPE_PHONE_BOOK g_phone_book_ctrl = {
    10,
	0,
	0,
	PHONEBOOK_UNVALID_ID,
	NULL,NULL
};

extern const TYPE_GUI_AREA_HANDLE gui_phone_book_item[];
extern const TYPE_GUI_AREA_HANDLE gui_phone_book_retang[];
void Init(TYPE_CTRL_INPUT_DIGTAL* pCtrl, int type);
static void ButtonProcOther(HWND hWnd, int message, int wParam, int lParam);
void InputDigtal_NewScreen(HWND hWnd);
static void PaintButtonEx(PRECT pRc, void* pData);
static void PaintPhonebookInfo(PRECT pRc, void* pData);
static void PhonebookInfoProc(HWND hWnd, int message, int wParam, int lParam);

extern TYPE_CTRL_INPUT_DIGTAL g_input_ctrl_type;
#define PHONE_BTN_W	157
#define PHONE_BTN_H	58
#define PHONE_BTN_L	50
const TYPE_GUI_AREA_HANDLE gui_phone_book_item[] =
{
    {0  , {PHONE_BTN_L , 75      , PHONE_BTN_L+PHONE_BTN_W , 75+PHONE_BTN_H}   , "phone_g_1_down.png" , "phone_g_1_up.png" , PaintButton , ButtonProc} , //0
    {1  , {PHONE_BTN_L , 75+PHONE_BTN_H   , PHONE_BTN_L+PHONE_BTN_W , 75+PHONE_BTN_H*2} , "phone_g_2_up.png" , "phone_g_2_down.png" , PaintButton , ButtonProc} , //1
    {2  , {PHONE_BTN_L , 75+PHONE_BTN_H*2 , PHONE_BTN_L+PHONE_BTN_W , 75+PHONE_BTN_H*3} , "phone_g_3_up.png" , "phone_g_3_down.png" , PaintButton , ButtonProc} , //2
    {3  , {PHONE_BTN_L , 75+PHONE_BTN_H*3 , PHONE_BTN_L+PHONE_BTN_W , 75+PHONE_BTN_H*3+61} , "phone_g_4_up.png" , "phone_g_4_down.png" , PaintButton , ButtonProc} , //3
    //{4  , {125 , 75+40*4 , 125+120 , 75+40*5} , "phone_bar_up.png" , "phone_bar_down.png" , PaintButton , ButtonProc} , //4
    //{5  , {125 , 75+40*5 , 125+120 , 75+40*6} , "phone_bar_up.png" , "phone_bar_down.png" , PaintButton , ButtonProc} , //5
    //{6  , {125 , 75+40*6 , 125+120 , 75+40*7} , "phone_bar_up.png" , "phone_bar_down.png" , PaintButton , ButtonProc} , //6
    //{7  , {125 , 75+40*7 , 125+120 , 75+40*8} , "phone_bar_up.png" , "phone_bar_down.png" , PaintButton , ButtonProc} , //7
    //{8  , {125 , 75+40*8 , 125+120 , 75+40*9} , "phone_bar_up.png" , "phone_bar_down.png" , PaintButton , ButtonProc} , //8
    {-1 , {0   , 0       , 0       , 0}       , NULL             , NULL             , NULL        , NULL}       , 
};

#define PHONE_RECT_W	542
#define PHONE_RECT_H	342

#define PHONEBOOK_INFO_OFFSET		320/5

const TYPE_GUI_AREA_HANDLE gui_phone_book_retang[] = 
{
    {0  , {PHONE_BTN_L+PHONE_BTN_W-10 , 75 , PHONE_BTN_L+PHONE_BTN_W+PHONE_RECT_W-10 , 75+PHONE_RECT_H} , "phone_book_retang.png" , NULL , PaintPhonebookInfo , PhonebookInfoProc} , //框
    {-1 , {0       , 0  , 0            , 0}      , NULL                    , NULL , NULL        , NULL}       , 
};

const TYPE_GUI_AREA_HANDLE gui_phone_book_other[] = 
{
    {0 , {BTN_BACK_L, BTN_BACK_T ,	BTN_BACK_R,BTN_BACK_B 	} , BTN_BACK_UP         , BTN_BACK_DOWN, PaintButtonEx , ButtonProcOther} , // 返回
    {-1 , {0       , 0  , 0            , 0}      , NULL                    , NULL , NULL        , NULL}       , 
};

#define MAX_EXTER_PHONEBOOK 16
int phonebook_list_destory(struct list_head * phonebook_head)
{
    if(!phonebook_head)
        return -1;

    struct list_head * head = phonebook_head;
    struct list_head * p;
    while((p = head->next) && (p != head))
    {
        phonebook_t* atype = list_entry(p, phonebook_t, list);
        list_del(p);
        if(atype)
        {
            free(atype);
        }
    }
    return 0;
}

phonebook_t *phonebook_find_by_id(struct list_head* phonebook_head,int id)
{
    struct list_head *p;
    struct list_head *head = phonebook_head;
    phonebook_t *phonebook_ptr;
    info_t *ainfo;
    list_for_each(p, head)
    {
        phonebook_ptr = list_entry(p, phonebook_t, list);
        if (phonebook_ptr->id ==  id)
    	{
            return phonebook_ptr;
    	}
    }

    return NULL;
}
void print_phonebook_list(struct list_head* phonebook_list)
{

    if(!phonebook_list)
        return;

    struct list_head * p;
    phonebook_t *phonebook_ptr;
    info_t *ainfo;
    int i ;
    list_for_each(p, phonebook_list)
    {
        phonebook_ptr = list_entry(p, phonebook_t, list);
        printf("phonebook : [%d] ------------------- \n ",phonebook_ptr->id);
        for(i=0; i<PHONEBOOK_PAGE_NUM; i++)
        {
            ainfo = &phonebook_ptr->info[i];
            printf("info[%d] %d - %s - %s \n",i,ainfo->type,ainfo->id,ainfo->name);
        }
    }

    return 0;
}

int phonebook_list_init(struct list_head * phonebook_head)
{
    phonebook_list_destory(phonebook_head);
    INIT_LIST_HEAD(phonebook_head);
    return 0;
}

int phonebook_list_sync_file(void)
{
    LpConfig *lpconfig;
    char sec[100], str[100]; 
	char TmpId[8] = {0};
    phonebook_t *phonebook;
    info_t* ainfo;
    char * astr;
    int i,j;
    char *name = NULL;
    struct list_head *phonebook_head = &g_phone_book_ctrl.phonebook_list;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    char cl[128];

    lpconfig = lp_config_new(INI_PHONEBOOK_FILE);
    if (lpconfig == NULL)
    {
        SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
        return -1;
    }

    phonebook_list_init(phonebook_head);

    for(i = 1; i <= MAX_EXTER_PHONEBOOK ; i++)
    {
        memset(sec,0,100);
        sprintf(sec,"board_%d",i);
        if(lp_config_has_section(lpconfig, sec) == 0)
        {
            /* SPON_LOG_ERR("no %s exist ,check [%s] is completed ?\n",sec,INI_PHONEBOOK_FILE); */
            break;
        }

        phonebook = ortp_new0(phonebook_t, 1);
        list_add(&phonebook->list, phonebook_head);
        phonebook->id = i;
        for(j = 0; j < PHONEBOOK_PAGE_NUM ; j++)
        {
            ainfo =  &(phonebook->info[j]);
            memset(str,0,100);
            sprintf(str,"key_%d",j);
            astr = lp_config_get_string(lpconfig,sec,str,NULL);
            if(astr)
            {
                memset(str,0,100);
                strcpy(str,astr);

                char *ptr = strstr(str,"_");
                if(ptr)
                {
                    *ptr = 0;
                    memcpy(ainfo->id, str, 7);          //sip_id
                    ainfo->type = *(ptr+1) - '0';   //sip_type

                    if (ainfo->type != INFO_SIP)
                    {
						memcpy(TmpId, ainfo->id, 7);
						TmpId[7] = 0;
						name = ncs_manager->ncs_term_name_get_by_id(ncs_manager, atoi(TmpId));

                        if(name)
                        {
                            strcpy(ainfo->name,name);
                        }
                        else
                        {
                            sprintf(ainfo->name,"Terminal-%s",TmpId);
							//SPON_LOG_ERR("error can't find terminal name with id : %s \n", TmpId);
                        }
                    }
                }

                if (ainfo->type == INFO_SIP)
                {
                    ptr = ptr + 1;
                    ptr = strstr(ptr, "_");
                    ptr = strstr(ptr+1, "_");
                    if (ptr)
                    {
                        utf8_to_gb2312(ptr + 1, ainfo->name, sizeof(ainfo->name));
                        // sprintf(ainfo->name, "%s", ptr + 1);
                    }

                    if (strlen(ainfo->name) == 0)
                        strcpy(ainfo->name, "Unknown");
                }

            }
        }
    }
    lp_config_destroy(lpconfig);

	//print_phonebook_list(phonebook_head);
    return 0;
}

void phonebook_info_process(HWND hWnd, int g_id, int p_id)
{
    int target_id;
    int data[10] = {0};
    int ret = 0;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;
    phonebook_t *phonebook = g_phone_book_ctrl.phonebook;

    if(!phonebook)
        return;

    info_type_t fun_type = phonebook->info[p_id].type;
    target_id = atoi(phonebook->info[p_id].id);
    data[0] = target_id;

    task_t task = get_system_task();
	printf("task : %d\n", task);
    switch (fun_type)
    {
		case INFO_SIP:
			if (task == TASK_NONE)
			{
				ncs_manager->ncs_monitor_stop(ncs_manager);
				term->sip_manager->call_request(phonebook->info[p_id].id, strlen(phonebook->info[p_id].id));

				set_system_task(TASK_SIP_CALL_OUTGOING);
				EnterWin(hWnd, guicall_out_proc);
			}
			break;
		case INFO_TERM_TALK:
			//set_system_task(TASK_NCS_CALL_OUTGOING);
			if(task == TASK_NCS_CALL_OUTGOING || task == TASK_SIP_CALL_OUTGOING)
			{
				ret = ncs_manager->ncs_ip_request(ncs_manager, target_id, 0, TALK);
				if(ret < 0)
					return ;
			
				EnterWin(hWnd, guicall_out_proc);
			}
			else if(task == TASK_NCS_MONITOR_OUTGOING)
			{
				ret = ncs_manager->ncs_ip_request(ncs_manager, target_id, 0, MONITOR);
				if(ret < 0)
					return ;
				EnterWin(hWnd, guicall_chatting_proc);
			}
			else if(task == TASK_NCS_CONFERENCE)
			{
				ret = ncs_manager->ncs_ip_request(ncs_manager, target_id, 0, TALK);
				if(ret < 0)
					return ;
				EnterWin(hWnd, guicall_conference_proc);
			}
			else if(task == TASK_NCS_CALL_TRANSFER)
			{
				ncs_manager->ncs_call_transfer(ncs_manager, target_id);
				/*EnterWin(hWnd, guicall_conference_proc);*/
			}
			break;
		default:
			break;
    }

}

int phonebook_info_init(void)
{
    if(phonebook_list_sync_file() == -1)
    {
        SPON_LOG_ERR("=====>%s - error phonebook_get_from_file\n", __func__);
		return -1;
    }

	return 0;
}
int phonebook_info_update(void)
{
	int ret = 0;
    ret = phonebook_info_init();
	
    usleep(500*1000);

    term->sip_manager->blf_state_update();

    return ret;
}

static int Phonebook_select_id(PRECT pRc, int x, int y)
{
	int idx = PHONEBOOK_UNVALID_ID,i = 0;
	short left = 0,right = 0,bottom = 0,top = 0;
	short iy = pRc->top + 30;
	short ix = pRc->left;

	for(;i<PHONEBOOK_PAGE_NUM;i++)
	{
		if(i >= PHONEBOOK_PAGE_NUM/2)
		{
			top = iy + PHONEBOOK_INFO_OFFSET*(i-5);
			bottom = top + PHONEBOOK_INFO_OFFSET;
			left = ix + PHONE_RECT_W/2;
			right = ix + PHONE_RECT_W;
		}
		else
		{
			top = iy + PHONEBOOK_INFO_OFFSET*i;
			bottom = top + PHONEBOOK_INFO_OFFSET;
			left = ix;
			right = ix + PHONE_RECT_W/2;
		}

		if(x > left && x <= right && y > top && y <= bottom)
		{
			idx = i;
			break;
		}
	}
	
	SPON_LOG_INFO("Phonebook_select_id %d\n",idx);
	return idx;
}

static void PhonebookInfoProc(HWND hWnd, int message, int wParam, int lParam)
{
	int idx = PHONEBOOK_UNVALID_ID;
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
    switch(message)
    {
        case MSG_LBUTTONDOWN:
 			if(g_phone_book_ctrl.select_id== PHONEBOOK_UNVALID_ID)
			{
				idx = Phonebook_select_id(&p->rc,LOWORD(lParam), HIWORD (lParam));
				if(idx != PHONEBOOK_UNVALID_ID)
				{
					g_phone_book_ctrl.select_id= idx;
				}
			}
			break;
        case MSG_LBUTTONUP:
			if(g_phone_book_ctrl.select_id != PHONEBOOK_UNVALID_ID)
			{
				phonebook_info_process(hWnd, g_phone_book_ctrl.cur_press_bar_id+1,g_phone_book_ctrl.select_id);
				g_phone_book_ctrl.select_id = PHONEBOOK_UNVALID_ID;
			}
            break;
    }
}

static void ButtonProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	TYPE_GUI_AREA_HANDLE *pTemp = &gui_phone_book_item[g_phone_book_ctrl.old_press_bar_id];
	task_t task = get_system_task();
	if(p->id == pTemp->id)
	{
		return;
	}

	InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    switch(message)
    {
        case MSG_LBUTTONDOWN:
			system_info.hdc = BeginPaint(hWnd);
			if(p->id == 0)
			{
				p->pReFlash(&p->rc, p->pData0);
			}
			else
			{
				p->pReFlash(&p->rc, p->pData1);
			}
			g_phone_book_ctrl.cur_press_bar_id = p->id;
			EndPaint(hWnd, system_info.hdc);
            break;
        case MSG_LBUTTONUP:
			InvalidateRect_Ex(hWnd, &pTemp->rc, TRUE); //先清除区域, 再刷新
			InvalidateRect_Ex(hWnd, &gui_phone_book_retang[0].rc, TRUE); //先清除区域, 再刷新
			system_info.hdc = BeginPaint(hWnd);
			g_pWinPrivatePrt = pTemp;
			if(pTemp->id == 0)
			{
				pTemp->pReFlash(&pTemp->rc, pTemp->pData1);
			}
			else
			{
				pTemp->pReFlash(&pTemp->rc, pTemp->pData0);
			}
			gui_phone_book_retang[0].pReFlash(&gui_phone_book_retang[0].rc, gui_phone_book_retang[0].pData0);

			g_pWinPrivatePrt = p;
			if(p->id == 0)
			{
				p->pReFlash(&p->rc, p->pData0);
			}
			else
			{
				p->pReFlash(&p->rc, p->pData1);
			}
			EndPaint(hWnd, system_info.hdc);
			g_phone_book_ctrl.old_press_bar_id = g_phone_book_ctrl.cur_press_bar_id;
            break;
    }
}

static void ButtonProcOther(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData1);
            EndPaint(hWnd, system_info.hdc);
            break;
        case MSG_LBUTTONUP:
            /*system_info.hdc = BeginPaint(hWnd);*/
            /*p->pReFlash(&p->rc, p->pData0);*/
            /*EndPaint(hWnd, system_info.hdc);*/
			switch(p->id)
			{
				case 0:
					InputDigtal_NewScreen(hWnd);
					break;
				default:
					break;
			}
            break;
		default:
			break;
    }
}

static void PaintButtonEx(PRECT pRc, void* pData)
{	
	BITMAP * bitmap = NULL;


	bitmap  = png_ha_res_find_by_name(pData);
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
}

static void PaintPhonebookInfo(PRECT pRc, void* pData)
{
	int x, y, i = 0;
	char buf[128] = {0};
    char tmp[128] = {0};
	
	info_t *ainfo;
	RECT rc;
	int page_id = g_phone_book_ctrl.cur_press_bar_id+1;
	struct list_head *phonebook_head = &g_phone_book_ctrl.phonebook_list;

    g_phone_book_ctrl.phonebook = phonebook_find_by_id(phonebook_head,page_id);
    if(!g_phone_book_ctrl.phonebook)
	{
		SPON_LOG_INFO("phonebook is null");
        return;
	}
	
	PaintButtonEx(pRc, pData);

	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	memcpy(&rc, pRc, sizeof(RECT));
	
	y = rc.top + 30;
	x = rc.left;
	for(i=0;i < PHONEBOOK_PAGE_NUM;i++)
	{
		if(i >= PHONEBOOK_PAGE_NUM/2)
		{
			rc.top = y + PHONEBOOK_INFO_OFFSET*(i-5);
			rc.bottom =rc.top + PHONEBOOK_INFO_OFFSET;
			rc.left = x + PHONE_RECT_W/2;
			rc.right = x + PHONE_RECT_W;
		}
		else
		{
			rc.top = y + PHONEBOOK_INFO_OFFSET*i;
			rc.bottom =rc.top + PHONEBOOK_INFO_OFFSET;
			rc.left = x;
			rc.right = x + PHONE_RECT_W/2;
		}

		ainfo = &g_phone_book_ctrl.phonebook->info[i];
		sprintf(tmp,"ID:%s - %s\n",ainfo->id,ainfo->name);
        if (strlen(tmp) > 25) 
        {
            strncpy(buf, tmp, 25);
			strcat(buf, "...");
		}
        else
    	{
			strcpy(buf, tmp);
    	}
		
		DrawText(system_info.hdc, buf, -1, &rc, DT_CENTER);
		if(i%(PHONEBOOK_PAGE_NUM/2) != PHONEBOOK_PAGE_NUM/2 - 1)
		{
			SetPenColor(system_info.hdc, COLOR_lightwhite);
			MoveTo(system_info.hdc, rc.left+20, rc.bottom-20);
			LineTo(system_info.hdc, rc.right-20, rc.bottom-20);
		}
	}
}

static void PaintButton(PRECT pRc, void* pData)
{	
	BITMAP * bitmap = NULL;

	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	char buf[128] = {0};
	RECT rc;

	bitmap  = png_ha_res_find_by_name(pData);
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
#if 0
	sprintf(buf, "分组%d", p->id+1);
	memcpy(&rc, pRc, sizeof(RECT));
	rc.top+=10;
	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	DrawText(system_info.hdc, buf, -1, &rc, DT_CENTER);
#endif	
}

int PhoneBookWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;


	if(g_firstEnter)
	{
		SPON_LOG_INFO("===============> Enter PhoneBookWinProc\n");
		g_firstEnter = FALSE;
	}
	switch(message)
    {
        case MSG_CREATE:
            break;
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
            /*case MSG_NCS_INPUT_DIGTAL_PAGE:*/
            HandlePageProcEx((TYPE_GUI_AREA_HANDLE*)gui_phone_book_item, hWnd, message, wParam, lParam, g_phone_book_ctrl.bar_cout);
			HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_phone_book_retang, hWnd, message, wParam, lParam);
			HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_phone_book_other, hWnd, message, wParam, lParam);
            break;
        case MSG_PAINT:
            system_info.hdc = BeginPaint(hWnd);
            HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_statuebar_item, hWnd);
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_phone_book_retang, hWnd);
			HandleCreateExx((TYPE_GUI_AREA_HANDLE*)gui_phone_book_item, hWnd, g_phone_book_ctrl.bar_cout);
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_phone_book_other, hWnd);
            EndPaint(hWnd, system_info.hdc);
			g_phone_book_ctrl.cur_press_bar_id = g_phone_book_ctrl.old_press_bar_id = 0;
            return 0;
        case MSG_CLOSE:
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;
        case MSG_ERASEBKGND:
            bitmap  = png_ha_res_find_by_name("othermainwindowbg.png");
            FillBoxWithBitmap((HDC)wParam,0,0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
            return 0;
        case MSG_DESTROY:
            return 0;
        default:
            break;
    }
	return 1;
}


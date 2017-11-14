#include "client_watch_dog.h"

int ncs_watchdog_data_report_append(ncs_watchdog_client_t* client,ncs_thread_t* thread,int index)
{
    ncs_thread_t* p_thread = (ncs_thread_t*)client->data_report;
    if (p_thread &&  index < client->monitor_thread_count)
    {
        memcpy(p_thread+index,thread,sizeof(ncs_thread_t));
    }

    return index;
}

bool_t ncs_watchdog_thread_info_report(ncs_watchdog_client_t* client)
{
    ncs_thread_t * this_thread = NULL;
    struct list_head *header = &client->monitor_thread_header;
    struct list_head *p = NULL;
    int thread_cound = 0;
    int len = -1;
    int data_size = client->monitor_thread_count * sizeof(ncs_thread_t);

    if (!client->data_report)
    {
        client->data_report = malloc (client->monitor_thread_count * sizeof(ncs_thread_t));
    }

    list_for_each(p, header)
    {
        this_thread = list_entry(p, ncs_thread_t, list);
        ncs_thread_is_running(this_thread);
        ncs_watchdog_data_report_append(client,this_thread,thread_cound);
        thread_cound++;
//		ncs_thread_print(this_thread);
    }

    if (client->is_connected)
    {
        len = socket_client_pipe_write(client->client_fd,client->data_report,data_size);
        if (len < 0)
        {
            SPON_LOG_ERR("send data error \n");
            socket_pipe_close(client->client_fd);
            client->client_fd = -1;
            client->is_connected = FALSE;
        }
    }

    if (client->data_report)
    {
        free(client->data_report);
        client->data_report = NULL;
    }

    return client->is_connected;
}

int ncs_watchdog_monitor_add(ncs_watchdog_client_t* client, ncs_thread_t * thread)
{
    if (client->monitor_thread_count > NCS_MONITOR_MAX_SIZE)
    {
        SPON_LOG_ERR("Add Error \n");
        return FALSE;
    }

    if (ncs_thread_list_add(thread, &client->monitor_thread_header))
    {
        client->monitor_thread_count++;
        return TRUE;
    }

    return FALSE;
}

#define NCS_WATCHDOG_CLIENT_REPORT_TIMEOUT		(3)

void* ncs_watchdog_client_monitor(void *arg)
{
    ncs_thread_t* this_thread = NCS_PTR_CONVERT(ncs_thread_t, arg);
    ncs_thread_state_set(this_thread, NCS_THREAD_START);
    ncs_thread_noblock_set(this_thread, TRUE);

    int ncs_error_count = 0;
    ncs_watchdog_client_t* client = NCS_PTR_CONVERT(ncs_watchdog_client_t, this_thread->private_data);
    do
    {
        ncs_thread_running_update(this_thread);
        if (!client->is_connected)
            ncs_connect_watch_connect(client);
        if (client->is_connected)
            ncs_watchdog_thread_info_report(client);
        sleep(NCS_WATCHDOG_CLIENT_REPORT_TIMEOUT);
    }while(TRUE);
    ncs_thread_state_set(this_thread, NCS_THREAD_STOP);
	pthread_exit(NULL);
	return NULL;
}

int ncs_connect_watch_connect(ncs_watchdog_client_t* client)
{
    client->client_fd = -1;
    client->is_connected = FALSE;
    client->client_fd = socket_client_pipe_connect(WATCHDOG_PIPE);

    /*SPON_LOG_INFO("create %s ,client_fd:%d \n",WATCHDOG_PIPE,client->client_fd);*/

    if (client->client_fd < 0)
        client->is_connected = FALSE;
    else
        client->is_connected = TRUE;

    return client->is_connected;
}

ncs_watchdog_client_t* ncs_watchdog_client_create(void* arg)
{
    ncs_watchdog_client_t* client = ncs_new(ncs_watchdog_client_t);
    ncs_clear(client, ncs_watchdog_client_t);
    list_init(&client->monitor_thread_header);
    client->private_data = arg;

    return client;
}

int ncs_connect_watch_start(ncs_watchdog_client_t* client,void * arg)
{
    ncs_thread_init(&client->client_monitor_pid);

    if (ncs_thread_start(&client->client_monitor_pid, "NCS Monitor",ncs_watchdog_client_monitor,arg) < 0)
    {
        SPON_LOG_FATAL("monitor start  Error !");
        return -1;
    }
    return TRUE;
}


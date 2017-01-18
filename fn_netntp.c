#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_bdb_status.h"
#include "iopccmd_service.h"

#define SERVICE_NAME		"netntp"
#define SERVICE_WAIT		"netdev"

static void* action_start(void* arg)
{
    uint8_t cmd[STR_LEN];
    uint8_t *ntp_server = "tock.stdtime.gov.tw";
    uint8_t *ntp_exe = "/usr/sbin/ntpd";
    pthread_t thread_id = pthread_self();
    printf("%s(%lu)\n", __func__, thread_id);

    printf("Starting network NTP...\n");

    GET_INSTANCE(ops_cfg_bdb_status)->wait_service_started(SERVICE_WAIT);

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "%s -p %s -qNn", ntp_exe, ntp_server);
    GET_INSTANCE(ops_misc)->execute_cmd(cmd, NULL);
    //printf("NTP: %s\n", cmd);

    GET_INSTANCE(ops_cfg_bdb_status)->set_service_started(SERVICE_NAME);
    GET_INSTANCE(ops_cfg_bdb_status)->wait_service_stoped(SERVICE_NAME);

    pthread_exit((void*)0);
    return 0;
}

static void* action_stop(void* arg)
{

    GET_INSTANCE(ops_cfg_bdb_status)->set_service_stoped(SERVICE_NAME);
    return 0;
}


static void* action_restart(void* arg)
{
}

uint32_t hn_netntp(uint8_t* preq, uint8_t* pres)
{
    struct req_service_t* req = (struct req_service_t*)preq;
    struct res_service_t* res = (struct res_service_t*)pres;
    uint8_t action = req->action;

    switch(action) {
    case SERVICE_ACTION_STATUS:
    break;
    case SERVICE_ACTION_START:
        GET_INSTANCE(ops_cfg_bdb_status)->set_service_starting(SERVICE_NAME);
        GET_INSTANCE(ops_misc)->create_task(action_start);
        //action_start(NULL);
    break;
    case SERVICE_ACTION_STOP:
        GET_INSTANCE(ops_cfg_bdb_status)->set_service_stoping(SERVICE_NAME);
        action_stop(NULL);
    break;
    case SERVICE_ACTION_RESTART:
        action_restart(NULL);
    break;
    default:
        res->status = 0x01;
    break;
    }

    res->action = req->action;
    res->service_status = GET_INSTANCE(ops_cfg_bdb_status)->get_service_status(SERVICE_NAME);

    return sizeof(struct res_service_t);
}


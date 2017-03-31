#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopccmd_service.h"

#define SERVICE_NAME		"netssh"
#define SERVICE_WAIT		"netdev"

static struct root_t ssh_init_list[] = {
    INIT_NODE(ROOT_TYPE_CREATE_DIR, "/etc/dropbear", DEFAULT_MODE, 0 ),
    INIT_NODE(ROOT_TYPE_CREATE_FILE, "/var/log/lastlog", DEFAULT_MODE, 0 ),
    INIT_NODE(ROOT_TYPE_CREATE_SYMBOL, "/usr/sbin/dropbear", "/usr/sbin/dropbearkey", 0 ),
    INIT_NODE(ROOT_TYPE_CREATE_SYMBOL, "/usr/sbin/dropbear", "/usr/sbin/scp", 0 ),
};

static void* action_start(void* arg)
{
    uint8_t cmd[STR_LEN];
    uint8_t *dropbear_cfg = "/etc/dropbear";
    uint8_t *dropbear_key_rsa = "dropbear_rsa_host_key";
    uint8_t *dropbear_bin = "/usr/sbin/dropbear";
    uint8_t *dropbearkey_link = "/usr/sbin/dropbearkey";
//    uint8_t *scp_link = "/usr/sbin/scp";
    int ssh_init_list_size = (sizeof(ssh_init_list) / sizeof(ssh_init_list[0]));
    pthread_t thread_id = pthread_self();
    printf("%s(%lu)\n", __func__, thread_id);

    printf("Starting Network SSH...\n");

    GET_INSTANCE_CFG_STATUS()->wait_service_started(SERVICE_WAIT);

    GET_INSTANCE_MISC_OBJ()->create_system_by_list(ssh_init_list_size, &ssh_init_list[0]);

    //Generate key...
    printf("Generating dropbear key...\n");
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "%s -t rsa -f %s/%s", dropbearkey_link, dropbear_cfg, dropbear_key_rsa);
    GET_INSTANCE_MISC_OBJ()->execute_cmd(cmd, NULL);

    printf("Starting dropbear...\n");
    memset(cmd, 0, sizeof(cmd));
    //sprintf(cmd, "%s -r %s/%s -R -E -s -g -F", dropbear_bin, dropbear_cfg, dropbear_key_rsa);
    sprintf(cmd, "%s -r %s/%s -s -g -E ", dropbear_bin, dropbear_cfg, dropbear_key_rsa);
    GET_INSTANCE_MISC_OBJ()->execute_cmd(cmd, NULL);

    printf("Set service started...\n");
    GET_INSTANCE_CFG_STATUS()->set_service_started(SERVICE_NAME);
    GET_INSTANCE_CFG_STATUS()->wait_service_stoped(SERVICE_NAME);

    pthread_exit((void*)0);
    return 0;
}

static void* action_stop(void* arg)
{
}

static void* action_restart(void* arg)
{
}

uint32_t hn_netssh(uint8_t* preq, uint8_t* pres)
{
    struct req_service_t* req = (struct req_service_t*)preq;
    struct res_service_t* res = (struct res_service_t*)pres;
    uint8_t action = req->action;

    switch(action) {
    case SERVICE_ACTION_STATUS:
    break;
    case SERVICE_ACTION_START:
        GET_INSTANCE_CFG_STATUS()->set_service_starting(SERVICE_NAME);
        GET_INSTANCE_MISC_OBJ()->create_task(action_start);
    break;
    case SERVICE_ACTION_STOP:
        GET_INSTANCE_CFG_STATUS()->set_service_stoping(SERVICE_NAME);
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
    res->service_status = GET_INSTANCE_CFG_STATUS()->get_service_status(SERVICE_NAME);

    return sizeof(struct res_service_t);
}


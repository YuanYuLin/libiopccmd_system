#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_bdb.h"
#include "iopcops_cfg_bdb_status.h"
#include "iopcops_cfg_bdb_platform.h"
#include "iopccmd_service.h"

#define SERVICE_NAME	"mntbase"
#define SERVICE_WAIT	""

static void startByIndex(uint8_t platform_idx, uint8_t index)
{
    uint16_t str_len = 0;
    uint8_t src[STR_LEN];
    uint8_t dst[STR_LEN];
    uint8_t type[STR_LEN];
    uint8_t name[STR_LEN];
    uint8_t loop[STR_LEN];
    uint8_t enabled = 0;

    memset(name, 0, STR_LEN);
    str_len = GET_INSTANCE(ops_cfg_bdb_platform)->get_partition_name(platform_idx, index, &name[0]);

    memset(src, 0, STR_LEN);
    str_len = GET_INSTANCE(ops_cfg_bdb_platform)->get_partition_src(platform_idx, index, &src[0]);

    memset(dst, 0, STR_LEN);
    str_len = GET_INSTANCE(ops_cfg_bdb_platform)->get_partition_dst(platform_idx, index, &dst[0]);

    memset(type, 0, STR_LEN);
    str_len = GET_INSTANCE(ops_cfg_bdb_platform)->get_partition_type(platform_idx, index, &type[0]);

    enabled = GET_INSTANCE(ops_cfg_bdb_platform)->get_partition_enabled(platform_idx, index);

    if(0)
        printf("%d\n", str_len);

    if(enabled) {
        //printf("MOUNT %d, %s, %s, %s\n", index, src, dst, type);
        if(strcmp(type, "binddir") == 0) {
            GET_INSTANCE(ops_misc)->create_dir_recursive(src, DEFAULT_MODE);
            GET_INSTANCE(ops_misc)->create_dir_recursive(dst, DEFAULT_MODE);
            GET_INSTANCE(ops_misc)->mountfs(src, dst, NULL, 0x9000, NULL);
        } else if(strcmp(type, "squashfs") == 0) {
            memset(loop, 0, STR_LEN);
            sprintf(loop, "/dev/loop%d", index);
            GET_INSTANCE(ops_misc)->bind_loopdev(src, loop, 0);
	    GET_INSTANCE(ops_misc)->create_dir_recursive(dst, DEFAULT_MODE);
            GET_INSTANCE(ops_misc)->mountfs(loop, dst, "squashfs", 0x8001, NULL);
        } else {
            GET_INSTANCE(ops_misc)->create_dir_recursive(dst, DEFAULT_MODE);
            GET_INSTANCE(ops_misc)->mountfs(src, dst, type, 0, NULL);
        }

	if(strcmp("persist", name) == 0) {
            //GET_INSTANCE(ops_cfg_bdb)->save_persist2cfgsdb();
	}
    }
}

static void* action_start(void* arg)
{
    int no_part = 0;
    int i = 0;
    uint8_t platform_idx = 0;
    struct arg_t *parm = (struct arg_t*)arg;
    pthread_t thread_id = pthread_self();
    printf("%s(%lu)\n", __func__, thread_id);

    GET_INSTANCE(ops_cfg_bdb_status)->wait_service_started(SERVICE_WAIT);

    if(arg == NULL) {
        no_part = GET_INSTANCE(ops_cfg_bdb_platform)->get_no_partitions(platform_idx);
        printf("mount starting...%d\n", no_part);

        for(i=0;i<no_part;i++) {
            startByIndex(platform_idx, i);
        }
    } else {
        printf("SPECIFIC:%d\n", parm->index);
        startByIndex(platform_idx, parm->index);
    }

    GET_INSTANCE(ops_cfg_bdb_status)->set_service_started(SERVICE_NAME);
    if(arg != NULL)
        free(arg);
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
    return 0;
}

uint32_t hn_mntbase(uint8_t* preq, uint8_t* pres)
{
    struct req_service_t* req = (struct req_service_t*)preq;
    struct res_service_t* res = (struct res_service_t*)pres;
    struct arg_t *arg = (struct arg_t*)(preq + sizeof(struct req_service_t));
    uint8_t action = req->action;

    printf("%s running\n", __func__);
    switch(action) {
    case SERVICE_ACTION_STATUS:
    break;
    case SERVICE_ACTION_START:
        GET_INSTANCE(ops_cfg_bdb_status)->set_service_starting(SERVICE_NAME);
	GET_INSTANCE(ops_misc)->create_task(action_start);
    break;
    case SERVICE_ACTION_STOP:
        GET_INSTANCE(ops_cfg_bdb_status)->set_service_stoping(SERVICE_NAME);
        action_stop(NULL);
    break;
    case SERVICE_ACTION_RESTART:
        action_restart(NULL);
    break;
    case SERVICE_ACTION_START_SPECIFIC:
        GET_INSTANCE(ops_cfg_bdb_status)->set_service_starting(SERVICE_NAME);
	GET_INSTANCE(ops_misc)->create_task_with_arg(action_start, (void*)arg, sizeof(struct arg_t));
    break;
    default:
        res->status = 0x01;
    break;
    }

    res->service_status = GET_INSTANCE(ops_cfg_bdb_status)->get_service_status(SERVICE_NAME);
    res->action = req->action;

    return sizeof(struct res_service_t);
}


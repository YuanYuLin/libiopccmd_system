#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopcops_cfg_platform.h"
#include "iopccmd_service.h"

#define SERVICE_NAME	"raiddev"
#define SERVICE_WAIT	"mntbase"

static void startByIndex(uint8_t platform_idx, uint8_t index)
{
    uint16_t str_len = 0;
    uint8_t name[STR_LEN];
    uint8_t type[STR_LEN];
    uint8_t enabled;
    uint32_t size;
    uint8_t device_enabled;
    uint8_t dev_name[STR_LEN];
    uint8_t mount_option[STR_LEN];
    int i = 0;

    memset(name, 0, STR_LEN);
    str_len = GET_INSTANCE_CFG_PLATFORM()->get_raid_name(platform_idx, index, &name[0]);

    memset(type, 0, STR_LEN);
    str_len = GET_INSTANCE_CFG_PLATFORM()->get_raid_type(platform_idx, index, &type[0]);

    enabled = GET_INSTANCE_CFG_PLATFORM()->get_raid_enabled(platform_idx, index);

    size = GET_INSTANCE_CFG_PLATFORM()->get_raid_device_size(platform_idx, index);

    memset(mount_option, 0, STR_LEN);
    for(i=0;i<size;i++) {
        device_enabled = GET_INSTANCE_CFG_PLATFORM()->get_raid_device_enabled(platform_idx, index, i);
	if(device_enabled) {
            memset(dev_name, 0, STR_LEN);
            str_len = GET_INSTANCE_CFG_PLATFORM()->get_raid_device_path(platform_idx, index, i, &dev_name[0]);
	    strcat(mount_option, "device=");
	    strcat(mount_option, dev_name);
	    strcat(mount_option, ",");

	    while(1) {
                if(GET_INSTANCE_MISC_OBJ()->is_exist(dev_name) < 0) {
			printf("%s not exist, sleeping 1s\n", dev_name);
			sleep(1);
		} else {
			break;
		}
	    }
	}
    }

    printf("option:%s, %s\n", dev_name, mount_option);

    if(0)
        printf("%d\n", str_len);

    if(enabled) {
	//mount('/dev/sdc','/mnt/','btrfs',0x00008000,'device=/dev/sdb,device=/dev/sdc')
        if(strcmp(type, "btrfs_raid1") == 0) {
            GET_INSTANCE_MISC_OBJ()->create_dir_recursive(IOPC_DIR_RAID, DEFAULT_MODE);
            GET_INSTANCE_MISC_OBJ()->mountfs(dev_name, IOPC_DIR_RAID, "btrfs", 0x8000, mount_option);
        } else {
            printf("error: type[%s] not supported\n", type);
        }
    }
}

static void* action_start(void* arg)
{
    int size = 0;
    int i = 0;
    uint8_t platform_idx = 0;
    struct arg_t *parm = (struct arg_t*)arg;
    pthread_t thread_id = pthread_self();
    GET_INSTANCE_CFG_STATUS()->wait_service_started(SERVICE_WAIT);
    printf("%s(%lu)\n", __func__, thread_id);

    if(arg == NULL) {
        size = GET_INSTANCE_CFG_PLATFORM()->get_raid_size(platform_idx);
        printf("RAID starting...%d\n", size);

        for(i=0;i<size;i++) {
            startByIndex(platform_idx, i);
        }
    } else {
        printf("SPECIFIC:%d\n", parm->index);
        startByIndex(platform_idx, parm->index);
    }

    GET_INSTANCE_CFG_STATUS()->set_service_started(SERVICE_NAME);
    if(arg != NULL)
        free(arg);
    pthread_exit((void*)0);
//    pthread_join(thread_id, NULL);
    return 0;
}

static void* action_stop(void* arg)
{
    GET_INSTANCE_CFG_STATUS()->set_service_stoped(SERVICE_NAME);
    return 0;
}

static void* action_restart(void* arg)
{
    return 0;
}

uint32_t hn_raiddev(uint8_t* preq, uint8_t* pres)
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
    case SERVICE_ACTION_START_SPECIFIC:
        GET_INSTANCE_CFG_STATUS()->set_service_starting(SERVICE_NAME);
	GET_INSTANCEMISC_OBJ()->create_task_with_arg(action_start, (void*)arg, sizeof(struct arg_t));
    break;
    default:
        res->status = 0x01;
    break;
    }

    res->service_status = GET_INSTANCE_CFG_STATUS()->get_service_status(SERVICE_NAME);
    res->action = req->action;

    return sizeof(struct res_service_t);
}


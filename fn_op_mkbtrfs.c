#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
//#include "iopccmd_service.h"
#include "iopccmd_op_mkbtrfs.h"

#define PROGRESS_NAME		"mkbtrfs"
#define STAGE_STATUS(STATUS)	(PROGRESS_TYPE_STAGE | STATUS)

static uint8_t progress_id = 0x01;

static void* action_exec(void* arg)
{
    uint8_t *cmd_str = (uint8_t*)arg;
    //mkfs.btrfs -m raid1 -d raid1 /dev/sdb /dev/sdc -f
    GET_INSTANCE_MISC_OBJ()->execute_cmd(cmd_str, NULL);
    GET_INSTANCE_CFG_STATUS()->set_progress_status(PROGRESS_NAME, STAGE_STATUS(PS_STAGE_SUCCESSED));
    if(arg != NULL)
        free(arg);
    pthread_exit((void*)0);
}

uint32_t hn_mkbtrfs(uint8_t* preq, uint8_t* pres)
{
    struct req_mkbtrfs_t* req = (struct req_mkbtrfs_t*)preq;
    struct res_mkbtrfs_t* res = (struct res_mkbtrfs_t*)pres;
    uint8_t cmd_str[STR_LEN];
    int i = 0;
    int enabled_count = 0;

    if((GET_INSTANCE_CFG_STATUS()->get_progress_status(PROGRESS_NAME) & (PS_TYPE_MASK | PS_STAGE_MASK)) == STAGE_STATUS(PS_STAGE_INPROGRESS)) {
        res->status = 0xF0;
        return sizeof(struct res_mkbtrfs_t);
    }

    GET_INSTANCE_CFG_STATUS(ops_cfg_bdb_status)->set_progress_status(PROGRESS_NAME, STAGE_STATUS(PS_STAGE_INPROGRESS));

    res->status = 0;
    res->progress_id = progress_id;
    memset(cmd_str, 0, STR_LEN);

    sprintf(cmd_str, "mkfs.btrfs -f -m %s -d %s ", req->type);
    if(req->dev_size < MIN_RAID_COUNT) {
        res->status = 2;
        GET_INSTANCE_CFG_STATUS()->set_progress_status(PROGRESS_NAME, STAGE_STATUS(PS_STAGE_FAILED | 2));
	return sizeof(struct res_mkbtrfs_t);
    }
    if(req->dev_size > MAX_RAID_COUNT) {
        res->status = 1;
        GET_INSTANCE_CFG_STATUS()->set_progress_status(PROGRESS_NAME, STAGE_STATUS(PS_STAGE_FAILED | 1));
        return sizeof(struct res_mkbtrfs_t);
    }

    for(i=0;i<req->dev_size;i++) {
        if(req->dev[i].enabled) {
            enabled_count++;
            strcat(cmd_str, " ");
            strcat(cmd_str, req->dev[i].path);
	}
    }

    if(enabled_count < MIN_RAID_COUNT) {
        res->status = 3;
        GET_INSTANCE_CFG_STATUS()->set_progress_status(PROGRESS_NAME, STAGE_STATUS(PS_STAGE_FAILED | 3));
        return sizeof(struct res_mkbtrfs_t);
    }

    GET_INSTANCE_MISC_OBJ()->create_task_with_arg(action_exec, (void*)req, sizeof(struct req_mkbtrfs_t));

    return sizeof(struct res_mkbtrfs_t);
}

uint32_t hn_mkbtrfs_status(uint8_t* preq, uint8_t* pres)
{
    struct req_mkbtrfs_status_t* req = (struct req_mkbtrfs_status_t*)preq;
    struct res_mkbtrfs_status_t* res = (struct res_mkbtrfs_status_t*)pres;
    res->status = 0;

    if(req->progress_id != progress_id) {
        res->status = 1;
	return sizeof(struct res_mkbtrfs_status_t);
    }
    res->progress_status = GET_INSTANCE_CFG_STATUS()->get_progress_status(PROGRESS_NAME);
    return sizeof(struct res_mkbtrfs_status_t);
}


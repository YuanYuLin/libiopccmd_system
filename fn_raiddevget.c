#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_bdb_status.h"
#include "iopcops_cfg_bdb_platform.h"
#include "iopccmd_raiddevget.h"

uint32_t hn_raiddevget(uint8_t* preq, uint8_t* pres)
{
    struct req_raiddevget_t* req = (struct req_raiddevget_t*)preq;
    struct res_raiddevget_t* res = (struct res_raiddevget_t*)pres;
    uint8_t platform_idx = 0;
    uint16_t len = 0;
    uint8_t count = 0;
    uint8_t index = 0;
    int i = 0;
    res->status = 0;
    index = req->index;

    count = GET_INSTANCE(ops_cfg_bdb_platform)->get_raid_size(platform_idx);
    if(index >= count) {
	    res->status = 1;
	    return sizeof(struct res_raiddevget_t);
    }

    res->index = index;
    res->enabled = GET_INSTANCE(ops_cfg_bdb_platform)->get_raid_enabled(platform_idx, index);
    len = GET_INSTANCE(ops_cfg_bdb_platform)->get_raid_name(platform_idx, index, &res->name[0]);
    printf("len:%d\n", len);
    len = GET_INSTANCE(ops_cfg_bdb_platform)->get_raid_type(platform_idx, index, &res->type[0]);
    res->dev_size = GET_INSTANCE(ops_cfg_bdb_platform)->get_raid_device_size(platform_idx, index);
    for(i=0;i<res->dev_size;i++) {
	res->dev[i].enabled = GET_INSTANCE(ops_cfg_bdb_platform)->get_raid_device_enabled(platform_idx, index, i);
	GET_INSTANCE(ops_cfg_bdb_platform)->get_raid_device_path(platform_idx, index, i, &res->dev[i].path[0]);
    }

    return sizeof(struct res_raiddevget_t);
}


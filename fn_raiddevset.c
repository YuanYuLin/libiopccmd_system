#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopcops_cfg_platform.h"
#include "iopccmd_raiddevset.h"

uint32_t hn_raiddevset(uint8_t* preq, uint8_t* pres)
{
    struct req_raiddevset_t* req = (struct req_raiddevset_t*)preq;
    struct res_raiddevset_t* res = (struct res_raiddevset_t*)pres;
    uint8_t platform_idx = 0;
    uint8_t raiddev_count = 0;
    uint8_t raiddev_device_count = 0;
    uint8_t index = 0;
    int i = 0;
    res->status = 0;
    index = req->index;

    raiddev_count = GET_INSTANCE_CFG_PLATFORM()->get_raid_size(platform_idx);
    if(index >= raiddev_count) {
        res->status = 1;
	printf("index(%d) >= raiddev_count(%d)\n", index, raiddev_count);
        return sizeof(struct res_raiddevset_t);
    }

    GET_INSTANCE_CFG_PLATFORM()->set_raid_enabled(platform_idx, index, req->enabled);
    GET_INSTANCE_CFG_PLATFORM()->set_raid_type(platform_idx, index, req->type);
    GET_INSTANCE_CFG_PLATFORM()->set_raid_name(platform_idx, index, req->name);
    raiddev_device_count = GET_INSTANCE_CFG_PLATFORM()->get_raid_device_size(platform_idx, index);
    for(i=0;i<raiddev_device_count;i++) {
        printf("set %d, %s\n", req->dev[i].enabled, req->dev[i].path);
        GET_INSTANCE_CFG_PLATFORM()->set_raid_device_enabled(platform_idx, index, i, req->dev[i].enabled);
        GET_INSTANCE_CFG_PLATFORM()->set_raid_device_path(platform_idx, index, i, req->dev[i].path);
    }

    return sizeof(struct res_raiddevset_t);
}


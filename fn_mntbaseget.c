#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopcops_cfg_platform.h"
#include "iopccmd_mntbaseget.h"

uint32_t hn_mntbaseget(uint8_t* preq, uint8_t* pres)
{
    struct req_mntbaseget_t* req = (struct req_mntbaseget_t*)preq;
    struct res_mntbaseget_t* res = (struct res_mntbaseget_t*)pres;
    uint8_t platform_idx = 0;
    uint16_t len = 0;
    uint8_t count = 0;
    uint8_t index = 0;
    res->status = 0;
    index = req->index;

    count = GET_INSTANCE_CFG_PLATFORM()->get_no_partitions(platform_idx);
    if(index >= count) {
	    res->status = 1;
	    return sizeof(struct res_mntbaseget_t);
    }

    res->index = index;
    res->enabled = GET_INSTANCE_CFG_PLATFORM()->get_partition_enabled(platform_idx, index);
    res->fixed = GET_INSTANCE_CFG_PLATFORM()->get_partition_fixed(platform_idx, index);
    len = GET_INSTANCE_CFG_PLATFORM()->get_partition_name(platform_idx, index, &res->name[0]);
    printf("len:%d\n", len);
    len = GET_INSTANCE_CFG_PLATFORM()->get_partition_src(platform_idx, index, &res->src[0]);
    len = GET_INSTANCE_CFG_PLATFORM()->get_partition_dst(platform_idx, index, &res->dst[0]);
    len = GET_INSTANCE_CFG_PLATFORM()->get_partition_type(platform_idx, index, &res->type[0]);

    return sizeof(struct res_mntbaseget_t);
}


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopcops_cfg_platform.h"
#include "iopccmd_mntbaseset.h"

uint32_t hn_mntbaseset(uint8_t* preq, uint8_t* pres)
{
    struct req_mntbaseset_t* req = (struct req_mntbaseset_t*)preq;
    struct res_mntbaseset_t* res = (struct res_mntbaseset_t*)pres;
    uint8_t platform_idx = 0;
    uint8_t mntbase_count = 0;
    uint8_t index = 0;
    uint8_t fixed = 0;
    res->status = 0;
    index = req->index;

    mntbase_count = GET_INSTANCE_CFG_PLATFORM()->get_no_partitions(platform_idx);
    if(index >= mntbase_count) {
        res->status = 1;
	printf("index(%d) >= mntbase_count(%d)\n", index, mntbase_count);
        return sizeof(struct res_mntbaseset_t);
    }

    fixed = GET_INSTANCE_CFG_PLATFORM()->get_partition_fixed(platform_idx, index);
    if(fixed) {
        res->status = 2;
	printf("FIXED(%d), could NOT changed\n", index);
        return sizeof(struct res_mntbaseset_t);
    }

    GET_INSTANCE_CFG_PLATFORM()->set_partition_enabled(platform_idx, index, req->enabled);
    GET_INSTANCE_CFG_PLATFORM()->set_partition_src(platform_idx, index, req->src);
    GET_INSTANCE_CFG_PLATFORM()->set_partition_dst(platform_idx, index, req->dst);
    GET_INSTANCE_CFG_PLATFORM()->set_partition_type(platform_idx, index, req->type);
    GET_INSTANCE_CFG_PLATFORM()->set_partition_name(platform_idx, index, req->name);

    return sizeof(struct res_mntbaseset_t);
}


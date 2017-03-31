#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopcops_cfg_platform.h"
#include "iopccmd_mntbasecount.h"

uint32_t hn_mntbasecount(uint8_t* preq, uint8_t* pres)
{
    //struct req_mntbasecount_t* req = (struct req_mntbasecount_t*)preq;
    struct res_mntbasecount_t* res = (struct res_mntbasecount_t*)pres;
    uint8_t mntbase_count = 0;
    uint8_t platform_idx = 0;
    res->status = 0;

    mntbase_count = GET_INSTANCE_CFG_PLATFORM()->get_no_partitions(platform_idx);
    res->count = mntbase_count;

    return sizeof(struct res_mntbasecount_t);
}


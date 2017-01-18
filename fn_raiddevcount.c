#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_bdb_status.h"
#include "iopcops_cfg_bdb_platform.h"
#include "iopccmd_raiddevcount.h"

uint32_t hn_raiddevcount(uint8_t* preq, uint8_t* pres)
{
    //struct req_mntbasecount_t* req = (struct req_mntbasecount_t*)preq;
    struct res_raiddevcount_t* res = (struct res_raiddevcount_t*)pres;
    uint8_t platform_idx = 0;
    res->status = 0;

    printf("%s get...\n", __func__);
    res->count = GET_INSTANCE(ops_cfg_bdb_platform)->get_raid_size(platform_idx);
    printf("%s:count%d\n", __func__, res->count);

    return sizeof(struct res_raiddevcount_t);
}


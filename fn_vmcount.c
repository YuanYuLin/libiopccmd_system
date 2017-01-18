#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_bdb_status.h"
#include "iopcops_cfg_bdb_vm.h"
#include "iopccmd_vmcount.h"

uint32_t hn_vmcount(uint8_t* preq, uint8_t* pres)
{
    //struct req_vmcount_t* req = (struct req_vmcount_t*)preq;
    struct res_vmcount_t* res = (struct res_vmcount_t*)pres;
    uint8_t vm_count = 0;
    res->status = 0;

    vm_count = GET_INSTANCE(ops_cfg_bdb_vm)->get_cfg_size();
    res->count = vm_count;

    return sizeof(struct res_vmcount_t);
}


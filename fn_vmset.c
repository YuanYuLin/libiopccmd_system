#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopcops_cfg_vm.h"
#include "iopccmd_vmset.h"

uint32_t hn_vmset(uint8_t* preq, uint8_t* pres)
{
    struct req_vmset_t* req = (struct req_vmset_t*)preq;
    struct res_vmset_t* res = (struct res_vmset_t*)pres;
    uint8_t vm_count = 0;
    uint8_t vm_index = 0;
    res->status = 0;
    vm_index = req->index;

    vm_count = GET_INSTANCE_CFG_VM()->get_cfg_size();
    if(vm_index >= vm_count) {
	    res->status = 1;
	    return sizeof(struct res_vmset_t);
    }

    GET_INSTANCE_CFG_VM()->set_autostart(vm_index, req->auto_start);
    GET_INSTANCE_CFG_VM()->set_name(vm_index, req->name);
    GET_INSTANCE_CFG_VM()->set_base_path(vm_index, req->base_path);
    GET_INSTANCE_CFG_VM()->set_nettype(vm_index, req->nettype);
    GET_INSTANCE_CFG_VM()->set_nethwlink(vm_index, req->nethwlink);
    GET_INSTANCE_CFG_VM()->set_nethwaddr(vm_index, req->nethwaddr);

    return sizeof(struct res_vmset_t);
}


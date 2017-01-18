#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_bdb_status.h"
#include "iopcops_cfg_bdb_vm.h"
#include "iopccmd_vmget.h"

uint32_t hn_vmget(uint8_t* preq, uint8_t* pres)
{
    struct req_vmget_t* req = (struct req_vmget_t*)preq;
    struct res_vmget_t* res = (struct res_vmget_t*)pres;
    uint16_t len = 0;
    uint8_t vm_count = 0;
    uint8_t vm_index = 0;
    uint8_t vm_name[VM_NAME_LEN];
    uint8_t vm_basepath[STR_LEN];
    uint8_t vm_nettype[VM_NETTYPE_LEN];
    uint8_t vm_nethwlink[VM_NETHWLINK_LEN];
    uint8_t vm_nethwaddr[VM_NETHWADDR_LEN];
    res->status = 0;
    vm_index = req->index;

    vm_count = GET_INSTANCE(ops_cfg_bdb_vm)->get_cfg_size();
    if(vm_index >= vm_count) {
	    res->status = 1;
	    return sizeof(struct res_vmget_t);
    }

    memset(vm_name, 0, VM_NAME_LEN);
    memset(vm_basepath, 0, STR_LEN);
    memset(vm_nettype, 0, VM_NETTYPE_LEN);
    memset(vm_nethwlink, 0, VM_NETHWLINK_LEN);
    memset(vm_nethwaddr, 0, VM_NETHWADDR_LEN);

    res->index = vm_index;
    res->auto_start = GET_INSTANCE(ops_cfg_bdb_vm)->get_autostart(vm_index);
    len = GET_INSTANCE(ops_cfg_bdb_vm)->get_name(vm_index, &vm_name[0]);
    memcpy(res->name, vm_name, len);
    len = GET_INSTANCE(ops_cfg_bdb_vm)->get_base_path(vm_index, &vm_basepath[0]);
    memcpy(res->base_path, vm_basepath, len);
    len = GET_INSTANCE(ops_cfg_bdb_vm)->get_nettype(vm_index, &vm_nettype[0]);
    memcpy(res->nettype, vm_nettype, len);
    len = GET_INSTANCE(ops_cfg_bdb_vm)->get_nethwlink(vm_index, &vm_nethwlink[0]);
    memcpy(res->nethwlink, vm_nethwlink, len);
    len = GET_INSTANCE(ops_cfg_bdb_vm)->get_nethwaddr(vm_index, &vm_nethwaddr[0]);
    memcpy(res->nethwaddr, vm_nethwaddr, len);

    return sizeof(struct res_vmget_t);
}


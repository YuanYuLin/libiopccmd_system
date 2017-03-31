#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "iopcdefine.h"

#define DEFINE_EXTERN_FN(NAME) extern uint32_t NAME(uint8_t* preq, uint8_t* pres)
DEFINE_EXTERN_FN(hn_mkbtrfs);
DEFINE_EXTERN_FN(hn_mkbtrfs_status);
DEFINE_EXTERN_FN(hn_mntbase);
DEFINE_EXTERN_FN(hn_mntbasecount);
DEFINE_EXTERN_FN(hn_mntbaseget);
DEFINE_EXTERN_FN(hn_mntbaseset);
DEFINE_EXTERN_FN(hn_netdev);
DEFINE_EXTERN_FN(hn_netssh);
DEFINE_EXTERN_FN(hn_netntp);
DEFINE_EXTERN_FN(hn_nethttp);
DEFINE_EXTERN_FN(hn_vmcontainer);
DEFINE_EXTERN_FN(hn_vmcount);
DEFINE_EXTERN_FN(hn_vmget);
DEFINE_EXTERN_FN(hn_vmset);
DEFINE_EXTERN_FN(hn_vmadd);
DEFINE_EXTERN_FN(hn_raiddev);
DEFINE_EXTERN_FN(hn_raiddevcount);
DEFINE_EXTERN_FN(hn_raiddevget);
DEFINE_EXTERN_FN(hn_raiddevset);
/*
DEFINE_EXTERN_FN(hn_db_setbool_f);
DEFINE_EXTERN_FN(hn_db_getbool_f);
DEFINE_EXTERN_FN(hn_db_setuint32_f);
DEFINE_EXTERN_FN(hn_db_getuint32_f);
DEFINE_EXTERN_FN(hn_db_setstring_f);
DEFINE_EXTERN_FN(hn_db_getstring_f);
DEFINE_EXTERN_FN(hn_db_save2persist);
DEFINE_EXTERN_FN(hn_db_setbool_r);
DEFINE_EXTERN_FN(hn_db_getbool_r);
DEFINE_EXTERN_FN(hn_db_setuint32_r);
DEFINE_EXTERN_FN(hn_db_getuint32_r);
DEFINE_EXTERN_FN(hn_db_setstring_r);
DEFINE_EXTERN_FN(hn_db_getstring_r);
DEFINE_EXTERN_FN(hn_db_restore2default);
*/
struct cmd_desc_t cmd_descs[] = {
    { "mkbtrfs"			,NULL, hn_mkbtrfs		},
    { "mkbtrfs_status"		,NULL, hn_mkbtrfs_status	},

    { "mntbase"			,NULL, hn_mntbase		},
    { "mntbasecount"		,NULL, hn_mntbasecount		},
    { "mntbaseget"		,NULL, hn_mntbaseget		},
    { "mntbaseset"		,NULL, hn_mntbaseset		},

    { "netdev"			,NULL, hn_netdev		},
    { "netssh"			,NULL, hn_netssh		},
    { "netntp"			,NULL, hn_netntp		},
    { "nethttp"			,NULL, hn_nethttp		},

    { "vmcontainer"		,NULL, hn_vmcontainer		},
    { "vmcount"			,NULL, hn_vmcount		},
    { "vmget"			,NULL, hn_vmget			},
    { "vmset"			,NULL, hn_vmset			},
    { "vmadd"			,NULL, hn_vmadd			},

    { "raiddev"			,NULL, hn_raiddev		},
    { "raiddevcount"		,NULL, hn_raiddevcount		},
    { "raiddevget"		,NULL, hn_raiddevget		},
    { "raiddevset"		,NULL, hn_raiddevset		},
/*
    { "db_setbool_f"		,NULL, hn_db_setbool_f		},
    { "db_getbool_f"		,NULL, hn_db_getbool_f		},
    { "db_setuint32_f"		,NULL, hn_db_setuint32_f	},
    { "db_getuint32_f"		,NULL, hn_db_getuint32_f	},
    { "db_setstring_f"		,NULL, hn_db_setstring_f	},
    { "db_getstring_f"		,NULL, hn_db_getstring_f	},
    { "db_save2persist"		,NULL, hn_db_save2persist	},

    { "db_setbool_r"		,NULL, hn_db_setbool_r		},
    { "db_getbool_r"		,NULL, hn_db_getbool_r		},
    { "db_setuint32_r"		,NULL, hn_db_setuint32_r	},
    { "db_getuint32_r"		,NULL, hn_db_getuint32_r	},
    { "db_setstring_r"		,NULL, hn_db_setstring_r	},
    { "db_getstring_r"		,NULL, hn_db_getstring_r	},
    { "db_restore2default"	,NULL, hn_db_restore2default	},
*/
    { ""			,NULL, NULL			}
};


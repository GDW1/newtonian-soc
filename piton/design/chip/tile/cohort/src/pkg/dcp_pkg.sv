`include "dcp.h"
// TODO: add documentation and make sure it's aligned with pmesh and tri
package dcp_pkg;
    typedef  logic [      `MSG_TYPE_WIDTH-1:0]  req_type_t;
    typedef  logic [    `DCP_MSHRID_WIDTH-1:0]  mshrid_t;
    typedef  logic [      `DCP_PADDR_MASK]      paddr_t;
    typedef  logic [     `DCP_UNPARAM_2_0]      size_t;
    typedef  logic [`PACKET_HOME_ID_WIDTH-1:0]  homeid_t;
    typedef  logic [  `MSG_AMO_MASK_WIDTH-1:0]  write_mask_t;
    typedef  logic [      `DCP_UNPARAM_63_0]    data_t;
endpackage : dcp_pkg

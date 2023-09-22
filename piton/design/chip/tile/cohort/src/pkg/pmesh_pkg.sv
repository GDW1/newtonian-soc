package pmesh_pkg;
	`include "dcp.h"
    typedef  logic [      `MSG_TYPE_WIDTH-1:0]  req_type_t;
    typedef  logic [    `DCP_MSHRID_WIDTH-1:0]  mshrid_t;
    typedef  logic [      `DCP_PADDR_MASK]      paddr_t;
    typedef  logic [     `DCP_UNPARAM_2_0]      size_t;
    typedef  logic [`PACKET_HOME_ID_WIDTH-1:0]  homeid_t;
    typedef  logic [  `MSG_AMO_MASK_WIDTH-1:0]  write_mask_t;
    typedef  logic [      `DCP_UNPARAM_63_0]    data_t;
    typedef  logic [`DCP_NOC_RES_DATA_SIZE-1:0] resp_data_t;

    // input and output are defined in the context of a client to the bus
	typedef struct packed {
		logic        valid     ;
		req_type_t   req_type  ;
		mshrid_t     mshrid    ;
		paddr_t      address   ;
		// real size = size * 8 bytes
		size_t       size      ;
		homeid_t     homeid    ;
		write_mask_t write_mask;
		data_t       data_0    ;
		data_t       data_1    ;
	} pmesh_noc2_o_t;

	typedef struct packed {
		logic ready;
	} pmesh_noc2_i_t;

	typedef struct packed {
		logic valid;
		mshrid_t mshrid;
		resp_data_t resp_data;
	} pmesh_noc3_in_t;
	
endpackage : pmesh_pkg
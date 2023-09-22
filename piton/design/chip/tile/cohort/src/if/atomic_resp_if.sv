import dcp_pkg::*;
interface atomic_resp_if (
    input clk,
    input rst_n
);
    logic valid;
    mshrid_t mshrid;
    logic [`DCP_NOC_RES_DATA_SIZE-1:0] data;

    modport slave(
        input valid, mshrid, data
    );

    modport master(
        output valid, mshrid, data
    );
endinterface : atomic_resp_if

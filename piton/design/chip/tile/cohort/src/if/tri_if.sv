`include "assert_macros.svh"
interface tri_if (
    input clk,
    input rst_n
);
    // this is the transducer_l15_* signals
    logic req_valid;
    tri_pkg::l15_reqtypes_t req_type;
    logic [3:0] req_amo_op;
    logic [2:0] req_size;
    logic [39:0] req_addr;
    logic [127:0] req_data;
    logic req_ack;

    // these are response signals
    logic resp_val;
    tri_pkg::l15_rtrntypes_t resp_type;
    logic resp_atomic;
    logic [127:0] resp_data;
    logic [15:4] resp_inv_addr;
    logic resp_inv_valid;
    logic resp_ack;

    modport master (
        output req_valid, req_type,
        req_amo_op, req_size, req_addr, req_data,
        resp_ack,
        input resp_val, resp_type, resp_data, resp_inv_addr,
        resp_inv_valid, resp_atomic,
        req_ack
    );

    modport slave (
        input req_valid, req_type,
        req_amo_op, req_size, req_addr, req_data,
        resp_ack,
        output resp_val, resp_type, resp_data, resp_inv_addr,
        resp_inv_valid, resp_atomic,
        req_ack
    );

    //TODO: bind me
    // change both to ready valid interface
    //`fpv_ready_valid_if(req_valid, req_ack, {req_type, req_amo_op, req_size, req_ack, req_data})
    //`fpv_ready_valid_if(resp_val, resp_ack, {resp_type, resp_data})


endinterface : tri_if

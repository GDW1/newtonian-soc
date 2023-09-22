`include "assert_macros.svh"
interface mem_req_if (
    input logic clk,
    input logic rst_n
);

    import dcp_pkg::*;
    logic        ready     ;
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


    modport master (
        input ready,
        output valid, req_type, mshrid, address, size, homeid, write_mask, data_0, data_1
    );

    modport slave (
        output ready,
        input valid, req_type, mshrid, address, size, homeid, write_mask, data_0, data_1
    );


        `assert_hold_valid({req_type, mshrid, address, size, homeid, write_mask, data_0, data_1}, valid, ready, "memory request should hold valid")

        `assert_finish_handshake(valid, ready, {req_type, mshrid, address, size, homeid, write_mask, data_0, data_1}, "memory request should finish handshake")

`ifndef SYNTHESIS
    task initialize();
        ready = 1'b0;
    endtask : initialize

    task check_output(input paddr_t ref_addr);
        wait(valid == 1);
        if (address != ref_addr) begin :check_address
            $fatal(1, "Output address is not equal to reference address");
        end
    endtask : check_output
`endif


endinterface : mem_req_if

`include "dcp.h"
module mem_parse (
    input clk,    // Clock
    input rst_n,  // Asynchronous reset active low
    
    mem_req_if.master load_req,
    atomic_resp_if.slave atomic_resp
);
    /* in this simplified state, there is no communication between
       mem_parse and mem_controller
       If we want to ignore some request, simply de-assert the ack
       this will simply things a great deal.
       Assuming all responses are from load requests, we
       load things from the load request, and then return the load
       to some other locaton
       The current fsm implementation requires that there are no
       outstanding requests from mem_controller
       We could allocate different mshr id to mitigate this problem
       TODO:
       - make this thing into a fifo
       - communication with mem_controller
       - signal to the cpu that we're done
      */


    typedef enum logic [1:0] {S_IDLE, S_REQ, S_RESP, S_DONE} state_t;
    state_t state, state_d;
    paddr_t addr_d, addr;
    assign load_req.valid = state_d == S_REQ;
    //FIXME: fix corrent type and size
    assign load_req.req_type = `MSG_TYPE_STORE_REQ;
    //FIXME: fix load request size to 8 bytes now
    assign load_req.size     = 2'h1;
    assign load_req.address  = addr_d;

    //FIXME: get the noc stuff right
    assign load_req.mshrid = 8'd145;
    assign load_req.homeid = '0;
    assign load_req.write_mask = '1;
    assign load_req.data_0 = 32'hdeadbeef;
    assign load_req.data_1 = 32'hbeefdead;

    always_ff @(posedge clk) begin
        if(~rst_n) begin
            state_d <= S_IDLE;
        end else begin
            state_d <= state;
        end
    end

    always_comb begin
        unique case (state_d)
            S_IDLE  : state = (atomic_resp.valid) ? S_REQ : S_IDLE;
            S_REQ   : state = ((load_req.valid & load_req.ready)) ? S_RESP : S_REQ;
            S_RESP  : state = (atomic_resp.valid) ? S_DONE : S_RESP;
            S_DONE  : state = S_IDLE;
            default : ;
        endcase // state_d
    end

    always_ff @(posedge clk) begin
        if(~rst_n) begin
            addr_d <= 0;
        end else begin
            addr_d <= addr;
        end
    end

    assign addr = (atomic_resp.valid) ? atomic_resp.data : addr_d;


endmodule

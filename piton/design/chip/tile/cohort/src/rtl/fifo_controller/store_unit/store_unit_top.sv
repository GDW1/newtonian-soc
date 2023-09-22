module store_unit_top
	import tri_pkg::*;
(
	input clk,    // Clock
	input rst_n,  // Asynchronous reset active low
	input fifo_ctrl_pkg::transact_o_t transact_i,
	output fifo_ctrl_pkg::transact_i_t transact_o,
	output tri_pkg::tri_req_t tri_req,
	input tri_pkg::tri_resp_t tri_resp	
);
	// always store, ignore invalidation
	// pipe the input manually and use arbiter to split the queue before this
	// this is to maximize the threadid concurrency

	//TODO: aligh the size encoding between pmesh and tri interface
	//TODO: add back threadid
	typedef enum logic [1:0] {S_IDLE, S_REQ, S_RESP} state_t;
	state_t state, state_next;

	fifo_ctrl_pkg::transact_o_t transact_d, transact_q;

	assign transact_o.ready = state == S_IDLE;
	assign tri_req.req_valid = state == S_REQ;
	assign tri_req.req_type = TRI_STORE_RQ;
	//FIXME: fix the correct size conversion
	assign tri_req.req_size = 3'h1;
	assign tri_req.req_addr = transact_q.addr;
	assign tri_req.req_data = transact_q.data;
	assign tri_req.resp_ack = state == S_RESP;
	assign tri_req.req_amo_op = '0;

	always_ff @(posedge clk) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			transact_q <= '0;
		end else begin
			state <= state_next;
			transact_q <= transact_d;
		end
	end
	
	always_comb begin : proc_state_next
		state_next = state;
		transact_d = transact_q;
		case (state)
			S_IDLE  : begin 
				if (transact_i.valid && transact_o.ready) begin 
					state_next = S_REQ;
					transact_d = transact_i;
				end
			end
			S_REQ   : begin 
				if (tri_req.req_valid && tri_resp.req_ack) begin 
					state_next = S_IDLE; // Don't wait for ack for now
				end
			end
			S_RESP  : begin 
				if (tri_resp.resp_val && tri_req.resp_ack) begin 
					state_next = S_IDLE;
				end
			end
			default : ;
		endcase
	end

endmodule
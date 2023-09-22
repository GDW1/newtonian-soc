module req_to_noc2_adapter
	import fifo_ctrl_pkg::*;
	import pmesh_pkg::pmesh_noc2_o_t;
	import pmesh_pkg::pmesh_noc2_i_t;
(
	input clk,    // Clock
	input rst_n,  // Asynchronous reset active low
	input ld_req_i_t ld_req_i,
	output ld_req_o_t ld_req_o,
	input pmesh_pkg::pmesh_noc2_i_t noc2_i,
	output pmesh_pkg::pmesh_noc2_o_t noc2_o
);
	typedef enum logic {S_IDLE, S_REQ} state_t;
	state_t state, state_next;
	addr_t addr_d, addr_q;
	size_t size_d, size_q;

	assign ld_req_o.ready = state == S_IDLE;
	assign noc2_o.valid = state == S_REQ;
	assign noc2_o.req_type = 6'd60;
	//TODO: add mshrid tracking to build a fetch queue
	assign noc2_o.mshrid = ld_req_i.mshrid;
	assign noc2_o.address = addr_q;
	assign noc2_o.size = size_q;
	//TODO: check for correct homeid
	assign noc2_o.homeid = '0;
	assign noc2_o.write_mask = '0;
	assign noc2_o.data_0 = '0;
	assign noc2_o.data_1 = '0;

	always_ff @(posedge clk) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			addr_q <= '0;
			size_q <= '0;
		end else begin
			state <= state_next;
			addr_q <= addr_d;
			size_q <= size_d;
		end
	end
	
	always_comb begin : proc_state_next
		state_next = state;
		addr_d = addr_q;
		size_d = size_q;
		unique case (state)
			S_IDLE  : begin 
				if (ld_req_i.valid) begin 
					state_next = S_REQ;
					addr_d = ld_req_i.addr;
					size_d = ld_req_i.size;
				end
			end
			S_REQ   : begin 
				if (noc2_i.ready) begin 
					state_next = S_IDLE;
				end
			end
		endcase
	end

endmodule : req_to_noc2_adapter

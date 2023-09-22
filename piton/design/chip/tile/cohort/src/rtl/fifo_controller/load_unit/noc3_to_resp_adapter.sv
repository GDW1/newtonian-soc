module noc3_to_resp_adapter
	import pmesh_pkg::pmesh_noc3_in_t;
	import fifo_ctrl_pkg::*;
(
	input clk,    // Clock
	input rst_n,  // Asynchronous reset active low
	input pmesh_pkg::pmesh_noc3_in_t noc3_in,
	output ld_resp_o_t ld_resp_o
);

	typedef enum logic {S_IDLE, S_REQ} state_t;

	state_t state, state_next;
	cacheline_t data_d, data_q;
	mshrid_t mshrid_d, mshrid_q;

	assign ld_resp_o.data = data_q;
	assign ld_resp_o.mshrid = mshrid_q;
	assign ld_resp_o.valid = state == S_REQ;

	always_ff @(posedge clk) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			data_q <= '0;
			mshrid_q <= '0;
		end else begin
			state <= state_next;
			data_q <= data_d;
			mshrid_q <= mshrid_d;
		end
	end
	
	always_comb begin : proc_state_next
		state_next = state;
		data_d = data_q;
		mshrid_d = mshrid_q;
		unique case (state)
			S_IDLE  : begin 
				if (noc3_in.valid) begin
					state_next = S_REQ;
					data_d = noc3_in.resp_data;
					mshrid_d = noc3_in.mshrid;
				end
			end
			S_REQ   : begin 
					state_next = S_IDLE;
			end
		endcase
	end

endmodule

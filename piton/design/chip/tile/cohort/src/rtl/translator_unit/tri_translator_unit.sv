module tri_translator_unit (
	input clk,    // Clock
	input rst_n,  // Asynchronous reset active low
	tri_if.slave tri_req_vpn,
	tri_if.master tri_req_ppn,
	tlb_if.master tlb_req	
);

	// idle: nothing
	// req: request the translation
	// resp: the translation arrives

    import translator_pkg::pn_t;
	typedef enum logic [1:0] {S_IDLE, S_QUERY, S_PASSTHROUGH} state_t;

    pn_t req_addr_ppn_r, req_addr_ppn_n;
	

	state_t state, state_next;


	assign tri_req_ppn.req_type = tri_req_vpn.req_type;
	assign tri_req_ppn.req_amo_op = tri_req_vpn.req_amo_op;
	assign tri_req_ppn.req_size = tri_req_vpn.req_size;
	assign tri_req_ppn.req_data = tri_req_vpn.req_data;
	assign tri_req_ppn.resp_ack = tri_req_vpn.resp_ack;
	assign tri_req_ppn.req_addr = {req_addr_ppn_r, tri_req_vpn.req_addr[11:0]};
	assign tri_req_ppn.req_valid = state == S_PASSTHROUGH;
	assign tri_req_vpn.req_ack = state == S_PASSTHROUGH & tri_req_ppn.req_ack;
	assign tri_req_vpn.resp_val = tri_req_ppn.resp_val;
	assign tri_req_vpn.resp_type = tri_req_ppn.resp_type;
	assign tri_req_vpn.resp_data = tri_req_ppn.resp_data;
	assign tri_req_vpn.resp_inv_addr = tri_req_ppn.resp_inv_addr;
	assign tri_req_vpn.resp_inv_valid = tri_req_ppn.resp_inv_valid;

	assign tlb_req.vpn = tri_req_vpn.req_addr >> 12;
	assign tlb_req.valid = state == S_QUERY;


	always_ff @(posedge clk) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			req_addr_ppn_r <= '0;
		end else begin
			state <= state_next;
			req_addr_ppn_r <= req_addr_ppn_n;
		end
	end
	
	always_comb begin : proc_state_next
		state_next = state;
		req_addr_ppn_n = req_addr_ppn_r;
		case (state)
			S_IDLE  : begin
				if (tri_req_vpn.req_valid) begin
					state_next = S_QUERY;
				end
			end
			S_QUERY : begin
				if (tlb_req.ack) begin
					state_next = S_PASSTHROUGH;
					req_addr_ppn_n = tlb_req.ppn ; // the upper 12 bits
				end
			end
			S_PASSTHROUGH  : begin
				if (tri_req_ppn.req_valid & tri_req_ppn.req_ack) begin
					state_next = S_IDLE;
				end
			end
			default : ;
		endcase
	end	

endmodule : tri_translator_unit

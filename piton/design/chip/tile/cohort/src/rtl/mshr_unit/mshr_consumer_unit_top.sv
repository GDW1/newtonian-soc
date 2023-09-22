// this is the top unit for mshr of consumer unit
module mshr_consumer_unit_top (
	input                             clk       , // Clock
	input                             rst_n     , // Asynchronous reset active low
	decoupled_vr_if.slave            trans_info,
	decoupled_vr_if.master             recv_info ,
	input fifo_config_pkg::fifo_config_t fifo_config_r,
	input  pmesh_pkg::pmesh_noc2_i_t  noc2_i    ,
	output pmesh_pkg::pmesh_noc2_o_t  noc2_o    ,
	input  pmesh_pkg::pmesh_noc3_in_t noc3_in
);
	import fifo_ctrl_pkg::cacheline_t;

	typedef enum logic [1:0] {S_IDLE, S_WAIT, S_ACK} state_t;

	state_t state, state_next;

	fifo_ctrl_pkg::ld_req_i_t ld_req_i;
	fifo_ctrl_pkg::ld_req_o_t ld_req_o;

	cacheline_t data_r, data_n;

	always_ff @(posedge clk) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			data_r <= '0;
		end else begin
			state <= state_next;
			data_r <= data_n;
		end
	end

	always_comb begin : proc_state_next
		state_next = state;
		unique case (state)
			S_IDLE  : state_next = trans_info.valid & trans_info.ready ? S_WAIT : S_IDLE;
			S_WAIT  : state_next = ld_resp_o.valid ? S_ACK : S_WAIT;
			S_ACK   : state_next = recv_info.valid & recv_info.ready ? S_IDLE : S_ACK;
		endcase
	end;

	always_comb begin : proc_data
		data_n = data_r;
		if (state == S_WAIT & ld_resp_o.valid) begin
			data_n = ld_resp_o.data;
		end
	end


	assign trans_info.ready = ld_req_o.ready & state == S_IDLE;

	assign ld_req_i.valid  = trans_info.valid & state == S_IDLE;
	assign ld_req_i.size   = 3'b101; // Load 128 bits (a full cacheline), and deserialize to 2 transactions
	assign ld_req_i.mshrid = 8'd148; // TODO: check if this is true
	assign ld_req_i.addr   = trans_info.data;

	assign recv_info.valid = state == S_ACK;
	assign recv_info.data = data_r;

	// make sure that they're of the same width
	// assert($bits(ld_req_i.addr) == $bits(trans_info.data));


	req_to_noc2_adapter i_req_to_noc2_adapter (
		.clk     (clk     ),
		.rst_n   (rst_n   ),
		.ld_req_i(ld_req_i),
		.ld_req_o(ld_req_o),
		.noc2_i  (noc2_i  ),
		.noc2_o  (noc2_o  )
	);


	fifo_ctrl_pkg::ld_resp_o_t ld_resp_o;

	
	noc3_to_resp_adapter i_noc3_to_resp_adapter (
		.clk      (clk      ),
		.rst_n    (rst_n    ),
		.noc3_in  (noc3_in  ),
		.ld_resp_o(ld_resp_o)
	);



endmodule : mshr_consumer_unit_top

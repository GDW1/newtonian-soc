`include "dcp.h"
module mshr_noc_unit #(parameter int unsigned source_num = 4) (
	input clk  , // Clock
	input rst_n, // Asynchronous reset active low
	mem_req_if.slave mem_req_source[source_num-1:0],
	mem_req_if.master mem_req_sink,
	atomic_resp_if.slave atomic_resp_source,
	atomic_resp_if.master atomic_resp_sink[source_num-1:0]
);

	import dcp_pkg::*;

	typedef enum logic [1:0] {S_IDLE, S_REQ, S_RESP, S_U='x} state_t;

	logic [$clog2(source_num):0] index_r, index_n; // index for the arbiter

	state_t state, state_next;

	logic [$clog2(source_num):0] encoder_index;
	logic [source_num-1:0] elements;
	logic encoder_onehot;
	
	priority_encoder #(.NumberOfElement(source_num)) i_priority_encoder (
		.elements (elements      ),
		.index    (encoder_index ),
		.is_onehot(encoder_onehot)
	);

	assign mem_req_sink.valid = state == S_REQ;

	mshrid_t mshrid_r, mshrid_n, mshrid_sel;

    req_type_t   req_type_w  [source_num-1:0];
    mshrid_t     mshrid_w    [source_num-1:0];
    paddr_t      address_w   [source_num-1:0];
    size_t       size_w      [source_num-1:0];
    homeid_t     homeid_w    [source_num-1:0];
    write_mask_t write_mask_w[source_num-1:0];
    data_t       data_0_w    [source_num-1:0];
    data_t       data_1_w    [source_num-1:0];


	generate
		for (genvar i = 0; i < source_num; i++) begin : generate_source
			assign elements[i] = mem_req_source[i].valid;
			assign atomic_resp_sink[i].data = atomic_resp_source.data;
			assign atomic_resp_sink[i].mshrid = atomic_resp_source.mshrid;
			assign atomic_resp_sink[i].valid = (index_r == i) && atomic_resp_source.valid && (state == S_RESP) && (atomic_resp_source.mshrid == mshrid_r);
			assign mem_req_source[i].ready = mem_req_sink.ready && (index_r == i) && (state == S_REQ);


			// wires
		assign req_type_w   [i]  = mem_req_source[i].req_type  ;
		assign mshrid_w     [i]  = mem_req_source[i].mshrid    ;
		assign address_w    [i]  = mem_req_source[i].address   ;
		assign size_w       [i]  = mem_req_source[i].size      ;
		assign homeid_w     [i]  = mem_req_source[i].homeid    ;
		assign write_mask_w [i]  = mem_req_source[i].write_mask;
		assign data_0_w     [i]  = mem_req_source[i].data_0    ;
		assign data_1_w     [i]  = mem_req_source[i].data_1    ;

		end
	endgenerate

	always_comb begin
		mshrid_sel = 'X;
		mem_req_sink.req_type   = 'X;
		mem_req_sink.mshrid     = 'X;
		mem_req_sink.address    = 'X;
		mem_req_sink.size       = 'X;
		mem_req_sink.homeid     = 'X;
		mem_req_sink.write_mask = 'X;
		mem_req_sink.data_0     = 'X;
		mem_req_sink.data_1     = 'X;
		for (int i = 0; i < source_num; i++) begin
			if (encoder_index == i) begin
				mshrid_sel = mshrid_w[i];
			end
			if (index_r == i) begin
				mem_req_sink.req_type   = req_type_w   [i];
				mem_req_sink.mshrid     = mshrid_w     [i];
				mem_req_sink.address    = address_w    [i];
				mem_req_sink.size       = size_w       [i];
				mem_req_sink.homeid     = homeid_w     [i];
				mem_req_sink.write_mask = write_mask_w [i];
				mem_req_sink.data_0     = data_0_w     [i];
				mem_req_sink.data_1     = data_1_w     [i];
			end
		end
	end


	always_ff @(posedge clk) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			index_r <= '0;
			mshrid_r <= 'x;
		end else begin
			state <= state_next;
			index_r <= index_n;
			mshrid_r <= mshrid_n;
		end
	end


	always_comb begin : proc_state_next
		state_next = state;
		index_n = index_r;
		mshrid_n = mshrid_r;
		unique case (state)
			S_IDLE       : begin
				if (encoder_onehot) begin
					state_next = S_REQ;
					index_n = encoder_index;
					mshrid_n = mshrid_sel;
				end
			end
			S_REQ   : begin
				if (mem_req_sink.ready && mem_req_sink.valid) begin
					state_next = S_RESP;
				end
			end
			S_RESP  : begin
				if (atomic_resp_source.valid) begin
					state_next = S_IDLE; // always ack immediately
				end
			end
		endcase
	end	

endmodule

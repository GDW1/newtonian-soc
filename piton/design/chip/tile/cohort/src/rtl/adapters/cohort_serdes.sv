// this should only act as the top level wrapper for acc
// the data parts are completely decoupled now without address information
// this is dummy and the data width can be adjusted at the higher level
module cohort_serdes #(
	parameter integer serialization_ratio = 1,
	parameter integer deserialization_ratio = 1
	) (
	input                       clk                  , // Clock
	input                       rst_n                , // Asynchronous reset active low
	decoupled_vr_if.slave       input_data        ,
	decoupled_vr_if.master      output_data
);
	localparam inputwidth = $bits(input_data.data);
	localparam outputwidth = $bits(output_data.data);

	logic [15:0] s_counter_r;
	logic [15:0] s_counter_n;
	logic [15:0] d_counter_r;
	logic [15:0] d_counter_n;

	initial begin
		assert (inputwidth*serialization_ratio == outputwidth*deserialization_ratio) else $fatal("Width Mismatch");
	end

	logic [inputwidth*serialization_ratio - 1: 0] intermediate_data;
	logic [inputwidth*serialization_ratio - 1: 0] intermediate_data_next;

	typedef enum logic [1:0] {S_IDLE, S_SERIALIZE,  S_DESERIALIZE} state_t;

	state_t state, state_next; 

	always_ff @(posedge clk) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			s_counter_r <= '0;
			d_counter_r <= '0;
			intermediate_data <= '0;
		end else begin
			state <= state_next;
			s_counter_r <= s_counter_n;
			d_counter_r <= d_counter_n;
			intermediate_data <= intermediate_data_next;
		end
	end

	assign output_data.valid = state == S_DESERIALIZE;
	assign input_data.ready = state == S_SERIALIZE;

	always_comb begin : proc_state_next
		state_next  = state;
		s_counter_n = s_counter_r;
		d_counter_n = d_counter_r;
		unique case (state)
			S_IDLE : begin
				if (input_data.valid) begin
					state_next  = S_SERIALIZE;
					s_counter_n = '0;
					d_counter_n = '0;
				end
			end
			S_SERIALIZE : begin
				if (input_data.ready & input_data.valid) begin
					s_counter_n = s_counter_r + 1;
				end
				if (s_counter_n == serialization_ratio) begin
					state_next = S_DESERIALIZE;
				end
			end
			S_DESERIALIZE : begin
				if (output_data.ready & output_data.valid) begin
					d_counter_n = d_counter_r + 1;
				end
				if (d_counter_n == deserialization_ratio) begin
					state_next = S_IDLE;
				end
			end
		endcase
	end

	always_comb begin
		intermediate_data_next = intermediate_data;
		for (int i = 0; i < serialization_ratio; i++) begin
			if (i == s_counter_r && state == S_SERIALIZE && input_data.ready && input_data.valid) begin
				intermediate_data_next[i*inputwidth+:inputwidth] = input_data.data;
			end
		end
		for (int i = 0; i < deserialization_ratio; i++) begin
			if (i == d_counter_r && state == S_DESERIALIZE) begin
				output_data.data = intermediate_data[i*outputwidth +: outputwidth];
			end
		end
	end


endmodule : cohort_serdes

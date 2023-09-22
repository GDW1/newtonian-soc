// this module generates the addresses for the actual load module and store module.
// what it does is that it **DOES NOT MAINTAIN ANY STATE**! But instead, it will look
// at the data it's been given each time, and **BLOCK ON THOSE** until it finishes.

module addr_gen
	import fifo_ctrl_pkg::*;
(
	input logic clk,    // Clock
	input logic rst_n,  // Asynchronous reset active low
	input fifo_cfg_o_t cfg_i,
	output fifo_cfg_i_t cfg_o,
	output addr_gen_o_t addr_gen_o,
	input addr_gen_i_t addr_gen_i
);
	
	typedef enum logic [1:0] {S_IDLE, S_PIPE, S_ACK} state_t;
	state_t state_r, state_r_next, state_n;

	addr_t addr_r, addr_n;
	// As head pointer for the fifo doesn't change in this duration,
	// we'll just always assign it.
	ptr_t head_ptr;
	assign head_ptr = cfg_i.head_ptr;
	size_t element_size = cfg_i.size;
	ptr_t tail_ptr_r, tail_ptr_n;

	assign tail_ptr_o = tail_ptr_r;

	wire addr_gen_handshake = addr_gen_o.valid & addr_gen_i.ready;
	wire fifo_complete = head_ptr == tail_ptr_r;
	wire tail_wrap = tail_ptr_r == cfg_i.depth;

	assign cfg_o.ack = state_r == S_ACK;
	assign addr_gen_o.addr = addr_r;
	//TODO: size check
	assign addr_gen_o.size = cfg_i.size;
	assign addr_gen_o.valid = ((state_r == S_PIPE) && !fifo_complete);

	always_ff @(posedge clk) begin : proc_state_r
		if(~rst_n) begin
			state_r <= S_IDLE;
			tail_ptr_r <= '0;
			addr_r <= '0;
		end else begin
			state_r <= state_n;
			tail_ptr_r <= tail_ptr_n;
			addr_r <= addr_n;
		end
	end
	
	always_comb begin : proc_state_n
		state_n = state_r;
		tail_ptr_n = tail_ptr_r;
		addr_n = addr_r;
		unique case (state_r)
			S_IDLE  : begin
				if (cfg_i.valid) begin
					state_n = S_PIPE;
					addr_n = cfg_i.base + element_size * cfg_i.tail_ptr;
					tail_ptr_n = cfg_i.tail_ptr;
				end
			end
			S_PIPE  : begin
				state_n = fifo_complete ? S_ACK : S_PIPE;
				if (addr_gen_handshake) begin
					tail_ptr_n = tail_wrap ? 1'b0 : tail_ptr_r + 1'b1;
					addr_n = addr_r + element_size;
				end
			end
			S_ACK   : state_n = S_IDLE;
		endcase
	end : proc_state_n

endmodule

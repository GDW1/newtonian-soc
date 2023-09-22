// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright (c) 2022 Tianrui Wei
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the authors nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

module aes_top (
	input                       clk          , // Clock
	input                       rst_n        , // Asynchronous reset active low
	input acc_pkg::acc_config_t acc_config   , // acclerator uncached configuration interface
	decoupled_vr_if.slave       consumer_data,
	decoupled_vr_if.master      producer_data
);
	
	//TODO: do we need configurable width?
	// nope. Can fix it at compilation time

	typedef enum logic [2:0] {S_IDLE, S_CONSUME, S_PUT, S_GET, S_PRODUCE} state_t;

	logic [63:0] data_n;

	state_t state, state_next;

	logic [$clog2(128) - 1 : 0] counter_n, counter_r;

	logic [63:0] data_in [127:0];
	logic valid_out;

	assign consumer_data.ready = state == S_CONSUME;
	assign producer_data.valid = state == S_PRODUCE;
	assign producer_data.data = (state == S_PRODUCE) ? data_in[counter_r] : '0;

	always_ff @(posedge clk or negedge rst_n) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			counter_r <= '0;
		end else begin
			state <= state_next;
			counter_r <= counter_n;
			if (state == S_CONSUME) begin
				data_in[counter_r] <= data_n;
			end
		end
	end
	
	always_comb begin : proc_state_next
		state_next = state;
		counter_n = counter_r;
		case (state)
			S_IDLE    : begin
				if (consumer_data.valid) begin
					state_next = S_CONSUME;
					counter_n = 0;
				end
			end
			S_CONSUME : begin
				if (consumer_data.valid & consumer_data.ready) begin
					data_n = consumer_data.data;
					counter_n = counter_r + 1;
					if (counter_r == 127) begin
						state_next = S_PUT;
					end
				end
			end
			S_PUT: begin
				state_next = S_GET;
			end
			S_GET: begin
				// if (valid_out) begin
				state_next = S_PRODUCE;
				// data_lower_n = plain_text[63:0];
				// data_higher_n = plain_text[127:64];
				counter_n = 0;
				// end
			end
			S_PRODUCE : begin
				if (producer_data.valid & producer_data.ready) begin
					counter_n = counter_r + 1;
					if (counter_r == 127) begin
						state_next = S_IDLE;
					end
				end
			end
		endcase
	end	



endmodule : aes_top

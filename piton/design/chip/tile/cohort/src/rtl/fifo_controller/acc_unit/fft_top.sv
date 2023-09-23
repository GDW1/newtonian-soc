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

module fft_top #(parameter int number_inputs = 128, parameter int number_outputs = 128, parameter int stage = 0) (
	input                       				clk          , // Clock
	input                       				rst_n        , // Asynchronous reset active low
	input acc_pkg::acc_config_t 				acc_config   , // acclerator uncached configuration interface
	decoupled_vr_if.slave       				consumer_data,
	decoupled_vr_if.master      				producer_data,
	data_forward_if.data_forward_in 		    data_forward_in,
	data_forward_if.data_forward_out 		    data_forward_out,
	input logic[2:0]							bypass_control

);
	
	//TODO: do we need configurable width?
	// nope. Can fix it at compilation time

	typedef enum logic [2:0] {S_IDLE, S_CONSUME, S_PUT, S_GET, S_PRODUCE} state_t;

	logic [63:0] data_n;

	state_t state, state_next;

	logic [$clog2(number_inputs) - 1 : 0] counter_n, counter_r;

	logic [63:0] data_in [number_inputs - 1:0]  ;
	logic [63:0] data_out [number_outputs - 1:0];
	
	logic valid_out;

	assign consumer_data.ready = state == S_CONSUME;
	assign producer_data.valid = state == S_PRODUCE;
	assign producer_data.data = (state == S_PRODUCE) ? data_out[counter_r] : '0;
	assign data_forward_in.rdy = state == S_PRODUCE;
	assign data_forward_in.data = data_out;

	always_ff @(posedge clk or negedge rst_n) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			counter_r <= '0;
		end else begin
			state <= state_next;
			counter_r <= counter_n;
			if (state == S_CONSUME) begin
				if (bypass_control[1] == 1'b1) begin
					data_in[counter_r] <= data_n;
				end else begin
					data_in <= data_forward_out.data;
				end
			end
		end
	end
	
	always_comb begin : proc_state_next
		state_next = state;
		counter_n = counter_r;
		case (state)
			S_IDLE    : begin
				if (consumer_data.valid | (~bypass_control[1] & data_forward_out.rdy) == 1'b1) begin
					state_next = S_CONSUME;
					counter_n = 0;
				end
			end
			S_CONSUME : begin
				if (bypass_control[1] == 1'b1) begin
					if (consumer_data.valid & consumer_data.ready) begin
						data_n = consumer_data.data;
						counter_n = counter_r + 1;
						if (counter_r == number_inputs - 1) begin
							state_next = S_PUT;
						end
					end
				end else begin
					state_next = S_PUT;
				end
			end
			S_PUT: begin
				state_next = S_GET;
			end
			S_GET: begin
				if (valid_out) begin
					state_next = S_PRODUCE;
					counter_n = 0;
				end
			end
			S_PRODUCE : begin
				if (bypass_control[0] == 1'b1) begin
					if (producer_data.valid & producer_data.ready) begin
						counter_n = counter_r + 1;
						if (counter_r == number_outputs - 1) begin
							state_next = S_IDLE;
						end
					end
				end else begin
					state_next = S_IDLE;
				end
			end
		endcase
	end	


	/**
		BEGIN Assign outputs using modules here. Additionally valid out should be assigned.
	**/

	logic [2048 - 1 : 0] fft_imag_out;
	logic [2048 - 1 : 0] fft_real_out;

	logic [2048 - 1 : 0] fft_real_in;
	logic [2048 - 1 : 0] fft_imag_in;

	generate
	genvar i;
	for (i = 0; i < number_inputs; i++) begin // assign fft_real_in to lower 32 bits of data_in[i] and fft_imag_in to upper 32 bits of data_in[i]
		if (i < 64) begin
			assign fft_real_in[i * 32 +: 32] = data_in[i][31:0];
			assign data_out[i] = fft_real_out[i * 32 +: 32];
		end else begin
			assign fft_imag_in[(i - 64) * 32 +: 32] = data_in[i][31:0];
			assign data_out[i] = fft_imag_out[(i - 64) * 32 +: 32];
		end
	end
	endgenerate

	assign valid_out = 1'b1;


	logic o_valid;
	logic o_ready;
	generate
	if (stage == 0) begin
		fft_64_stage_1_inner I_fft_64_stage_1_inner (
			.clk(clk),
			.rst(~rst_n),
			.i_data_in_real(fft_real_in),
			.i_data_in_imag(fft_imag_in),
			.o_data_out_real(fft_real_out),
			.o_data_out_imag(fft_imag_out),
			.i_valid(1'b1),
			.i_ready(1'b1),
			.o_valid(o_valid),
			.o_ready(o_ready)
		);
	end else if (stage == 1) begin
		fft_64_stage_2_inner I_fft_64_stage_2_inner (
			.clk(clk),
			.rst(~rst_n),
			.i_data_in_real(fft_real_in),
			.i_data_in_imag(fft_imag_in),
			.o_data_out_real(fft_real_out),
			.o_data_out_imag(fft_imag_out),
			.i_valid(1'b1),
			.i_ready(1'b1),
			.o_valid(o_valid),
			.o_ready(o_ready)

		);
	end else if (stage == 2) begin
		fft_64_stage_3_inner I_fft_64_stage_3_inner (
			.clk(clk),
			.rst(~rst_n),
			.i_data_in_real(fft_real_in),
			.i_data_in_imag(fft_imag_in),
			.o_data_out_real(fft_real_out),
			.o_data_out_imag(fft_imag_out),
			.i_valid(1'b1),
			.i_ready(1'b1),
			.o_valid(o_valid),
			.o_ready(o_ready)

		);
	end else if (stage == 3) begin
		fft_64_stage_4_inner I_fft_64_stage_4_inner (
			.clk(clk),
			.rst(~rst_n),
			.i_data_in_real(fft_real_in),
			.i_data_in_imag(fft_imag_in),
			.o_data_out_real(fft_real_out),
			.o_data_out_imag(fft_imag_out),
			.i_valid(1'b1),
			.i_ready(1'b1),
			.o_valid(o_valid),
			.o_ready(o_ready)

		);
	end else if (stage == 4) begin
		fft_64_stage_5_inner I_fft_64_stage_5_inner (
			.clk(clk),
			.rst(~rst_n),
			.i_data_in_real(fft_real_in),
			.i_data_in_imag(fft_imag_in),
			.o_data_out_real(fft_real_out),
			.o_data_out_imag(fft_imag_out),
			.i_valid(1'b1),
			.i_ready(1'b1),
			.o_valid(o_valid),
			.o_ready(o_ready)

		);
	end else if (stage == 5) begin
		fft_64_stage_6_inner I_fft_64_stage_6_inner (
			.clk(clk),
			.rst(~rst_n),
			.i_data_in_real(fft_real_in),
			.i_data_in_imag(fft_imag_in),
			.o_data_out_real(fft_real_out),
			.o_data_out_imag(fft_imag_out),
			.i_valid(1'b1),
			.i_ready(1'b1),
			.o_valid(o_valid),
			.o_ready(o_ready)

		);
	end

	endgenerate

	/**
		END Assign outputs using modules here. Additionally valid out should be assigned.
	**/


endmodule : fft_top

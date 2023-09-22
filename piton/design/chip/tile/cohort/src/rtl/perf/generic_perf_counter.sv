/*
3 input.
The counter only counts when toggle trigger is high
inc condition decides under which circumstance does it count
clear trigger clears the value to zero
and then there's always the classic output
Normally toggle and clear are toggled at chip level, whereas
inc condition is toggled at (sub) module level
*/

module generic_perf_counter (
	input clk,    // Clock
	input rst_n,  // Asynchronous reset active low
	input logic toggle_trigger,
	input logic inc_condition,
	input logic clear_trigger,
	output logic [63:0] perf_counter_value	
);
	import perf_pkg::counter_t;

	typedef enum logic [1:0] {S_IDLE, S_COUNTING, S_RESET} state_t;

	counter_t counter_r, counter_n;
	state_t state, state_next;

	assign perf_counter_value = counter_r;

	always_ff @(posedge clk or negedge rst_n) begin : proc_state
		if(~rst_n) begin
			state     <= S_IDLE;
			counter_r <= '0;
		end else begin
			state     <= state_next;
			counter_r <= counter_n;
		end
	end

	always_comb begin : proc_state_next
		state_next = state;
		counter_n  = counter_r;
		unique case (state)
			S_IDLE : begin
				if (toggle_trigger) begin
					state_next = S_COUNTING;
				end
			end
			S_COUNTING : begin
				counter_n = counter_r + (inc_condition & toggle_trigger);
				if (clear_trigger) begin
					state_next = S_RESET;
				end
			end
			S_RESET    : begin
				state_next = S_IDLE;
				counter_n = '0;
			end
		endcase
	end 
endmodule : generic_perf_counter

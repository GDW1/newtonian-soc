interface perf_if
	#(parameter int unsigned perf_reg_num = 1)
		(
			input			logic clk,
			input			logic rst_n
		);

		import perf_pkg::counter_t;

	logic clear_trigger;
	logic toggle_trigger;
	counter_t counter_r [perf_reg_num - 1: 0];

	modport slave (
		input clear_trigger, toggle_trigger,
		output counter_r
	);

	modport master (
		output clear_trigger, toggle_trigger,
		input counter_r
	);

endinterface : perf_if
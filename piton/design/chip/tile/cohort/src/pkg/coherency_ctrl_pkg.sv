package coherency_ctrl_pkg;

	// how many cache lines to watch in the coherency controller
	parameter integer unsigned cache_line_num = 4;

	// how many bits of backoff is allowed
	parameter integer unsigned backoff_width = 16;

	// denotes how many cache lines should we watch in this batch 
	typedef logic [$clog2(cache_line_num) : 0] size_t;

	typedef dcp_pkg::paddr_t addr_t;

	typedef logic [backoff_width-1: 0] backoff_t;

endpackage : coherency_ctrl_pkg
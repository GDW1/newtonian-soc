
module producer_transaction_generator
	import fifo_ctrl_pkg::*;
(
	input                  clk                 , // Clock
	input                  rst_n               , // synchronous reset active low
	input  fifo_config_pkg::fifo_config_t fifo_config_r      , // registered input
	input                  trans_ack           , // the ack for a previous transmission, this is for bumping the ack'ed head pointer
	decoupled_vr_if.master producer_transaction,
	output ptr_t           producer_tail_ptr_o   // notify the outer coherency unit here's something to publish
);
	

	// tail pointer prior to issuing ( I have issued memory req to this position)
	ptr_t tail_ptr_issue_r, tail_ptr_issue_n;
	// tail pointer after acked ( I can tell consumer my tail pointer is here )
	ptr_t tail_ptr_ack_r, tail_ptr_ack_n;

	assign producer_tail_ptr_o = tail_ptr_ack_r; // registered output

	// this is to avoid the costly address calculation
	fifo_ctrl_pkg::addr_t addr_issue_r, addr_issue_n;

	// logic to calculate whether the fifo is full
	logic fifo_full;
	assign fifo_full = fifo_ctrl_pkg::fifo_is_full(fifo_config_r.fifo_ptr.head, tail_ptr_issue_r, fifo_config_r.fifo_length);

	// logic that determines whether an issue is successful
	logic transact_valid;
	assign transact_valid = !fifo_full;


	logic transact_handshake;
	assign transact_handshake = transact_valid & (producer_transaction.ready);

	assign producer_transaction.valid = transact_valid;
	assign producer_transaction.data = addr_issue_r;


	always_ff @(posedge clk) begin : proc_ptr
		if(~rst_n) begin
			tail_ptr_issue_r <= '0;
			tail_ptr_ack_r <= '0;
			addr_issue_r <= '0;
		end else begin
			tail_ptr_issue_r <= tail_ptr_issue_n;
			tail_ptr_ack_r <= tail_ptr_ack_n;
			addr_issue_r <= addr_issue_n;
		end
	end

	always_comb begin : proc_ptr_next
		tail_ptr_ack_n = tail_ptr_ack_r;
		tail_ptr_issue_n = tail_ptr_issue_r;
		addr_issue_n = addr_issue_r;

		if (transact_handshake) begin
			tail_ptr_issue_n = inc_ptr_one(tail_ptr_issue_r, fifo_config_r.fifo_length);
			addr_issue_n = inc_addr(addr_issue_r, tail_ptr_issue_r, fifo_config_r.addr_base, fifo_config_r.fifo_length, fifo_config_r.element_size);
		end else if (addr_issue_r < fifo_config_r.addr_base) begin
			addr_issue_n = fifo_config_r.addr_base;
		end

		if (trans_ack) begin
			tail_ptr_ack_n = inc_ptr_one(tail_ptr_ack_r, fifo_config_r.fifo_length);
		end
	end



endmodule : producer_transaction_generator

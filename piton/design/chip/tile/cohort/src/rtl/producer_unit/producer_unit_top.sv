// top level of producer unit
// needs 3 interfaces: one to tri, one to decoupled data from acc, and
// one "uncached" that takes data from main bus
module producer_unit_top (
	input                                 clk                , // Clock
	input                                 rst_n              , // Asynchronous reset active low
	input  fifo_config_pkg::fifo_config_t fifo_config_r      ,
	decoupled_vr_if.slave                 producer_data      ,
	tri_if.master                         tri_bus            , // tri bus directly to l15, admittedly through an adapter
	output fifo_ctrl_pkg::ptr_t           producer_tail_ptr_o
);

	logic trans_ack;
	decoupled_vr_if producer_transaction(.clk(clk), .rst_n(rst_n));

	producer_transaction_generator i_producer_transaction_generator (
		.clk                 (clk                        ),
		.rst_n               (rst_n                      ),
		.fifo_config_r       (fifo_config_r              ),
		.trans_ack           (trans_ack                  ),
		.producer_transaction(producer_transaction.master),
		.producer_tail_ptr_o (producer_tail_ptr_o        )
	);


	producer_transaction_to_tri_adapter i_producer_transaction_to_tri_adapter (
		.clk          (clk                       ),
		.rst_n        (rst_n                     ),
		.fifo_config_r(fifo_config_r             ),
		.trans        (producer_transaction.slave),
		.acc_data     (producer_data             ),
		.trans_ack    (trans_ack                 ),
		.tri_intf     (tri_bus                   )
	);

endmodule : producer_unit_top
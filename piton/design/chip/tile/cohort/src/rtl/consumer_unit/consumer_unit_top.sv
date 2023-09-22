// wrapper over the old load unit top
// this is to unify the common interfaces

module consumer_unit_top (
	input                                 clk                 , // Clock
	input                                 rst_n               , // Asynchronous reset active low
	input  pmesh_pkg::pmesh_noc2_i_t      noc2_i              , // chip bus IO
	output pmesh_pkg::pmesh_noc2_o_t      noc2_o              , //
	input  pmesh_pkg::pmesh_noc3_in_t     noc3_in             , //
	input  fifo_config_pkg::fifo_config_t fifo_config_r       , // simple config, go through adapters in inner module
	decoupled_vr_if.master                consumer_transaction, // consumer load and hand over to acc
	output fifo_ctrl_pkg::ptr_t           consumer_head_ptr_o   //TODO: align ptr_t types between packages
);

	decoupled_vr_if trans_info ();
	logic trans_ack;

	assign trans_ack = input_data.valid & input_data.ready;

	consumer_load_transaction_generator i_consumer_load_transaction_generator (
		.clk                (clk                ),
		.rst_n              (rst_n              ),
		.fifo_config_r      (fifo_config_r      ),
		.trans_info         (trans_info         ),
		.trans_ack          (trans_ack          ), // specifically, ack from a load
		.consumer_head_ptr_o(consumer_head_ptr_o)
	);


	decoupled_vr_if #(.DataWidth(128)) input_data();

	mshr_consumer_unit_top i_mshr_consumer_unit_top (
		.clk          (clk          ),
		.rst_n        (rst_n        ),
		.trans_info   (trans_info   ),
		.recv_info    (input_data   ),
		.fifo_config_r(fifo_config_r),
		.noc2_i       (noc2_i       ),
		.noc2_o       (noc2_o       ),
		.noc3_in      (noc3_in      )
	);

cohort_serdes #(.serialization_ratio(1), .deserialization_ratio(2)) i_cohort_serdes (
	.clk          (clk          ),
	.rst_n        (rst_n        ),
	.input_data(input_data),
	.output_data(consumer_transaction)
);


endmodule : consumer_unit_top

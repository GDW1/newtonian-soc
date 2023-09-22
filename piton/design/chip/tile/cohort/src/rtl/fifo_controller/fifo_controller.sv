// note that we should split the parameterization into producer data width and consumer data width
// this is to mitigate the case where the acc has different producer vs consumer data width
module fifo_controller #(parameter int acc_index = 0) (
	input                                 clk                  , // Clock
	input                                 rst_n                , // Asynchronous reset active low
	input  pmesh_pkg::pmesh_noc2_i_t      noc2_i               ,
	output pmesh_pkg::pmesh_noc2_o_t      noc2_o               ,
	input  pmesh_pkg::pmesh_noc3_in_t     noc3_in              ,
	input  acc_pkg::acc_config_t          acc_config           ,
	input  fifo_config_pkg::fifo_config_t consumer_config      ,
	input  fifo_config_pkg::fifo_config_t producer_config      ,
	input  logic [15:0]                   serialization_ratio  ,
	input  logic [15:0]                   deserialization_ratio,
	input  logic [13:0]                   wait_cycles,
	tri_if.master                         tri_bus              ,
	output fifo_ctrl_pkg::ptr_t           producer_tail_ptr_o  ,
	output fifo_ctrl_pkg::ptr_t           consumer_head_ptr_o  ,
	data_forward_if.data_forward_in		  				  data_forward_in_a    ,
	data_forward_if.data_forward_out				      data_forward_out_b   ,
	input logic[1:0]					  bypass_control
);


	decoupled_vr_if #(.DataWidth(fifo_ctrl_pkg::data_width)) consumer_data (
		.clk  (clk  ),
		.rst_n(rst_n)
	);
	
	decoupled_vr_if #(.DataWidth(fifo_ctrl_pkg::data_width)) producer_data (
		.clk  (clk  ),
		.rst_n(rst_n)
	);

	consumer_unit_top i_consumer_unit_top (
		.clk                 (clk                 ),
		.rst_n               (rst_n               ),
		.noc2_i              (noc2_i              ),
		.noc2_o              (noc2_o              ),
		.noc3_in             (noc3_in             ),
		.fifo_config_r       (consumer_config     ),
		.consumer_transaction(consumer_data.master),
		.consumer_head_ptr_o (consumer_head_ptr_o )
	);

	acc_dummy #(.acc_index(0)) i_acc_dummy (
		.clk                  (clk                  ),
		.rst_n                (rst_n                ),
		.acc_config           (acc_config           ),
		.serialization_ratio  (serialization_ratio  ),
		.deserialization_ratio(deserialization_ratio),
		.wait_cycles          (wait_cycles),
		.consumer_data        (consumer_data.slave  ),
		.producer_data        (producer_data.master ),
		.data_forward_in      (data_forward_in_a      ),
		.data_forward_out     (data_forward_out_b     ),
		.bypass_control       (bypass_control  )
	);

	producer_unit_top i_producer_unit_top (
		.clk                (clk                 ),
		.rst_n              (rst_n               ),
		.fifo_config_r      (producer_config     ),
		.producer_data      (producer_data.slave),
		.tri_bus            (tri_bus             ),
		.producer_tail_ptr_o(producer_tail_ptr_o )
	);

endmodule

module aes_top_wrapper #(parameter int stage = 0) (
	input                       				clk          , // Clock
	input                       				rst_n        , // Asynchronous reset active low
	input acc_pkg::acc_config_t 				acc_config   , // acclerator uncached configuration interface
	decoupled_vr_if.slave       				consumer_data,
	decoupled_vr_if.master      				producer_data,
	data_forward_if.data_forward_in 		    data_forward_in,
	data_forward_if.data_forward_out 		    data_forward_out,
	input logic[2:0]							bypass_control

);


generate
    if (stage == 0) begin : stage_0
        aes_top #(
            .number_inputs (4),
            .number_outputs(18),
            .stage(0)
        )aes_top_inst (
            .clk(clk),
            .rst_n(rst_n),
            .acc_config(acc_config),
            .consumer_data(consumer_data),
            .producer_data(producer_data),
            .data_forward_in(data_forward_in),
            .data_forward_out(data_forward_out),
            .bypass_control(bypass_control)
        );
    end else if (stage == 1) begin : stage_1
        aes_top #(
            .number_inputs (18),
            .number_outputs(10),
            .stage(1)
        )aes_top_inst (
            .clk(clk),
            .rst_n(rst_n),
            .acc_config(acc_config),
            .consumer_data(consumer_data),
            .producer_data(producer_data),
            .data_forward_in(data_forward_in),
            .data_forward_out(data_forward_out),
            .bypass_control(bypass_control)
        );
    end else if (stage == 2) begin : stage_2
        aes_top #(
            .number_inputs (10),
            .number_outputs(2),
            .stage(2)
        )aes_top_inst (
            .clk(clk),
            .rst_n(rst_n),
            .acc_config(acc_config),
            .consumer_data(consumer_data),
            .producer_data(producer_data),
            .data_forward_in(data_forward_in),
            .data_forward_out(data_forward_out),
            .bypass_control(bypass_control)
        );
    end
endgenerate

endmodule
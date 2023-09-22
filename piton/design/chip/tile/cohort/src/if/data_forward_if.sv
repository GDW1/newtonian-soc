interface data_forward_if #(parameter int data_width = 64, parameter int vector_size = 400) (
    input clk,
    input rst_n
);

    
logic [data_width-1:0] data[vector_size-1:0];
logic rdy;

modport data_forward_in (
            output data,
            output rdy
        );

modport data_forward_out (
            input data,
            input rdy
        );


endinterface
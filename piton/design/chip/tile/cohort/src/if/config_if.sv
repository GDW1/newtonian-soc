// uncached signals
interface config_if (
    input logic clk,
    input logic rst_n
);
    logic valid;
    config_pkg::paddr_t addr;
    config_pkg::data_t data;
    config_pkg::size_t size;
    config_pkg::transaction_t config_type;

    config_pkg::data_t read_data;
    logic read_valid;


    modport slave(
        input valid, addr, data, size, config_type,
        output read_data, read_valid
    );

    modport master(
        output valid, addr, data, size, config_type,
        input read_data, read_valid
    );


`ifndef SYNTHESIS
    task set_config(input paddr_t addr_in, data_t data_in);
        $display("Adding configuration");
        @(negedge clk);
        valid = 1'b1;
        data = data_in;
        addr = addr_in;
        @(posedge clk);
    endtask: set_config

    task transact(input paddr_t addr_in, size_t size_in, req_type_t op_in);
        $display("Beginning transaction\n");
        set_config(12'h0, addr_in);
        set_config(12'h4, op_in);
        set_config(12'h8, size_in);
        set_config(12'hc, 1'b1);
        valid = 1'b0;
    endtask: transact
`endif

endinterface : config_if

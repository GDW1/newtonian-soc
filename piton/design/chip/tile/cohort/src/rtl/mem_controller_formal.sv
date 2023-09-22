bind mem_controller mem_controller_formal mem_controller_formal_inst(.*);
typedef enum logic {S_IDLE, S_REQ} state_t;
module mem_controller_formal (
    input               clk      , // Clock
    input               rst_n    , // Asynchronous reset active low
    config_if.slave   conf     ,
    mem_req_if.master load_req ,
    input               conf_fire,
    state_t             state_d
);

    default clocking
        @(posedge clk);
    endclocking
        default disable iff (rst_n);

    only_fire_on_idle: assert property (
        conf_fire |-> state_d == S_IDLE ##1 state_d == S_WRITE
    ) else $fatal(1, "Write request through noc2 should only fire when it's idle");

    conf_address_in_range: assert property (
        conf_fire |-> conf.addr inside {ADDR_BASE, ADDR_SIZE, ADDR_FIRE}
    ) else $fatal(1, "Invalid configuration address");


endmodule

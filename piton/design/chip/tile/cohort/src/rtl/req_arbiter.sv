module req_arbiter (
    mem_req_if.slave mem_controller_if,
    mem_req_if.slave mem_if       ,
    mem_req_if.master load_req
);
    // 0 is for mem_controller interface
    // 1 is for memory parser interface
    // if mem_controller interface is up, always issue request for mem_controller first
    // otherwise issue request for memory parse interface
    logic select;

    //TODO: is it safe to leave it at this? Would it cause a glitch?
    assign select = (mem_controller_if.valid) ? 1'b0 : 1'b1;

    assign mem_controller_if.ready = load_req.ready & !select;
    assign mem_if.ready        = load_req.ready & select;
    assign load_req.valid      = mem_controller_if.valid | mem_if.valid;
    assign load_req.req_type   = (select) ? mem_if.req_type : mem_controller_if.req_type;
    assign load_req.mshrid     = (select) ? mem_if.mshrid : mem_controller_if.mshrid;
    assign load_req.address   = (select) ? mem_if.address : mem_controller_if.address;
    assign load_req.size   = (select) ? mem_if.size : mem_controller_if.size;
    assign load_req.homeid   = (select) ? mem_if.homeid : mem_controller_if.homeid;
    assign load_req.write_mask   = (select) ? mem_if.write_mask : mem_controller_if.write_mask;
    assign load_req.data_0   = (select) ? mem_if.data_0 : mem_controller_if.data_0;
    assign load_req.data_1  = (select) ? mem_if.data_1 : mem_controller_if.data_1;

endmodule

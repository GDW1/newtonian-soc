module tb_cohort ();

    bit clk;
    bit rst_n;

    config_intf      conf(.clk(clk), .rst_n(rst_n))       ;
    mem_req_intf     load_req (.clk(clk), .rst_n(rst_n))   ;
    atomic_resp_intf atomic_resp(.clk(clk), .rst_n(rst_n)) ;


    task clk_gen ();
        forever begin
            clk <= 1'b0;
            #1;
            clk <= 1'b1;
            #1;
        end
    endtask : clk_gen

    task apply_rst_n();
        #100 rst_n <= 0;
        rst_n <= 1;
    endtask : apply_rst_n

    task apply_stimulus();
        // apply input at the posedge clk
        conf.transact(32'hdeadbeef, 3'h7, 4'h4);
        load_req.check_output(32'hdeadbeef);
        #10;
        $finish();
    endtask	

    initial begin
        clk_gen();
    end

    initial begin
        apply_rst_n();
        apply_stimulus();
    end

    initial begin
        load_req.initialize();
    end

    initial begin
        $fsdbDumpfile("tb_cohort.fsdb",50);
        $fsdbDumpvars(0, tb_cohort,"+all");
    end

    cohort_impl cohort_impl_inst(.*);

endmodule

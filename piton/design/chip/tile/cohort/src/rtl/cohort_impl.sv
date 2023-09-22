`include "dcp.h"


module cohort_impl #(parameter int consumer_num = 6) (
    input logic clk,
    input logic rst_n,
    config_if.slave conf,
    mem_req_if.master load_req,
    atomic_resp_if.slave atomic_resp,
    tlb_if.master tlb_req,
    tri_if.master tri_l2
);
    localparam int unsigned reg_per_consumer = 8;
    localparam int unsigned uncached_regfile_num = reg_per_consumer * consumer_num;

    tri_if tri_l2_untranslated (
        .clk  (clk),
        .rst_n(rst_n)
    );

    tri_if tri_source [2*consumer_num-1:0] (
            .clk(clk),
            .rst_n(rst_n)
    );

    mem_req_if load_req_source[consumer_num - 1:0] (
        .clk  (clk  ),
        .rst_n(rst_n)
    );


    mem_req_if load_req_untranslated (
        .clk  (clk  ),
        .rst_n(rst_n)
    );

	atomic_resp_if atomic_resp_sink[consumer_num-1:0] (
		.clk(clk),
		.rst_n(rst_n)
	);

    logic [63:0] uncached_reg[uncached_regfile_num - 1:0];

    // maybe we don't even need reset anymore? 
    // [7] is clear
    // [8] is start 
    uncached_configuration_unit #(
        .reg_number        (uncached_regfile_num),
        .ro_register_number(35 + 20               )
    ) i_uncached_configuration_unit (
        .clk          (clk                 ),
        .rst_n        (rst_n               ),
        .conf         (conf                ),
		`ifdef COHORT_PERF
        .ro_register_i(performance_counters),
		`endif
        .uncached_reg (uncached_reg        )
    );

    // if we have i consumers, were going to want i - 1 data forwarding paths
    // Assume that the ith consumer is forwarding to the (i+1)th consumer
    data_forward_if #(.data_width(64), .vector_size(128)) data_forward [consumer_num:0] ();

    generate
	  for (genvar i = 0; i < consumer_num; i++) begin


        // our available mshr id range is 128-143
        // give 128-131 to mem_controller
        // give 132-143 to memory parser
        
        fifo_config_pkg::fifo_config_t consumer_config, producer_config;
        acc_pkg::acc_config_t acc_config;


        config_if config_if_inst (
            .clk  (clk  ),
            .rst_n(rst_n)
        );

        coherency_configure_if coherency_configure_if_inst (
            .clk  (clk  ),
            .rst_n(rst_n)
        );

        



        logic monitor_on;
        logic manual_reset;

        logic [15:0] backoff_value;

        logic [15:0] serialization_ratio;

        logic [15:0] deserialization_ratio;

        logic [13:0] wait_cycles;

        logic [1:0] bypass_control;

        // 0: reset
        // 1: monitor_on
        // 2: clear
        // 3: on/off
        // 63-48: backoff value
        logic [63:0] config_reg;
        assign config_reg = uncached_reg[(i+1) * reg_per_consumer - 1];
        assign manual_reset         = config_reg[0];
        assign monitor_on           = config_reg[1];
        assign bypass_control       = config_reg[3:2];
        assign wait_cycles          = config_reg[15:4];
        assign deserialization_ratio= config_reg[31:16];
        assign serialization_ratio  = config_reg[47:32];
        assign backoff_value        = config_reg[63:48];

	    `ifdef COHORT_PERF
        generate
            for (genvar perf_i = 0; perf_i < 35; perf_i++) begin : generate_performance_counter
                assign performance_counters[perf_i] = coherency_perf.counter_r[perf_i];
            end : generate_performance_counter
            for (genvar perf_i = 35; perf_i < 35+20; perf_i++) begin : generate_debug
                assign performance_counters[perf_i] = impl_debug.dbg_data[perf_i-35];
            end : generate_debug

        endgenerate
	    `endif
  
        

        logic [127:0]        reg_file_o       [4:0];
        fifo_ctrl_pkg::ptr_t producer_tail_ptr     ;
        fifo_ctrl_pkg::ptr_t consumer_head_ptr     ;

        logic element_fetched;

        logic secondary_rst_n;
        assign secondary_rst_n = rst_n & element_fetched & manual_reset;

        coherency_manager_top #(
            .source_num(reg_per_consumer - 1)
        ) i_coherency_manager_top (
            .clk                (clk                                   ),
            .rst_n              (rst_n                                 ),
            .monitor_on         (monitor_on                            ),
            .base_addr_r        (uncached_reg[(i * reg_per_consumer) +: (reg_per_consumer - 1)]),
            .backoff_value      (backoff_value                         ),
	    	`ifdef COHORT_PERF
            .perf               (coherency_perf                        ),
            .debug              (impl_debug                            ),
	    	`endif
            .element_fetched    (element_fetched                       ),
            .tri_l2             (tri_source[2*i].master                ),
            .reg_file_o         (reg_file_o                            ),
            .producer_tail_ptr_i(producer_tail_ptr                     ),
            .consumer_head_ptr_i(consumer_head_ptr                     )
        );

        // wire coherency manager and fifo config
        assign consumer_config.fifo_ptr.tail = reg_file_o[0];
        assign consumer_config.addr_base     = reg_file_o[1][63:0];
        assign consumer_config.element_size  = reg_file_o[1][95:64];
        assign consumer_config.fifo_length   = reg_file_o[1][127:96];

        assign producer_config.fifo_ptr.head = reg_file_o[2];
        assign producer_config.addr_base     = reg_file_o[3][63:0];
        assign producer_config.element_size  = reg_file_o[3][95:64];
        assign producer_config.fifo_length   = reg_file_o[3][127:96];

        assign acc_config = reg_file_o[4];

        // wire noc signals
        pmesh_pkg::pmesh_noc2_i_t  noc2_i ;
        pmesh_pkg::pmesh_noc2_o_t  noc2_o ;
        pmesh_pkg::pmesh_noc3_in_t noc3_in;

        assign noc3_in.valid =     atomic_resp_sink[i].valid;
        assign noc3_in.mshrid =    atomic_resp_sink[i].mshrid;
        assign noc3_in.resp_data = atomic_resp_sink[i].data;

        assign load_req_source[i].valid = noc2_o.valid;
        assign load_req_source[i].req_type = noc2_o.req_type;
        assign load_req_source[i].mshrid = noc2_o.mshrid;
        assign load_req_source[i].address = noc2_o.address;
        assign load_req_source[i].size = noc2_o.size;
        assign load_req_source[i].homeid = noc2_o.homeid;
        assign load_req_source[i].write_mask = noc2_o.write_mask;
        assign load_req_source[i].data_0 = noc2_o.data_0;
        assign load_req_source[i].data_1 = noc2_o.data_1;

        assign noc2_i.ready = load_req_source[i].ready;

        import fifo_config_pkg::fifo_config_t;

        // data_forward_if #(.data_width(128), .vector_size(10)) data_forward_in_a (
        //     .clk(clk),
        //     .rst_n(rst_n)
        // );

        // data_forward_if #(.data_width(128), .vector_size(10)) data_forward_out_b (
        //     .clk(clk),
        //     .rst_n(rst_n)
        // );

        fifo_controller #(.acc_index(i)) i_fifo_controller (
            .clk                  (clk                                   ),
            .rst_n                (secondary_rst_n                       ),
            .noc2_i               (noc2_i                                ),
            .noc2_o               (noc2_o                                ),
            .noc3_in              (noc3_in                               ),
            .acc_config           (acc_config                            ),
            .consumer_config      (consumer_config                       ),
            .producer_config      (producer_config                       ),
            .serialization_ratio  (serialization_ratio                   ),
            .deserialization_ratio(deserialization_ratio                 ),
            .wait_cycles          (wait_cycles                           ),
            .tri_bus              (tri_source[2*i + 1].master            ),
            .producer_tail_ptr_o  (producer_tail_ptr                     ),
            .consumer_head_ptr_o  (consumer_head_ptr                     ),
            .data_forward_in_a    (data_forward[i+1].data_forward_in     ),
            .data_forward_out_b   (data_forward[i].data_forward_out      ),
            .bypass_control       (bypass_control                        )  // 2'b11
        );
	end
	endgenerate



    mshr_tri_unit #(.source_num(2*consumer_num)) i_mshr_tri_unit (
        .clk       (clk                ),
        .rst_n     (rst_n              ),
        .tri_source(tri_source         ),
        .tri_sink  (tri_l2_untranslated)
    );
    
	mshr_noc_unit #(.source_num(consumer_num)) i_mshr_noc_unit (
        .clk       (clk                ),
        .rst_n     (rst_n              ),
        .mem_req_source(load_req_source),
        .mem_req_sink  (load_req_untranslated),
		.atomic_resp_source(atomic_resp),
		.atomic_resp_sink(atomic_resp_sink)
    );

    
    translator_unit_top i_translator_unit_top (
        .clk         (clk                  ),
        .rst_n       (rst_n                ),
        .tlb_req     (tlb_req              ),
        .tri_source  (tri_l2_untranslated  ),
        .tri_sink    (tri_l2               ),
        .pmesh_source(load_req_untranslated),
        .pmesh_sink  (load_req             )
    );

endmodule : cohort_impl

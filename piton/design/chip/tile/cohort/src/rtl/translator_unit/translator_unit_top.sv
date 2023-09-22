module translator_unit_top (
	input clk,    // Clock
	input rst_n,
	tlb_if.master tlb_req,
	tri_if.slave tri_source,
	tri_if.master tri_sink,
	mem_req_if.slave pmesh_source,
	mem_req_if.master pmesh_sink
);

	tlb_if tlb_req_untranslated[1:0](
        .clk(clk),
        .rst_n(rst_n)
    );

	tri_translator_unit i_tri_translator_unit (
		.clk        (clk                    ),
		.rst_n      (rst_n                  ),
		.tri_req_vpn(tri_source             ),
		.tri_req_ppn(tri_sink               ),
		.tlb_req    (tlb_req_untranslated[0])
	);

	pmesh_translator_unit i_pmesh_translator_unit (
		.clk         (clk                    ),
		.rst_n       (rst_n                  ),
		.load_req_vpn(pmesh_source           ),
		.load_req_ppn(pmesh_sink             ),
		.tlb_req     (tlb_req_untranslated[1])
	);

	tlb_arbiter i_tlb_arbiter (
		.clk           (clk                 ),
		.rst_n         (rst_n               ),
		.tlb_req_source(tlb_req_untranslated),
		.tlb_req_sink  (tlb_req             )
	);

endmodule : translator_unit_top

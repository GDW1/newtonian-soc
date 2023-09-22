/* this interface establishes the interface
 * which is a valid-ack interface. The valid is asserted when the master initiates a request
   and will keep asserted until it's acked
   the ack will assert for a single cycle if the data returns
 */

`include "assert_macros.svh"
interface tlb_if (
    input clk,
    input rst_n
);
    import dcp_pkg::*;

    logic valid;
    logic ack;
    logic [`DCP_VADDR   -12-1:0] vpn;
    logic [`DCP_VADDR   -12-1:0] ppn;

    modport slave (
        input valid, vpn, 
        output ack, ppn
    );

    // master will ack ppn
    modport master (
        input ack, ppn,
        output valid, vpn
    );

//        `assert_hold_valid(vpn, valid, ready, "Valid and vpn should hold until ack");
//       `assert_finish_handshake(valid, ready, "Valid should lower after handshake");

endinterface : tlb_if

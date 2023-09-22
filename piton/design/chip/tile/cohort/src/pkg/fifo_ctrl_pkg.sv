package fifo_ctrl_pkg;
	`include "dcp.h"
    
    import dcp_pkg::mshrid_t;

    // WARNING: make sure sw is updated if we ever want to change this
    localparam int unsigned addr_width = 64; // the base address width
    localparam int unsigned size_width = 32; //w the size of each element. Larger than necessary
    localparam int unsigned length_width = 32; //w length of the fifo: eg: 16 elements
    // the reason we make this fixed is because c semantics only allow uint64_t rather than uint128_t, so you'll need to break your data up.
    // note that float = 32bits, double = 64bits
    localparam int unsigned data_width = 64;
    localparam int unsigned ptr_width = length_width;
    localparam int unsigned cacheline_width = 128;

    typedef logic [ptr_width-1:0] ptr_t;
    typedef logic [addr_width-1:0] addr_t;
    typedef logic [size_width-1:0] size_t;
    typedef logic [length_width-1:0] length_t;
    typedef logic [data_width-1 : 0] data_t;
    typedef logic [cacheline_width-1:0] cacheline_t;



    /*calculate the next address
    * note that this does not need to be the case,
    * bug register is free anyway
    */
    typedef struct packed {
        addr_t addr;
        size_t size;
        logic valid;
    } addr_gen_o_t;

    typedef struct packed {
        logic ready;
    } addr_gen_i_t;

    /*define the formal request struct
    */
    typedef struct packed {
    	addr_t addr;
    	size_t size;
    	mshrid_t mshrid;
    	logic valid;
    } ld_req_i_t;

    typedef struct packed {
    	logic ready;
    } ld_req_o_t;

    /*transaction data type in the pipeline
    * carries the address of the data
    * note that this does not need to be the case,
    * bug register is free anyway
    */
    typedef struct packed {
        data_t data;
        logic valid;
    } transact_o_t;

    typedef struct packed {
        logic ready;
    } transact_i_t;

    typedef struct packed {
    	cacheline_t data;
    	logic valid;
    	mshrid_t mshrid;
    } ld_resp_o_t;

    typedef struct packed {
    	logic ready;
    } ld_resp_i_t;


    function automatic ptr_t inc_ptr_two(ptr_t ptr, length_t len);
        inc_ptr_two = '0;
        if ((ptr >> 1) == ((len - 1) >> 1)) begin
            inc_ptr_two = '0;
        end
        else begin
            inc_ptr_two = ptr + 2;
        end
    endfunction : inc_ptr_two

    function automatic ptr_t inc_ptr_one(ptr_t ptr, length_t len);
        inc_ptr_one = '0;
        if ((ptr ) == ((len - 1))) begin
            inc_ptr_one = '0;
        end
        else begin
            inc_ptr_one = ptr + 1;
        end
    endfunction : inc_ptr_one


    function automatic addr_t inc_addr(addr_t addr, ptr_t ptr, addr_t base, length_t len, size_t size);
        inc_addr = '0;
        if (ptr == (len - 1)) begin
            inc_addr = base;
        end
        else begin
            inc_addr = addr + size;
        end
    endfunction : inc_addr

    // note: we use the following function to abstract over the complex case
    // basically the fifo cannot truly be full and empty at the same time
    // we make the fifo capable of being empty but not full
    function automatic logic fifo_is_full(ptr_t tail, ptr_t head, length_t len);
        fifo_is_full = 1'b0;
        if (inc_ptr_one(head, len) == tail) begin
            fifo_is_full = 1'b1;
        end
    endfunction : fifo_is_full

    // note: the same as above. Just a layer of abstraction. Really nothing fancy here
    function automatic logic fifo_is_empty(ptr_t tail, ptr_t head, length_t len);
        fifo_is_empty = 1'b0;
        if (tail == head) begin
            fifo_is_empty = 1'b1;
        end 
    endfunction : fifo_is_empty

endpackage : fifo_ctrl_pkg

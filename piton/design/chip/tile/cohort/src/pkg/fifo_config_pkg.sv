// provides the fifo configuration package
// it can be used unversally across producer and consumer
// thanks to using union
package fifo_config_pkg;
	
	import fifo_ctrl_pkg::*;

	// there is a packed union here to help
	// decipher whether it's a head ptr or a tail ptr
	// we only use one kind of pointer in either consumer
	// or producer
	typedef union packed {
		ptr_t tail;
		ptr_t head;
	} fifo_ptr_t;

	typedef struct packed {
		fifo_ptr_t fifo_ptr    ;
		addr_t     addr_base   ;
		size_t     element_size;
		length_t   fifo_length ;
	} fifo_config_t;

endpackage : fifo_config_pkg
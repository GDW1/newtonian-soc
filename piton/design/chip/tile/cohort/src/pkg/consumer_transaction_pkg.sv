// documents the packages for transactions
// note that most of the data types would be universal, thus in fifo_ctrl_pkg
// the other settings namely data type and id will be documented for consumer and producer
// separately
package consumer_transaction_pkg;

	localparam int unsigned addr_width = 64;
	localparam int unsigned id_width = 4;

	typedef logic [id_width-1:0] id_t;
	
	typedef logic [addr_width-1:0] addr_t;

endpackage : consumer_transaction_pkg
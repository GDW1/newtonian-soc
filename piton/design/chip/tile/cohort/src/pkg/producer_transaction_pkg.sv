// documents the packages for transactions
// note that most of the data types would be universal, thus in fifo_ctrl_pkg
// the other settings namely data type and id will be documented for consumer and producer
// separately
package producer_transaction_pkg;

	localparam int unsigned data_width = 512;
	localparam int unsigned id_width = 4;

	typedef logic [id_width-1:0] id_t;
	
	typedef logic [data_width-1:0] data_t;

endpackage : producer_transaction_pkg
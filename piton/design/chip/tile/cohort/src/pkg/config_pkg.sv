package config_pkg;
	
    typedef enum logic {T_LOAD, T_STORE} transaction_t;

    typedef logic [63:0] data_t;

    typedef logic [2:0] size_t;
    typedef logic [39:0] paddr_t;

endpackage : config_pkg
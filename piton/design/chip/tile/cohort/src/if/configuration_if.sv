// this interface is to handle the configuration unit and the
// producer/consumer/acc unit. It provides a parameterized interface
// for the configuration to happen. Notice that each configuration will hold
// and will not change, until it has been acked.
interface configuration_if ();
	
endinterface : configuration_if
// backoff interface
// the request initializes with valid to high
// valid is expected to hold that way until there's an ack
// in which case the backoff is over
interface backoff_if ();

	logic valid;

	logic ack;

	logic interrupt;

	modport master (output valid, interrupt, input ack);

	modport slave (input valid, interrupt, output ack);

endinterface : backoff_if
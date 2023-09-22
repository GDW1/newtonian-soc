module FFT_Core #(parameter int stage) (
	input                       clk          , // Clock
	input                       rst_n        , // Asynchronous reset active low
	input acc_pkg::acc_config_t acc_config   , // acclerator uncached configuration interface
	decoupled_vr_if.slave       consumer_data,
	decoupled_vr_if.master      producer_data,
    data_forward_if.data_forward_in 		    data_forward_in ,
	data_forward_if.data_forward_out 		    data_forward_out,
	input logic[2:0]							bypass_control
);

typedef enum logic [2:0] {S_IDLE, S_SERIALIZE, S_COMPUTE, S_DESERIALIZE} state_t;

state_t state, state_next;


// Declare input wires for FFT module
/// FFT Real inputs is a 4096 wide wire
logic [2048 - 1:0] fft_real_in;
/// FFT Imag inputs is a 4096 wide wire
logic [2048 - 1:0] fft_imag_in;

// Declare output wires for FFT module
/// FFT Real outputs is a 4096 wide wire
logic [2048 - 1:0] fft_real_out;
/// FFT Imag outputs is a 4096 wide wire
logic [2048 - 1:0] fft_imag_out;

// assign fft_imag_in = 0;

// Internal counters
logic [$clog2(128) : 0] counter, counter_next;

// Assign outgoing val/rdy signals
assign consumer_data.ready   = (state == S_SERIALIZE);
assign producer_data.valid   = (state == S_DESERIALIZE);
assign data_forward_in.rdy = (state == S_DESERIALIZE);
assign data_forward_in.data[0][0 +: 2048] = fft_imag_out;
assign data_forward_in.data[0][2048 +: 2048] = fft_real_out;

assign fft_real_out = fft_real_in;
assign fft_imag_out = fft_imag_in;


always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= S_IDLE;
        counter <= 0;
    end else begin
        state <= state_next;
        counter <= counter_next;
    end
end

always_comb begin
    state_next = state;
    case(state)
        S_IDLE: begin
            if (consumer_data.valid) begin
                state_next = S_SERIALIZE;
                counter_next = 0;
            end
        end
        S_SERIALIZE: begin
            // if (bypass_control[1] == 1'b1) begin
                if (consumer_data.valid & consumer_data.ready) begin
                    // get data from consumer
                    // consumer data is 64 wide so we need to incrementally assign
                    // the data to the fft_real_in and fft_imag_in wires
                    if (counter < 64) begin
                        fft_imag_in[32*counter +: 32] = consumer_data.data[31:0];
                    end else begin
                        fft_real_in[32*(counter - 64) +: 32] = consumer_data.data[31:0];
                    end
                    if (counter_next == 128) begin
                        state_next = S_COMPUTE;
                    end else begin
                        counter_next = counter + 1;
                    end
                end
            // end else begin
            //     // get data from data_forward
            //     // data_forward data is 4096 * 2 wide so we need to immediately assign
            //     // the data to the fft_real_in and fft_imag_in wires
            //     fft_imag_in = data_forward_out.data[0][2047:0];
            //     fft_real_in = data_forward_out.data[0][2048+:2048];
            //     state_next = S_COMPUTE;
            // end
        end
        S_COMPUTE: begin
            state_next = S_DESERIALIZE;
            counter_next = 0;
        end
        S_DESERIALIZE: begin
            // if (bypass_control[0] == 1'b1) begin
                if (producer_data.valid & producer_data.ready) begin
                    // send data to producer
                    // producer data is 64 wide so we need to incrementally assign
                    // the data to the fft_real_out and fft_imag_out wires
                    if (counter < 64) begin
                        producer_data.data <= fft_imag_out[32*counter +: 32];
                    end else begin
                        producer_data.data <= fft_real_out[32*(counter - 64) +: 32];
                    end
                    if (counter_next == 128) begin
                        state_next = S_IDLE;
                    end else begin
                        counter_next = counter + 1;
                    end
                end
                if (counter_next == 128) begin
                        state_next = S_IDLE;
                end
            // end else begin
            //     // send data to data_forward
            //     // data_forward data is 4096 * 2 wide so we need to immediately assign
            //     // the data to the fft_real_out and fft_imag_out wires
            //     // data_forward_in.data[4095:0] <= fft_imag_out;
            //     // data_forward_in.data[8192 - 1:4096] <= fft_real_out;
            //     state_next = S_IDLE;
            // end
        end
    endcase
end



// Declare FFT module
/// stage i should instantiate fft_64_stage_i_inner

// logic o_valid;
// logic o_ready;
// generate
// if (stage == 0) begin
//     fft_64_stage_1_inner I_fft_64_stage_1_inner (
//         .clk(clk),
//         .rst(~rst_n),
//         .i_data_in_real(fft_real_in),
//         .i_data_in_imag(fft_imag_in),
//         .o_data_out_real(fft_real_out),
//         .o_data_out_imag(fft_imag_out),
//         .i_valid(1'b1),
//         .i_ready(1'b1),
//         .o_valid(o_valid),
//         .o_ready(o_ready)
//     );
// end else if (stage == 1) begin
//     fft_64_stage_2_inner I_fft_64_stage_2_inner (
//         .clk(clk),
//         .rst(~rst_n),
//         .i_data_in_real(fft_real_in),
//         .i_data_in_imag(fft_imag_in),
//         .o_data_out_real(fft_real_out),
//         .o_data_out_imag(fft_imag_out),
//         .i_valid(1'b1),
//         .i_ready(1'b1),
//         .o_valid(o_valid),
//         .o_ready(o_ready)

//     );
// end else if (stage == 2) begin
//     fft_64_stage_3_inner I_fft_64_stage_3_inner (
//         .clk(clk),
//         .rst(~rst_n),
//         .i_data_in_real(fft_real_in),
//         .i_data_in_imag(fft_imag_in),
//         .o_data_out_real(fft_real_out),
//         .o_data_out_imag(fft_imag_out),
//         .i_valid(1'b1),
//         .i_ready(1'b1),
//         .o_valid(o_valid),
//         .o_ready(o_ready)

//     );
// end else if (stage == 3) begin
//     fft_64_stage_4_inner I_fft_64_stage_4_inner (
//         .clk(clk),
//         .rst(~rst_n),
//         .i_data_in_real(fft_real_in),
//         .i_data_in_imag(fft_imag_in),
//         .o_data_out_real(fft_real_out),
//         .o_data_out_imag(fft_imag_out),
//         .i_valid(1'b1),
//         .i_ready(1'b1),
//         .o_valid(o_valid),
//         .o_ready(o_ready)

//     );
// end else if (stage == 4) begin
//     fft_64_stage_5_inner I_fft_64_stage_5_inner (
//         .clk(clk),
//         .rst(~rst_n),
//         .i_data_in_real(fft_real_in),
//         .i_data_in_imag(fft_imag_in),
//         .o_data_out_real(fft_real_out),
//         .o_data_out_imag(fft_imag_out),
//         .i_valid(1'b1),
//         .i_ready(1'b1),
//         .o_valid(o_valid),
//         .o_ready(o_ready)

//     );
// end else if (stage == 5) begin
//     fft_64_stage_6_inner I_fft_64_stage_6_inner (
//         .clk(clk),
//         .rst(~rst_n),
//         .i_data_in_real(fft_real_in),
//         .i_data_in_imag(fft_imag_in),
//         .o_data_out_real(fft_real_out),
//         .o_data_out_imag(fft_imag_out),
//         .i_valid(1'b1),
//         .i_ready(1'b1),
//         .o_valid(o_valid),
//         .o_ready(o_ready)

//     );
// end

// endgenerate

endmodule
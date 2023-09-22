module aes_core_s1_top (
	input                       clk          , // Clock
	input                       rst_n        , // Asynchronous reset active low
	input acc_pkg::acc_config_t acc_config   , // acclerator uncached configuration interface
	decoupled_vr_if.slave       consumer_data,
	decoupled_vr_if.master      producer_data,
    data_forward_if.data_forward_in 		    data_forward_in ,
	data_forward_if.data_forward_out 		    data_forward_out,
	input logic[2:0]							bypass_control
);

typedef enum logic [2:0] {S_IDLE, S_SERIALIZE, S_COMPUTE, S_WAIT, S_DESERIALIZE} state_t;

state_t state, next_state;

logic [127 : 0] data_in, data_in_n;
logic [127 : 0] data_out, data_out_n;

logic [127 : 0] acc_out;

logic acc_in_valid;
logic acc_out_valid;

logic [127:0] aes_key;

logic [$clog2(10): 0] counter, counter_n;

/*
delcare all the keys as logic
*/
logic [127:0] keys [0:9];


assign aes_key = 'h8_88_88_8f_ff;

assign acc_in_valid = (state == S_COMPUTE);

assign consumer_data.ready = (state == S_SERIALIZE);
assign producer_data.valid = (state == S_DESERIALIZE);
assign producer_data.data = (state == S_DESERIALIZE) ? (data_out) : 0;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= S_IDLE;
        data_in <= 64'b0;
        counter <= 0;
    end else begin
        state <= next_state;
        data_in <= data_in_n;
        counter <= counter_n;
    end
end


always_comb begin
    next_state = state;
    data_out_n = data_out;
    data_in_n = data_in;
    case (state)
        S_IDLE: begin
            if (consumer_data.valid) begin
                next_state = S_SERIALIZE;
                counter_n = 0;
            end
        end
        S_SERIALIZE: begin
            // use 1 cycle to get upper 64 bits and 1 cycle to get lower 64 bits
            if(consumer_data.valid && consumer_data.ready) begin
                if (counter == 0) begin
                    data_in_n[127:64] = consumer_data.data;
                end else begin
                    data_in_n[63:0] = consumer_data.data;
                    next_state = S_WAIT;

                end
                counter_n = counter + 1;
            end
        end
        S_WAIT: begin
            next_state = S_COMPUTE;
            counter_n = 0;
        end
        S_COMPUTE: begin
            if(acc_out_valid) begin
                next_state = S_DESERIALIZE;
                data_out_n = acc_out;
                counter_n = 0;
            end
        end
        S_DESERIALIZE: begin
            if (bypass_control[0] == 1) begin
                /*
                    When counter is 0, we need to send the upper 64 bits of data_out
                    When counter is 1, we need to send the lower 64 bits of data_out

                    After, send each key in 2 cycles in the same manner for a total of 16 cycles
                */
                if (producer_data.valid && producer_data.ready) begin
                    if (counter == 0) begin
                        data_out = acc_out[127:64];
                    end else if (counter == 1) begin
                        data_out = acc_out[63:0];
                    end else if (counter == 2) begin
                        data_out = keys[0][127:64];
                    end else if (counter == 3) begin
                        data_out = keys[0][63:0];
                    end else if (counter == 4) begin
                        data_out = keys[1][127:64];
                    end else if (counter == 5) begin
                        data_out = keys[1][63:0];
                    end else if (counter == 6) begin
                        data_out = keys[2][127:64];
                    end else if (counter == 7) begin
                        data_out = keys[2][63:0];
                    end else if (counter == 8) begin
                        data_out = keys[3][127:64];
                    end else if (counter == 9) begin
                        data_out = keys[3][63:0];
                    end else if (counter == 10) begin
                        data_out = keys[4][127:64];
                    end else if (counter == 11) begin
                        data_out = keys[4][63:0];
                    end else if (counter == 12) begin
                        data_out = keys[5][127:64];
                    end else if (counter == 13) begin
                        data_out = keys[5][63:0];
                    end else if (counter == 14) begin
                        data_out = keys[6][127:64];
                    end else if (counter == 15) begin
                        data_out = keys[6][63:0];
                    end else if (counter == 16) begin
                        data_out = keys[7][127:64];
                    end else if (counter == 17) begin
                        data_out = keys[7][63:0];
                        next_state = S_IDLE;
                    end
                    counter_n = counter + 1;
                end
            end else begin
                next_state = S_IDLE;
            end
        end
    endcase
end


AES_Stage_One_Visc aes_one(
    .key_in(aes_key),
    .data_in(data_in),
    .data_out(acc_out),

    .clk(clk),
    .rst_n(rst_n),

    .ready_in(acc_in_valid),
    .valid_out(acc_out_valid),

    // one output for each key and one for the output data
    .key_s2_out(keys[0]),
    .key_s3_out(keys[1]),
    .key_s4_out(keys[2]),
    .key_s5_out(keys[3]),
    .key_s6_out(keys[4]),
    .key_s7_out(keys[5]),
    .key_s8_out(keys[6]),
    .key_s9_out(keys[7])
);


/*
data_forward_out:
[0] = data_out
[1] = key_s2_out
[2] = key_s3_out
[3] = key_s4_out
[4] = key_s5_out
[5] = key_s6_out
[6] = key_s7_out
[7] = key_s8_out
[8] = key_s9_out
*/

assign data_forward_in.data[0] = acc_out;
assign data_forward_in.data[1] = keys[0];
assign data_forward_in.data[2] = keys[1];
assign data_forward_in.data[3] = keys[2];
assign data_forward_in.data[4] = keys[3];
assign data_forward_in.data[5] = keys[4];
assign data_forward_in.data[6] = keys[5];
assign data_forward_in.data[7] = keys[6];
assign data_forward_in.data[8] = keys[7];
assign data_forward_in.rdy = next_state == S_IDLE & state == S_DESERIALIZE;

endmodule : aes_core_s1_top
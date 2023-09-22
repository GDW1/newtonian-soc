module aes_core_s3_top (
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

logic [$clog2(20): 0] counter, counter_n;

/*
delcare all the keys as logic
*/
logic [127:0] keys [0:9];


assign acc_in_valid = (state == S_COMPUTE);

assign consumer_data.ready = (state == S_SERIALIZE);
assign producer_data.valid = (state == S_DESERIALIZE);
assign producer_data.data = (state == S_DESERIALIZE) ? (data_out) : 0;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        state <= S_IDLE;
        counter <= 0;
    end else begin
        state <= next_state;
        counter <= counter_n;
    end
end


always_comb begin
    next_state = state;
    data_out_n = data_out;
    data_in_n = data_in;
    case (state)
        S_IDLE: begin
            if (consumer_data.valid | (((bypass_control[1] == 0) & data_forward_out.rdy))) begin
                next_state = S_SERIALIZE;
                counter_n = 0;
            end
        end
        S_SERIALIZE: begin
            if (bypass_control[1] == 1) begin
                if(consumer_data.valid && consumer_data.ready) begin
                    // there are 18 inputs the first two are the data and the next 16 are the keys
                    counter_n = counter + 1;
                    if (counter == 0) begin
                        data_in[127:0] = consumer_data.data;
                    end else if (counter == 1) begin
                        data_in[127:0] = consumer_data.data;
                    end else if (counter == 2) begin
                        keys[0][127:0] = consumer_data.data;
                    end else if (counter == 3) begin
                        keys[0][127:0] = consumer_data.data;
                    end else if (counter == 4) begin
                        keys[1][127:0] = consumer_data.data;
                    end else if (counter == 5) begin
                        keys[1][127:0] = consumer_data.data;
                    end else if (counter == 6) begin
                        keys[2][127:0] = consumer_data.data;
                    end else if (counter == 7) begin
                        keys[2][127:0] = consumer_data.data;
                    end else if (counter == 8) begin
                        keys[3][127:0] = consumer_data.data;
                    end else if (counter == 9) begin
                        keys[3][127:0] = consumer_data.data;
                        next_state = S_WAIT;
                    end
                end
            end else begin 
                data_in = data_forward_out.data[0];
                keys[0] = data_forward_out.data[1];
                keys[1] = data_forward_out.data[2];
                keys[2] = data_forward_out.data[3];
                keys[3] = data_forward_out.data[4];
                next_state = S_WAIT;
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
                if (producer_data.valid && producer_data.ready) begin
                    if (counter == 0) begin
                        data_out = acc_out[127:64];
                    end else if (counter == 1) begin
                        data_out = acc_out[63:0];
                        next_state = S_IDLE;
                    end
                    counter_n = counter + 1;
                end
            end
        end
    endcase
end


AES_Stage_Three_Visc aes_one(
    .data_in(data_in),
    .data_out(acc_out),

    .clk(clk),
    .rst_n(rst_n),

    .ready_in(acc_in_valid),
    .valid_out(acc_out_valid),

    // one output for each key and one for the output data
    .key_s6(keys[0]),
    .key_s7(keys[1]),
    .key_s8(keys[2]),
    .key_s9(keys[3])
);

endmodule : aes_core_s3_top
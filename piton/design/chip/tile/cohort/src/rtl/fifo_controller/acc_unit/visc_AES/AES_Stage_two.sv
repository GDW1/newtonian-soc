module AES_Stage_Two_Visc(
        input  logic[127:0] data_in,
        output logic[127:0] data_out,

        input  logic clk,
        input  logic rst_n,

        input  logic ready_in,

        output logic valid_out,

        input logic[127:0] key_s2,
        input logic[127:0] key_s3,
        input logic[127:0] key_s4,
        input logic[127:0] key_s5
);


// FSM which counts 10 cyclers after the ready signal is set and then sets the valid signal

typedef enum logic[1:0] {S_IDLE, S_COMPUTE, S_FINISHED} state_t;
state_t state, next_state;

logic[$clog2(10)-1:0] counter, next_counter;

always_ff @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        state <= S_IDLE;
        counter <= 0;
    end
    else
    begin
        state <= next_state;
        counter <= next_counter;
    end
end

always_comb begin
    next_state = state;
    case (state) 
        S_IDLE: begin
            if(ready_in) begin
                next_state = S_COMPUTE;
                next_counter = 0;
            end
        end
        S_COMPUTE: begin
            if(counter == 9) begin
                next_state = S_FINISHED;
                next_counter = 0;
            end
            else begin
                next_state = S_COMPUTE;
                next_counter = counter + 1;
            end
        end
        S_FINISHED: begin
            next_state = S_IDLE;
            next_counter = 0;
        end
        default: begin
            next_state = S_IDLE;
            next_counter = 0;
        end
    endcase
end

assign valid_out = (state == S_FINISHED);

logic[127:0] r1_data_out, r2_data_out, r3_data_out, r4_data_out, r5_data_out;

assign r1_data_out = data_in;

// AES_Stage_two
round_visc r2(clk,r1_data_out,key_s2,r2_data_out);
round_visc r3(clk,r2_data_out,key_s3,r3_data_out);
round_visc r4(clk,r3_data_out,key_s4,r4_data_out);
round_visc r5(clk,r4_data_out,key_s5,r5_data_out);

assign data_out = r5_data_out;

endmodule
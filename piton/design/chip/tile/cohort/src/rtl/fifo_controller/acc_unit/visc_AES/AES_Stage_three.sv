module AES_Stage_Three_Visc(
        input  logic[127:0] data_in,
        output logic[127:0] data_out,

        input  logic clk,
        input  logic rst_n,

        input  logic ready_in,

        output logic valid_out,

        input logic[127:0] key_s6,
        input logic[127:0] key_s7,
        input logic[127:0] key_s8,
        input logic[127:0] key_s9
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

logic[127:0] r5_data_out, r6_data_out, r7_data_out, r8_data_out, r9_data_out;

assign r5_data_out = data_in;

// AES_Stage_two
round_visc r6(clk,r5_data_out,key_s6,r6_data_out);
round_visc r7(clk,r6_data_out,key_s7,r7_data_out);
round_visc r8(clk,r7_data_out,key_s8,r8_data_out);
last_round_visc r9(clk,r8_data_out,key_s9,r9_data_out);

assign data_out = r9_data_out;

endmodule
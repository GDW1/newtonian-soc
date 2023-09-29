module aes_top #(parameter int number_inputs = 4, parameter int number_outputs = 9 * 2, parameter int stage = 0) (
	input                       				clk          , // Clock
	input                       				rst_n        , // Asynchronous reset active low
	input acc_pkg::acc_config_t 				acc_config   , // acclerator uncached configuration interface
	decoupled_vr_if.slave       				consumer_data,
	decoupled_vr_if.master      				producer_data,
	data_forward_if.data_forward_in 		    data_forward_in,
	data_forward_if.data_forward_out 		    data_forward_out,
	input logic[2:0]							bypass_control

);
	typedef enum logic [2:0] {S_IDLE, S_CONSUME, S_PUT, S_GET, S_PRODUCE} state_t;

	logic [63:0] data_n;

	state_t state, state_next;

	logic [$clog2(22) : 0] counter_n, counter_r;

	logic [63:0] data_in [22 - 1:0]  ;
	logic [63:0] data_out [22 - 1:0];
	
	logic valid_out;

	assign consumer_data.ready = state == S_CONSUME;
	assign producer_data.valid = state == S_PRODUCE;
	assign producer_data.data = (state == S_PRODUCE) ? data_out[counter_r] : '0;
	assign data_forward_in.rdy = state == S_PRODUCE;
	assign data_forward_in.data = data_out;

	always_ff @(posedge clk or negedge rst_n) begin : proc_state
		if(~rst_n) begin
			state <= S_IDLE;
			counter_r <= '0;
		end else begin
			state <= state_next;
			counter_r <= counter_n;
			if (state == S_CONSUME) begin
				if (bypass_control[1] == 1'b1) begin
					data_in[counter_r] <= data_n;
				end else begin
					data_in <= data_forward_out.data;
				end
			end
		end
	end
	
	always_comb begin : proc_state_next
		state_next = state;
		counter_n = counter_r;
		case (state)
			S_IDLE    : begin
				if (consumer_data.valid | (~bypass_control[1] & data_forward_out.rdy) == 1'b1) begin
					state_next = S_CONSUME;
					counter_n = 0;
				end
			end
			S_CONSUME : begin
				if (bypass_control[1] == 1'b1) begin
					if (consumer_data.valid & consumer_data.ready) begin
						data_n = consumer_data.data;
						counter_n = counter_r + 1;
						if (counter_r == number_inputs - 1) begin
							state_next = S_PUT;
						end
					end
				end else begin
					state_next = S_PUT;
				end
			end
			S_PUT: begin
				state_next = S_GET;
			end
			S_GET: begin
				if (valid_out) begin
					state_next = S_PRODUCE;
					counter_n = 0;
				end
			end
			S_PRODUCE : begin
				if (bypass_control[0] == 1'b1) begin
					if (producer_data.valid & producer_data.ready) begin
						counter_n = counter_r + 1;
						if (counter_r == number_outputs - 1) begin
							state_next = S_IDLE;
						end
					end
				end else begin
					state_next = S_IDLE;
				end
			end
		endcase
	end	


	/**
		BEGIN Assign outputs using modules here. Additionally valid out should be assigned.
	**/

    assign valid_out = 1'b1;
    generate;
        if (stage == 0) begin : key_expansion
            logic[127:0] key_in;
            logic[127:0] acc_data_in;

            always_comb begin : data_unmarshall
                acc_data_in = {data_in[0], data_in[1]};
                key_in = {data_in[2], data_in[3]};
            end

            logic[127:0] acc_data_out;
            logic[127:0] keys [9:0];

            wire [704-1:0] o_keys_out_higher;
            wire [704-1:0] o_keys_out_lower;
            wire [127:0] key_s,key_s0,key_s1,key_s2,key_s3,key_s4,key_s5,key_s6,key_s7,key_s8,key_s9;


            assign acc_data_out=acc_data_in^key_s;

            key_expansion_inner a0(
                .clk(clk),
                .i_key_lower(key_in[63:0]),
                .i_key_higher(key_in[127:64]),
                .o_keys_out_higher(o_keys_out_higher),
                .o_keys_out_lower(o_keys_out_lower)
            );

            assign key_s =  {o_keys_out_higher[63:0]   , o_keys_out_lower[63:0]   };
            assign key_s0 = {o_keys_out_higher[127:64] , o_keys_out_lower[127:64] };
            assign key_s1 = {o_keys_out_higher[191:128], o_keys_out_lower[191:128]};
            assign key_s2 = {o_keys_out_higher[255:192], o_keys_out_lower[255:192]};
            assign key_s3 = {o_keys_out_higher[319:256], o_keys_out_lower[319:256]};
            assign key_s4 = {o_keys_out_higher[383:320], o_keys_out_lower[383:320]};
            assign key_s5 = {o_keys_out_higher[447:384], o_keys_out_lower[447:384]};
            assign key_s6 = {o_keys_out_higher[511:448], o_keys_out_lower[511:448]};
            assign key_s7 = {o_keys_out_higher[575:512], o_keys_out_lower[575:512]};
            assign key_s8 = {o_keys_out_higher[639:576], o_keys_out_lower[639:576]};
            assign key_s9 = {o_keys_out_higher[703:640], o_keys_out_lower[703:640]};

            // put the outputs into the correct format

            assign data_out[0] = acc_data_out[63:0];
            assign data_out[1] = acc_data_out[127:64];

            assign data_out[2] = key_s0[63:0];
            assign data_out[3] = key_s0[127:64];

            assign data_out[4] = key_s1[63:0];
            assign data_out[5] = key_s1[127:64];

            assign data_out[6] = key_s2[63:0];
            assign data_out[7] = key_s2[127:64];

            assign data_out[8] = key_s3[63:0];
            assign data_out[9] = key_s3[127:64];

            assign data_out[10] = key_s4[63:0];
            assign data_out[11] = key_s4[127:64];

            assign data_out[12] = key_s5[63:0];
            assign data_out[13] = key_s5[127:64];
            
            assign data_out[14] = key_s6[63:0];
            assign data_out[15] = key_s6[127:64];

            assign data_out[16] = key_s7[63:0];
            assign data_out[17] = key_s7[127:64];

            assign data_out[18] = key_s8[63:0];
            assign data_out[19] = key_s8[127:64];

            assign data_out[20] = key_s9[63:0];
            assign data_out[21] = key_s9[127:64];

            // number of outputs = 22
        end else if (0 < stage < 10) begin : rounds_0_though_8 // There are nine stages 
            // We want to keep all keys the same so data out 2-21 are the same
            logic [127:0] acc_data_in;
            logic [127:0] keys [9:0];

            always_comb begin : data_unmarshall
                acc_data_in = {data_in[1], data_in[0]};
            end

            // we only need to unmarshall the keys for the i-th round
            logic [127:0] key_i;
            assign key_i = {data_in[((stage) * 2) + 1], data_in[(stage) * 2]};

            // assign data out 2-21 to be the same as data in 2-21
            genvar i;
            for (i = 2; i < 22; i++) begin : data_marshall
                assign data_out[i] = data_in[i];
            end

            logic [127:0] acc_data_out;

            round_visc r_i(clk,acc_data_in,key_i,acc_data_out);

            assign data_out[0] = acc_data_out[63:0];
            assign data_out[1] = acc_data_out[127:64];
        end else if (stage == 10) begin
            // We want to keep all keys the same so data out 2-21 are the same
            logic [127:0] acc_data_in;
            logic [127:0] keys [9:0];

            always_comb begin : data_unmarshall
                acc_data_in = {data_in[1], data_in[0]};
            end

            // we only need to unmarshall the keys for the i-th round
            logic [127:0] key_i;
            assign key_i = {data_in[((stage) * 2) + 1], data_in[(stage) * 2]};

            // assign data out 2-21 to be the same as data in 2-21
            genvar i;
            for (i = 2; i < 22; i++) begin : data_marshall
                assign data_out[i] = data_in[i];
            end

            logic [127:0] acc_data_out;

            last_round r_i(clk,acc_data_in,key_i,acc_data_out);

            assign data_out[0] = acc_data_out[63:0];
            assign data_out[1] = acc_data_out[127:64];
        end
        // if (stage == 0) begin : stage_0
        //     /* Define the inputs to the AES_Stage_One_Visc module*/
        //     logic[127:0] key_in;
        //     logic[127:0] acc_data_in;

        //     // put the inputs into the correct format
        //     always_comb begin : data_unmarshall
        //         acc_data_in = {data_in[0], data_in[1]};
        //         key_in = {data_in[2], data_in[3]};
        //     end

        //     /* define the outputs to the AES_Stage_One_Visc module*/   
        //     logic[127:0] acc_data_out;
        //     logic[127:0] keys [8:0];

        //     // instantiate the AES_Stage_One_Visc module 
        //     AES_Stage_One_Visc aes_stage_one_visc (
        //         .key_in(key_in),
        //         .data_in(acc_data_in),
        //         .data_out(acc_data_out),
        //         .clk(clk),
        //         .rst_n(rst_n),
        //         .ready_in(state == S_GET),
        //         .valid_out(valid_out),
        //         .key_s2_out(keys[0]),
        //         .key_s3_out(keys[1]),
        //         .key_s4_out(keys[2]),
        //         .key_s5_out(keys[3]),
        //         .key_s6_out(keys[4]),
        //         .key_s7_out(keys[5]),
        //         .key_s8_out(keys[6]),
        //         .key_s9_out(keys[7])
        //     );     

        //     // put the outputs into the correct format
            
        //     genvar i;
        //     assign data_out[0] = acc_data_out[63:0];
        //     assign data_out[1] = acc_data_out[127:64];
        //     for (i = 1; i < 9; i++) begin : data_marshall
        //         assign data_out[i * 2] = keys[i - 1][63:0];
        //         assign data_out[(i * 2) + 1] = keys[i - 1][127:64];
        //     end

        //     // number of outputs = 9 * 2 = 18
        // end else if (stage == 1) begin : stage_1
        //     /* Define the inputs to the AES_Stage_Two_Visc module*/
        //     logic[127:0] acc_data_in;
        //     logic[127:0] keys [8:0];

        //     // put the inputs into the correct format
        //     always_comb begin : data_unmarshall
        //         acc_data_in = {data_in[1], data_in[0]};
        //     end

        //     genvar i;
        //     for (i = 0; i < 8; i++) begin : data_marshall
        //         assign keys[i] = {data_in[((i+1) * 2) + 1], data_in[(i+1) * 2]};
        //     end

        //     /* define the outputs to the AES_Stage_Two_Visc module*/
        //     logic[127:0] acc_data_out;

        //     // instantiate the AES_Stage_Two_Visc module
        //     AES_Stage_Two_Visc aes_stage_two_visc (
        //         .data_in(acc_data_in),
        //         .data_out(acc_data_out),
        //         .clk(clk),
        //         .rst_n(rst_n),
        //         .ready_in(state == S_GET),
        //         .valid_out(valid_out),
        //         .key_s2(keys[0]),
        //         .key_s3(keys[1]),
        //         .key_s4(keys[2]),
        //         .key_s5(keys[3])
        //     );

        //     // put the outputs into the correct format
        //     assign data_out[0] = acc_data_out[63:0];
        //     assign data_out[1] = acc_data_out[127:64];
        //     assign data_out[2] = keys[4][63:0]; // keys_s6
        //     assign data_out[3] = keys[4][127:64]; // keys_s6
        //     assign data_out[4] = keys[5][63:0]; // keys_s7
        //     assign data_out[5] = keys[5][127:64]; // keys_s7
        //     assign data_out[6] = keys[6][63:0]; // keys_s8
        //     assign data_out[7] = keys[6][127:64]; // keys_s8
        //     assign data_out[8] = keys[7][63:0]; // keys_s9
        //     assign data_out[9] = keys[7][127:64]; // keys_s9    

        //     // number of ouputs = 10    
        // end else if (stage == 2) begin : stage_2
        //     /* Define the inputs to the AES_Stage_Three_Visc module*/
        //     logic[127:0] acc_data_in;
        //     logic[127:0] keys [3:0];

        //     // put the inputs into the correct format
        //     always_comb begin : data_unmarshall
        //         acc_data_in = {data_in[1], data_in[0]};
        //     end

        //     genvar i;
        //     for (i = 0; i < 4; i++) begin : data_marshall
        //         assign keys[i] = {data_in[((i+1) * 2) + 1], data_in[(i+1) * 2]};
        //     end

        //     /* define the outputs to the AES_Stage_Three_Visc module*/
        //     logic[127:0] acc_data_out;

        //     // instantiate the AES_Stage_Three_Visc module
        //     AES_Stage_Three_Visc aes_stage_three_visc (
        //         .data_in(acc_data_in),
        //         .data_out(acc_data_out),
        //         .clk(clk),
        //         .rst_n(rst_n),
        //         .ready_in(state == S_GET),
        //         .valid_out(valid_out),
        //         .key_s6(keys[0]),
        //         .key_s7(keys[1]),
        //         .key_s8(keys[2]),
        //         .key_s9(keys[3])
        //     );

        //     // put the outputs into the correct format
        //     assign data_out[0] = acc_data_out[63:0];
        //     assign data_out[1] = acc_data_out[127:64];
        // end
    endgenerate



	/**
		END Assign outputs using modules here. Additionally valid out should be assigned.
	**/


endmodule : aes_top

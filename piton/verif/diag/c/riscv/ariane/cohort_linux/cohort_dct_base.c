#include <stdio.h>
#include <string.h>

#include "util.h"

#ifndef FIFO_SIZE 
#define FIFO_SIZE 128
#endif

#ifndef REG_PER_CONSUMER
#define REG_PER_CONSUMER 8
#endif

#ifndef WAIT_COUNTER_VAL 
#define WAIT_COUNTER_VAL 16
#endif

#ifndef SERIALIZATION_VAL
#define SERIALIZATION_VAL 1
#endif

#ifndef DESERIALIZATION_VAL
#define DESERIALIZATION_VAL 1
#endif

#ifndef BACKOFF_COUNTER_VAL
#define BACKOFF_COUNTER_VAL 240
#endif

#ifndef LOOP_NUM
#define LOOP_NUM 20
#endif

#ifdef BARE_METAL
#define NO_DRIVER
#endif

#ifdef NO_DRIVER
    #include "cohort.h"
#else
    #include "cohort_uapi.h"
#endif

#define NUM_WORDS 32

#define NUM_QUEUES 5

#define BATCH_SIZE 128


extern uint64_t back_off_count;


struct _dct_8_stage_1_output {
uint64_t data_out[8];
};

typedef struct _dct_8_stage_1_output dct_8_stage_1_output;
dct_8_stage_1_output dct_8_stage_1(uint64_t data_in[8]);

struct _dct_8_stage_2_output {
uint64_t data_out[8];
};

typedef struct _dct_8_stage_2_output dct_8_stage_2_output;
dct_8_stage_2_output dct_8_stage_2(uint64_t data_in[8]);


struct _dct_8_stage_3_output {
uint64_t data_out[8];
};

typedef struct _dct_8_stage_3_output dct_8_stage_3_output;
dct_8_stage_3_output dct_8_stage_3(uint64_t data_in[8]);

struct _dct_8_stage_4_output {
uint64_t data_out[8];
};

typedef struct _dct_8_stage_4_output dct_8_stage_4_output;
dct_8_stage_4_output dct_8_stage_4(uint64_t data_in[8]);

struct _dct_8_stage_5_output {
uint64_t data_out[8];
};

typedef struct _dct_8_stage_5_output dct_8_stage_5_output;
dct_8_stage_5_output dct_8_stage_5(uint64_t data_in[8]);


static uint64_t A,D;
static uint32_t Dp[2] = {0x000000FF,0x000000AA};
static uint32_t Ap[NUM_WORDS/2] = {0x33221100,
                                                 0x77665544,
                                                 0xBBAA9988,
                                                 0xFFEEDDCC,
                                                 0x11111111,
                                                 0x22222222,
                                                 0x33333333,
                                                 0x44444444,
                                                 0x55555555,
                                                 0x66666666,
                                                 0x77777777,
                                                 0x88888888,
                                                 0x99999999,
                                                 0xAAAAAAAA,
                                                 0xBBBBBBBB,
                                                 0xCCCCCCCC};


#ifndef NUM_A
    #define NUM_A 1
#endif

void _kernel_(uint32_t id, uint32_t core_num){
    dec_open_producer(id);
    dec_open_consumer(id);
    // Enable Virtual Addr Translation
    dec_set_tlb_ptbase(0,0);

    A = (uint64_t)Ap | 0x7F00000000LL;
    D = (uint64_t)Dp | 0x7F00000000LL;

    uint64_t ppn = ((uint64_t)Ap >> 12);
    uint64_t vpn = ((uint64_t)A >> 12);
    print64("virtual page number", vpn);
    uint64_t mmpage = 0x0LL;
    mmpage |= ppn << 4; // [31:4] PPN
    mmpage |= vpn << 32; // [58:32] VPN
    // [63:62] is page size (1G,2M), flag bits are all 1's
    print64("Set TLB entry",mmpage);
    dec_set_tlb_mmpage(0, mmpage);

    //print64("Get TLB fault",dec_get_tlb_fault(0) );
    for (int i=0;i<1;i++){
        // Lower 4 bits {dirty bit, is_2M,is_1G,valid}
        // [31:4] is PPN
        // [59:32] is Asid, VPN
        // [63:60] Flag bits
        //print64("Get TLB entry",dec_snoop_tlb_entry(0) );
    }

    dec_set_base32(id,(void *)A);
    for (int i=0;i<4;i++){
        //dec_produce32(id,42);
        //dec_load32_asynci_llc(id,i%2);
        dec_atomic_fetch_add_asynci(id,i%2,1);
        __sync_synchronize;
        print32("A +1",dec_consume32(id));

        dec_set_base32(id,(void *)D);
        dec_atomic_compare_exchange_asynci(id,i%2, Dp[i%2], Dp[i%2]+1);
        print32("D CAS(+1)",dec_consume32(id));

        dec_set_base32(id,(void *)A);
        dec_atomic_compare_exchange_asynci(id,i%2, Ap[i%2], Ap[i%2]+1);
        print32("A CAS(+1)",dec_consume32(id));

        dec_load32_asynci(id,i%2);
        print32("TLoad A",dec_consume32(id));

        //__sync_synchronize; FENCE;
        //if (data!=A[0]) return 1;
    }
}


int cohort_set_tlb(uint64_t vpn, uint64_t ppn)
{
//    print64("virtual page number", vpn);
//    print64("physical page number", ppn);
    uint64_t mmpage = 0x0ULL;
    mmpage |= ppn << 4; // [31:4] PPN
    mmpage |= vpn << 32; // [58:32] VPN
    // [63:62] is page size (1G,2M), flag bits are all 1's
//    print64("Set TLB entry",mmpage);
    dec_set_tlb_mmpage(0, mmpage);
//    print64("Get TLB entry",dec_snoop_tlb_entry(0) );
}

int main(int argc, char ** argv) {

    volatile static uint32_t amo_cnt = 0;
    uint32_t id, core_num;
#ifdef BARE_METAL
    id = argv[0][0];
    core_num = argv[0][1];
#else
    id = 0;
    core_num = 1;
#endif
    // only make the first ariane initialize the tile
    if (id == 0) init_tile(NUM_A);
    if (id == 1) while(true);

// #ifdef BARE_METAL
//     cohort_set_tlb(0x80004, 0x80004);
//     cohort_set_tlb(0x80005, 0x80005);
// #endif

#ifdef NO_DRIVER
    const c_id_t c_id = 0;

    unsigned long long int write_value = 11;
    unsigned long long int serialization_value = 1;
    unsigned long long int deserialization_value = 1;
    unsigned long long int wait_counter = WAIT_COUNTER_VAL;
    unsigned long long int backoff_counter = BACKOFF_COUNTER_VAL;

    write_value |= backoff_counter << 48;
    write_value |= serialization_value << 32;
    write_value |= deserialization_value << 16;
    write_value |= wait_counter << 4;
    // zero out bits 2 and 3
    write_value &= 0xFFFFFFFFFFFFFFF3;
#else
    uint8_t c_id = 0;
#endif
    printf("Cohort ID: %d\n", c_id);
    printf("Number of Queues: %d\n", NUM_QUEUES);
    printf("FIFO Size: %d\n", FIFO_SIZE);
    
    // Create array of fifos
    fifo_ctrl_t *sw_to_cohort_fifo[NUM_QUEUES];
    fifo_ctrl_t *cohort_to_sw_fifo[NUM_QUEUES];

    for (int i = 0; i < NUM_QUEUES; i++) {
        sw_to_cohort_fifo[i] = fifo_init( FIFO_SIZE+1, 64, 0, c_id, i);
        cohort_to_sw_fifo[i] = fifo_init( FIFO_SIZE+1, 64, 1, c_id, i);
    }

    void *acc_address = memalign(128, 128);
    memset(acc_address, 0, 128);

    uint64_t bypass[NUM_QUEUES] = {0b00, 0b00, 0b00, 0b00, 0b01};
    bool active[NUM_QUEUES] =     {false, false, false, false, false};
#ifdef NO_DRIVER
    for (int i = 0; i < NUM_QUEUES; i++) {
        baremetal_write(0, 6 + (8 * i), (uint64_t) acc_address);
    }

    cohort_on(c_id);


    for (int i = 0; i < NUM_QUEUES; i++) {
        baremetal_write(0, 7 + (8 * i), (uint64_t) write_value | (bypass[i] << 2));
    }
#else
    // int beg_status_0 = syscall(258, (uint64_t) sw_to_cohort_fifo->head_ptr, (uint64_t) cohort_to_sw_fifo->head_ptr, (uint64_t) acc_address, BACKOFF_COUNTER_VAL, 0, 0b10);
    // int beg_status_1 = syscall(258, (uint64_t) sw_to_cohort_fifo_1->head_ptr, (uint64_t) cohort_to_sw_fifo_1->head_ptr, (uint64_t) acc_address, BACKOFF_COUNTER_VAL, 1, 0b01);

    for (int i = 0; i < NUM_QUEUES; i++) {
        int beg_status = syscall(258, (uint64_t) sw_to_cohort_fifo[i]->head_ptr, (uint64_t) cohort_to_sw_fifo[i]->head_ptr, (uint64_t) acc_address, BACKOFF_COUNTER_VAL, i, bypass[i]);
    }
#endif

    // Set Data
    uint64_t temp[BATCH_SIZE];

    for (int i = 0; i < BATCH_SIZE; i++) {
        temp[i] = i;
    }

    printf("Starting NNHW process\n");

    unsigned long _c = -rdcycle();

	dec_open_producer(0);
	dec_open_consumer(0);

    /*
        BEGIN PUSH DATA
    */

    for (int i = 0; i < NUM_QUEUES; i++) {
        if (!active[i]){
			printf("processing stage %d om software\n", i);
            uint64_t data_in[8];
            for (int j = 0; j < 8; j++) {
                data_in[j] = temp[j];
            }
            if (i == 0) {
                dct_8_stage_1_output output = dct_8_stage_1(data_in);
                for (int j = 0; j < 8; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if(i == 1) {
                dct_8_stage_2_output output = dct_8_stage_2(data_in);
                for (int j = 0; j < 8; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 2) {
                dct_8_stage_3_output output = dct_8_stage_3(data_in);
                for (int j = 0; j < 8; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 3) {
                dct_8_stage_4_output output = dct_8_stage_4(data_in);
                for (int j = 0; j < 8; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 4) {
                dct_8_stage_5_output output = dct_8_stage_5(data_in);
                for (int j = 0; j < 8; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            }
        } else {
            if ((bypass[i] & 0x2) == 0x2) {
				#ifdef NO_DRIVER
					printf("Pushing data to queue %d\n", i);
				#endif
                for (int j = 0; j < BATCH_SIZE; j++) {
                    fifo_push_64(temp[j], sw_to_cohort_fifo[i], j);
                }
                fifo_push_sync(sw_to_cohort_fifo[i], BATCH_SIZE);
            }
            if ((bypass[i] & 0x1) == 0x1) {
				#ifdef NO_DRIVER
                	printf("Popping data from queue %d\n", i);
				#endif
                for (int j = 0; j < BATCH_SIZE; j++) {
                    temp[j] = fifo_pop_64(cohort_to_sw_fifo[i], j);
                    // printf("temp[%d]: %lx\n", j, temp[j]);
                }
                fifo_pop_sync(cohort_to_sw_fifo[i], BATCH_SIZE);
            }
        }
		// print out temp

    }
	printf("temp: {");
	for (int j = 0; j < BATCH_SIZE; j++) {
		printf("0x%x\n, ", temp[j]);
	}
	printf("}\n");
    /*
        END PUSH DATA
    */
    
	dec_close_producer(0);
	dec_close_consumer(0);
    
    _c += rdcycle();

    // print out the number of cycles and instructions
    printf("Cycles: %ld\n", _c);


    // print_stats_fifos(1);


#ifdef NO_DRIVER
    cohort_off(c_id);
#else
    int status = syscall(257);
#endif
    for (int i = 0; i < NUM_QUEUES; i++) {
        fifo_deinit(sw_to_cohort_fifo[i]);
        fifo_deinit(cohort_to_sw_fifo[i]);
    }
    free(acc_address);

	return 0;
}

dct_8_stage_1_output dct_8_stage_1(uint64_t data_in[8]) {
	uint64_t data_out[8];
	// Combinational function
for(uint64_t i = 0; i < 4; i++) {
uint64_t _temp0 = data_in[i];
uint64_t input_1 = _temp0;
uint64_t _temp1 = 0x8;
uint64_t _temp2 = _temp1 - i;
uint64_t _temp3 = data_in[_temp2];
uint64_t input_2 = _temp3;
uint64_t _temp4 = input_1 + input_2;
uint64_t output_1 = _temp4;
uint64_t _temp5 = input_1 - input_2;
uint64_t output_2 = _temp5;
data_out[i] = output_1;
uint64_t _temp7 = 0x8;
uint64_t _temp8 = _temp7 - i;
data_out[_temp8] = output_2;
}

dct_8_stage_1_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8_stage_2_output dct_8_stage_2(uint64_t data_in[8]) {
	uint64_t data_out[8];
	// Combinational function
uint64_t _temp0 = 0x0;
uint64_t _temp1 = data_in[_temp0];
uint64_t input_0 = _temp1;
uint64_t _temp2 = 0x1;
uint64_t _temp3 = data_in[_temp2];
uint64_t input_1 = _temp3;
uint64_t _temp4 = 0x2;
uint64_t _temp5 = data_in[_temp4];
uint64_t input_2 = _temp5;
uint64_t _temp6 = 0x3;
uint64_t _temp7 = data_in[_temp6];
uint64_t input_3 = _temp7;
uint64_t _temp8 = 0x4;
uint64_t _temp9 = data_in[_temp8];
uint64_t input_4 = _temp9;
uint64_t _temp10 = 0x5;
uint64_t _temp11 = data_in[_temp10];
uint64_t input_5 = _temp11;
uint64_t _temp12 = 0x6;
uint64_t _temp13 = data_in[_temp12];
uint64_t input_6 = _temp13;
uint64_t _temp14 = 0x7;
uint64_t _temp15 = data_in[_temp14];
uint64_t input_7 = _temp15;
uint64_t _temp16 = input_0 + input_3;
uint64_t a01 = _temp16;
uint64_t _temp17 = input_1 + input_2;
uint64_t a11 = _temp17;
uint64_t _temp18 = input_1 - input_2;
uint64_t a21 = _temp18;
uint64_t _temp19 = input_0 - input_3;
uint64_t a31 = _temp19;
uint64_t _temp20 = input_4 + input_5;
uint64_t neg_a41 = _temp20;
uint64_t _temp21 = input_5 + input_6;
uint64_t a51 = _temp21;
uint64_t _temp22 = input_6 + input_7;
uint64_t a61 = _temp22;
uint64_t _temp23 = 0x0;
data_out[_temp23] = a01;
uint64_t _temp25 = 0x1;
data_out[_temp25] = a11;
uint64_t _temp27 = 0x2;
data_out[_temp27] = a21;
uint64_t _temp29 = 0x3;
data_out[_temp29] = a31;
uint64_t _temp31 = 0x4;
data_out[_temp31] = neg_a41;
uint64_t _temp33 = 0x5;
data_out[_temp33] = a51;
uint64_t _temp35 = 0x6;
data_out[_temp35] = a61;
uint64_t _temp37 = 0x7;
uint64_t _temp38 = 0x7;
uint64_t _temp39 = data_in[_temp38];
data_out[_temp37] = _temp39;

dct_8_stage_2_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8_stage_3_output dct_8_stage_3(uint64_t data_in[8]) {
	uint64_t data_out[8];
	// Combinational function
uint64_t _temp0 = 0x0;
uint64_t _temp1 = data_in[_temp0];
uint64_t input_0 = _temp1;
uint64_t _temp2 = 0x1;
uint64_t _temp3 = data_in[_temp2];
uint64_t input_1 = _temp3;
uint64_t _temp4 = 0x2;
uint64_t _temp5 = data_in[_temp4];
uint64_t input_2 = _temp5;
uint64_t _temp6 = 0x3;
uint64_t _temp7 = data_in[_temp6];
uint64_t input_3 = _temp7;
uint64_t _temp8 = 0x4;
uint64_t _temp9 = data_in[_temp8];
uint64_t input_4 = _temp9;
uint64_t _temp10 = 0x5;
uint64_t _temp11 = data_in[_temp10];
uint64_t input_5 = _temp11;
uint64_t _temp12 = 0x6;
uint64_t _temp13 = data_in[_temp12];
uint64_t input_6 = _temp13;
uint64_t _temp14 = 0x7;
uint64_t _temp15 = data_in[_temp14];
uint64_t input_7 = _temp15;
uint64_t _temp16 = input_2 + input_3;
uint64_t a22 = _temp16;
uint64_t _temp17 = 0x0;
uint64_t _temp18 = 0x0;
uint64_t _temp19 = data_in[_temp18];
data_out[_temp17] = _temp19;
uint64_t _temp21 = 0x1;
uint64_t _temp22 = 0x1;
uint64_t _temp23 = data_in[_temp22];
data_out[_temp21] = _temp23;
uint64_t _temp25 = 0x2;
data_out[_temp25] = a22;
uint64_t _temp27 = 0x3;
uint64_t _temp28 = 0x3;
uint64_t _temp29 = data_in[_temp28];
data_out[_temp27] = _temp29;
uint64_t _temp31 = 0x4;
uint64_t _temp32 = 0x4;
uint64_t _temp33 = data_in[_temp32];
data_out[_temp31] = _temp33;
uint64_t _temp35 = 0x5;
uint64_t _temp36 = 0x5;
uint64_t _temp37 = data_in[_temp36];
data_out[_temp35] = _temp37;
uint64_t _temp39 = 0x6;
uint64_t _temp40 = 0x6;
uint64_t _temp41 = data_in[_temp40];
data_out[_temp39] = _temp41;
uint64_t _temp43 = 0x7;
uint64_t _temp44 = 0x7;
uint64_t _temp45 = data_in[_temp44];
data_out[_temp43] = _temp45;

dct_8_stage_3_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8_stage_4_output dct_8_stage_4(uint64_t data_in[8]) {
	uint64_t data_out[8];
	// Combinational function
uint64_t _temp0 = 0x0;
uint64_t _temp1 = data_in[_temp0];
uint64_t input_0 = _temp1;
uint64_t _temp2 = 0x1;
uint64_t _temp3 = data_in[_temp2];
uint64_t input_1 = _temp3;
uint64_t _temp4 = 0x2;
uint64_t _temp5 = data_in[_temp4];
uint64_t input_2 = _temp5;
uint64_t _temp6 = 0x3;
uint64_t _temp7 = data_in[_temp6];
uint64_t input_3 = _temp7;
uint64_t _temp8 = 0x4;
uint64_t _temp9 = data_in[_temp8];
uint64_t input_4 = _temp9;
uint64_t _temp10 = 0x5;
uint64_t _temp11 = data_in[_temp10];
uint64_t input_5 = _temp11;
uint64_t _temp12 = 0x6;
uint64_t _temp13 = data_in[_temp12];
uint64_t input_6 = _temp13;
uint64_t _temp14 = 0x7;
uint64_t _temp15 = data_in[_temp14];
uint64_t input_7 = _temp15;
uint64_t _temp16 = 0x10;
uint64_t A1 = _temp16;
uint64_t _temp17 = 0x10;
uint64_t A2 = _temp17;
uint64_t _temp18 = 0x10;
uint64_t A3 = _temp18;
uint64_t _temp19 = 0x10;
uint64_t A4 = _temp19;
uint64_t _temp20 = 0x10;
uint64_t A5 = _temp20;
uint64_t _temp21 = input_2 * A1;
uint64_t a23 = _temp21;
uint64_t _temp22 = input_6 - input_4;
uint64_t _temp23 = _temp22 * A5;
uint64_t mul5 = _temp23;
uint64_t _temp24 = input_4 * A2;
uint64_t _temp25 = _temp24 - mul5;
uint64_t a43 = _temp25;
uint64_t _temp26 = input_5 * A3;
uint64_t a53 = _temp26;
uint64_t _temp27 = input_6 * A4;
uint64_t _temp28 = _temp27 - mul5;
uint64_t a63 = _temp28;
uint64_t _temp29 = 0x0;
uint64_t _temp30 = 0x0;
uint64_t _temp31 = data_in[_temp30];
data_out[_temp29] = _temp31;
uint64_t _temp33 = 0x1;
uint64_t _temp34 = 0x1;
uint64_t _temp35 = data_in[_temp34];
data_out[_temp33] = _temp35;
uint64_t _temp37 = 0x2;
data_out[_temp37] = a23;
uint64_t _temp39 = 0x3;
uint64_t _temp40 = 0x3;
uint64_t _temp41 = data_in[_temp40];
data_out[_temp39] = _temp41;
uint64_t _temp43 = 0x4;
data_out[_temp43] = a43;
uint64_t _temp45 = 0x5;
data_out[_temp45] = a53;
uint64_t _temp47 = 0x6;
data_out[_temp47] = a63;
uint64_t _temp49 = 0x7;
uint64_t _temp50 = 0x7;
uint64_t _temp51 = data_in[_temp50];
data_out[_temp49] = _temp51;

dct_8_stage_4_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8_stage_5_output dct_8_stage_5(uint64_t data_in[8]) {
	uint64_t data_out[8];
	// Combinational function
uint64_t _temp0 = 0x0;
uint64_t _temp1 = data_in[_temp0];
uint64_t input_0 = _temp1;
uint64_t _temp2 = 0x1;
uint64_t _temp3 = data_in[_temp2];
uint64_t input_1 = _temp3;
uint64_t _temp4 = 0x2;
uint64_t _temp5 = data_in[_temp4];
uint64_t input_2 = _temp5;
uint64_t _temp6 = 0x3;
uint64_t _temp7 = data_in[_temp6];
uint64_t input_3 = _temp7;
uint64_t _temp8 = 0x4;
uint64_t _temp9 = data_in[_temp8];
uint64_t input_4 = _temp9;
uint64_t _temp10 = 0x5;
uint64_t _temp11 = data_in[_temp10];
uint64_t input_5 = _temp11;
uint64_t _temp12 = 0x6;
uint64_t _temp13 = data_in[_temp12];
uint64_t input_6 = _temp13;
uint64_t _temp14 = 0x7;
uint64_t _temp15 = data_in[_temp14];
uint64_t input_7 = _temp15;
uint64_t _temp16 = input_7 + input_5;
uint64_t a54 = _temp16;
uint64_t _temp17 = input_7 - input_5;
uint64_t a74 = _temp17;
uint64_t _temp18 = 0x0;
uint64_t _temp19 = input_0 + input_1;
data_out[_temp18] = _temp19;
uint64_t _temp21 = 0x4;
uint64_t _temp22 = input_0 - input_1;
data_out[_temp21] = _temp22;
uint64_t _temp24 = 0x2;
uint64_t _temp25 = input_3 + input_2;
data_out[_temp24] = _temp25;
uint64_t _temp27 = 0x6;
uint64_t _temp28 = input_3 - input_2;
data_out[_temp27] = _temp28;
uint64_t _temp30 = 0x5;
uint64_t _temp31 = a74 + input_4;
data_out[_temp30] = _temp31;
uint64_t _temp33 = 0x1;
uint64_t _temp34 = a54 + input_6;
data_out[_temp33] = _temp34;
uint64_t _temp36 = 0x7;
uint64_t _temp37 = a54 - input_6;
data_out[_temp36] = _temp37;
uint64_t _temp39 = 0x3;
uint64_t _temp40 = a74 - input_4;
data_out[_temp39] = _temp40;

dct_8_stage_5_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

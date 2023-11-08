#include <stdio.h>
#include <string.h>

#include "util.h"

#ifndef FIFO_SIZE 
#define FIFO_SIZE 64
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

#define NUM_QUEUES 10

#define BATCH_SIZE 64


extern uint64_t back_off_count;


struct _dct_8x8_stage_1_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_1_output dct_8x8_stage_1_output;
dct_8x8_stage_1_output dct_8x8_stage_1(uint32_t data_in[64]);

struct _dct_8x8_stage_2_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_2_output dct_8x8_stage_2_output;
dct_8x8_stage_2_output dct_8x8_stage_2(uint32_t data_in[64]);


struct _dct_8x8_stage_3_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_3_output dct_8x8_stage_3_output;
dct_8x8_stage_3_output dct_8x8_stage_3(uint32_t data_in[64]);

struct _dct_8x8_stage_4_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_4_output dct_8x8_stage_4_output;
dct_8x8_stage_4_output dct_8x8_stage_4(uint32_t data_in[64]);

struct _dct_8x8_stage_5_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_5_output dct_8x8_stage_5_output;
dct_8x8_stage_5_output dct_8x8_stage_5(uint32_t data_in[64]);

struct _dct_8x8_stage_6_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_6_output dct_8x8_stage_6_output;
dct_8x8_stage_6_output dct_8x8_stage_6(uint32_t data_in[64]);

struct _dct_8x8_stage_7_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_7_output dct_8x8_stage_7_output;
dct_8x8_stage_7_output dct_8x8_stage_7(uint32_t data_in[64]);

struct _dct_8x8_stage_8_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_8_output dct_8x8_stage_8_output;
dct_8x8_stage_8_output dct_8x8_stage_8(uint32_t data_in[64]);

struct _dct_8x8_stage_9_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_9_output dct_8x8_stage_9_output;
dct_8x8_stage_9_output dct_8x8_stage_9(uint32_t data_in[64]);

struct _dct_8x8_stage_10_output {
uint32_t data_out[64];
};

typedef struct _dct_8x8_stage_10_output dct_8x8_stage_10_output;
dct_8x8_stage_10_output dct_8x8_stage_10(uint32_t data_in[64]);

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
        sw_to_cohort_fifo[i] = fifo_init( FIFO_SIZE+1, 32, 0, c_id, i);
        cohort_to_sw_fifo[i] = fifo_init( FIFO_SIZE+1, 32, 1, c_id, i);
    }

    void *acc_address = memalign(128, 128);
    memset(acc_address, 0, 128);

    uint64_t bypass[NUM_QUEUES] = {0b00, 0b10, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01};
    bool active[NUM_QUEUES] =     {false, true, true, true, true, true, true, true, true, true};
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
    uint32_t temp[BATCH_SIZE];

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
            uint32_t data_in[64];
            for (int j = 0; j < 64; j++) {
                data_in[j] = temp[j];
            }
            if (i == 0) {
                dct_8x8_stage_1_output output = dct_8x8_stage_1(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if(i == 1) {
                dct_8x8_stage_2_output output = dct_8x8_stage_2(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 2) {
                dct_8x8_stage_3_output output = dct_8x8_stage_3(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 3) {
                dct_8x8_stage_4_output output = dct_8x8_stage_4(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 4) {
                dct_8x8_stage_5_output output = dct_8x8_stage_5(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if(i == 5) {
                dct_8x8_stage_6_output output = dct_8x8_stage_6(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 6) {
                dct_8x8_stage_7_output output = dct_8x8_stage_7(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 7) {
                dct_8x8_stage_8_output output = dct_8x8_stage_8(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 8) {
                dct_8x8_stage_9_output output = dct_8x8_stage_9(data_in);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out[j];
                    
                }
            } else if (i == 9) {
                dct_8x8_stage_10_output output = dct_8x8_stage_10(data_in);
                for (int j = 0; j < 64; j++) {
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

#include <stdio.h>
#include "util.h"

dct_8x8_stage_1_output dct_8x8_stage_1(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp0 = 0x8;
uint32_t _temp1 = _temp0 * i;
uint32_t _temp2 = data_in[_temp1];
uint32_t input_0 = _temp2;
uint32_t _temp3 = 0x8;
uint32_t _temp4 = _temp3 * i;
uint32_t _temp5 = 0x1;
uint32_t _temp6 = _temp4 + _temp5;
uint32_t _temp7 = data_in[_temp6];
uint32_t input_1 = _temp7;
uint32_t _temp8 = 0x8;
uint32_t _temp9 = _temp8 * i;
uint32_t _temp10 = 0x2;
uint32_t _temp11 = _temp9 + _temp10;
uint32_t _temp12 = data_in[_temp11];
uint32_t input_2 = _temp12;
uint32_t _temp13 = 0x8;
uint32_t _temp14 = _temp13 * i;
uint32_t _temp15 = 0x3;
uint32_t _temp16 = _temp14 + _temp15;
uint32_t _temp17 = data_in[_temp16];
uint32_t input_3 = _temp17;
uint32_t _temp18 = 0x8;
uint32_t _temp19 = _temp18 * i;
uint32_t _temp20 = 0x4;
uint32_t _temp21 = _temp19 + _temp20;
uint32_t _temp22 = data_in[_temp21];
uint32_t input_4 = _temp22;
uint32_t _temp23 = 0x8;
uint32_t _temp24 = _temp23 * i;
uint32_t _temp25 = 0x5;
uint32_t _temp26 = _temp24 + _temp25;
uint32_t _temp27 = data_in[_temp26];
uint32_t input_5 = _temp27;
uint32_t _temp28 = 0x8;
uint32_t _temp29 = _temp28 * i;
uint32_t _temp30 = 0x6;
uint32_t _temp31 = _temp29 + _temp30;
uint32_t _temp32 = data_in[_temp31];
uint32_t input_6 = _temp32;
uint32_t _temp33 = 0x8;
uint32_t _temp34 = _temp33 * i;
uint32_t _temp35 = 0x7;
uint32_t _temp36 = _temp34 + _temp35;
uint32_t _temp37 = data_in[_temp36];
uint32_t input_7 = _temp37;
uint32_t _temp38 = input_0 + input_7;
uint32_t a00 = _temp38;
uint32_t _temp39 = input_1 + input_6;
uint32_t a10 = _temp39;
uint32_t _temp40 = input_2 + input_5;
uint32_t a20 = _temp40;
uint32_t _temp41 = input_3 + input_4;
uint32_t a30 = _temp41;
uint32_t _temp42 = input_3 - input_4;
uint32_t a40 = _temp42;
uint32_t _temp43 = input_2 - input_5;
uint32_t a50 = _temp43;
uint32_t _temp44 = input_1 - input_6;
uint32_t a60 = _temp44;
uint32_t _temp45 = input_0 - input_7;
uint32_t a70 = _temp45;
uint32_t _temp46 = 0x8;
uint32_t _temp47 = _temp46 * i;
data_out[_temp47] = a00;
uint32_t _temp49 = 0x8;
uint32_t _temp50 = _temp49 * i;
uint32_t _temp51 = 0x1;
uint32_t _temp52 = _temp50 + _temp51;
data_out[_temp52] = a10;
uint32_t _temp54 = 0x8;
uint32_t _temp55 = _temp54 * i;
uint32_t _temp56 = 0x2;
uint32_t _temp57 = _temp55 + _temp56;
data_out[_temp57] = a20;
uint32_t _temp59 = 0x8;
uint32_t _temp60 = _temp59 * i;
uint32_t _temp61 = 0x3;
uint32_t _temp62 = _temp60 + _temp61;
data_out[_temp62] = a30;
uint32_t _temp64 = 0x8;
uint32_t _temp65 = _temp64 * i;
uint32_t _temp66 = 0x4;
uint32_t _temp67 = _temp65 + _temp66;
data_out[_temp67] = a40;
uint32_t _temp69 = 0x8;
uint32_t _temp70 = _temp69 * i;
uint32_t _temp71 = 0x5;
uint32_t _temp72 = _temp70 + _temp71;
data_out[_temp72] = a50;
uint32_t _temp74 = 0x8;
uint32_t _temp75 = _temp74 * i;
uint32_t _temp76 = 0x6;
uint32_t _temp77 = _temp75 + _temp76;
data_out[_temp77] = a60;
uint32_t _temp79 = 0x8;
uint32_t _temp80 = _temp79 * i;
uint32_t _temp81 = 0x7;
uint32_t _temp82 = _temp80 + _temp81;
data_out[_temp82] = a70;
}

dct_8x8_stage_1_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_2_output dct_8x8_stage_2(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp0 = 0x8;
uint32_t _temp1 = _temp0 * i;
uint32_t _temp2 = data_in[_temp1];
uint32_t input_0 = _temp2;
uint32_t _temp3 = 0x8;
uint32_t _temp4 = _temp3 * i;
uint32_t _temp5 = 0x1;
uint32_t _temp6 = _temp4 + _temp5;
uint32_t _temp7 = data_in[_temp6];
uint32_t input_1 = _temp7;
uint32_t _temp8 = 0x8;
uint32_t _temp9 = _temp8 * i;
uint32_t _temp10 = 0x2;
uint32_t _temp11 = _temp9 + _temp10;
uint32_t _temp12 = data_in[_temp11];
uint32_t input_2 = _temp12;
uint32_t _temp13 = 0x8;
uint32_t _temp14 = _temp13 * i;
uint32_t _temp15 = 0x3;
uint32_t _temp16 = _temp14 + _temp15;
uint32_t _temp17 = data_in[_temp16];
uint32_t input_3 = _temp17;
uint32_t _temp18 = 0x8;
uint32_t _temp19 = _temp18 * i;
uint32_t _temp20 = 0x4;
uint32_t _temp21 = _temp19 + _temp20;
uint32_t _temp22 = data_in[_temp21];
uint32_t input_4 = _temp22;
uint32_t _temp23 = 0x8;
uint32_t _temp24 = _temp23 * i;
uint32_t _temp25 = 0x5;
uint32_t _temp26 = _temp24 + _temp25;
uint32_t _temp27 = data_in[_temp26];
uint32_t input_5 = _temp27;
uint32_t _temp28 = 0x8;
uint32_t _temp29 = _temp28 * i;
uint32_t _temp30 = 0x6;
uint32_t _temp31 = _temp29 + _temp30;
uint32_t _temp32 = data_in[_temp31];
uint32_t input_6 = _temp32;
uint32_t _temp33 = 0x8;
uint32_t _temp34 = _temp33 * i;
uint32_t _temp35 = 0x7;
uint32_t _temp36 = _temp34 + _temp35;
uint32_t _temp37 = data_in[_temp36];
uint32_t input_7 = _temp37;
uint32_t _temp38 = input_0 + input_3;
uint32_t a01 = _temp38;
uint32_t _temp39 = input_1 + input_2;
uint32_t a11 = _temp39;
uint32_t _temp40 = input_1 - input_2;
uint32_t a21 = _temp40;
uint32_t _temp41 = input_0 - input_3;
uint32_t a31 = _temp41;
uint32_t _temp42 = input_4 + input_5;
uint32_t neg_a41 = _temp42;
uint32_t _temp43 = input_5 + input_6;
uint32_t a51 = _temp43;
uint32_t _temp44 = input_6 + input_7;
uint32_t a61 = _temp44;
uint32_t _temp45 = 0x8;
uint32_t _temp46 = _temp45 * i;
data_out[_temp46] = a01;
uint32_t _temp48 = 0x8;
uint32_t _temp49 = _temp48 * i;
uint32_t _temp50 = 0x1;
uint32_t _temp51 = _temp49 + _temp50;
data_out[_temp51] = a11;
uint32_t _temp53 = 0x8;
uint32_t _temp54 = _temp53 * i;
uint32_t _temp55 = 0x2;
uint32_t _temp56 = _temp54 + _temp55;
data_out[_temp56] = a21;
uint32_t _temp58 = 0x8;
uint32_t _temp59 = _temp58 * i;
uint32_t _temp60 = 0x3;
uint32_t _temp61 = _temp59 + _temp60;
data_out[_temp61] = a31;
uint32_t _temp63 = 0x8;
uint32_t _temp64 = _temp63 * i;
uint32_t _temp65 = 0x4;
uint32_t _temp66 = _temp64 + _temp65;
data_out[_temp66] = neg_a41;
uint32_t _temp68 = 0x8;
uint32_t _temp69 = _temp68 * i;
uint32_t _temp70 = 0x5;
uint32_t _temp71 = _temp69 + _temp70;
data_out[_temp71] = a51;
uint32_t _temp73 = 0x8;
uint32_t _temp74 = _temp73 * i;
uint32_t _temp75 = 0x6;
uint32_t _temp76 = _temp74 + _temp75;
data_out[_temp76] = a61;
uint32_t _temp78 = 0x8;
uint32_t _temp79 = _temp78 * i;
uint32_t _temp80 = 0x7;
uint32_t _temp81 = _temp79 + _temp80;
data_out[_temp81] = input_7;
}

dct_8x8_stage_2_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_3_output dct_8x8_stage_3(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp0 = 0x8;
uint32_t _temp1 = _temp0 * i;
uint32_t _temp2 = 0x2;
uint32_t _temp3 = _temp1 + _temp2;
uint32_t _temp4 = data_in[_temp3];
uint32_t input_2 = _temp4;
uint32_t _temp5 = 0x8;
uint32_t _temp6 = _temp5 * i;
uint32_t _temp7 = 0x3;
uint32_t _temp8 = _temp6 + _temp7;
uint32_t _temp9 = data_in[_temp8];
uint32_t input_3 = _temp9;
uint32_t _temp10 = input_2 + input_3;
uint32_t a22 = _temp10;
uint32_t _temp11 = 0x8;
uint32_t _temp12 = _temp11 * i;
uint32_t _temp13 = 0x8;
uint32_t _temp14 = _temp13 * i;
uint32_t _temp15 = data_in[_temp14];
data_out[_temp12] = _temp15;
uint32_t _temp17 = 0x8;
uint32_t _temp18 = _temp17 * i;
uint32_t _temp19 = 0x1;
uint32_t _temp20 = _temp18 + _temp19;
uint32_t _temp21 = 0x8;
uint32_t _temp22 = _temp21 * i;
uint32_t _temp23 = 0x1;
uint32_t _temp24 = _temp22 + _temp23;
uint32_t _temp25 = data_in[_temp24];
data_out[_temp20] = _temp25;
uint32_t _temp27 = 0x8;
uint32_t _temp28 = _temp27 * i;
uint32_t _temp29 = 0x2;
uint32_t _temp30 = _temp28 + _temp29;
data_out[_temp30] = a22;
uint32_t _temp32 = 0x8;
uint32_t _temp33 = _temp32 * i;
uint32_t _temp34 = 0x3;
uint32_t _temp35 = _temp33 + _temp34;
uint32_t _temp36 = 0x8;
uint32_t _temp37 = _temp36 * i;
uint32_t _temp38 = 0x3;
uint32_t _temp39 = _temp37 + _temp38;
uint32_t _temp40 = data_in[_temp39];
data_out[_temp35] = _temp40;
uint32_t _temp42 = 0x8;
uint32_t _temp43 = _temp42 * i;
uint32_t _temp44 = 0x4;
uint32_t _temp45 = _temp43 + _temp44;
uint32_t _temp46 = 0x8;
uint32_t _temp47 = _temp46 * i;
uint32_t _temp48 = 0x4;
uint32_t _temp49 = _temp47 + _temp48;
uint32_t _temp50 = data_in[_temp49];
data_out[_temp45] = _temp50;
uint32_t _temp52 = 0x8;
uint32_t _temp53 = _temp52 * i;
uint32_t _temp54 = 0x5;
uint32_t _temp55 = _temp53 + _temp54;
uint32_t _temp56 = 0x8;
uint32_t _temp57 = _temp56 * i;
uint32_t _temp58 = 0x5;
uint32_t _temp59 = _temp57 + _temp58;
uint32_t _temp60 = data_in[_temp59];
data_out[_temp55] = _temp60;
uint32_t _temp62 = 0x8;
uint32_t _temp63 = _temp62 * i;
uint32_t _temp64 = 0x6;
uint32_t _temp65 = _temp63 + _temp64;
uint32_t _temp66 = 0x8;
uint32_t _temp67 = _temp66 * i;
uint32_t _temp68 = 0x6;
uint32_t _temp69 = _temp67 + _temp68;
uint32_t _temp70 = data_in[_temp69];
data_out[_temp65] = _temp70;
uint32_t _temp72 = 0x8;
uint32_t _temp73 = _temp72 * i;
uint32_t _temp74 = 0x7;
uint32_t _temp75 = _temp73 + _temp74;
uint32_t _temp76 = 0x8;
uint32_t _temp77 = _temp76 * i;
uint32_t _temp78 = 0x7;
uint32_t _temp79 = _temp77 + _temp78;
uint32_t _temp80 = data_in[_temp79];
data_out[_temp75] = _temp80;
}

dct_8x8_stage_3_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_4_output dct_8x8_stage_4(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
uint32_t _temp0 = 0x10;
uint32_t A1 = _temp0;
uint32_t _temp1 = 0x10;
uint32_t A2 = _temp1;
uint32_t _temp2 = 0x10;
uint32_t A3 = _temp2;
uint32_t _temp3 = 0x10;
uint32_t A4 = _temp3;
uint32_t _temp4 = 0x10;
uint32_t A5 = _temp4;
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp5 = 0x8;
uint32_t _temp6 = _temp5 * i;
uint32_t _temp7 = data_in[_temp6];
uint32_t input_0 = _temp7;
uint32_t _temp8 = 0x8;
uint32_t _temp9 = _temp8 * i;
uint32_t _temp10 = 0x1;
uint32_t _temp11 = _temp9 + _temp10;
uint32_t _temp12 = data_in[_temp11];
uint32_t input_1 = _temp12;
uint32_t _temp13 = 0x8;
uint32_t _temp14 = _temp13 * i;
uint32_t _temp15 = 0x2;
uint32_t _temp16 = _temp14 + _temp15;
uint32_t _temp17 = data_in[_temp16];
uint32_t input_2 = _temp17;
uint32_t _temp18 = 0x8;
uint32_t _temp19 = _temp18 * i;
uint32_t _temp20 = 0x3;
uint32_t _temp21 = _temp19 + _temp20;
uint32_t _temp22 = data_in[_temp21];
uint32_t input_3 = _temp22;
uint32_t _temp23 = 0x8;
uint32_t _temp24 = _temp23 * i;
uint32_t _temp25 = 0x4;
uint32_t _temp26 = _temp24 + _temp25;
uint32_t _temp27 = data_in[_temp26];
uint32_t input_4 = _temp27;
uint32_t _temp28 = 0x8;
uint32_t _temp29 = _temp28 * i;
uint32_t _temp30 = 0x5;
uint32_t _temp31 = _temp29 + _temp30;
uint32_t _temp32 = data_in[_temp31];
uint32_t input_5 = _temp32;
uint32_t _temp33 = 0x8;
uint32_t _temp34 = _temp33 * i;
uint32_t _temp35 = 0x6;
uint32_t _temp36 = _temp34 + _temp35;
uint32_t _temp37 = data_in[_temp36];
uint32_t input_6 = _temp37;
uint32_t _temp38 = 0x8;
uint32_t _temp39 = _temp38 * i;
uint32_t _temp40 = 0x7;
uint32_t _temp41 = _temp39 + _temp40;
uint32_t _temp42 = data_in[_temp41];
uint32_t input_7 = _temp42;
uint32_t _temp43 = input_2 * A1;
uint32_t a23 = _temp43;
uint32_t _temp44 = input_6 - input_4;
uint32_t _temp45 = _temp44 * A5;
uint32_t mul5 = _temp45;
uint32_t _temp46 = input_4 * A2;
uint32_t _temp47 = _temp46 - mul5;
uint32_t a43 = _temp47;
uint32_t _temp48 = input_5 * A3;
uint32_t a53 = _temp48;
uint32_t _temp49 = input_6 * A4;
uint32_t _temp50 = _temp49 - mul5;
uint32_t a63 = _temp50;
uint32_t _temp51 = 0x8;
uint32_t _temp52 = _temp51 * i;
uint32_t _temp53 = 0x8;
uint32_t _temp54 = _temp53 * i;
uint32_t _temp55 = data_in[_temp54];
data_out[_temp52] = _temp55;
uint32_t _temp57 = 0x8;
uint32_t _temp58 = _temp57 * i;
uint32_t _temp59 = 0x1;
uint32_t _temp60 = _temp58 + _temp59;
uint32_t _temp61 = 0x8;
uint32_t _temp62 = _temp61 * i;
uint32_t _temp63 = 0x1;
uint32_t _temp64 = _temp62 + _temp63;
uint32_t _temp65 = data_in[_temp64];
data_out[_temp60] = _temp65;
uint32_t _temp67 = 0x8;
uint32_t _temp68 = _temp67 * i;
uint32_t _temp69 = 0x2;
uint32_t _temp70 = _temp68 + _temp69;
data_out[_temp70] = a23;
uint32_t _temp72 = 0x8;
uint32_t _temp73 = _temp72 * i;
uint32_t _temp74 = 0x3;
uint32_t _temp75 = _temp73 + _temp74;
uint32_t _temp76 = 0x8;
uint32_t _temp77 = _temp76 * i;
uint32_t _temp78 = 0x3;
uint32_t _temp79 = _temp77 + _temp78;
uint32_t _temp80 = data_in[_temp79];
data_out[_temp75] = _temp80;
uint32_t _temp82 = 0x8;
uint32_t _temp83 = _temp82 * i;
uint32_t _temp84 = 0x4;
uint32_t _temp85 = _temp83 + _temp84;
data_out[_temp85] = a43;
uint32_t _temp87 = 0x8;
uint32_t _temp88 = _temp87 * i;
uint32_t _temp89 = 0x5;
uint32_t _temp90 = _temp88 + _temp89;
data_out[_temp90] = a53;
uint32_t _temp92 = 0x8;
uint32_t _temp93 = _temp92 * i;
uint32_t _temp94 = 0x6;
uint32_t _temp95 = _temp93 + _temp94;
data_out[_temp95] = a63;
uint32_t _temp97 = 0x8;
uint32_t _temp98 = _temp97 * i;
uint32_t _temp99 = 0x7;
uint32_t _temp100 = _temp98 + _temp99;
uint32_t _temp101 = 0x8;
uint32_t _temp102 = _temp101 * i;
uint32_t _temp103 = 0x7;
uint32_t _temp104 = _temp102 + _temp103;
uint32_t _temp105 = data_in[_temp104];
data_out[_temp100] = _temp105;
}

dct_8x8_stage_4_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_5_output dct_8x8_stage_5(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp0 = 0x8;
uint32_t _temp1 = _temp0 * i;
uint32_t _temp2 = data_in[_temp1];
uint32_t input_0 = _temp2;
uint32_t _temp3 = 0x8;
uint32_t _temp4 = _temp3 * i;
uint32_t _temp5 = 0x1;
uint32_t _temp6 = _temp4 + _temp5;
uint32_t _temp7 = data_in[_temp6];
uint32_t input_1 = _temp7;
uint32_t _temp8 = 0x8;
uint32_t _temp9 = _temp8 * i;
uint32_t _temp10 = 0x2;
uint32_t _temp11 = _temp9 + _temp10;
uint32_t _temp12 = data_in[_temp11];
uint32_t input_2 = _temp12;
uint32_t _temp13 = 0x8;
uint32_t _temp14 = _temp13 * i;
uint32_t _temp15 = 0x3;
uint32_t _temp16 = _temp14 + _temp15;
uint32_t _temp17 = data_in[_temp16];
uint32_t input_3 = _temp17;
uint32_t _temp18 = 0x8;
uint32_t _temp19 = _temp18 * i;
uint32_t _temp20 = 0x4;
uint32_t _temp21 = _temp19 + _temp20;
uint32_t _temp22 = data_in[_temp21];
uint32_t input_4 = _temp22;
uint32_t _temp23 = 0x8;
uint32_t _temp24 = _temp23 * i;
uint32_t _temp25 = 0x5;
uint32_t _temp26 = _temp24 + _temp25;
uint32_t _temp27 = data_in[_temp26];
uint32_t input_5 = _temp27;
uint32_t _temp28 = 0x8;
uint32_t _temp29 = _temp28 * i;
uint32_t _temp30 = 0x6;
uint32_t _temp31 = _temp29 + _temp30;
uint32_t _temp32 = data_in[_temp31];
uint32_t input_6 = _temp32;
uint32_t _temp33 = 0x8;
uint32_t _temp34 = _temp33 * i;
uint32_t _temp35 = 0x7;
uint32_t _temp36 = _temp34 + _temp35;
uint32_t _temp37 = data_in[_temp36];
uint32_t input_7 = _temp37;
uint32_t _temp38 = input_7 + input_5;
uint32_t a54 = _temp38;
uint32_t _temp39 = input_7 - input_5;
uint32_t a74 = _temp39;
uint32_t _temp40 = 0x8;
uint32_t _temp41 = _temp40 * i;
uint32_t _temp42 = input_0 + input_1;
data_out[_temp41] = _temp42;
uint32_t _temp44 = 0x8;
uint32_t _temp45 = _temp44 * i;
uint32_t _temp46 = 0x4;
uint32_t _temp47 = _temp45 + _temp46;
uint32_t _temp48 = input_0 - input_1;
data_out[_temp47] = _temp48;
uint32_t _temp50 = 0x8;
uint32_t _temp51 = _temp50 * i;
uint32_t _temp52 = 0x2;
uint32_t _temp53 = _temp51 + _temp52;
uint32_t _temp54 = input_3 + input_2;
data_out[_temp53] = _temp54;
uint32_t _temp56 = 0x8;
uint32_t _temp57 = _temp56 * i;
uint32_t _temp58 = 0x6;
uint32_t _temp59 = _temp57 + _temp58;
uint32_t _temp60 = input_3 - input_2;
data_out[_temp59] = _temp60;
uint32_t _temp62 = 0x8;
uint32_t _temp63 = _temp62 * i;
uint32_t _temp64 = 0x5;
uint32_t _temp65 = _temp63 + _temp64;
uint32_t _temp66 = a74 + input_4;
data_out[_temp65] = _temp66;
uint32_t _temp68 = 0x8;
uint32_t _temp69 = _temp68 * i;
uint32_t _temp70 = 0x1;
uint32_t _temp71 = _temp69 + _temp70;
uint32_t _temp72 = a54 + input_6;
data_out[_temp71] = _temp72;
uint32_t _temp74 = 0x8;
uint32_t _temp75 = _temp74 * i;
uint32_t _temp76 = 0x7;
uint32_t _temp77 = _temp75 + _temp76;
uint32_t _temp78 = a54 - input_6;
data_out[_temp77] = _temp78;
uint32_t _temp80 = 0x8;
uint32_t _temp81 = _temp80 * i;
uint32_t _temp82 = 0x3;
uint32_t _temp83 = _temp81 + _temp82;
uint32_t _temp84 = a74 - input_4;
data_out[_temp83] = _temp84;
}

dct_8x8_stage_5_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_6_output dct_8x8_stage_6(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp0 = data_in[i];
uint32_t input_0 = _temp0;
uint32_t _temp1 = 0x8;
uint32_t _temp2 = _temp1 + i;
uint32_t _temp3 = data_in[_temp2];
uint32_t input_1 = _temp3;
uint32_t _temp4 = 0x10;
uint32_t _temp5 = _temp4 + i;
uint32_t _temp6 = data_in[_temp5];
uint32_t input_2 = _temp6;
uint32_t _temp7 = 0x18;
uint32_t _temp8 = _temp7 + i;
uint32_t _temp9 = data_in[_temp8];
uint32_t input_3 = _temp9;
uint32_t _temp10 = 0x20;
uint32_t _temp11 = _temp10 + i;
uint32_t _temp12 = data_in[_temp11];
uint32_t input_4 = _temp12;
uint32_t _temp13 = 0x28;
uint32_t _temp14 = _temp13 + i;
uint32_t _temp15 = data_in[_temp14];
uint32_t input_5 = _temp15;
uint32_t _temp16 = 0x30;
uint32_t _temp17 = _temp16 + i;
uint32_t _temp18 = data_in[_temp17];
uint32_t input_6 = _temp18;
uint32_t _temp19 = 0x38;
uint32_t _temp20 = _temp19 + i;
uint32_t _temp21 = data_in[_temp20];
uint32_t input_7 = _temp21;
uint32_t _temp22 = input_0 + input_7;
uint32_t a00 = _temp22;
uint32_t _temp23 = input_1 + input_6;
uint32_t a10 = _temp23;
uint32_t _temp24 = input_2 + input_5;
uint32_t a20 = _temp24;
uint32_t _temp25 = input_3 + input_4;
uint32_t a30 = _temp25;
uint32_t _temp26 = input_3 - input_4;
uint32_t a40 = _temp26;
uint32_t _temp27 = input_2 - input_5;
uint32_t a50 = _temp27;
uint32_t _temp28 = input_1 - input_6;
uint32_t a60 = _temp28;
uint32_t _temp29 = input_0 - input_7;
uint32_t a70 = _temp29;
data_out[i] = a00;
uint32_t _temp31 = 0x8;
uint32_t _temp32 = _temp31 + i;
data_out[_temp32] = a10;
uint32_t _temp34 = 0x10;
uint32_t _temp35 = _temp34 + i;
data_out[_temp35] = a20;
uint32_t _temp37 = 0x18;
uint32_t _temp38 = _temp37 + i;
data_out[_temp38] = a30;
uint32_t _temp40 = 0x20;
uint32_t _temp41 = _temp40 + i;
data_out[_temp41] = a40;
uint32_t _temp43 = 0x28;
uint32_t _temp44 = _temp43 + i;
data_out[_temp44] = a50;
uint32_t _temp46 = 0x30;
uint32_t _temp47 = _temp46 + i;
data_out[_temp47] = a60;
uint32_t _temp49 = 0x38;
uint32_t _temp50 = _temp49 + i;
data_out[_temp50] = a70;
}

dct_8x8_stage_6_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_7_output dct_8x8_stage_7(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp0 = data_in[i];
uint32_t input_0 = _temp0;
uint32_t _temp1 = 0x8;
uint32_t _temp2 = _temp1 + i;
uint32_t _temp3 = data_in[_temp2];
uint32_t input_1 = _temp3;
uint32_t _temp4 = 0x10;
uint32_t _temp5 = _temp4 + i;
uint32_t _temp6 = data_in[_temp5];
uint32_t input_2 = _temp6;
uint32_t _temp7 = 0x18;
uint32_t _temp8 = _temp7 + i;
uint32_t _temp9 = data_in[_temp8];
uint32_t input_3 = _temp9;
uint32_t _temp10 = 0x20;
uint32_t _temp11 = _temp10 + i;
uint32_t _temp12 = data_in[_temp11];
uint32_t input_4 = _temp12;
uint32_t _temp13 = 0x28;
uint32_t _temp14 = _temp13 + i;
uint32_t _temp15 = data_in[_temp14];
uint32_t input_5 = _temp15;
uint32_t _temp16 = 0x30;
uint32_t _temp17 = _temp16 + i;
uint32_t _temp18 = data_in[_temp17];
uint32_t input_6 = _temp18;
uint32_t _temp19 = 0x38;
uint32_t _temp20 = _temp19 + i;
uint32_t _temp21 = data_in[_temp20];
uint32_t input_7 = _temp21;
uint32_t _temp22 = input_0 + input_3;
uint32_t a01 = _temp22;
uint32_t _temp23 = input_1 + input_2;
uint32_t a11 = _temp23;
uint32_t _temp24 = input_1 - input_2;
uint32_t a21 = _temp24;
uint32_t _temp25 = input_0 - input_3;
uint32_t a31 = _temp25;
uint32_t _temp26 = input_4 + input_5;
uint32_t neg_a41 = _temp26;
uint32_t _temp27 = input_5 + input_6;
uint32_t a51 = _temp27;
uint32_t _temp28 = input_6 + input_7;
uint32_t a61 = _temp28;
data_out[i] = a01;
uint32_t _temp30 = 0x8;
uint32_t _temp31 = _temp30 + i;
data_out[_temp31] = a11;
uint32_t _temp33 = 0x10;
uint32_t _temp34 = _temp33 + i;
data_out[_temp34] = a21;
uint32_t _temp36 = 0x18;
uint32_t _temp37 = _temp36 + i;
data_out[_temp37] = a31;
uint32_t _temp39 = 0x20;
uint32_t _temp40 = _temp39 + i;
data_out[_temp40] = neg_a41;
uint32_t _temp42 = 0x28;
uint32_t _temp43 = _temp42 + i;
data_out[_temp43] = a51;
uint32_t _temp45 = 0x30;
uint32_t _temp46 = _temp45 + i;
data_out[_temp46] = a61;
uint32_t _temp48 = 0x38;
uint32_t _temp49 = _temp48 + i;
data_out[_temp49] = input_7;
}

dct_8x8_stage_7_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_8_output dct_8x8_stage_8(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp0 = 0x10;
uint32_t _temp1 = _temp0 + i;
uint32_t _temp2 = data_in[_temp1];
uint32_t input_2 = _temp2;
uint32_t _temp3 = 0x18;
uint32_t _temp4 = _temp3 + i;
uint32_t _temp5 = data_in[_temp4];
uint32_t input_3 = _temp5;
uint32_t _temp6 = input_2 + input_3;
uint32_t a22 = _temp6;
uint32_t _temp7 = data_in[i];
data_out[i] = _temp7;
uint32_t _temp9 = 0x8;
uint32_t _temp10 = _temp9 + i;
uint32_t _temp11 = 0x8;
uint32_t _temp12 = _temp11 + i;
uint32_t _temp13 = data_in[_temp12];
data_out[_temp10] = _temp13;
uint32_t _temp15 = 0x10;
uint32_t _temp16 = _temp15 + i;
data_out[_temp16] = a22;
uint32_t _temp18 = 0x18;
uint32_t _temp19 = _temp18 + i;
uint32_t _temp20 = 0x18;
uint32_t _temp21 = _temp20 + i;
uint32_t _temp22 = data_in[_temp21];
data_out[_temp19] = _temp22;
uint32_t _temp24 = 0x20;
uint32_t _temp25 = _temp24 + i;
uint32_t _temp26 = 0x20;
uint32_t _temp27 = _temp26 + i;
uint32_t _temp28 = data_in[_temp27];
data_out[_temp25] = _temp28;
uint32_t _temp30 = 0x28;
uint32_t _temp31 = _temp30 + i;
uint32_t _temp32 = 0x28;
uint32_t _temp33 = _temp32 + i;
uint32_t _temp34 = data_in[_temp33];
data_out[_temp31] = _temp34;
uint32_t _temp36 = 0x30;
uint32_t _temp37 = _temp36 + i;
uint32_t _temp38 = 0x30;
uint32_t _temp39 = _temp38 + i;
uint32_t _temp40 = data_in[_temp39];
data_out[_temp37] = _temp40;
uint32_t _temp42 = 0x38;
uint32_t _temp43 = _temp42 + i;
uint32_t _temp44 = 0x38;
uint32_t _temp45 = _temp44 + i;
uint32_t _temp46 = data_in[_temp45];
data_out[_temp43] = _temp46;
}

dct_8x8_stage_8_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_9_output dct_8x8_stage_9(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
uint32_t _temp0 = 0x10;
uint32_t A1 = _temp0;
uint32_t _temp1 = 0x10;
uint32_t A2 = _temp1;
uint32_t _temp2 = 0x10;
uint32_t A3 = _temp2;
uint32_t _temp3 = 0x10;
uint32_t A4 = _temp3;
uint32_t _temp4 = 0x10;
uint32_t A5 = _temp4;
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp5 = data_in[i];
uint32_t input_0 = _temp5;
uint32_t _temp6 = 0x8;
uint32_t _temp7 = _temp6 + i;
uint32_t _temp8 = data_in[_temp7];
uint32_t input_1 = _temp8;
uint32_t _temp9 = 0x10;
uint32_t _temp10 = _temp9 + i;
uint32_t _temp11 = data_in[_temp10];
uint32_t input_2 = _temp11;
uint32_t _temp12 = 0x18;
uint32_t _temp13 = _temp12 + i;
uint32_t _temp14 = data_in[_temp13];
uint32_t input_3 = _temp14;
uint32_t _temp15 = 0x20;
uint32_t _temp16 = _temp15 + i;
uint32_t _temp17 = data_in[_temp16];
uint32_t input_4 = _temp17;
uint32_t _temp18 = 0x28;
uint32_t _temp19 = _temp18 + i;
uint32_t _temp20 = data_in[_temp19];
uint32_t input_5 = _temp20;
uint32_t _temp21 = 0x30;
uint32_t _temp22 = _temp21 + i;
uint32_t _temp23 = data_in[_temp22];
uint32_t input_6 = _temp23;
uint32_t _temp24 = 0x38;
uint32_t _temp25 = _temp24 + i;
uint32_t _temp26 = data_in[_temp25];
uint32_t input_7 = _temp26;
uint32_t _temp27 = input_2 * A1;
uint32_t a23 = _temp27;
uint32_t _temp28 = input_6 - input_4;
uint32_t _temp29 = _temp28 * A5;
uint32_t mul5 = _temp29;
uint32_t _temp30 = input_4 * A2;
uint32_t _temp31 = _temp30 - mul5;
uint32_t a43 = _temp31;
uint32_t _temp32 = input_5 * A3;
uint32_t a53 = _temp32;
uint32_t _temp33 = input_6 * A4;
uint32_t _temp34 = _temp33 - mul5;
uint32_t a63 = _temp34;
uint32_t _temp35 = data_in[i];
data_out[i] = _temp35;
uint32_t _temp37 = 0x8;
uint32_t _temp38 = _temp37 + i;
uint32_t _temp39 = 0x8;
uint32_t _temp40 = _temp39 + i;
uint32_t _temp41 = data_in[_temp40];
data_out[_temp38] = _temp41;
uint32_t _temp43 = 0x10;
uint32_t _temp44 = _temp43 + i;
data_out[_temp44] = a23;
uint32_t _temp46 = 0x18;
uint32_t _temp47 = _temp46 + i;
uint32_t _temp48 = 0x18;
uint32_t _temp49 = _temp48 + i;
uint32_t _temp50 = data_in[_temp49];
data_out[_temp47] = _temp50;
uint32_t _temp52 = 0x20;
uint32_t _temp53 = _temp52 + i;
data_out[_temp53] = a43;
uint32_t _temp55 = 0x28;
uint32_t _temp56 = _temp55 + i;
data_out[_temp56] = a53;
uint32_t _temp58 = 0x30;
uint32_t _temp59 = _temp58 + i;
data_out[_temp59] = a63;
uint32_t _temp61 = 0x38;
uint32_t _temp62 = _temp61 + i;
uint32_t _temp63 = 0x38;
uint32_t _temp64 = _temp63 + i;
uint32_t _temp65 = data_in[_temp64];
data_out[_temp62] = _temp65;
}

dct_8x8_stage_9_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

dct_8x8_stage_10_output dct_8x8_stage_10(uint32_t data_in[64]) {
	uint32_t data_out[64];
	// Combinational function
for(uint32_t i = 0; i < 8; i++) {
uint32_t _temp0 = data_in[i];
uint32_t input_0 = _temp0;
uint32_t _temp1 = 0x8;
uint32_t _temp2 = _temp1 + i;
uint32_t _temp3 = data_in[_temp2];
uint32_t input_1 = _temp3;
uint32_t _temp4 = 0x10;
uint32_t _temp5 = _temp4 + i;
uint32_t _temp6 = data_in[_temp5];
uint32_t input_2 = _temp6;
uint32_t _temp7 = 0x18;
uint32_t _temp8 = _temp7 + i;
uint32_t _temp9 = data_in[_temp8];
uint32_t input_3 = _temp9;
uint32_t _temp10 = 0x20;
uint32_t _temp11 = _temp10 + i;
uint32_t _temp12 = data_in[_temp11];
uint32_t input_4 = _temp12;
uint32_t _temp13 = 0x28;
uint32_t _temp14 = _temp13 + i;
uint32_t _temp15 = data_in[_temp14];
uint32_t input_5 = _temp15;
uint32_t _temp16 = 0x30;
uint32_t _temp17 = _temp16 + i;
uint32_t _temp18 = data_in[_temp17];
uint32_t input_6 = _temp18;
uint32_t _temp19 = 0x38;
uint32_t _temp20 = _temp19 + i;
uint32_t _temp21 = data_in[_temp20];
uint32_t input_7 = _temp21;
uint32_t _temp22 = input_7 + input_5;
uint32_t a54 = _temp22;
uint32_t _temp23 = input_7 - input_5;
uint32_t a74 = _temp23;
uint32_t _temp24 = input_0 + input_1;
data_out[i] = _temp24;
uint32_t _temp26 = 0x20;
uint32_t _temp27 = _temp26 + i;
uint32_t _temp28 = input_0 - input_1;
data_out[_temp27] = _temp28;
uint32_t _temp30 = 0x10;
uint32_t _temp31 = _temp30 + i;
uint32_t _temp32 = input_3 + input_2;
data_out[_temp31] = _temp32;
uint32_t _temp34 = 0x30;
uint32_t _temp35 = _temp34 + i;
uint32_t _temp36 = input_3 - input_2;
data_out[_temp35] = _temp36;
uint32_t _temp38 = 0x28;
uint32_t _temp39 = _temp38 + i;
uint32_t _temp40 = a74 + input_4;
data_out[_temp39] = _temp40;
uint32_t _temp42 = 0x8;
uint32_t _temp43 = _temp42 + i;
uint32_t _temp44 = a54 + input_6;
data_out[_temp43] = _temp44;
uint32_t _temp46 = 0x38;
uint32_t _temp47 = _temp46 + i;
uint32_t _temp48 = a54 - input_6;
data_out[_temp47] = _temp48;
uint32_t _temp50 = 0x18;
uint32_t _temp51 = _temp50 + i;
uint32_t _temp52 = a74 - input_4;
data_out[_temp51] = _temp52;
}

dct_8x8_stage_10_output output;
memcpy(output.data_out, data_out, sizeof(output.data_out));
	return output;
}

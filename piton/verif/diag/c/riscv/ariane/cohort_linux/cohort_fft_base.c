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

#define NUM_QUEUES 6

#define BATCH_SIZE 128


extern uint64_t back_off_count;


struct _fft_64_stage_1_output {
uint32_t data_out_real[64];
uint32_t data_out_imag[64];
};

typedef struct _fft_64_stage_1_output fft_64_stage_1_output;
fft_64_stage_1_output fft_64_stage_1(uint32_t data_in_real[64], uint32_t data_in_imag[64]);

struct _fft_64_stage_2_output {
uint32_t data_out_real[64];
uint32_t data_out_imag[64];
};

typedef struct _fft_64_stage_2_output fft_64_stage_2_output;
fft_64_stage_2_output fft_64_stage_2(uint32_t data_in_real[64], uint32_t data_in_imag[64]);


struct _fft_64_stage_3_output {
uint32_t data_out_real[64];
uint32_t data_out_imag[64];
};

typedef struct _fft_64_stage_3_output fft_64_stage_3_output;
fft_64_stage_3_output fft_64_stage_3(uint32_t data_in_real[64], uint32_t data_in_imag[64]);

struct _fft_64_stage_4_output {
uint32_t data_out_real[64];
uint32_t data_out_imag[64];
};

typedef struct _fft_64_stage_4_output fft_64_stage_4_output;
fft_64_stage_4_output fft_64_stage_4(uint32_t data_in_real[64], uint32_t data_in_imag[64]);

struct _fft_64_stage_5_output {
uint32_t data_out_real[64];
uint32_t data_out_imag[64];
};

typedef struct _fft_64_stage_5_output fft_64_stage_5_output;
fft_64_stage_5_output fft_64_stage_5(uint32_t data_in_real[64], uint32_t data_in_imag[64]);

struct _fft_64_stage_6_output {
uint32_t data_out_real[64];
uint32_t data_out_imag[64];
};

typedef struct _fft_64_stage_6_output fft_64_stage_6_output;
fft_64_stage_6_output fft_64_stage_6(uint32_t data_in_real[64], uint32_t data_in_imag[64]);

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

    uint64_t bypass[NUM_QUEUES] = {0b00, 0b00, 0b00, 0b00, 0b00, 0b01};
    bool active[NUM_QUEUES] =     {false, false, false, false, false, false};
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
            uint32_t data_in_real[64];
            uint32_t data_in_imag[64];
            for (int j = 0; j < 64; j++) {
                data_in_real[j] = temp[j];
                data_in_imag[j] = temp[j + 64];
            }
            if (i == 0) {
                fft_64_stage_1_output output = fft_64_stage_1(data_in_real, data_in_imag);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out_real[j];
                    temp[j + 64] = output.data_out_imag[j];
                }
            } else if(i == 1) {
                fft_64_stage_2_output output = fft_64_stage_2(data_in_real, data_in_imag);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out_real[j];
                    temp[j + 64] = output.data_out_imag[j];
                }
            } else if (i == 2) {
                fft_64_stage_3_output output = fft_64_stage_3(data_in_real, data_in_imag);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out_real[j];
                    temp[j + 64] = output.data_out_imag[j];
                }
            } else if (i == 3) {
                fft_64_stage_4_output output = fft_64_stage_4(data_in_real, data_in_imag);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out_real[j];
                    temp[j + 64] = output.data_out_imag[j];
                }
            } else if (i == 4) {
                fft_64_stage_5_output output = fft_64_stage_5(data_in_real, data_in_imag);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out_real[j];
                    temp[j + 64] = output.data_out_imag[j];
                }
            } else if (i == 5) {
                fft_64_stage_6_output output = fft_64_stage_6(data_in_real, data_in_imag);
                for (int j = 0; j < 64; j++) {
                    // set temp
                    temp[j] = output.data_out_real[j];
                    temp[j + 64] = output.data_out_imag[j];
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


fft_64_stage_1_output fft_64_stage_1(uint32_t data_in_real[64], uint32_t data_in_imag[64]) {
	uint32_t data_out_real[64];
	uint32_t data_out_imag[64];
	// Combinational function
	for(uint32_t i = 0; i < 32; i++) {
	uint32_t _temp0 = 2;
	uint32_t _temp1 = _temp0 * i;
	uint32_t _temp2 = data_in_real[_temp1];
	uint32_t input_real_1 = _temp2;
	uint32_t _temp3 = 2;
	uint32_t _temp4 = _temp3 * i;
	uint32_t _temp5 = data_in_imag[_temp4];
	uint32_t input_imag_1 = _temp5;
	uint32_t _temp6 = 2;
	uint32_t _temp7 = _temp6 * i;
	uint32_t _temp8 = 1;
	uint32_t _temp9 = _temp7 + _temp8;
	uint32_t _temp10 = data_in_real[_temp9];
	uint32_t input_real_2 = _temp10;
	uint32_t _temp11 = 2;
	uint32_t _temp12 = _temp11 * i;
	uint32_t _temp13 = 1;
	uint32_t _temp14 = _temp12 + _temp13;
	uint32_t _temp15 = data_in_imag[_temp14];
	uint32_t input_imag_2 = _temp15;
	uint32_t _temp16 = input_real_1 + input_real_2;
	uint32_t output_real_1 = _temp16;
	uint32_t _temp17 = input_imag_1 + input_imag_2;
	uint32_t output_imag_1 = _temp17;
	uint32_t _temp18 = input_real_1 - input_real_2;
	uint32_t output_real_2 = _temp18;
	uint32_t _temp19 = input_imag_1 - input_imag_2;
	uint32_t output_imag_2 = _temp19;
	uint32_t _temp20 = 2;
	uint32_t _temp21 = _temp20 * i;
	data_out_real[_temp21] = output_real_1;
	uint32_t _temp23 = 2;
	uint32_t _temp24 = _temp23 * i;
	data_out_imag[_temp24] = output_imag_1;
	uint32_t _temp26 = 2;
	uint32_t _temp27 = _temp26 * i;
	uint32_t _temp28 = 1;
	uint32_t _temp29 = _temp27 + _temp28;
	data_out_real[_temp29] = output_real_2;
	uint32_t _temp31 = 2;
	uint32_t _temp32 = _temp31 * i;
	uint32_t _temp33 = 1;
	uint32_t _temp34 = _temp32 + _temp33;
	data_out_imag[_temp34] = output_imag_2;
	}

	fft_64_stage_1_output output;
	memcpy(output.data_out_real, data_out_real, sizeof(data_out_real));
	memcpy(output.data_out_imag, data_out_imag, sizeof(data_out_imag));
	return output;
}

fft_64_stage_2_output fft_64_stage_2(uint32_t data_in_real[64], uint32_t data_in_imag[64]) {
	uint32_t data_out_real[64];
	uint32_t data_out_imag[64];
	// Combinational function
	uint32_t tmp_real[64];
	memcpy(tmp_real, data_in_real, sizeof(tmp_real));
	uint32_t tmp_imag[64];
	memcpy(tmp_imag, data_in_imag, sizeof(tmp_imag));
	for(uint32_t i = 0; i < 16; i++) {
	uint32_t _temp0 = 4;
	uint32_t _temp1 = i * _temp0;
	uint32_t j = _temp1;
	uint32_t _temp2 = tmp_real[j];
	uint32_t a_real = _temp2;
	uint32_t _temp3 = tmp_imag[j];
	uint32_t a_imag = _temp3;
	uint32_t _temp4 = 2;
	uint32_t _temp5 = j + _temp4;
	uint32_t _temp6 = tmp_real[_temp5];
	uint32_t b_real = _temp6;
	uint32_t _temp7 = 2;
	uint32_t _temp8 = j + _temp7;
	uint32_t _temp9 = tmp_imag[_temp8];
	uint32_t b_imag = _temp9;
	uint32_t _temp10 = 1;
	uint32_t _temp11 = j + _temp10;
	uint32_t _temp12 = tmp_real[_temp11];
	uint32_t c_real = _temp12;
	uint32_t _temp13 = 1;
	uint32_t _temp14 = j + _temp13;
	uint32_t _temp15 = tmp_imag[_temp14];
	uint32_t c_imag = _temp15;
	uint32_t _temp16 = 3;
	uint32_t _temp17 = j + _temp16;
	uint32_t _temp18 = tmp_real[_temp17];
	uint32_t d_real = _temp18;
	uint32_t _temp19 = 3;
	uint32_t _temp20 = j + _temp19;
	uint32_t _temp21 = tmp_imag[_temp20];
	uint32_t d_imag = _temp21;
	uint32_t _temp22 = a_real + b_real;
	uint32_t e_real = _temp22;
	uint32_t _temp23 = a_imag + b_imag;
	uint32_t e_imag = _temp23;
	uint32_t _temp24 = a_real - b_real;
	uint32_t f_real = _temp24;
	uint32_t _temp25 = a_imag - b_imag;
	uint32_t f_imag = _temp25;
	uint32_t _temp26 = c_real + d_real;
	uint32_t g_real = _temp26;
	uint32_t _temp27 = c_imag + d_imag;
	uint32_t g_imag = _temp27;
	uint32_t _temp28 = c_real - d_real;
	uint32_t h_real = _temp28;
	uint32_t _temp29 = c_imag - d_imag;
	uint32_t h_imag = _temp29;
	tmp_real[j] = e_real;
	tmp_imag[j] = e_imag;
	uint32_t _temp32 = 2;
	uint32_t _temp33 = j + _temp32;
	tmp_real[_temp33] = f_real;
	uint32_t _temp35 = 2;
	uint32_t _temp36 = j + _temp35;
	tmp_imag[_temp36] = f_imag;
	uint32_t _temp38 = 1;
	uint32_t _temp39 = j + _temp38;
	tmp_real[_temp39] = g_real;
	uint32_t _temp41 = 1;
	uint32_t _temp42 = j + _temp41;
	tmp_imag[_temp42] = g_imag;
	uint32_t _temp44 = 3;
	uint32_t _temp45 = j + _temp44;
	tmp_real[_temp45] = h_real;
	uint32_t _temp47 = 3;
	uint32_t _temp48 = j + _temp47;
	tmp_imag[_temp48] = h_imag;
	}
	memcpy(data_out_real, tmp_real, sizeof(tmp_real));
	memcpy(data_out_imag, tmp_imag, sizeof(tmp_imag));

	fft_64_stage_2_output output;
	memcpy(output.data_out_real, data_out_real, sizeof(data_out_real));
	memcpy(output.data_out_imag, data_out_imag, sizeof(data_out_imag));
		return output;
}

fft_64_stage_3_output fft_64_stage_3(uint32_t data_in_real[64], uint32_t data_in_imag[64]) {
	uint32_t data_out_real[64];
	uint32_t data_out_imag[64];
	// Combinational function
	uint32_t tmp_real[64];
	memcpy(tmp_real, data_in_real, sizeof(tmp_real));
	uint32_t tmp_imag[64];
	memcpy(tmp_imag, data_in_imag, sizeof(tmp_imag));
	for(uint32_t i = 0; i < 8; i++) {
	uint32_t _temp0 = 8;
	uint32_t _temp1 = i * _temp0;
	uint32_t j = _temp1;
	uint32_t _temp2 = tmp_real[j];
	uint32_t a_real = _temp2;
	uint32_t _temp3 = tmp_imag[j];
	uint32_t a_imag = _temp3;
	uint32_t _temp4 = 4;
	uint32_t _temp5 = j + _temp4;
	uint32_t _temp6 = tmp_real[_temp5];
	uint32_t b_real = _temp6;
	uint32_t _temp7 = 4;
	uint32_t _temp8 = j + _temp7;
	uint32_t _temp9 = tmp_imag[_temp8];
	uint32_t b_imag = _temp9;
	uint32_t _temp10 = 1;
	uint32_t _temp11 = j + _temp10;
	uint32_t _temp12 = tmp_real[_temp11];
	uint32_t c_real = _temp12;
	uint32_t _temp13 = 1;
	uint32_t _temp14 = j + _temp13;
	uint32_t _temp15 = tmp_imag[_temp14];
	uint32_t c_imag = _temp15;
	uint32_t _temp16 = 5;
	uint32_t _temp17 = j + _temp16;
	uint32_t _temp18 = tmp_real[_temp17];
	uint32_t d_real = _temp18;
	uint32_t _temp19 = 5;
	uint32_t _temp20 = j + _temp19;
	uint32_t _temp21 = tmp_imag[_temp20];
	uint32_t d_imag = _temp21;
	uint32_t _temp22 = 2;
	uint32_t _temp23 = j + _temp22;
	uint32_t _temp24 = tmp_real[_temp23];
	uint32_t e_real = _temp24;
	uint32_t _temp25 = 2;
	uint32_t _temp26 = j + _temp25;
	uint32_t _temp27 = tmp_imag[_temp26];
	uint32_t e_imag = _temp27;
	uint32_t _temp28 = 6;
	uint32_t _temp29 = j + _temp28;
	uint32_t _temp30 = tmp_real[_temp29];
	uint32_t f_real = _temp30;
	uint32_t _temp31 = 6;
	uint32_t _temp32 = j + _temp31;
	uint32_t _temp33 = tmp_imag[_temp32];
	uint32_t f_imag = _temp33;
	uint32_t _temp34 = 3;
	uint32_t _temp35 = j + _temp34;
	uint32_t _temp36 = tmp_real[_temp35];
	uint32_t g_real = _temp36;
	uint32_t _temp37 = 3;
	uint32_t _temp38 = j + _temp37;
	uint32_t _temp39 = tmp_imag[_temp38];
	uint32_t g_imag = _temp39;
	uint32_t _temp40 = 7;
	uint32_t _temp41 = j + _temp40;
	uint32_t _temp42 = tmp_real[_temp41];
	uint32_t h_real = _temp42;
	uint32_t _temp43 = 7;
	uint32_t _temp44 = j + _temp43;
	uint32_t _temp45 = tmp_imag[_temp44];
	uint32_t h_imag = _temp45;
	uint32_t _temp46 = a_real + b_real;
	uint32_t p_real = _temp46;
	uint32_t _temp47 = a_imag + b_imag;
	uint32_t p_imag = _temp47;
	uint32_t _temp48 = a_real - b_real;
	uint32_t q_real = _temp48;
	uint32_t _temp49 = a_imag - b_imag;
	uint32_t q_imag = _temp49;
	uint32_t _temp50 = c_real + d_real;
	uint32_t r_real = _temp50;
	uint32_t _temp51 = c_imag + d_imag;
	uint32_t r_imag = _temp51;
	uint32_t _temp52 = c_real - d_real;
	uint32_t s_real = _temp52;
	uint32_t _temp53 = c_imag - d_imag;
	uint32_t s_imag = _temp53;
	uint32_t _temp54 = e_real + f_real;
	uint32_t t_real = _temp54;
	uint32_t _temp55 = e_imag + f_imag;
	uint32_t t_imag = _temp55;
	uint32_t _temp56 = e_real - f_real;
	uint32_t u_real = _temp56;
	uint32_t _temp57 = e_imag - f_imag;
	uint32_t u_imag = _temp57;
	uint32_t _temp58 = g_real + h_real;
	uint32_t v_real = _temp58;
	uint32_t _temp59 = g_imag + h_imag;
	uint32_t v_imag = _temp59;
	uint32_t _temp60 = g_real - h_real;
	uint32_t w_real = _temp60;
	uint32_t _temp61 = g_imag - h_imag;
	uint32_t w_imag = _temp61;
	tmp_real[j] = p_real;
	tmp_imag[j] = p_imag;
	uint32_t _temp64 = 4;
	uint32_t _temp65 = j + _temp64;
	tmp_real[_temp65] = q_real;
	uint32_t _temp67 = 4;
	uint32_t _temp68 = j + _temp67;
	tmp_imag[_temp68] = q_imag;
	uint32_t _temp70 = 1;
	uint32_t _temp71 = j + _temp70;
	tmp_real[_temp71] = r_real;
	uint32_t _temp73 = 1;
	uint32_t _temp74 = j + _temp73;
	tmp_imag[_temp74] = r_imag;
	uint32_t _temp76 = 5;
	uint32_t _temp77 = j + _temp76;
	tmp_real[_temp77] = s_real;
	uint32_t _temp79 = 5;
	uint32_t _temp80 = j + _temp79;
	tmp_imag[_temp80] = s_imag;
	uint32_t _temp82 = 2;
	uint32_t _temp83 = j + _temp82;
	tmp_real[_temp83] = t_real;
	uint32_t _temp85 = 2;
	uint32_t _temp86 = j + _temp85;
	tmp_imag[_temp86] = t_imag;
	uint32_t _temp88 = 6;
	uint32_t _temp89 = j + _temp88;
	tmp_real[_temp89] = u_real;
	uint32_t _temp91 = 6;
	uint32_t _temp92 = j + _temp91;
	tmp_imag[_temp92] = u_imag;
	uint32_t _temp94 = 3;
	uint32_t _temp95 = j + _temp94;
	tmp_real[_temp95] = v_real;
	uint32_t _temp97 = 3;
	uint32_t _temp98 = j + _temp97;
	tmp_imag[_temp98] = v_imag;
	uint32_t _temp100 = 7;
	uint32_t _temp101 = j + _temp100;
	tmp_real[_temp101] = w_real;
	uint32_t _temp103 = 7;
	uint32_t _temp104 = j + _temp103;
	tmp_imag[_temp104] = w_imag;
	}
	memcpy(data_out_real, tmp_real, sizeof(tmp_real));
	memcpy(data_out_imag, tmp_imag, sizeof(tmp_imag));

	fft_64_stage_3_output output;
	memcpy(output.data_out_real, data_out_real, sizeof(data_out_real));
	memcpy(output.data_out_imag, data_out_imag, sizeof(data_out_imag));
	return output;
}

fft_64_stage_4_output fft_64_stage_4(uint32_t data_in_real[64], uint32_t data_in_imag[64]) {
	uint32_t data_out_real[64];
	uint32_t data_out_imag[64];
	// Combinational function
	uint32_t tmp_real[64];
	memcpy(tmp_real, data_in_real, sizeof(tmp_real));
	uint32_t tmp_imag[64];
	memcpy(tmp_imag, data_in_imag, sizeof(tmp_imag));
	for(uint32_t i = 0; i < 4; i++) {
	uint32_t _temp0 = 16;
	uint32_t _temp1 = i * _temp0;
	uint32_t j = _temp1;
	uint32_t _temp2 = tmp_real[j];
	uint32_t a_real = _temp2;
	uint32_t _temp3 = tmp_imag[j];
	uint32_t a_imag = _temp3;
	uint32_t _temp4 = 8;
	uint32_t _temp5 = j + _temp4;
	uint32_t _temp6 = tmp_real[_temp5];
	uint32_t b_real = _temp6;
	uint32_t _temp7 = 8;
	uint32_t _temp8 = j + _temp7;
	uint32_t _temp9 = tmp_imag[_temp8];
	uint32_t b_imag = _temp9;
	uint32_t _temp10 = 1;
	uint32_t _temp11 = j + _temp10;
	uint32_t _temp12 = tmp_real[_temp11];
	uint32_t c_real = _temp12;
	uint32_t _temp13 = 1;
	uint32_t _temp14 = j + _temp13;
	uint32_t _temp15 = tmp_imag[_temp14];
	uint32_t c_imag = _temp15;
	uint32_t _temp16 = 9;
	uint32_t _temp17 = j + _temp16;
	uint32_t _temp18 = tmp_real[_temp17];
	uint32_t d_real = _temp18;
	uint32_t _temp19 = 9;
	uint32_t _temp20 = j + _temp19;
	uint32_t _temp21 = tmp_imag[_temp20];
	uint32_t d_imag = _temp21;
	uint32_t _temp22 = 2;
	uint32_t _temp23 = j + _temp22;
	uint32_t _temp24 = tmp_real[_temp23];
	uint32_t e_real = _temp24;
	uint32_t _temp25 = 2;
	uint32_t _temp26 = j + _temp25;
	uint32_t _temp27 = tmp_imag[_temp26];
	uint32_t e_imag = _temp27;
	uint32_t _temp28 = 10;
	uint32_t _temp29 = j + _temp28;
	uint32_t _temp30 = tmp_real[_temp29];
	uint32_t f_real = _temp30;
	uint32_t _temp31 = 10;
	uint32_t _temp32 = j + _temp31;
	uint32_t _temp33 = tmp_imag[_temp32];
	uint32_t f_imag = _temp33;
	uint32_t _temp34 = 3;
	uint32_t _temp35 = j + _temp34;
	uint32_t _temp36 = tmp_real[_temp35];
	uint32_t g_real = _temp36;
	uint32_t _temp37 = 3;
	uint32_t _temp38 = j + _temp37;
	uint32_t _temp39 = tmp_imag[_temp38];
	uint32_t g_imag = _temp39;
	uint32_t _temp40 = 11;
	uint32_t _temp41 = j + _temp40;
	uint32_t _temp42 = tmp_real[_temp41];
	uint32_t h_real = _temp42;
	uint32_t _temp43 = 11;
	uint32_t _temp44 = j + _temp43;
	uint32_t _temp45 = tmp_imag[_temp44];
	uint32_t h_imag = _temp45;
	uint32_t _temp46 = 4;
	uint32_t _temp47 = j + _temp46;
	uint32_t _temp48 = tmp_real[_temp47];
	uint32_t i_real = _temp48;
	uint32_t _temp49 = 4;
	uint32_t _temp50 = j + _temp49;
	uint32_t _temp51 = tmp_imag[_temp50];
	uint32_t i_imag = _temp51;
	uint32_t _temp52 = 12;
	uint32_t _temp53 = j + _temp52;
	uint32_t _temp54 = tmp_real[_temp53];
	uint32_t j_real = _temp54;
	uint32_t _temp55 = 12;
	uint32_t _temp56 = j + _temp55;
	uint32_t _temp57 = tmp_imag[_temp56];
	uint32_t j_imag = _temp57;
	uint32_t _temp58 = 5;
	uint32_t _temp59 = j + _temp58;
	uint32_t _temp60 = tmp_real[_temp59];
	uint32_t k_real = _temp60;
	uint32_t _temp61 = 5;
	uint32_t _temp62 = j + _temp61;
	uint32_t _temp63 = tmp_imag[_temp62];
	uint32_t k_imag = _temp63;
	uint32_t _temp64 = 13;
	uint32_t _temp65 = j + _temp64;
	uint32_t _temp66 = tmp_real[_temp65];
	uint32_t l_real = _temp66;
	uint32_t _temp67 = 13;
	uint32_t _temp68 = j + _temp67;
	uint32_t _temp69 = tmp_imag[_temp68];
	uint32_t l_imag = _temp69;
	uint32_t _temp70 = 6;
	uint32_t _temp71 = j + _temp70;
	uint32_t _temp72 = tmp_real[_temp71];
	uint32_t m_real = _temp72;
	uint32_t _temp73 = 6;
	uint32_t _temp74 = j + _temp73;
	uint32_t _temp75 = tmp_imag[_temp74];
	uint32_t m_imag = _temp75;
	uint32_t _temp76 = 14;
	uint32_t _temp77 = j + _temp76;
	uint32_t _temp78 = tmp_real[_temp77];
	uint32_t n_real = _temp78;
	uint32_t _temp79 = 14;
	uint32_t _temp80 = j + _temp79;
	uint32_t _temp81 = tmp_imag[_temp80];
	uint32_t n_imag = _temp81;
	uint32_t _temp82 = 7;
	uint32_t _temp83 = j + _temp82;
	uint32_t _temp84 = tmp_real[_temp83];
	uint32_t o_real = _temp84;
	uint32_t _temp85 = 7;
	uint32_t _temp86 = j + _temp85;
	uint32_t _temp87 = tmp_imag[_temp86];
	uint32_t o_imag = _temp87;
	uint32_t _temp88 = 15;
	uint32_t _temp89 = j + _temp88;
	uint32_t _temp90 = tmp_real[_temp89];
	uint32_t p_real = _temp90;
	uint32_t _temp91 = 15;
	uint32_t _temp92 = j + _temp91;
	uint32_t _temp93 = tmp_imag[_temp92];
	uint32_t p_imag = _temp93;
	uint32_t _temp94 = a_real + b_real;
	uint32_t q_real = _temp94;
	uint32_t _temp95 = a_imag + b_imag;
	uint32_t q_imag = _temp95;
	uint32_t _temp96 = a_real - b_real;
	uint32_t r_real = _temp96;
	uint32_t _temp97 = a_imag - b_imag;
	uint32_t r_imag = _temp97;
	uint32_t _temp98 = c_real + d_real;
	uint32_t s_real = _temp98;
	uint32_t _temp99 = c_imag + d_imag;
	uint32_t s_imag = _temp99;
	uint32_t _temp100 = c_real - d_real;
	uint32_t t_real = _temp100;
	uint32_t _temp101 = c_imag - d_imag;
	uint32_t t_imag = _temp101;
	uint32_t _temp102 = e_real + f_real;
	uint32_t u_real = _temp102;
	uint32_t _temp103 = e_imag + f_imag;
	uint32_t u_imag = _temp103;
	uint32_t _temp104 = e_real - f_real;
	uint32_t v_real = _temp104;
	uint32_t _temp105 = e_imag - f_imag;
	uint32_t v_imag = _temp105;
	uint32_t _temp106 = g_real + h_real;
	uint32_t w_real = _temp106;
	uint32_t _temp107 = g_imag + h_imag;
	uint32_t w_imag = _temp107;
	uint32_t _temp108 = g_real - h_real;
	uint32_t x_real = _temp108;
	uint32_t _temp109 = g_imag - h_imag;
	uint32_t x_imag = _temp109;
	uint32_t _temp110 = i_real + j_real;
	uint32_t y_real = _temp110;
	uint32_t _temp111 = i_imag + j_imag;
	uint32_t y_imag = _temp111;
	uint32_t _temp112 = i_real - j_real;
	uint32_t z_real = _temp112;
	uint32_t _temp113 = i_imag - j_imag;
	uint32_t z_imag = _temp113;
	uint32_t _temp114 = k_real + l_real;
	uint32_t aa_real = _temp114;
	uint32_t _temp115 = k_imag + l_imag;
	uint32_t aa_imag = _temp115;
	uint32_t _temp116 = k_real - l_real;
	uint32_t bb_real = _temp116;
	uint32_t _temp117 = k_imag - l_imag;
	uint32_t bb_imag = _temp117;
	uint32_t _temp118 = m_real + n_real;
	uint32_t cc_real = _temp118;
	uint32_t _temp119 = m_imag + n_imag;
	uint32_t cc_imag = _temp119;
	uint32_t _temp120 = m_real - n_real;
	uint32_t dd_real = _temp120;
	uint32_t _temp121 = m_imag - n_imag;
	uint32_t dd_imag = _temp121;
	uint32_t _temp122 = o_real + p_real;
	uint32_t ee_real = _temp122;
	uint32_t _temp123 = o_imag + p_imag;
	uint32_t ee_imag = _temp123;
	uint32_t _temp124 = o_real - p_real;
	uint32_t ff_real = _temp124;
	uint32_t _temp125 = o_imag - p_imag;
	uint32_t ff_imag = _temp125;
	tmp_real[j] = q_real;
	tmp_imag[j] = q_imag;
	uint32_t _temp128 = 8;
	uint32_t _temp129 = j + _temp128;
	tmp_real[_temp129] = r_real;
	uint32_t _temp131 = 8;
	uint32_t _temp132 = j + _temp131;
	tmp_imag[_temp132] = r_imag;
	uint32_t _temp134 = 1;
	uint32_t _temp135 = j + _temp134;
	tmp_real[_temp135] = s_real;
	uint32_t _temp137 = 1;
	uint32_t _temp138 = j + _temp137;
	tmp_imag[_temp138] = s_imag;
	uint32_t _temp140 = 9;
	uint32_t _temp141 = j + _temp140;
	tmp_real[_temp141] = t_real;
	uint32_t _temp143 = 9;
	uint32_t _temp144 = j + _temp143;
	tmp_imag[_temp144] = t_imag;
	uint32_t _temp146 = 2;
	uint32_t _temp147 = j + _temp146;
	tmp_real[_temp147] = u_real;
	uint32_t _temp149 = 2;
	uint32_t _temp150 = j + _temp149;
	tmp_imag[_temp150] = u_imag;
	uint32_t _temp152 = 10;
	uint32_t _temp153 = j + _temp152;
	tmp_real[_temp153] = v_real;
	uint32_t _temp155 = 10;
	uint32_t _temp156 = j + _temp155;
	tmp_imag[_temp156] = v_imag;
	uint32_t _temp158 = 3;
	uint32_t _temp159 = j + _temp158;
	tmp_real[_temp159] = w_real;
	uint32_t _temp161 = 3;
	uint32_t _temp162 = j + _temp161;
	tmp_imag[_temp162] = w_imag;
	uint32_t _temp164 = 11;
	uint32_t _temp165 = j + _temp164;
	tmp_real[_temp165] = x_real;
	uint32_t _temp167 = 11;
	uint32_t _temp168 = j + _temp167;
	tmp_imag[_temp168] = x_imag;
	uint32_t _temp170 = 4;
	uint32_t _temp171 = j + _temp170;
	tmp_real[_temp171] = y_real;
	uint32_t _temp173 = 4;
	uint32_t _temp174 = j + _temp173;
	tmp_imag[_temp174] = y_imag;
	uint32_t _temp176 = 12;
	uint32_t _temp177 = j + _temp176;
	tmp_real[_temp177] = z_real;
	uint32_t _temp179 = 12;
	uint32_t _temp180 = j + _temp179;
	tmp_imag[_temp180] = z_imag;
	uint32_t _temp182 = 5;
	uint32_t _temp183 = j + _temp182;
	tmp_real[_temp183] = aa_real;
	uint32_t _temp185 = 5;
	uint32_t _temp186 = j + _temp185;
	tmp_imag[_temp186] = aa_imag;
	uint32_t _temp188 = 13;
	uint32_t _temp189 = j + _temp188;
	tmp_real[_temp189] = bb_real;
	uint32_t _temp191 = 13;
	uint32_t _temp192 = j + _temp191;
	tmp_imag[_temp192] = bb_imag;
	uint32_t _temp194 = 6;
	uint32_t _temp195 = j + _temp194;
	tmp_real[_temp195] = cc_real;
	uint32_t _temp197 = 6;
	uint32_t _temp198 = j + _temp197;
	tmp_imag[_temp198] = cc_imag;
	uint32_t _temp200 = 14;
	uint32_t _temp201 = j + _temp200;
	tmp_real[_temp201] = dd_real;
	uint32_t _temp203 = 14;
	uint32_t _temp204 = j + _temp203;
	tmp_imag[_temp204] = dd_imag;
	uint32_t _temp206 = 7;
	uint32_t _temp207 = j + _temp206;
	tmp_real[_temp207] = ee_real;
	uint32_t _temp209 = 7;
	uint32_t _temp210 = j + _temp209;
	tmp_imag[_temp210] = ee_imag;
	uint32_t _temp212 = 15;
	uint32_t _temp213 = j + _temp212;
	tmp_real[_temp213] = ff_real;
	uint32_t _temp215 = 15;
	uint32_t _temp216 = j + _temp215;
	tmp_imag[_temp216] = ff_imag;
	}
	memcpy(data_out_real, tmp_real, sizeof(tmp_real));
	memcpy(data_out_imag, tmp_imag, sizeof(tmp_imag));

	fft_64_stage_4_output output;
	memcpy(output.data_out_real, data_out_real, sizeof(data_out_real));
	memcpy(output.data_out_imag, data_out_imag, sizeof(data_out_imag));
	return output;
}

fft_64_stage_5_output fft_64_stage_5(uint32_t data_in_real[64], uint32_t data_in_imag[64]) {
	uint32_t data_out_real[64];
	uint32_t data_out_imag[64];
	// Combinational function
	uint32_t tmp_real[64];
	memcpy(tmp_real, data_in_real, sizeof(tmp_real));
	uint32_t tmp_imag[64];
	memcpy(tmp_imag, data_in_imag, sizeof(tmp_imag));
	for(uint32_t i = 0; i < 2; i++) {
	uint32_t _temp0 = 32;
	uint32_t _temp1 = i * _temp0;
	uint32_t j = _temp1;
	uint32_t _temp2 = tmp_real[j];
	uint32_t a_real = _temp2;
	uint32_t _temp3 = tmp_imag[j];
	uint32_t a_imag = _temp3;
	uint32_t _temp4 = 16;
	uint32_t _temp5 = j + _temp4;
	uint32_t _temp6 = tmp_real[_temp5];
	uint32_t b_real = _temp6;
	uint32_t _temp7 = 16;
	uint32_t _temp8 = j + _temp7;
	uint32_t _temp9 = tmp_imag[_temp8];
	uint32_t b_imag = _temp9;
	uint32_t _temp10 = 1;
	uint32_t _temp11 = j + _temp10;
	uint32_t _temp12 = tmp_real[_temp11];
	uint32_t c_real = _temp12;
	uint32_t _temp13 = 1;
	uint32_t _temp14 = j + _temp13;
	uint32_t _temp15 = tmp_imag[_temp14];
	uint32_t c_imag = _temp15;
	uint32_t _temp16 = 17;
	uint32_t _temp17 = j + _temp16;
	uint32_t _temp18 = tmp_real[_temp17];
	uint32_t d_real = _temp18;
	uint32_t _temp19 = 17;
	uint32_t _temp20 = j + _temp19;
	uint32_t _temp21 = tmp_imag[_temp20];
	uint32_t d_imag = _temp21;
	uint32_t _temp22 = 2;
	uint32_t _temp23 = j + _temp22;
	uint32_t _temp24 = tmp_real[_temp23];
	uint32_t e_real = _temp24;
	uint32_t _temp25 = 2;
	uint32_t _temp26 = j + _temp25;
	uint32_t _temp27 = tmp_imag[_temp26];
	uint32_t e_imag = _temp27;
	uint32_t _temp28 = 18;
	uint32_t _temp29 = j + _temp28;
	uint32_t _temp30 = tmp_real[_temp29];
	uint32_t f_real = _temp30;
	uint32_t _temp31 = 18;
	uint32_t _temp32 = j + _temp31;
	uint32_t _temp33 = tmp_imag[_temp32];
	uint32_t f_imag = _temp33;
	uint32_t _temp34 = 3;
	uint32_t _temp35 = j + _temp34;
	uint32_t _temp36 = tmp_real[_temp35];
	uint32_t g_real = _temp36;
	uint32_t _temp37 = 3;
	uint32_t _temp38 = j + _temp37;
	uint32_t _temp39 = tmp_imag[_temp38];
	uint32_t g_imag = _temp39;
	uint32_t _temp40 = 19;
	uint32_t _temp41 = j + _temp40;
	uint32_t _temp42 = tmp_real[_temp41];
	uint32_t h_real = _temp42;
	uint32_t _temp43 = 19;
	uint32_t _temp44 = j + _temp43;
	uint32_t _temp45 = tmp_imag[_temp44];
	uint32_t h_imag = _temp45;
	uint32_t _temp46 = 4;
	uint32_t _temp47 = j + _temp46;
	uint32_t _temp48 = tmp_real[_temp47];
	uint32_t i_real = _temp48;
	uint32_t _temp49 = 4;
	uint32_t _temp50 = j + _temp49;
	uint32_t _temp51 = tmp_imag[_temp50];
	uint32_t i_imag = _temp51;
	uint32_t _temp52 = 20;
	uint32_t _temp53 = j + _temp52;
	uint32_t _temp54 = tmp_real[_temp53];
	uint32_t j_real = _temp54;
	uint32_t _temp55 = 20;
	uint32_t _temp56 = j + _temp55;
	uint32_t _temp57 = tmp_imag[_temp56];
	uint32_t j_imag = _temp57;
	uint32_t _temp58 = 5;
	uint32_t _temp59 = j + _temp58;
	uint32_t _temp60 = tmp_real[_temp59];
	uint32_t k_real = _temp60;
	uint32_t _temp61 = 5;
	uint32_t _temp62 = j + _temp61;
	uint32_t _temp63 = tmp_imag[_temp62];
	uint32_t k_imag = _temp63;
	uint32_t _temp64 = 21;
	uint32_t _temp65 = j + _temp64;
	uint32_t _temp66 = tmp_real[_temp65];
	uint32_t l_real = _temp66;
	uint32_t _temp67 = 21;
	uint32_t _temp68 = j + _temp67;
	uint32_t _temp69 = tmp_imag[_temp68];
	uint32_t l_imag = _temp69;
	uint32_t _temp70 = 6;
	uint32_t _temp71 = j + _temp70;
	uint32_t _temp72 = tmp_real[_temp71];
	uint32_t m_real = _temp72;
	uint32_t _temp73 = 6;
	uint32_t _temp74 = j + _temp73;
	uint32_t _temp75 = tmp_imag[_temp74];
	uint32_t m_imag = _temp75;
	uint32_t _temp76 = 22;
	uint32_t _temp77 = j + _temp76;
	uint32_t _temp78 = tmp_real[_temp77];
	uint32_t n_real = _temp78;
	uint32_t _temp79 = 22;
	uint32_t _temp80 = j + _temp79;
	uint32_t _temp81 = tmp_imag[_temp80];
	uint32_t n_imag = _temp81;
	uint32_t _temp82 = 7;
	uint32_t _temp83 = j + _temp82;
	uint32_t _temp84 = tmp_real[_temp83];
	uint32_t o_real = _temp84;
	uint32_t _temp85 = 7;
	uint32_t _temp86 = j + _temp85;
	uint32_t _temp87 = tmp_imag[_temp86];
	uint32_t o_imag = _temp87;
	uint32_t _temp88 = 23;
	uint32_t _temp89 = j + _temp88;
	uint32_t _temp90 = tmp_real[_temp89];
	uint32_t p_real = _temp90;
	uint32_t _temp91 = 23;
	uint32_t _temp92 = j + _temp91;
	uint32_t _temp93 = tmp_imag[_temp92];
	uint32_t p_imag = _temp93;
	uint32_t _temp94 = 8;
	uint32_t _temp95 = j + _temp94;
	uint32_t _temp96 = tmp_real[_temp95];
	uint32_t q_real = _temp96;
	uint32_t _temp97 = 8;
	uint32_t _temp98 = j + _temp97;
	uint32_t _temp99 = tmp_imag[_temp98];
	uint32_t q_imag = _temp99;
	uint32_t _temp100 = 24;
	uint32_t _temp101 = j + _temp100;
	uint32_t _temp102 = tmp_real[_temp101];
	uint32_t r_real = _temp102;
	uint32_t _temp103 = 24;
	uint32_t _temp104 = j + _temp103;
	uint32_t _temp105 = tmp_imag[_temp104];
	uint32_t r_imag = _temp105;
	uint32_t _temp106 = 9;
	uint32_t _temp107 = j + _temp106;
	uint32_t _temp108 = tmp_real[_temp107];
	uint32_t s_real = _temp108;
	uint32_t _temp109 = 9;
	uint32_t _temp110 = j + _temp109;
	uint32_t _temp111 = tmp_imag[_temp110];
	uint32_t s_imag = _temp111;
	uint32_t _temp112 = 25;
	uint32_t _temp113 = j + _temp112;
	uint32_t _temp114 = tmp_real[_temp113];
	uint32_t t_real = _temp114;
	uint32_t _temp115 = 25;
	uint32_t _temp116 = j + _temp115;
	uint32_t _temp117 = tmp_imag[_temp116];
	uint32_t t_imag = _temp117;
	uint32_t _temp118 = 10;
	uint32_t _temp119 = j + _temp118;
	uint32_t _temp120 = tmp_real[_temp119];
	uint32_t u_real = _temp120;
	uint32_t _temp121 = 10;
	uint32_t _temp122 = j + _temp121;
	uint32_t _temp123 = tmp_imag[_temp122];
	uint32_t u_imag = _temp123;
	uint32_t _temp124 = 26;
	uint32_t _temp125 = j + _temp124;
	uint32_t _temp126 = tmp_real[_temp125];
	uint32_t v_real = _temp126;
	uint32_t _temp127 = 26;
	uint32_t _temp128 = j + _temp127;
	uint32_t _temp129 = tmp_imag[_temp128];
	uint32_t v_imag = _temp129;
	uint32_t _temp130 = 11;
	uint32_t _temp131 = j + _temp130;
	uint32_t _temp132 = tmp_real[_temp131];
	uint32_t w_real = _temp132;
	uint32_t _temp133 = 11;
	uint32_t _temp134 = j + _temp133;
	uint32_t _temp135 = tmp_imag[_temp134];
	uint32_t w_imag = _temp135;
	uint32_t _temp136 = 27;
	uint32_t _temp137 = j + _temp136;
	uint32_t _temp138 = tmp_real[_temp137];
	uint32_t x_real = _temp138;
	uint32_t _temp139 = 27;
	uint32_t _temp140 = j + _temp139;
	uint32_t _temp141 = tmp_imag[_temp140];
	uint32_t x_imag = _temp141;
	uint32_t _temp142 = 12;
	uint32_t _temp143 = j + _temp142;
	uint32_t _temp144 = tmp_real[_temp143];
	uint32_t y_real = _temp144;
	uint32_t _temp145 = 12;
	uint32_t _temp146 = j + _temp145;
	uint32_t _temp147 = tmp_imag[_temp146];
	uint32_t y_imag = _temp147;
	uint32_t _temp148 = 28;
	uint32_t _temp149 = j + _temp148;
	uint32_t _temp150 = tmp_real[_temp149];
	uint32_t z_real = _temp150;
	uint32_t _temp151 = 28;
	uint32_t _temp152 = j + _temp151;
	uint32_t _temp153 = tmp_imag[_temp152];
	uint32_t z_imag = _temp153;
	uint32_t _temp154 = 13;
	uint32_t _temp155 = j + _temp154;
	uint32_t _temp156 = tmp_real[_temp155];
	uint32_t aa_real = _temp156;
	uint32_t _temp157 = 13;
	uint32_t _temp158 = j + _temp157;
	uint32_t _temp159 = tmp_imag[_temp158];
	uint32_t aa_imag = _temp159;
	uint32_t _temp160 = 29;
	uint32_t _temp161 = j + _temp160;
	uint32_t _temp162 = tmp_real[_temp161];
	uint32_t bb_real = _temp162;
	uint32_t _temp163 = 29;
	uint32_t _temp164 = j + _temp163;
	uint32_t _temp165 = tmp_imag[_temp164];
	uint32_t bb_imag = _temp165;
	uint32_t _temp166 = 14;
	uint32_t _temp167 = j + _temp166;
	uint32_t _temp168 = tmp_real[_temp167];
	uint32_t cc_real = _temp168;
	uint32_t _temp169 = 14;
	uint32_t _temp170 = j + _temp169;
	uint32_t _temp171 = tmp_imag[_temp170];
	uint32_t cc_imag = _temp171;
	uint32_t _temp172 = 30;
	uint32_t _temp173 = j + _temp172;
	uint32_t _temp174 = tmp_real[_temp173];
	uint32_t dd_real = _temp174;
	uint32_t _temp175 = 30;
	uint32_t _temp176 = j + _temp175;
	uint32_t _temp177 = tmp_imag[_temp176];
	uint32_t dd_imag = _temp177;
	uint32_t _temp178 = 15;
	uint32_t _temp179 = j + _temp178;
	uint32_t _temp180 = tmp_real[_temp179];
	uint32_t ee_real = _temp180;
	uint32_t _temp181 = 15;
	uint32_t _temp182 = j + _temp181;
	uint32_t _temp183 = tmp_imag[_temp182];
	uint32_t ee_imag = _temp183;
	uint32_t _temp184 = 31;
	uint32_t _temp185 = j + _temp184;
	uint32_t _temp186 = tmp_real[_temp185];
	uint32_t ff_real = _temp186;
	uint32_t _temp187 = 31;
	uint32_t _temp188 = j + _temp187;
	uint32_t _temp189 = tmp_imag[_temp188];
	uint32_t ff_imag = _temp189;
	uint32_t _temp190 = a_real + b_real;
	uint32_t a0_real = _temp190;
	uint32_t _temp191 = a_imag + b_imag;
	uint32_t a0_imag = _temp191;
	uint32_t _temp192 = a_real - b_real;
	uint32_t a1_real = _temp192;
	uint32_t _temp193 = a_imag - b_imag;
	uint32_t a1_imag = _temp193;
	uint32_t _temp194 = c_real + d_real;
	uint32_t b0_real = _temp194;
	uint32_t _temp195 = c_imag + d_imag;
	uint32_t b0_imag = _temp195;
	uint32_t _temp196 = c_real - d_real;
	uint32_t b1_real = _temp196;
	uint32_t _temp197 = c_imag - d_imag;
	uint32_t b1_imag = _temp197;
	uint32_t _temp198 = e_real + f_real;
	uint32_t c0_real = _temp198;
	uint32_t _temp199 = e_imag + f_imag;
	uint32_t c0_imag = _temp199;
	uint32_t _temp200 = e_real - f_real;
	uint32_t c1_real = _temp200;
	uint32_t _temp201 = e_imag - f_imag;
	uint32_t c1_imag = _temp201;
	uint32_t _temp202 = g_real + h_real;
	uint32_t d0_real = _temp202;
	uint32_t _temp203 = g_imag + h_imag;
	uint32_t d0_imag = _temp203;
	uint32_t _temp204 = g_real - h_real;
	uint32_t d1_real = _temp204;
	uint32_t _temp205 = g_imag - h_imag;
	uint32_t d1_imag = _temp205;
	uint32_t _temp206 = i_real + j_real;
	uint32_t e0_real = _temp206;
	uint32_t _temp207 = i_imag + j_imag;
	uint32_t e0_imag = _temp207;
	uint32_t _temp208 = i_real - j_real;
	uint32_t e1_real = _temp208;
	uint32_t _temp209 = i_imag - j_imag;
	uint32_t e1_imag = _temp209;
	uint32_t _temp210 = k_real + l_real;
	uint32_t f0_real = _temp210;
	uint32_t _temp211 = k_imag + l_imag;
	uint32_t f0_imag = _temp211;
	uint32_t _temp212 = k_real - l_real;
	uint32_t f1_real = _temp212;
	uint32_t _temp213 = k_imag - l_imag;
	uint32_t f1_imag = _temp213;
	uint32_t _temp214 = m_real + n_real;
	uint32_t g0_real = _temp214;
	uint32_t _temp215 = m_imag + n_imag;
	uint32_t g0_imag = _temp215;
	uint32_t _temp216 = m_real - n_real;
	uint32_t g1_real = _temp216;
	uint32_t _temp217 = m_imag - n_imag;
	uint32_t g1_imag = _temp217;
	uint32_t _temp218 = o_real + p_real;
	uint32_t h0_real = _temp218;
	uint32_t _temp219 = o_imag + p_imag;
	uint32_t h0_imag = _temp219;
	uint32_t _temp220 = o_real - p_real;
	uint32_t h1_real = _temp220;
	uint32_t _temp221 = o_imag - p_imag;
	uint32_t h1_imag = _temp221;
	uint32_t _temp222 = q_real + r_real;
	uint32_t i0_real = _temp222;
	uint32_t _temp223 = q_imag + r_imag;
	uint32_t i0_imag = _temp223;
	uint32_t _temp224 = q_real - r_real;
	uint32_t i1_real = _temp224;
	uint32_t _temp225 = q_imag - r_imag;
	uint32_t i1_imag = _temp225;
	uint32_t _temp226 = s_real + t_real;
	uint32_t j0_real = _temp226;
	uint32_t _temp227 = s_imag + t_imag;
	uint32_t j0_imag = _temp227;
	uint32_t _temp228 = s_real - t_real;
	uint32_t j1_real = _temp228;
	uint32_t _temp229 = s_imag - t_imag;
	uint32_t j1_imag = _temp229;
	uint32_t _temp230 = u_real + v_real;
	uint32_t k0_real = _temp230;
	uint32_t _temp231 = u_imag + v_imag;
	uint32_t k0_imag = _temp231;
	uint32_t _temp232 = u_real - v_real;
	uint32_t k1_real = _temp232;
	uint32_t _temp233 = u_imag - v_imag;
	uint32_t k1_imag = _temp233;
	uint32_t _temp234 = w_real + x_real;
	uint32_t l0_real = _temp234;
	uint32_t _temp235 = w_imag + x_imag;
	uint32_t l0_imag = _temp235;
	uint32_t _temp236 = w_real - x_real;
	uint32_t l1_real = _temp236;
	uint32_t _temp237 = w_imag - x_imag;
	uint32_t l1_imag = _temp237;
	uint32_t _temp238 = y_real + z_real;
	uint32_t m0_real = _temp238;
	uint32_t _temp239 = y_imag + z_imag;
	uint32_t m0_imag = _temp239;
	uint32_t _temp240 = y_real - z_real;
	uint32_t m1_real = _temp240;
	uint32_t _temp241 = y_imag - z_imag;
	uint32_t m1_imag = _temp241;
	uint32_t _temp242 = aa_real + bb_real;
	uint32_t n0_real = _temp242;
	uint32_t _temp243 = aa_imag + bb_imag;
	uint32_t n0_imag = _temp243;
	uint32_t _temp244 = aa_real - bb_real;
	uint32_t n1_real = _temp244;
	uint32_t _temp245 = aa_imag - bb_imag;
	uint32_t n1_imag = _temp245;
	uint32_t _temp246 = cc_real + dd_real;
	uint32_t o0_real = _temp246;
	uint32_t _temp247 = cc_imag + dd_imag;
	uint32_t o0_imag = _temp247;
	uint32_t _temp248 = cc_real - dd_real;
	uint32_t o1_real = _temp248;
	uint32_t _temp249 = cc_imag - dd_imag;
	uint32_t o1_imag = _temp249;
	uint32_t _temp250 = ee_real + ff_real;
	uint32_t p0_real = _temp250;
	uint32_t _temp251 = ee_imag + ff_imag;
	uint32_t p0_imag = _temp251;
	uint32_t _temp252 = ee_real - ff_real;
	uint32_t p1_real = _temp252;
	uint32_t _temp253 = ee_imag - ff_imag;
	uint32_t p1_imag = _temp253;
	tmp_real[j] = a0_real;
	tmp_imag[j] = a0_imag;
	uint32_t _temp256 = 16;
	uint32_t _temp257 = j + _temp256;
	tmp_real[_temp257] = a1_real;
	uint32_t _temp259 = 16;
	uint32_t _temp260 = j + _temp259;
	tmp_imag[_temp260] = a1_imag;
	uint32_t _temp262 = 1;
	uint32_t _temp263 = j + _temp262;
	tmp_real[_temp263] = b0_real;
	uint32_t _temp265 = 1;
	uint32_t _temp266 = j + _temp265;
	tmp_imag[_temp266] = b0_imag;
	uint32_t _temp268 = 17;
	uint32_t _temp269 = j + _temp268;
	tmp_real[_temp269] = b1_real;
	uint32_t _temp271 = 17;
	uint32_t _temp272 = j + _temp271;
	tmp_imag[_temp272] = b1_imag;
	uint32_t _temp274 = 2;
	uint32_t _temp275 = j + _temp274;
	tmp_real[_temp275] = c0_real;
	uint32_t _temp277 = 2;
	uint32_t _temp278 = j + _temp277;
	tmp_imag[_temp278] = c0_imag;
	uint32_t _temp280 = 18;
	uint32_t _temp281 = j + _temp280;
	tmp_real[_temp281] = c1_real;
	uint32_t _temp283 = 18;
	uint32_t _temp284 = j + _temp283;
	tmp_imag[_temp284] = c1_imag;
	uint32_t _temp286 = 3;
	uint32_t _temp287 = j + _temp286;
	tmp_real[_temp287] = d0_real;
	uint32_t _temp289 = 3;
	uint32_t _temp290 = j + _temp289;
	tmp_imag[_temp290] = d0_imag;
	uint32_t _temp292 = 19;
	uint32_t _temp293 = j + _temp292;
	tmp_real[_temp293] = d1_real;
	uint32_t _temp295 = 19;
	uint32_t _temp296 = j + _temp295;
	tmp_imag[_temp296] = d1_imag;
	uint32_t _temp298 = 4;
	uint32_t _temp299 = j + _temp298;
	tmp_real[_temp299] = e0_real;
	uint32_t _temp301 = 4;
	uint32_t _temp302 = j + _temp301;
	tmp_imag[_temp302] = e0_imag;
	uint32_t _temp304 = 20;
	uint32_t _temp305 = j + _temp304;
	tmp_real[_temp305] = e1_real;
	uint32_t _temp307 = 20;
	uint32_t _temp308 = j + _temp307;
	tmp_imag[_temp308] = e1_imag;
	uint32_t _temp310 = 5;
	uint32_t _temp311 = j + _temp310;
	tmp_real[_temp311] = f0_real;
	uint32_t _temp313 = 5;
	uint32_t _temp314 = j + _temp313;
	tmp_imag[_temp314] = f0_imag;
	uint32_t _temp316 = 21;
	uint32_t _temp317 = j + _temp316;
	tmp_real[_temp317] = f1_real;
	uint32_t _temp319 = 21;
	uint32_t _temp320 = j + _temp319;
	tmp_imag[_temp320] = f1_imag;
	uint32_t _temp322 = 6;
	uint32_t _temp323 = j + _temp322;
	tmp_real[_temp323] = g0_real;
	uint32_t _temp325 = 6;
	uint32_t _temp326 = j + _temp325;
	tmp_imag[_temp326] = g0_imag;
	uint32_t _temp328 = 22;
	uint32_t _temp329 = j + _temp328;
	tmp_real[_temp329] = g1_real;
	uint32_t _temp331 = 22;
	uint32_t _temp332 = j + _temp331;
	tmp_imag[_temp332] = g1_imag;
	uint32_t _temp334 = 7;
	uint32_t _temp335 = j + _temp334;
	tmp_real[_temp335] = h0_real;
	uint32_t _temp337 = 7;
	uint32_t _temp338 = j + _temp337;
	tmp_imag[_temp338] = h0_imag;
	uint32_t _temp340 = 23;
	uint32_t _temp341 = j + _temp340;
	tmp_real[_temp341] = h1_real;
	uint32_t _temp343 = 23;
	uint32_t _temp344 = j + _temp343;
	tmp_imag[_temp344] = h1_imag;
	uint32_t _temp346 = 8;
	uint32_t _temp347 = j + _temp346;
	tmp_real[_temp347] = i0_real;
	uint32_t _temp349 = 8;
	uint32_t _temp350 = j + _temp349;
	tmp_imag[_temp350] = i0_imag;
	uint32_t _temp352 = 24;
	uint32_t _temp353 = j + _temp352;
	tmp_real[_temp353] = i1_real;
	uint32_t _temp355 = 24;
	uint32_t _temp356 = j + _temp355;
	tmp_imag[_temp356] = i1_imag;
	uint32_t _temp358 = 9;
	uint32_t _temp359 = j + _temp358;
	tmp_real[_temp359] = j0_real;
	uint32_t _temp361 = 9;
	uint32_t _temp362 = j + _temp361;
	tmp_imag[_temp362] = j0_imag;
	uint32_t _temp364 = 25;
	uint32_t _temp365 = j + _temp364;
	tmp_real[_temp365] = j1_real;
	uint32_t _temp367 = 25;
	uint32_t _temp368 = j + _temp367;
	tmp_imag[_temp368] = j1_imag;
	uint32_t _temp370 = 10;
	uint32_t _temp371 = j + _temp370;
	tmp_real[_temp371] = k0_real;
	uint32_t _temp373 = 10;
	uint32_t _temp374 = j + _temp373;
	tmp_imag[_temp374] = k0_imag;
	uint32_t _temp376 = 26;
	uint32_t _temp377 = j + _temp376;
	tmp_real[_temp377] = k1_real;
	uint32_t _temp379 = 26;
	uint32_t _temp380 = j + _temp379;
	tmp_imag[_temp380] = k1_imag;
	uint32_t _temp382 = 11;
	uint32_t _temp383 = j + _temp382;
	tmp_real[_temp383] = l0_real;
	uint32_t _temp385 = 11;
	uint32_t _temp386 = j + _temp385;
	tmp_imag[_temp386] = l0_imag;
	uint32_t _temp388 = 27;
	uint32_t _temp389 = j + _temp388;
	tmp_real[_temp389] = l1_real;
	uint32_t _temp391 = 27;
	uint32_t _temp392 = j + _temp391;
	tmp_imag[_temp392] = l1_imag;
	uint32_t _temp394 = 12;
	uint32_t _temp395 = j + _temp394;
	tmp_real[_temp395] = m0_real;
	uint32_t _temp397 = 12;
	uint32_t _temp398 = j + _temp397;
	tmp_imag[_temp398] = m0_imag;
	uint32_t _temp400 = 28;
	uint32_t _temp401 = j + _temp400;
	tmp_real[_temp401] = m1_real;
	uint32_t _temp403 = 28;
	uint32_t _temp404 = j + _temp403;
	tmp_imag[_temp404] = m1_imag;
	uint32_t _temp406 = 13;
	uint32_t _temp407 = j + _temp406;
	tmp_real[_temp407] = n0_real;
	uint32_t _temp409 = 13;
	uint32_t _temp410 = j + _temp409;
	tmp_imag[_temp410] = n0_imag;
	uint32_t _temp412 = 29;
	uint32_t _temp413 = j + _temp412;
	tmp_real[_temp413] = n1_real;
	uint32_t _temp415 = 29;
	uint32_t _temp416 = j + _temp415;
	tmp_imag[_temp416] = n1_imag;
	uint32_t _temp418 = 14;
	uint32_t _temp419 = j + _temp418;
	tmp_real[_temp419] = o0_real;
	uint32_t _temp421 = 14;
	uint32_t _temp422 = j + _temp421;
	tmp_imag[_temp422] = o0_imag;
	uint32_t _temp424 = 30;
	uint32_t _temp425 = j + _temp424;
	tmp_real[_temp425] = o1_real;
	uint32_t _temp427 = 30;
	uint32_t _temp428 = j + _temp427;
	tmp_imag[_temp428] = o1_imag;
	uint32_t _temp430 = 15;
	uint32_t _temp431 = j + _temp430;
	tmp_real[_temp431] = p0_real;
	uint32_t _temp433 = 15;
	uint32_t _temp434 = j + _temp433;
	tmp_imag[_temp434] = p0_imag;
	uint32_t _temp436 = 31;
	uint32_t _temp437 = j + _temp436;
	tmp_real[_temp437] = p1_real;
	uint32_t _temp439 = 31;
	uint32_t _temp440 = j + _temp439;
	tmp_imag[_temp440] = p1_imag;
	}
	memcpy(data_out_real, tmp_real, sizeof(tmp_real));
	memcpy(data_out_imag, tmp_imag, sizeof(tmp_imag));

	fft_64_stage_5_output output;
	memcpy(output.data_out_real, data_out_real, sizeof(data_out_real));
	memcpy(output.data_out_imag, data_out_imag, sizeof(data_out_imag));
	return output;
}

fft_64_stage_6_output fft_64_stage_6(uint32_t data_in_real[64], uint32_t data_in_imag[64]) {
	uint32_t data_out_real[64];
	uint32_t data_out_imag[64];
	// Combinational function
	uint32_t tmp_real[64];
	memcpy(tmp_real, data_in_real, sizeof(tmp_real));
	uint32_t tmp_imag[64];
	memcpy(tmp_imag, data_in_imag, sizeof(tmp_imag));
	uint32_t _temp0 = 0;
	uint32_t _temp1 = tmp_real[_temp0];
	uint32_t _temp2 = 32;
	uint32_t _temp3 = tmp_real[_temp2];
	uint32_t _temp4 = _temp1 + _temp3;
	uint32_t a_real = _temp4;
	uint32_t _temp5 = 0;
	uint32_t _temp6 = tmp_imag[_temp5];
	uint32_t _temp7 = 32;
	uint32_t _temp8 = tmp_imag[_temp7];
	uint32_t _temp9 = _temp6 + _temp8;
	uint32_t a_imag = _temp9;
	uint32_t _temp10 = 0;
	uint32_t _temp11 = tmp_real[_temp10];
	uint32_t _temp12 = 32;
	uint32_t _temp13 = tmp_real[_temp12];
	uint32_t _temp14 = _temp11 - _temp13;
	uint32_t a1_real = _temp14;
	uint32_t _temp15 = 0;
	uint32_t _temp16 = tmp_imag[_temp15];
	uint32_t _temp17 = 32;
	uint32_t _temp18 = tmp_imag[_temp17];
	uint32_t _temp19 = _temp16 - _temp18;
	uint32_t a1_imag = _temp19;
	uint32_t _temp20 = 1;
	uint32_t _temp21 = tmp_real[_temp20];
	uint32_t _temp22 = 33;
	uint32_t _temp23 = tmp_real[_temp22];
	uint32_t _temp24 = _temp21 + _temp23;
	uint32_t b_real = _temp24;
	uint32_t _temp25 = 1;
	uint32_t _temp26 = tmp_imag[_temp25];
	uint32_t _temp27 = 33;
	uint32_t _temp28 = tmp_imag[_temp27];
	uint32_t _temp29 = _temp26 + _temp28;
	uint32_t b_imag = _temp29;
	uint32_t _temp30 = 1;
	uint32_t _temp31 = tmp_real[_temp30];
	uint32_t _temp32 = 33;
	uint32_t _temp33 = tmp_real[_temp32];
	uint32_t _temp34 = _temp31 - _temp33;
	uint32_t b1_real = _temp34;
	uint32_t _temp35 = 1;
	uint32_t _temp36 = tmp_imag[_temp35];
	uint32_t _temp37 = 33;
	uint32_t _temp38 = tmp_imag[_temp37];
	uint32_t _temp39 = _temp36 - _temp38;
	uint32_t b1_imag = _temp39;
	uint32_t _temp40 = 2;
	uint32_t _temp41 = tmp_real[_temp40];
	uint32_t _temp42 = 34;
	uint32_t _temp43 = tmp_real[_temp42];
	uint32_t _temp44 = _temp41 + _temp43;
	uint32_t c_real = _temp44;
	uint32_t _temp45 = 2;
	uint32_t _temp46 = tmp_imag[_temp45];
	uint32_t _temp47 = 34;
	uint32_t _temp48 = tmp_imag[_temp47];
	uint32_t _temp49 = _temp46 + _temp48;
	uint32_t c_imag = _temp49;
	uint32_t _temp50 = 2;
	uint32_t _temp51 = tmp_real[_temp50];
	uint32_t _temp52 = 34;
	uint32_t _temp53 = tmp_real[_temp52];
	uint32_t _temp54 = _temp51 - _temp53;
	uint32_t c1_real = _temp54;
	uint32_t _temp55 = 2;
	uint32_t _temp56 = tmp_imag[_temp55];
	uint32_t _temp57 = 34;
	uint32_t _temp58 = tmp_imag[_temp57];
	uint32_t _temp59 = _temp56 - _temp58;
	uint32_t c1_imag = _temp59;
	uint32_t _temp60 = 3;
	uint32_t _temp61 = tmp_real[_temp60];
	uint32_t _temp62 = 35;
	uint32_t _temp63 = tmp_real[_temp62];
	uint32_t _temp64 = _temp61 + _temp63;
	uint32_t d_real = _temp64;
	uint32_t _temp65 = 3;
	uint32_t _temp66 = tmp_imag[_temp65];
	uint32_t _temp67 = 35;
	uint32_t _temp68 = tmp_imag[_temp67];
	uint32_t _temp69 = _temp66 + _temp68;
	uint32_t d_imag = _temp69;
	uint32_t _temp70 = 3;
	uint32_t _temp71 = tmp_real[_temp70];
	uint32_t _temp72 = 35;
	uint32_t _temp73 = tmp_real[_temp72];
	uint32_t _temp74 = _temp71 - _temp73;
	uint32_t d1_real = _temp74;
	uint32_t _temp75 = 3;
	uint32_t _temp76 = tmp_imag[_temp75];
	uint32_t _temp77 = 35;
	uint32_t _temp78 = tmp_imag[_temp77];
	uint32_t _temp79 = _temp76 - _temp78;
	uint32_t d1_imag = _temp79;
	uint32_t _temp80 = 4;
	uint32_t _temp81 = tmp_real[_temp80];
	uint32_t _temp82 = 36;
	uint32_t _temp83 = tmp_real[_temp82];
	uint32_t _temp84 = _temp81 + _temp83;
	uint32_t e_real = _temp84;
	uint32_t _temp85 = 4;
	uint32_t _temp86 = tmp_imag[_temp85];
	uint32_t _temp87 = 36;
	uint32_t _temp88 = tmp_imag[_temp87];
	uint32_t _temp89 = _temp86 + _temp88;
	uint32_t e_imag = _temp89;
	uint32_t _temp90 = 4;
	uint32_t _temp91 = tmp_real[_temp90];
	uint32_t _temp92 = 36;
	uint32_t _temp93 = tmp_real[_temp92];
	uint32_t _temp94 = _temp91 - _temp93;
	uint32_t e1_real = _temp94;
	uint32_t _temp95 = 4;
	uint32_t _temp96 = tmp_imag[_temp95];
	uint32_t _temp97 = 36;
	uint32_t _temp98 = tmp_imag[_temp97];
	uint32_t _temp99 = _temp96 - _temp98;
	uint32_t e1_imag = _temp99;
	uint32_t _temp100 = 5;
	uint32_t _temp101 = tmp_real[_temp100];
	uint32_t _temp102 = 37;
	uint32_t _temp103 = tmp_real[_temp102];
	uint32_t _temp104 = _temp101 + _temp103;
	uint32_t f_real = _temp104;
	uint32_t _temp105 = 5;
	uint32_t _temp106 = tmp_imag[_temp105];
	uint32_t _temp107 = 37;
	uint32_t _temp108 = tmp_imag[_temp107];
	uint32_t _temp109 = _temp106 + _temp108;
	uint32_t f_imag = _temp109;
	uint32_t _temp110 = 5;
	uint32_t _temp111 = tmp_real[_temp110];
	uint32_t _temp112 = 37;
	uint32_t _temp113 = tmp_real[_temp112];
	uint32_t _temp114 = _temp111 - _temp113;
	uint32_t f1_real = _temp114;
	uint32_t _temp115 = 5;
	uint32_t _temp116 = tmp_imag[_temp115];
	uint32_t _temp117 = 37;
	uint32_t _temp118 = tmp_imag[_temp117];
	uint32_t _temp119 = _temp116 - _temp118;
	uint32_t f1_imag = _temp119;
	uint32_t _temp120 = 6;
	uint32_t _temp121 = tmp_real[_temp120];
	uint32_t _temp122 = 38;
	uint32_t _temp123 = tmp_real[_temp122];
	uint32_t _temp124 = _temp121 + _temp123;
	uint32_t g_real = _temp124;
	uint32_t _temp125 = 6;
	uint32_t _temp126 = tmp_imag[_temp125];
	uint32_t _temp127 = 38;
	uint32_t _temp128 = tmp_imag[_temp127];
	uint32_t _temp129 = _temp126 + _temp128;
	uint32_t g_imag = _temp129;
	uint32_t _temp130 = 6;
	uint32_t _temp131 = tmp_real[_temp130];
	uint32_t _temp132 = 38;
	uint32_t _temp133 = tmp_real[_temp132];
	uint32_t _temp134 = _temp131 - _temp133;
	uint32_t g1_real = _temp134;
	uint32_t _temp135 = 6;
	uint32_t _temp136 = tmp_imag[_temp135];
	uint32_t _temp137 = 38;
	uint32_t _temp138 = tmp_imag[_temp137];
	uint32_t _temp139 = _temp136 - _temp138;
	uint32_t g1_imag = _temp139;
	uint32_t _temp140 = 7;
	uint32_t _temp141 = tmp_real[_temp140];
	uint32_t _temp142 = 39;
	uint32_t _temp143 = tmp_real[_temp142];
	uint32_t _temp144 = _temp141 + _temp143;
	uint32_t h_real = _temp144;
	uint32_t _temp145 = 7;
	uint32_t _temp146 = tmp_imag[_temp145];
	uint32_t _temp147 = 39;
	uint32_t _temp148 = tmp_imag[_temp147];
	uint32_t _temp149 = _temp146 + _temp148;
	uint32_t h_imag = _temp149;
	uint32_t _temp150 = 7;
	uint32_t _temp151 = tmp_real[_temp150];
	uint32_t _temp152 = 39;
	uint32_t _temp153 = tmp_real[_temp152];
	uint32_t _temp154 = _temp151 - _temp153;
	uint32_t h1_real = _temp154;
	uint32_t _temp155 = 7;
	uint32_t _temp156 = tmp_imag[_temp155];
	uint32_t _temp157 = 39;
	uint32_t _temp158 = tmp_imag[_temp157];
	uint32_t _temp159 = _temp156 - _temp158;
	uint32_t h1_imag = _temp159;
	uint32_t _temp160 = 8;
	uint32_t _temp161 = tmp_real[_temp160];
	uint32_t _temp162 = 40;
	uint32_t _temp163 = tmp_real[_temp162];
	uint32_t _temp164 = _temp161 + _temp163;
	uint32_t i_real = _temp164;
	uint32_t _temp165 = 8;
	uint32_t _temp166 = tmp_imag[_temp165];
	uint32_t _temp167 = 40;
	uint32_t _temp168 = tmp_imag[_temp167];
	uint32_t _temp169 = _temp166 + _temp168;
	uint32_t i_imag = _temp169;
	uint32_t _temp170 = 8;
	uint32_t _temp171 = tmp_real[_temp170];
	uint32_t _temp172 = 40;
	uint32_t _temp173 = tmp_real[_temp172];
	uint32_t _temp174 = _temp171 - _temp173;
	uint32_t i1_real = _temp174;
	uint32_t _temp175 = 8;
	uint32_t _temp176 = tmp_imag[_temp175];
	uint32_t _temp177 = 40;
	uint32_t _temp178 = tmp_imag[_temp177];
	uint32_t _temp179 = _temp176 - _temp178;
	uint32_t i1_imag = _temp179;
	uint32_t _temp180 = 9;
	uint32_t _temp181 = tmp_real[_temp180];
	uint32_t _temp182 = 41;
	uint32_t _temp183 = tmp_real[_temp182];
	uint32_t _temp184 = _temp181 + _temp183;
	uint32_t j_real = _temp184;
	uint32_t _temp185 = 9;
	uint32_t _temp186 = tmp_imag[_temp185];
	uint32_t _temp187 = 41;
	uint32_t _temp188 = tmp_imag[_temp187];
	uint32_t _temp189 = _temp186 + _temp188;
	uint32_t j_imag = _temp189;
	uint32_t _temp190 = 9;
	uint32_t _temp191 = tmp_real[_temp190];
	uint32_t _temp192 = 41;
	uint32_t _temp193 = tmp_real[_temp192];
	uint32_t _temp194 = _temp191 - _temp193;
	uint32_t j1_real = _temp194;
	uint32_t _temp195 = 9;
	uint32_t _temp196 = tmp_imag[_temp195];
	uint32_t _temp197 = 41;
	uint32_t _temp198 = tmp_imag[_temp197];
	uint32_t _temp199 = _temp196 - _temp198;
	uint32_t j1_imag = _temp199;
	uint32_t _temp200 = 10;
	uint32_t _temp201 = tmp_real[_temp200];
	uint32_t _temp202 = 42;
	uint32_t _temp203 = tmp_real[_temp202];
	uint32_t _temp204 = _temp201 + _temp203;
	uint32_t k_real = _temp204;
	uint32_t _temp205 = 10;
	uint32_t _temp206 = tmp_imag[_temp205];
	uint32_t _temp207 = 42;
	uint32_t _temp208 = tmp_imag[_temp207];
	uint32_t _temp209 = _temp206 + _temp208;
	uint32_t k_imag = _temp209;
	uint32_t _temp210 = 10;
	uint32_t _temp211 = tmp_real[_temp210];
	uint32_t _temp212 = 42;
	uint32_t _temp213 = tmp_real[_temp212];
	uint32_t _temp214 = _temp211 - _temp213;
	uint32_t k1_real = _temp214;
	uint32_t _temp215 = 10;
	uint32_t _temp216 = tmp_imag[_temp215];
	uint32_t _temp217 = 42;
	uint32_t _temp218 = tmp_imag[_temp217];
	uint32_t _temp219 = _temp216 - _temp218;
	uint32_t k1_imag = _temp219;
	uint32_t _temp220 = 11;
	uint32_t _temp221 = tmp_real[_temp220];
	uint32_t _temp222 = 43;
	uint32_t _temp223 = tmp_real[_temp222];
	uint32_t _temp224 = _temp221 + _temp223;
	uint32_t l_real = _temp224;
	uint32_t _temp225 = 11;
	uint32_t _temp226 = tmp_imag[_temp225];
	uint32_t _temp227 = 43;
	uint32_t _temp228 = tmp_imag[_temp227];
	uint32_t _temp229 = _temp226 + _temp228;
	uint32_t l_imag = _temp229;
	uint32_t _temp230 = 11;
	uint32_t _temp231 = tmp_real[_temp230];
	uint32_t _temp232 = 43;
	uint32_t _temp233 = tmp_real[_temp232];
	uint32_t _temp234 = _temp231 - _temp233;
	uint32_t l1_real = _temp234;
	uint32_t _temp235 = 11;
	uint32_t _temp236 = tmp_imag[_temp235];
	uint32_t _temp237 = 43;
	uint32_t _temp238 = tmp_imag[_temp237];
	uint32_t _temp239 = _temp236 - _temp238;
	uint32_t l1_imag = _temp239;
	uint32_t _temp240 = 12;
	uint32_t _temp241 = tmp_real[_temp240];
	uint32_t _temp242 = 44;
	uint32_t _temp243 = tmp_real[_temp242];
	uint32_t _temp244 = _temp241 + _temp243;
	uint32_t m_real = _temp244;
	uint32_t _temp245 = 12;
	uint32_t _temp246 = tmp_imag[_temp245];
	uint32_t _temp247 = 44;
	uint32_t _temp248 = tmp_imag[_temp247];
	uint32_t _temp249 = _temp246 + _temp248;
	uint32_t m_imag = _temp249;
	uint32_t _temp250 = 12;
	uint32_t _temp251 = tmp_real[_temp250];
	uint32_t _temp252 = 44;
	uint32_t _temp253 = tmp_real[_temp252];
	uint32_t _temp254 = _temp251 - _temp253;
	uint32_t m1_real = _temp254;
	uint32_t _temp255 = 12;
	uint32_t _temp256 = tmp_imag[_temp255];
	uint32_t _temp257 = 44;
	uint32_t _temp258 = tmp_imag[_temp257];
	uint32_t _temp259 = _temp256 - _temp258;
	uint32_t m1_imag = _temp259;
	uint32_t _temp260 = 13;
	uint32_t _temp261 = tmp_real[_temp260];
	uint32_t _temp262 = 45;
	uint32_t _temp263 = tmp_real[_temp262];
	uint32_t _temp264 = _temp261 + _temp263;
	uint32_t n_real = _temp264;
	uint32_t _temp265 = 13;
	uint32_t _temp266 = tmp_imag[_temp265];
	uint32_t _temp267 = 45;
	uint32_t _temp268 = tmp_imag[_temp267];
	uint32_t _temp269 = _temp266 + _temp268;
	uint32_t n_imag = _temp269;
	uint32_t _temp270 = 13;
	uint32_t _temp271 = tmp_real[_temp270];
	uint32_t _temp272 = 45;
	uint32_t _temp273 = tmp_real[_temp272];
	uint32_t _temp274 = _temp271 - _temp273;
	uint32_t n1_real = _temp274;
	uint32_t _temp275 = 13;
	uint32_t _temp276 = tmp_imag[_temp275];
	uint32_t _temp277 = 45;
	uint32_t _temp278 = tmp_imag[_temp277];
	uint32_t _temp279 = _temp276 - _temp278;
	uint32_t n1_imag = _temp279;
	uint32_t _temp280 = 14;
	uint32_t _temp281 = tmp_real[_temp280];
	uint32_t _temp282 = 46;
	uint32_t _temp283 = tmp_real[_temp282];
	uint32_t _temp284 = _temp281 + _temp283;
	uint32_t o_real = _temp284;
	uint32_t _temp285 = 14;
	uint32_t _temp286 = tmp_imag[_temp285];
	uint32_t _temp287 = 46;
	uint32_t _temp288 = tmp_imag[_temp287];
	uint32_t _temp289 = _temp286 + _temp288;
	uint32_t o_imag = _temp289;
	uint32_t _temp290 = 14;
	uint32_t _temp291 = tmp_real[_temp290];
	uint32_t _temp292 = 46;
	uint32_t _temp293 = tmp_real[_temp292];
	uint32_t _temp294 = _temp291 - _temp293;
	uint32_t o1_real = _temp294;
	uint32_t _temp295 = 14;
	uint32_t _temp296 = tmp_imag[_temp295];
	uint32_t _temp297 = 46;
	uint32_t _temp298 = tmp_imag[_temp297];
	uint32_t _temp299 = _temp296 - _temp298;
	uint32_t o1_imag = _temp299;
	uint32_t _temp300 = 15;
	uint32_t _temp301 = tmp_real[_temp300];
	uint32_t _temp302 = 47;
	uint32_t _temp303 = tmp_real[_temp302];
	uint32_t _temp304 = _temp301 + _temp303;
	uint32_t p_real = _temp304;
	uint32_t _temp305 = 15;
	uint32_t _temp306 = tmp_imag[_temp305];
	uint32_t _temp307 = 47;
	uint32_t _temp308 = tmp_imag[_temp307];
	uint32_t _temp309 = _temp306 + _temp308;
	uint32_t p_imag = _temp309;
	uint32_t _temp310 = 15;
	uint32_t _temp311 = tmp_real[_temp310];
	uint32_t _temp312 = 47;
	uint32_t _temp313 = tmp_real[_temp312];
	uint32_t _temp314 = _temp311 - _temp313;
	uint32_t p1_real = _temp314;
	uint32_t _temp315 = 15;
	uint32_t _temp316 = tmp_imag[_temp315];
	uint32_t _temp317 = 47;
	uint32_t _temp318 = tmp_imag[_temp317];
	uint32_t _temp319 = _temp316 - _temp318;
	uint32_t p1_imag = _temp319;
	uint32_t _temp320 = 16;
	uint32_t _temp321 = tmp_real[_temp320];
	uint32_t _temp322 = 48;
	uint32_t _temp323 = tmp_real[_temp322];
	uint32_t _temp324 = _temp321 + _temp323;
	uint32_t q_real = _temp324;
	uint32_t _temp325 = 16;
	uint32_t _temp326 = tmp_imag[_temp325];
	uint32_t _temp327 = 48;
	uint32_t _temp328 = tmp_imag[_temp327];
	uint32_t _temp329 = _temp326 + _temp328;
	uint32_t q_imag = _temp329;
	uint32_t _temp330 = 16;
	uint32_t _temp331 = tmp_real[_temp330];
	uint32_t _temp332 = 48;
	uint32_t _temp333 = tmp_real[_temp332];
	uint32_t _temp334 = _temp331 - _temp333;
	uint32_t q1_real = _temp334;
	uint32_t _temp335 = 16;
	uint32_t _temp336 = tmp_imag[_temp335];
	uint32_t _temp337 = 48;
	uint32_t _temp338 = tmp_imag[_temp337];
	uint32_t _temp339 = _temp336 - _temp338;
	uint32_t q1_imag = _temp339;
	uint32_t _temp340 = 17;
	uint32_t _temp341 = tmp_real[_temp340];
	uint32_t _temp342 = 49;
	uint32_t _temp343 = tmp_real[_temp342];
	uint32_t _temp344 = _temp341 + _temp343;
	uint32_t r_real = _temp344;
	uint32_t _temp345 = 17;
	uint32_t _temp346 = tmp_imag[_temp345];
	uint32_t _temp347 = 49;
	uint32_t _temp348 = tmp_imag[_temp347];
	uint32_t _temp349 = _temp346 + _temp348;
	uint32_t r_imag = _temp349;
	uint32_t _temp350 = 17;
	uint32_t _temp351 = tmp_real[_temp350];
	uint32_t _temp352 = 49;
	uint32_t _temp353 = tmp_real[_temp352];
	uint32_t _temp354 = _temp351 - _temp353;
	uint32_t r1_real = _temp354;
	uint32_t _temp355 = 17;
	uint32_t _temp356 = tmp_imag[_temp355];
	uint32_t _temp357 = 49;
	uint32_t _temp358 = tmp_imag[_temp357];
	uint32_t _temp359 = _temp356 - _temp358;
	uint32_t r1_imag = _temp359;
	uint32_t _temp360 = 18;
	uint32_t _temp361 = tmp_real[_temp360];
	uint32_t _temp362 = 50;
	uint32_t _temp363 = tmp_real[_temp362];
	uint32_t _temp364 = _temp361 + _temp363;
	uint32_t s_real = _temp364;
	uint32_t _temp365 = 18;
	uint32_t _temp366 = tmp_imag[_temp365];
	uint32_t _temp367 = 50;
	uint32_t _temp368 = tmp_imag[_temp367];
	uint32_t _temp369 = _temp366 + _temp368;
	uint32_t s_imag = _temp369;
	uint32_t _temp370 = 18;
	uint32_t _temp371 = tmp_real[_temp370];
	uint32_t _temp372 = 50;
	uint32_t _temp373 = tmp_real[_temp372];
	uint32_t _temp374 = _temp371 - _temp373;
	uint32_t s1_real = _temp374;
	uint32_t _temp375 = 18;
	uint32_t _temp376 = tmp_imag[_temp375];
	uint32_t _temp377 = 50;
	uint32_t _temp378 = tmp_imag[_temp377];
	uint32_t _temp379 = _temp376 - _temp378;
	uint32_t s1_imag = _temp379;
	uint32_t _temp380 = 19;
	uint32_t _temp381 = tmp_real[_temp380];
	uint32_t _temp382 = 51;
	uint32_t _temp383 = tmp_real[_temp382];
	uint32_t _temp384 = _temp381 + _temp383;
	uint32_t t_real = _temp384;
	uint32_t _temp385 = 19;
	uint32_t _temp386 = tmp_imag[_temp385];
	uint32_t _temp387 = 51;
	uint32_t _temp388 = tmp_imag[_temp387];
	uint32_t _temp389 = _temp386 + _temp388;
	uint32_t t_imag = _temp389;
	uint32_t _temp390 = 19;
	uint32_t _temp391 = tmp_real[_temp390];
	uint32_t _temp392 = 51;
	uint32_t _temp393 = tmp_real[_temp392];
	uint32_t _temp394 = _temp391 - _temp393;
	uint32_t t1_real = _temp394;
	uint32_t _temp395 = 19;
	uint32_t _temp396 = tmp_imag[_temp395];
	uint32_t _temp397 = 51;
	uint32_t _temp398 = tmp_imag[_temp397];
	uint32_t _temp399 = _temp396 - _temp398;
	uint32_t t1_imag = _temp399;
	uint32_t _temp400 = 20;
	uint32_t _temp401 = tmp_real[_temp400];
	uint32_t _temp402 = 52;
	uint32_t _temp403 = tmp_real[_temp402];
	uint32_t _temp404 = _temp401 + _temp403;
	uint32_t u_real = _temp404;
	uint32_t _temp405 = 20;
	uint32_t _temp406 = tmp_imag[_temp405];
	uint32_t _temp407 = 52;
	uint32_t _temp408 = tmp_imag[_temp407];
	uint32_t _temp409 = _temp406 + _temp408;
	uint32_t u_imag = _temp409;
	uint32_t _temp410 = 20;
	uint32_t _temp411 = tmp_real[_temp410];
	uint32_t _temp412 = 52;
	uint32_t _temp413 = tmp_real[_temp412];
	uint32_t _temp414 = _temp411 - _temp413;
	uint32_t u1_real = _temp414;
	uint32_t _temp415 = 20;
	uint32_t _temp416 = tmp_imag[_temp415];
	uint32_t _temp417 = 52;
	uint32_t _temp418 = tmp_imag[_temp417];
	uint32_t _temp419 = _temp416 - _temp418;
	uint32_t u1_imag = _temp419;
	uint32_t _temp420 = 21;
	uint32_t _temp421 = tmp_real[_temp420];
	uint32_t _temp422 = 53;
	uint32_t _temp423 = tmp_real[_temp422];
	uint32_t _temp424 = _temp421 + _temp423;
	uint32_t v_real = _temp424;
	uint32_t _temp425 = 21;
	uint32_t _temp426 = tmp_imag[_temp425];
	uint32_t _temp427 = 53;
	uint32_t _temp428 = tmp_imag[_temp427];
	uint32_t _temp429 = _temp426 + _temp428;
	uint32_t v_imag = _temp429;
	uint32_t _temp430 = 21;
	uint32_t _temp431 = tmp_real[_temp430];
	uint32_t _temp432 = 53;
	uint32_t _temp433 = tmp_real[_temp432];
	uint32_t _temp434 = _temp431 - _temp433;
	uint32_t v1_real = _temp434;
	uint32_t _temp435 = 21;
	uint32_t _temp436 = tmp_imag[_temp435];
	uint32_t _temp437 = 53;
	uint32_t _temp438 = tmp_imag[_temp437];
	uint32_t _temp439 = _temp436 - _temp438;
	uint32_t v1_imag = _temp439;
	uint32_t _temp440 = 22;
	uint32_t _temp441 = tmp_real[_temp440];
	uint32_t _temp442 = 54;
	uint32_t _temp443 = tmp_real[_temp442];
	uint32_t _temp444 = _temp441 + _temp443;
	uint32_t w_real = _temp444;
	uint32_t _temp445 = 22;
	uint32_t _temp446 = tmp_imag[_temp445];
	uint32_t _temp447 = 54;
	uint32_t _temp448 = tmp_imag[_temp447];
	uint32_t _temp449 = _temp446 + _temp448;
	uint32_t w_imag = _temp449;
	uint32_t _temp450 = 22;
	uint32_t _temp451 = tmp_real[_temp450];
	uint32_t _temp452 = 54;
	uint32_t _temp453 = tmp_real[_temp452];
	uint32_t _temp454 = _temp451 - _temp453;
	uint32_t w1_real = _temp454;
	uint32_t _temp455 = 22;
	uint32_t _temp456 = tmp_imag[_temp455];
	uint32_t _temp457 = 54;
	uint32_t _temp458 = tmp_imag[_temp457];
	uint32_t _temp459 = _temp456 - _temp458;
	uint32_t w1_imag = _temp459;
	uint32_t _temp460 = 23;
	uint32_t _temp461 = tmp_real[_temp460];
	uint32_t _temp462 = 55;
	uint32_t _temp463 = tmp_real[_temp462];
	uint32_t _temp464 = _temp461 + _temp463;
	uint32_t x_real = _temp464;
	uint32_t _temp465 = 23;
	uint32_t _temp466 = tmp_imag[_temp465];
	uint32_t _temp467 = 55;
	uint32_t _temp468 = tmp_imag[_temp467];
	uint32_t _temp469 = _temp466 + _temp468;
	uint32_t x_imag = _temp469;
	uint32_t _temp470 = 23;
	uint32_t _temp471 = tmp_real[_temp470];
	uint32_t _temp472 = 55;
	uint32_t _temp473 = tmp_real[_temp472];
	uint32_t _temp474 = _temp471 - _temp473;
	uint32_t x1_real = _temp474;
	uint32_t _temp475 = 23;
	uint32_t _temp476 = tmp_imag[_temp475];
	uint32_t _temp477 = 55;
	uint32_t _temp478 = tmp_imag[_temp477];
	uint32_t _temp479 = _temp476 - _temp478;
	uint32_t x1_imag = _temp479;
	uint32_t _temp480 = 24;
	uint32_t _temp481 = tmp_real[_temp480];
	uint32_t _temp482 = 56;
	uint32_t _temp483 = tmp_real[_temp482];
	uint32_t _temp484 = _temp481 + _temp483;
	uint32_t y_real = _temp484;
	uint32_t _temp485 = 24;
	uint32_t _temp486 = tmp_imag[_temp485];
	uint32_t _temp487 = 56;
	uint32_t _temp488 = tmp_imag[_temp487];
	uint32_t _temp489 = _temp486 + _temp488;
	uint32_t y_imag = _temp489;
	uint32_t _temp490 = 24;
	uint32_t _temp491 = tmp_real[_temp490];
	uint32_t _temp492 = 56;
	uint32_t _temp493 = tmp_real[_temp492];
	uint32_t _temp494 = _temp491 - _temp493;
	uint32_t y1_real = _temp494;
	uint32_t _temp495 = 24;
	uint32_t _temp496 = tmp_imag[_temp495];
	uint32_t _temp497 = 56;
	uint32_t _temp498 = tmp_imag[_temp497];
	uint32_t _temp499 = _temp496 - _temp498;
	uint32_t y1_imag = _temp499;
	uint32_t _temp500 = 25;
	uint32_t _temp501 = tmp_real[_temp500];
	uint32_t _temp502 = 57;
	uint32_t _temp503 = tmp_real[_temp502];
	uint32_t _temp504 = _temp501 + _temp503;
	uint32_t z_real = _temp504;
	uint32_t _temp505 = 25;
	uint32_t _temp506 = tmp_imag[_temp505];
	uint32_t _temp507 = 57;
	uint32_t _temp508 = tmp_imag[_temp507];
	uint32_t _temp509 = _temp506 + _temp508;
	uint32_t z_imag = _temp509;
	uint32_t _temp510 = 25;
	uint32_t _temp511 = tmp_real[_temp510];
	uint32_t _temp512 = 57;
	uint32_t _temp513 = tmp_real[_temp512];
	uint32_t _temp514 = _temp511 - _temp513;
	uint32_t z1_real = _temp514;
	uint32_t _temp515 = 25;
	uint32_t _temp516 = tmp_imag[_temp515];
	uint32_t _temp517 = 57;
	uint32_t _temp518 = tmp_imag[_temp517];
	uint32_t _temp519 = _temp516 - _temp518;
	uint32_t z1_imag = _temp519;
	uint32_t _temp520 = 26;
	uint32_t _temp521 = tmp_real[_temp520];
	uint32_t _temp522 = 58;
	uint32_t _temp523 = tmp_real[_temp522];
	uint32_t _temp524 = _temp521 + _temp523;
	uint32_t aa_real = _temp524;
	uint32_t _temp525 = 26;
	uint32_t _temp526 = tmp_imag[_temp525];
	uint32_t _temp527 = 58;
	uint32_t _temp528 = tmp_imag[_temp527];
	uint32_t _temp529 = _temp526 + _temp528;
	uint32_t aa_imag = _temp529;
	uint32_t _temp530 = 26;
	uint32_t _temp531 = tmp_real[_temp530];
	uint32_t _temp532 = 58;
	uint32_t _temp533 = tmp_real[_temp532];
	uint32_t _temp534 = _temp531 - _temp533;
	uint32_t aa1_real = _temp534;
	uint32_t _temp535 = 26;
	uint32_t _temp536 = tmp_imag[_temp535];
	uint32_t _temp537 = 58;
	uint32_t _temp538 = tmp_imag[_temp537];
	uint32_t _temp539 = _temp536 - _temp538;
	uint32_t aa1_imag = _temp539;
	uint32_t _temp540 = 27;
	uint32_t _temp541 = tmp_real[_temp540];
	uint32_t _temp542 = 59;
	uint32_t _temp543 = tmp_real[_temp542];
	uint32_t _temp544 = _temp541 + _temp543;
	uint32_t bb_real = _temp544;
	uint32_t _temp545 = 27;
	uint32_t _temp546 = tmp_imag[_temp545];
	uint32_t _temp547 = 59;
	uint32_t _temp548 = tmp_imag[_temp547];
	uint32_t _temp549 = _temp546 + _temp548;
	uint32_t bb_imag = _temp549;
	uint32_t _temp550 = 27;
	uint32_t _temp551 = tmp_real[_temp550];
	uint32_t _temp552 = 59;
	uint32_t _temp553 = tmp_real[_temp552];
	uint32_t _temp554 = _temp551 - _temp553;
	uint32_t bb1_real = _temp554;
	uint32_t _temp555 = 27;
	uint32_t _temp556 = tmp_imag[_temp555];
	uint32_t _temp557 = 59;
	uint32_t _temp558 = tmp_imag[_temp557];
	uint32_t _temp559 = _temp556 - _temp558;
	uint32_t bb1_imag = _temp559;
	uint32_t _temp560 = 28;
	uint32_t _temp561 = tmp_real[_temp560];
	uint32_t _temp562 = 60;
	uint32_t _temp563 = tmp_real[_temp562];
	uint32_t _temp564 = _temp561 + _temp563;
	uint32_t cc_real = _temp564;
	uint32_t _temp565 = 28;
	uint32_t _temp566 = tmp_imag[_temp565];
	uint32_t _temp567 = 60;
	uint32_t _temp568 = tmp_imag[_temp567];
	uint32_t _temp569 = _temp566 + _temp568;
	uint32_t cc_imag = _temp569;
	uint32_t _temp570 = 28;
	uint32_t _temp571 = tmp_real[_temp570];
	uint32_t _temp572 = 60;
	uint32_t _temp573 = tmp_real[_temp572];
	uint32_t _temp574 = _temp571 - _temp573;
	uint32_t cc1_real = _temp574;
	uint32_t _temp575 = 28;
	uint32_t _temp576 = tmp_imag[_temp575];
	uint32_t _temp577 = 60;
	uint32_t _temp578 = tmp_imag[_temp577];
	uint32_t _temp579 = _temp576 - _temp578;
	uint32_t cc1_imag = _temp579;
	uint32_t _temp580 = 29;
	uint32_t _temp581 = tmp_real[_temp580];
	uint32_t _temp582 = 61;
	uint32_t _temp583 = tmp_real[_temp582];
	uint32_t _temp584 = _temp581 + _temp583;
	uint32_t dd_real = _temp584;
	uint32_t _temp585 = 29;
	uint32_t _temp586 = tmp_imag[_temp585];
	uint32_t _temp587 = 61;
	uint32_t _temp588 = tmp_imag[_temp587];
	uint32_t _temp589 = _temp586 + _temp588;
	uint32_t dd_imag = _temp589;
	uint32_t _temp590 = 29;
	uint32_t _temp591 = tmp_real[_temp590];
	uint32_t _temp592 = 61;
	uint32_t _temp593 = tmp_real[_temp592];
	uint32_t _temp594 = _temp591 - _temp593;
	uint32_t dd1_real = _temp594;
	uint32_t _temp595 = 29;
	uint32_t _temp596 = tmp_imag[_temp595];
	uint32_t _temp597 = 61;
	uint32_t _temp598 = tmp_imag[_temp597];
	uint32_t _temp599 = _temp596 - _temp598;
	uint32_t dd1_imag = _temp599;
	uint32_t _temp600 = 30;
	uint32_t _temp601 = tmp_real[_temp600];
	uint32_t _temp602 = 62;
	uint32_t _temp603 = tmp_real[_temp602];
	uint32_t _temp604 = _temp601 + _temp603;
	uint32_t ee_real = _temp604;
	uint32_t _temp605 = 30;
	uint32_t _temp606 = tmp_imag[_temp605];
	uint32_t _temp607 = 62;
	uint32_t _temp608 = tmp_imag[_temp607];
	uint32_t _temp609 = _temp606 + _temp608;
	uint32_t ee_imag = _temp609;
	uint32_t _temp610 = 30;
	uint32_t _temp611 = tmp_real[_temp610];
	uint32_t _temp612 = 62;
	uint32_t _temp613 = tmp_real[_temp612];
	uint32_t _temp614 = _temp611 - _temp613;
	uint32_t ee1_real = _temp614;
	uint32_t _temp615 = 30;
	uint32_t _temp616 = tmp_imag[_temp615];
	uint32_t _temp617 = 62;
	uint32_t _temp618 = tmp_imag[_temp617];
	uint32_t _temp619 = _temp616 - _temp618;
	uint32_t ee1_imag = _temp619;
	uint32_t _temp620 = 31;
	uint32_t _temp621 = tmp_real[_temp620];
	uint32_t _temp622 = 63;
	uint32_t _temp623 = tmp_real[_temp622];
	uint32_t _temp624 = _temp621 + _temp623;
	uint32_t ff_real = _temp624;
	uint32_t _temp625 = 31;
	uint32_t _temp626 = tmp_imag[_temp625];
	uint32_t _temp627 = 63;
	uint32_t _temp628 = tmp_imag[_temp627];
	uint32_t _temp629 = _temp626 + _temp628;
	uint32_t ff_imag = _temp629;
	uint32_t _temp630 = 31;
	uint32_t _temp631 = tmp_real[_temp630];
	uint32_t _temp632 = 63;
	uint32_t _temp633 = tmp_real[_temp632];
	uint32_t _temp634 = _temp631 - _temp633;
	uint32_t ff1_real = _temp634;
	uint32_t _temp635 = 31;
	uint32_t _temp636 = tmp_imag[_temp635];
	uint32_t _temp637 = 63;
	uint32_t _temp638 = tmp_imag[_temp637];
	uint32_t _temp639 = _temp636 - _temp638;
	uint32_t ff1_imag = _temp639;
	uint32_t _temp640 = 0;
	tmp_real[_temp640] = a_real;
	uint32_t _temp642 = 0;
	tmp_imag[_temp642] = a_imag;
	uint32_t _temp644 = 32;
	tmp_real[_temp644] = a1_real;
	uint32_t _temp646 = 32;
	tmp_imag[_temp646] = a1_imag;
	uint32_t _temp648 = 1;
	tmp_real[_temp648] = b_real;
	uint32_t _temp650 = 1;
	tmp_imag[_temp650] = b_imag;
	uint32_t _temp652 = 33;
	tmp_real[_temp652] = b1_real;
	uint32_t _temp654 = 33;
	tmp_imag[_temp654] = b1_imag;
	uint32_t _temp656 = 2;
	tmp_real[_temp656] = c_real;
	uint32_t _temp658 = 2;
	tmp_imag[_temp658] = c_imag;
	uint32_t _temp660 = 34;
	tmp_real[_temp660] = c1_real;
	uint32_t _temp662 = 34;
	tmp_imag[_temp662] = c1_imag;
	uint32_t _temp664 = 3;
	tmp_real[_temp664] = d_real;
	uint32_t _temp666 = 3;
	tmp_imag[_temp666] = d_imag;
	uint32_t _temp668 = 35;
	tmp_real[_temp668] = d1_real;
	uint32_t _temp670 = 35;
	tmp_imag[_temp670] = d1_imag;
	uint32_t _temp672 = 4;
	tmp_real[_temp672] = e_real;
	uint32_t _temp674 = 4;
	tmp_imag[_temp674] = e_imag;
	uint32_t _temp676 = 36;
	tmp_real[_temp676] = e1_real;
	uint32_t _temp678 = 36;
	tmp_imag[_temp678] = e1_imag;
	uint32_t _temp680 = 5;
	tmp_real[_temp680] = f_real;
	uint32_t _temp682 = 5;
	tmp_imag[_temp682] = f_imag;
	uint32_t _temp684 = 37;
	tmp_real[_temp684] = f1_real;
	uint32_t _temp686 = 37;
	tmp_imag[_temp686] = f1_imag;
	uint32_t _temp688 = 6;
	tmp_real[_temp688] = g_real;
	uint32_t _temp690 = 6;
	tmp_imag[_temp690] = g_imag;
	uint32_t _temp692 = 38;
	tmp_real[_temp692] = g1_real;
	uint32_t _temp694 = 38;
	tmp_imag[_temp694] = g1_imag;
	uint32_t _temp696 = 7;
	tmp_real[_temp696] = h_real;
	uint32_t _temp698 = 7;
	tmp_imag[_temp698] = h_imag;
	uint32_t _temp700 = 39;
	tmp_real[_temp700] = h1_real;
	uint32_t _temp702 = 39;
	tmp_imag[_temp702] = h1_imag;
	uint32_t _temp704 = 8;
	tmp_real[_temp704] = i_real;
	uint32_t _temp706 = 8;
	tmp_imag[_temp706] = i_imag;
	uint32_t _temp708 = 40;
	tmp_real[_temp708] = i1_real;
	uint32_t _temp710 = 40;
	tmp_imag[_temp710] = i1_imag;
	uint32_t _temp712 = 9;
	tmp_real[_temp712] = j_real;
	uint32_t _temp714 = 9;
	tmp_imag[_temp714] = j_imag;
	uint32_t _temp716 = 41;
	tmp_real[_temp716] = j1_real;
	uint32_t _temp718 = 41;
	tmp_imag[_temp718] = j1_imag;
	uint32_t _temp720 = 10;
	tmp_real[_temp720] = k_real;
	uint32_t _temp722 = 10;
	tmp_imag[_temp722] = k_imag;
	uint32_t _temp724 = 42;
	tmp_real[_temp724] = k1_real;
	uint32_t _temp726 = 42;
	tmp_imag[_temp726] = k1_imag;
	uint32_t _temp728 = 11;
	tmp_real[_temp728] = l_real;
	uint32_t _temp730 = 11;
	tmp_imag[_temp730] = l_imag;
	uint32_t _temp732 = 43;
	tmp_real[_temp732] = l1_real;
	uint32_t _temp734 = 43;
	tmp_imag[_temp734] = l1_imag;
	uint32_t _temp736 = 12;
	tmp_real[_temp736] = m_real;
	uint32_t _temp738 = 12;
	tmp_imag[_temp738] = m_imag;
	uint32_t _temp740 = 44;
	tmp_real[_temp740] = m1_real;
	uint32_t _temp742 = 44;
	tmp_imag[_temp742] = m1_imag;
	uint32_t _temp744 = 13;
	tmp_real[_temp744] = n_real;
	uint32_t _temp746 = 13;
	tmp_imag[_temp746] = n_imag;
	uint32_t _temp748 = 45;
	tmp_real[_temp748] = n1_real;
	uint32_t _temp750 = 45;
	tmp_imag[_temp750] = n1_imag;
	uint32_t _temp752 = 14;
	tmp_real[_temp752] = o_real;
	uint32_t _temp754 = 14;
	tmp_imag[_temp754] = o_imag;
	uint32_t _temp756 = 46;
	tmp_real[_temp756] = o1_real;
	uint32_t _temp758 = 46;
	tmp_imag[_temp758] = o1_imag;
	uint32_t _temp760 = 15;
	tmp_real[_temp760] = p_real;
	uint32_t _temp762 = 15;
	tmp_imag[_temp762] = p_imag;
	uint32_t _temp764 = 47;
	tmp_real[_temp764] = p1_real;
	uint32_t _temp766 = 47;
	tmp_imag[_temp766] = p1_imag;
	uint32_t _temp768 = 16;
	tmp_real[_temp768] = q_real;
	uint32_t _temp770 = 16;
	tmp_imag[_temp770] = q_imag;
	uint32_t _temp772 = 48;
	tmp_real[_temp772] = q1_real;
	uint32_t _temp774 = 48;
	tmp_imag[_temp774] = q1_imag;
	uint32_t _temp776 = 17;
	tmp_real[_temp776] = r_real;
	uint32_t _temp778 = 17;
	tmp_imag[_temp778] = r_imag;
	uint32_t _temp780 = 49;
	tmp_real[_temp780] = r1_real;
	uint32_t _temp782 = 49;
	tmp_imag[_temp782] = r1_imag;
	uint32_t _temp784 = 18;
	tmp_real[_temp784] = s_real;
	uint32_t _temp786 = 18;
	tmp_imag[_temp786] = s_imag;
	uint32_t _temp788 = 50;
	tmp_real[_temp788] = s1_real;
	uint32_t _temp790 = 50;
	tmp_imag[_temp790] = s1_imag;
	uint32_t _temp792 = 19;
	tmp_real[_temp792] = t_real;
	uint32_t _temp794 = 19;
	tmp_imag[_temp794] = t_imag;
	uint32_t _temp796 = 51;
	tmp_real[_temp796] = t1_real;
	uint32_t _temp798 = 51;
	tmp_imag[_temp798] = t1_imag;
	uint32_t _temp800 = 20;
	tmp_real[_temp800] = u_real;
	uint32_t _temp802 = 20;
	tmp_imag[_temp802] = u_imag;
	uint32_t _temp804 = 52;
	tmp_real[_temp804] = u1_real;
	uint32_t _temp806 = 52;
	tmp_imag[_temp806] = u1_imag;
	uint32_t _temp808 = 21;
	tmp_real[_temp808] = v_real;
	uint32_t _temp810 = 21;
	tmp_imag[_temp810] = v_imag;
	uint32_t _temp812 = 53;
	tmp_real[_temp812] = v1_real;
	uint32_t _temp814 = 53;
	tmp_imag[_temp814] = v1_imag;
	uint32_t _temp816 = 22;
	tmp_real[_temp816] = w_real;
	uint32_t _temp818 = 22;
	tmp_imag[_temp818] = w_imag;
	uint32_t _temp820 = 54;
	tmp_real[_temp820] = w1_real;
	uint32_t _temp822 = 54;
	tmp_imag[_temp822] = w1_imag;
	uint32_t _temp824 = 23;
	tmp_real[_temp824] = x_real;
	uint32_t _temp826 = 23;
	tmp_imag[_temp826] = x_imag;
	uint32_t _temp828 = 55;
	tmp_real[_temp828] = x1_real;
	uint32_t _temp830 = 55;
	tmp_imag[_temp830] = x1_imag;
	uint32_t _temp832 = 24;
	tmp_real[_temp832] = y_real;
	uint32_t _temp834 = 24;
	tmp_imag[_temp834] = y_imag;
	uint32_t _temp836 = 56;
	tmp_real[_temp836] = y1_real;
	uint32_t _temp838 = 56;
	tmp_imag[_temp838] = y1_imag;
	uint32_t _temp840 = 25;
	tmp_real[_temp840] = z_real;
	uint32_t _temp842 = 25;
	tmp_imag[_temp842] = z_imag;
	uint32_t _temp844 = 57;
	tmp_real[_temp844] = z1_real;
	uint32_t _temp846 = 57;
	tmp_imag[_temp846] = z1_imag;
	uint32_t _temp848 = 26;
	tmp_real[_temp848] = aa_real;
	uint32_t _temp850 = 26;
	tmp_imag[_temp850] = aa_imag;
	uint32_t _temp852 = 58;
	tmp_real[_temp852] = aa1_real;
	uint32_t _temp854 = 58;
	tmp_imag[_temp854] = aa1_imag;
	uint32_t _temp856 = 27;
	tmp_real[_temp856] = bb_real;
	uint32_t _temp858 = 27;
	tmp_imag[_temp858] = bb_imag;
	uint32_t _temp860 = 59;
	tmp_real[_temp860] = bb1_real;
	uint32_t _temp862 = 59;
	tmp_imag[_temp862] = bb1_imag;
	uint32_t _temp864 = 28;
	tmp_real[_temp864] = cc_real;
	uint32_t _temp866 = 28;
	tmp_imag[_temp866] = cc_imag;
	uint32_t _temp868 = 60;
	tmp_real[_temp868] = cc1_real;
	uint32_t _temp870 = 60;
	tmp_imag[_temp870] = cc1_imag;
	uint32_t _temp872 = 29;
	tmp_real[_temp872] = dd_real;
	uint32_t _temp874 = 29;
	tmp_imag[_temp874] = dd_imag;
	uint32_t _temp876 = 61;
	tmp_real[_temp876] = dd1_real;
	uint32_t _temp878 = 61;
	tmp_imag[_temp878] = dd1_imag;
	uint32_t _temp880 = 30;
	tmp_real[_temp880] = ee_real;
	uint32_t _temp882 = 30;
	tmp_imag[_temp882] = ee_imag;
	uint32_t _temp884 = 62;
	tmp_real[_temp884] = ee1_real;
	uint32_t _temp886 = 62;
	tmp_imag[_temp886] = ee1_imag;
	uint32_t _temp888 = 31;
	tmp_real[_temp888] = ff_real;
	uint32_t _temp890 = 31;
	tmp_imag[_temp890] = ff_imag;
	uint32_t _temp892 = 63;
	tmp_real[_temp892] = ff1_real;
	uint32_t _temp894 = 63;
	tmp_imag[_temp894] = ff1_imag;
	memcpy(data_out_real, tmp_real, sizeof(tmp_real));
	memcpy(data_out_imag, tmp_imag, sizeof(tmp_imag));

	fft_64_stage_6_output output;
	memcpy(output.data_out_real, data_out_real, sizeof(data_out_real));
	memcpy(output.data_out_imag, data_out_imag, sizeof(data_out_imag));
	return output;
}


#ifndef COHORT_FIFO_PARAM_H
#define COHORT_FIFO_PARAM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "dcpn.h"
#ifndef BARE_METAL
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef PRI
#define PRINTBT printf("%s\n", __func__);
//#define PRI_DEBUG
#else
#define PRINTBT 
#endif

typedef uint64_t addr_t; // though we only use the lower 32 bits
typedef uint32_t len_t; // length of fifo
typedef len_t ptr_t;
typedef uint32_t el_size_t; // element size width

struct _fifo_ctrl_t;

typedef struct _fifo_ctrl_t fifo_ctrl_t;

/*
 * generic type for fifo_push functions and pop functions
 * actual memory operation size depends on the configured fifo size
 * always wrap in 64 bits
 */
typedef void (*fifo_push_func_t)(uint64_t element, fifo_ctrl_t* fifo_ctrl);
typedef uint64_t (*fifo_pop_func_t)(fifo_ctrl_t* fifo_ctrl);


typedef struct __attribute__((__packed__)) {
    addr_t addr;
    el_size_t size;
    len_t len;
} meta_t;

struct _fifo_ctrl_t {
	ptr_t private_ptr;
    uint32_t fifo_length;
    uint32_t element_size;
    volatile ptr_t* head_ptr;
    volatile ptr_t* tail_ptr;
    volatile meta_t* meta_ptr;
    volatile void* data_array;
};


void fifo_push_64(uint64_t element, fifo_ctrl_t* fifo_ctrl, uint32_t idx);
uint64_t fifo_pop_64 (fifo_ctrl_t* fifo_ctrl, uint32_t idx);

uint16_t clog2(uint16_t el);

//TODO: 128 bits are not supported, see https://github.com/rust-lang/rust/issues/54341

void fifo_start(fifo_ctrl_t *fifo_ctrl, bool is_consumer, uint8_t c_id, uint8_t acc_index);

/**
 *@fifo_length: the length of the fifo, in bytes
 *@element_size: the size of each element in fifo, in bytes
 *@is_consumer: if it's true, then it is a consumer; otherwise it's a producer. It's used to calculate uncached_write offset. The software producer thread produces into 0-2, the other produces to 3-5
 */
fifo_ctrl_t *fifo_init(uint32_t fifo_length, uint16_t element_size, bool is_consumer, uint8_t c_id, uint8_t acc_index)
{
    PRINTBT
    // 128 is the cache line width of openpiton
    fifo_ctrl_t *fifo_ctrl = (fifo_ctrl_t *) malloc(sizeof(fifo_ctrl_t));

    fifo_ctrl->head_ptr =   memalign(128, 128);
    fifo_ctrl->meta_ptr =   memalign(128, 128);
    fifo_ctrl->tail_ptr =   memalign(128, 128);
    fifo_ctrl->data_array = memalign(128, sizeof(uint64_t) * fifo_length);

#ifdef PRI
    printf("fhead %lx\n", fifo_ctrl->head_ptr);
    printf("fmeta %lx\n", fifo_ctrl->meta_ptr);
    printf("ftail %lx\n", fifo_ctrl->tail_ptr);
    printf("fdata %lx\n", fifo_ctrl->data_array);
#endif

    fifo_ctrl->fifo_length = fifo_length;
    fifo_ctrl->element_size = (element_size / 8);
	fifo_ctrl->private_ptr = 0;

    
    fifo_start(fifo_ctrl, is_consumer, c_id, acc_index);

    //TODO: use generic push/pop here
    return fifo_ctrl;
}

void fifo_start(fifo_ctrl_t *fifo_ctrl, bool is_consumer, uint8_t c_id, uint8_t acc_index)
{
    PRINTBT
    *(fifo_ctrl->tail_ptr) = 0x00000000ULL;
    *(fifo_ctrl->head_ptr) = 0x00000000ULL;
    fifo_ctrl->meta_ptr->addr = (uint64_t) fifo_ctrl->data_array;
    fifo_ctrl->meta_ptr->len = fifo_ctrl->fifo_length;
    fifo_ctrl->meta_ptr->size = fifo_ctrl->element_size;
	for (int i = 0; i < fifo_ctrl->fifo_length; i++) {
		*(((volatile uint64_t *) fifo_ctrl->data_array) + i) = 0;
	}
    if (is_consumer) {
        cohort_ni_write(c_id, 3 + acc_index*8,( uint64_t )fifo_ctrl->head_ptr);
        cohort_ni_write(c_id, 4 + acc_index*8,( uint64_t )fifo_ctrl->meta_ptr);
        cohort_ni_write(c_id, 5 + acc_index*8,( uint64_t )fifo_ctrl->tail_ptr);
    } else {
        cohort_ni_write(c_id, 0 + acc_index*8,( uint64_t ) fifo_ctrl->tail_ptr);
        cohort_ni_write(c_id, 1 + acc_index*8,( uint64_t ) fifo_ctrl->meta_ptr);
        cohort_ni_write(c_id, 2 + acc_index*8,( uint64_t ) fifo_ctrl->head_ptr);
    }
}



ptr_t private_get_tail(fifo_ctrl_t *fifo_ctrl)
{
    return  *((ptr_t *)(fifo_ctrl->tail_ptr));
}

ptr_t private_get_head(fifo_ctrl_t *fifo_ctrl)
{
    return *((ptr_t *)(fifo_ctrl->head_ptr));
}

/**
 *@return: 0 if not empty, 1 if empty
 **/
int fifo_is_empty(fifo_ctrl_t* fifo_ctrl, int pos)
{
#ifdef PRI
   printf("e pos %lx\n", pos);
   printf("e tail %lx\n", private_get_tail(fifo_ctrl));
#endif
   return private_get_tail(fifo_ctrl) == pos;
}

int fifo_is_full(fifo_ctrl_t* fifo_ctrl, int pos)
{
	//usleep(10000);
#ifdef PRI
    printf("f pos is %lx\n", pos);
    printf("f head ptr is %lx\n", private_get_head(fifo_ctrl));
#endif
	return ((pos + 1) % fifo_ctrl->fifo_length) == private_get_head(fifo_ctrl);
}

addr_t fifo_get_base(fifo_ctrl_t fifo_ctrl)
{
    PRINTBT
    return (uint64_t) fifo_ctrl.data_array;
}

void fifo_deinit(fifo_ctrl_t *fifo_ctrl)
{
    PRINTBT
    // first free the large data array
    free((void*)fifo_ctrl->data_array);

    // then free 3 cachelines
    free((void*)fifo_ctrl->head_ptr);
    free((void*)fifo_ctrl->tail_ptr);
    free((void*)fifo_ctrl->meta_ptr);

    // at long last free the fifo pointer
    free(fifo_ctrl);
}


// philosophy: don't chunk requests in software for better transparency
// as 128 bits aren't supported, 64 would suffice
//
//
void fifo_push_64(uint64_t element, fifo_ctrl_t* fifo_ctrl, uint32_t pos)
{
    // loop whilie the fifo is full
	while (fifo_is_full(fifo_ctrl, pos));
    *((volatile uint64_t *)((volatile uint64_t *) fifo_ctrl->data_array) + (pos)) = element;
}

uint64_t fifo_pop_64(fifo_ctrl_t *fifo_ctrl, uint32_t pos)
{
	while (fifo_is_empty(fifo_ctrl, pos));
	//wrong predicate
    uint64_t element = *((volatile uint64_t *)(((volatile uint64_t *) fifo_ctrl->data_array) + pos ));
    return element;
}

void fifo_dump(fifo_ctrl_t *fifo_ctrl)
{    
	printf("fifo length: %d\n", fifo_ctrl->fifo_length);
//	for (int pos = 0; pos < fifo_ctrl->fifo_length; pos++) {
//	uint64_t element = *((volatile uint64_t *)(((volatile uint64_t *) fifo_ctrl->data_array) + pos * 2 ));
//    printf("dump %d, %llx\n", pos, element);
//	}

}


void fifo_push_sync(volatile fifo_ctrl_t* fifo_ctrl, uint32_t pos)
{
    __sync_synchronize();
	*(fifo_ctrl->tail_ptr) = pos;
    __sync_synchronize();
//	printf("%s, fifo head ptr: %lx\n",__func__, fifo_ctrl->head_ptr);
//	printf("%s, fifo tail ptr: %lx\n",__func__, fifo_ctrl->tail_ptr);
}

void fifo_pop_sync(volatile fifo_ctrl_t* fifo_ctrl, uint32_t pos)
{
    __sync_synchronize();
	*(fifo_ctrl->head_ptr) = pos;
    __sync_synchronize();
//	printf("%s, fifo head ptr: %lx\n",__func__, fifo_ctrl->head_ptr);
//	printf("%s, fifo tail ptr: %lx\n",__func__, fifo_ctrl->tail_ptr);
}

// note that we cannot operate at sub-byte level in c
// this is not how c works
// note also that el is bits
// we need a byte clog2

uint16_t clog2(uint16_t bitswidth)
{
    uint16_t el = bitswidth / 8;
    switch (el) {
        case 8:
            return 3;
        case 4:
            return 2;
        case 2:
            return 1;
        case 1:
            return 0;
    }
}



#endif // COHORT_FIFO_PARAM_H

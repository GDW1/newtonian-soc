
void assert(int condition){
    if (condition==0){
        printf("ASSERT!!\n");
    }
}
#include "dec_decoupling.h"

#define BYTE         8
#define TILE_X       28
#define TILE_Y       24
#define WIDTH        2
#define FIFO         9
#define BASE_MAPLE 0xe100800000
#define BASE_SPD   0xe100900000
#define BASE_MMU   0xe100A00000
#define BASE_DREAM 0xe100B00000
#define BASE_NIBBL 0xe100C00000

#define MAX_TILES 16
#define INVALID_FIFO 65536

//#define DEC_DEBUG 1 // Do some runtime checks, but potentially slows
#define DEBUG_INIT assert(initialized);
#define DEBUG_NOT_INIT assert(!initialized);

#ifndef BARE_METAL  
  #include "dcp_alloc.h"
  #ifdef SWDAE
    #include "dcps.h"
  #endif
#endif

//static variables
static uint32_t num_tiles;
static volatile uint32_t initialized = 0;
static uint32_t queues_per_tile;
static uint64_t fpid[MAX_QUEUES];
static uint64_t fcid[MAX_QUEUES];
static uint64_t base[MAX_TILES];
static uint64_t dream_base[MAX_TILES];
static uint64_t flush_base[MAX_TILES];
static uint64_t mmub[MAX_TILES];
volatile static int result = 0;

#define DCP_SIZE_8  0
#define DCP_SIZE_16 1
#define DCP_SIZE_32 2
#define DCP_SIZE_48 3
#define DCP_SIZE_64 4
#define DCP_SIZE_80 5
#define DCP_SIZE_96 6
#define DCP_SIZE_128 7

///LIMA
#define LOOP_TLOAD32 48 // Set Tload 32 bits
#define LOOP_TLOAD64 49 // Set Tload 64 bits
#define LOOP_PRODUCE 50 // Set Produce
#define LOOP_LLC_32  52 // Set LLC load 32 bits
#define LOOP_LLC_64  53 // Set LLC load 64 bits
#define LOOP_PREFETCH 54 // Set Prefetch
#define LOOP_ACC 55 // DMA to ACC

static uint64_t push_loop = (uint64_t)(14*BYTE);

//CONSUME/PRODUCE
static uint64_t cons_addr = (uint64_t)(7*BYTE);
static uint64_t prod_addr = (uint64_t)(8*BYTE);
//TLOAD
static uint64_t tload_addr32 = (uint64_t)(10*BYTE);
static uint64_t tload_addr64 = (uint64_t)(11*BYTE);
//ATOMICs
static uint64_t add_addr  = (uint64_t)(32*BYTE);
static uint64_t and_addr  = (uint64_t)(33*BYTE);
static uint64_t or_addr   = (uint64_t)(34*BYTE);
static uint64_t xor_addr  = (uint64_t)(35*BYTE);
static uint64_t max_addr  = (uint64_t)(36*BYTE);
static uint64_t maxu_addr = (uint64_t)(37*BYTE);
static uint64_t min_addr  = (uint64_t)(38*BYTE);
static uint64_t minu_addr = (uint64_t)(39*BYTE);
static uint64_t swap_addr = (uint64_t)(40*BYTE);
static uint64_t cas1_addr = (uint64_t)(41*BYTE);
static uint64_t cas2_addr = (uint64_t)(42*BYTE);
static uint64_t prefetch_addr = (uint64_t)(43*BYTE);
static uint64_t llcload_addr32 = (uint64_t)(44*BYTE);
static uint64_t llcload_addr64 = (uint64_t)(45*BYTE);

//CONFIG
static uint64_t destroy_tile_addr  = (uint64_t)(1*BYTE);
//CONFIG B base
static uint64_t base_addr32 = (uint64_t)(57*BYTE);
static uint64_t base_addr64 = (uint64_t)(58*BYTE);

//UNUSED
static uint64_t fifoadd_addr  = (uint64_t)(6*BYTE);
static uint64_t fifoclr_addr  = (uint64_t)(9*BYTE);
static uint64_t stats_addr    = (uint64_t)(12*BYTE);
static uint64_t debug_addr    = (uint64_t)(13*BYTE);

//CONF TLB (only on the MMU page)
static uint64_t tlb_flush            = (uint64_t)(13*BYTE);
static uint64_t get_tlb_fault        = (uint64_t)(13*BYTE | 1 << FIFO);
static uint64_t tlb_snoop_entry      = (uint64_t)(13*BYTE | 2 << FIFO);
static uint64_t disable_tlb          = (uint64_t)(15*BYTE);
static uint64_t conf_tlb_ptbase      = (uint64_t)(15*BYTE | 1 << FIFO);
static uint64_t resolve_tlb_fault    = (uint64_t)(15*BYTE | 2 << FIFO);
static uint64_t conf_tlb_mmpage      = (uint64_t)(15*BYTE | 6 << FIFO);

//CONNECT
static uint64_t producer_conf_addr  = (uint64_t)(2*BYTE);
static uint64_t consumer_conf_addr  = (uint64_t)(3*BYTE);
static uint64_t producer_dconf_addr = (uint64_t)(4*BYTE);
static uint64_t consumer_dconf_addr = (uint64_t)(5*BYTE);

void printDebug(uint64_t s);
uint64_t dec_fifo_debug(uint64_t tile, uint32_t id);
//////////////////////////////////////
//// OPEN/CLOSE PRODUCER/CONSUMER////
/////////////////////////////////////

uint32_t dec_open_producer(uint64_t qid) {
#ifdef PRI
printf("ENTER_PROD\n");
#endif
while (!initialized);
#if defined(DEC_DEBUG)
  assert(fpid[qid] == INVALID_FIFO);
#endif
  #ifdef RES
  ATOMIC_OP(result, 1, add, w);
  #endif
  uint32_t tile = qid/queues_per_tile;
  uint64_t fifo = base[tile] | ( (qid%queues_per_tile) << FIFO);
  uint32_t res_producer_conf; 
  // connect can return 0 if queue does not exist or if it is already taken 
  do {res_producer_conf = *(volatile uint64_t*)(producer_conf_addr | fifo);
    #ifdef PRI
    printf("OPROD:%d\n",res_producer_conf);
    #endif
    } while (res_producer_conf == 0);
  fpid[qid] = fifo;
  return 1;
}

uint32_t dec_open_consumer(uint64_t qid) {
while (!initialized);
//while (fpid[qid] == INVALID_FIFO);
  //here we could check for example if the fcid is taken, SW checks!
#if defined(DEC_DEBUG)
  assert(fcid[qid] == INVALID_FIFO);
#endif
  #ifdef RES
  ATOMIC_OP(result, 1, add, w);
  #endif
  uint32_t tile = qid/queues_per_tile;
  uint64_t fifo = base[tile] | ( (qid%queues_per_tile) << FIFO);
  uint32_t res_consumer_conf; 
  // connect can return 0 if queue does not exist or if it is already taken 
  do {res_consumer_conf = *(volatile uint64_t*)(consumer_conf_addr | fifo); 
    #ifdef PRI
    printf("OCONS:%d\n",res_consumer_conf);
    #endif
    } while (res_consumer_conf == 0);
  fcid[qid] = fifo;
  return 1;
}

uint32_t dec_close_producer(uint64_t qid) {
  uint64_t fifo = fpid[qid];
#if defined(DEC_DEBUG)
  assert(fifo !=INVALID_FIFO);
  //fpid[qid] = INVALID_FIFO;
#endif
  // close can return 0 if the queue does not exist or it is not configured by the core
  volatile uint32_t res_producer_conf = *(volatile uint64_t*)(producer_dconf_addr | fifo);
  #ifdef PRI
  printf("CPROD:%d\n",res_producer_conf);
  #endif
  #ifdef RES
  ATOMIC_OP(result, -1, add, w);
  #endif
  return res_producer_conf;
}

uint32_t dec_close_consumer(uint64_t qid) {
  uint64_t fifo = fcid[qid];
#if defined(DEC_DEBUG)
  assert(fifo !=INVALID_FIFO);
  //fcid[qid] = INVALID_FIFO;
#endif
  volatile uint32_t res_consumer_conf = *(volatile uint64_t*)(consumer_dconf_addr | fifo);
  #ifdef PRI
  printf("CCONS:%d\n",res_consumer_conf);
  #endif
  #ifdef RES
  ATOMIC_OP(result, -1, add, w);
  #endif
  return res_consumer_conf;
}

// TLB management
// SNOOP TLB ENTRIES
uint64_t dec_snoop_tlb_entry(uint64_t tile) {
  uint64_t res = *(volatile uint64_t *)(tlb_snoop_entry | mmub[tile]);
  return res;
}
// GET PAGE FAULTS
uint64_t dec_get_tlb_fault(uint64_t tile) {
  uint64_t res = *(volatile uint64_t *)(get_tlb_fault | mmub[tile]);
  return res;
}
// FLUSH TLB
uint64_t dec_flush_tlb (uint64_t tile) {
   uint64_t res = *(volatile uint64_t*)(tlb_flush | mmub[tile]);
   return res;
}
// CONFIG THE PAGE TABLE BASE OF THE TLB
void dec_set_tlb_ptbase(uint64_t tile, uint64_t conf_tlb_addr) {
// conf_tlb_addr is 28 bits, so set to bits [27:0]
// The other 36 bits of the data are as follow
//     35 disable_int
//     34 reserved
//     33:32 chipid[7],chipid[0]
//     31:28 fbits
//     27:20 ypos
//     19:12 xpos
//     11:10  type
//     9     threadid
//     8     0:level, 1:edge
//     7     0:rising, 1:falling
//     6:0   source id
  dec_flush_tlb(tile);
  *(volatile uint64_t*)(conf_tlb_ptbase | mmub[tile]) = (conf_tlb_addr & 0x0FFFFFFFULL) | 0x0003001020000000;
  #ifdef PRI
  printf("Config MAPLE ptbase %p\n", (uint64_t*)conf_tlb_addr);
  #endif
}
// SET TLB ENTRY THRU DCP and RESOLVE PAGE FAULT if lower bits are set
void dec_set_tlb_mmpage(uint64_t tile, uint64_t conf_tlb_entry) {
  *(volatile uint64_t*)(conf_tlb_mmpage | mmub[tile]) = conf_tlb_entry; 
}
// RESOLVE PAGE FAULT, but not load entry into TLB, let PTW do it
void dec_resolve_page_fault(uint64_t tile, uint64_t conf_tlb_entry) {
  *(volatile uint64_t*)(resolve_tlb_fault | mmub[tile]) = conf_tlb_entry; 
}
// DISABLE TLB TRANSLATION
void dec_disable_tlb(uint64_t tile) {
  *(volatile uint64_t*)(disable_tlb | mmub[tile]) = 0; 
}

///////////////////
/// INIT TILE////
//////////////////

uint32_t config_loop_unit(int config_tiles, void * A, void * B, uint32_t op) {
  uint64_t cr_conf_A = op*BYTE;
  // The last bit of cr_conf_A indicates whether the B[i] load is coherent or not (1: coherent)
  // Whether A[B[i]] is loaded coherently, depends on the 'op'
  // LOOP_TLOAD32 48 (non coherent), vs  LOOP_LLC_32  52 (coherent)
  #ifndef COHERENT
    #define COHERENT 1
  #endif
  #if COHERENT==1
  cr_conf_A |= 1 << FIFO;
  #endif

  if (A!=0 || B!=0){
    for (int j=0; j<config_tiles; j++){
      *(volatile uint64_t*)(cr_conf_A | base[j]) = (uint64_t)A;
      if (B!=0) *(volatile uint64_t*)(base_addr32 | base[j]) = (uint64_t)B;
    }
  }
}
  
uint32_t dec_fifo_init_conf(uint32_t count, uint32_t size, void * A, void * B, uint32_t op) {
#if defined(DEC_DEBUG)
  // hardware should also ignore init messages once to a tile which is already initialized
  DEBUG_NOT_INIT;
  for(uint32_t i=0; i<MAX_QUEUES;i++){
    fcid[i] = INVALID_FIFO;
    fpid[i] = INVALID_FIFO;
  }
  assert (count); //check that count is bigger than 0
#endif
	
	//FIXME: can queues per tile always be 1 for MAPLE?
  switch(size)
    { /* 32x4, 64x2, 128x1 are the ones that make the most sense*/
        case DCP_SIZE_8:
        case DCP_SIZE_16:
        case DCP_SIZE_32: 
            queues_per_tile = 1;
            break;
        
        case DCP_SIZE_48: 
        case DCP_SIZE_64: 
            queues_per_tile = 1;
            break;
        /*
        case DCP_SIZE_80: 
        case DCP_SIZE_96:
        case DCP_SIZE_128: 
            queues_per_tile = 1;
            queues_per_tile = 1;
            break;
        */
        default:
            queues_per_tile = 1;
    }

  uint32_t partial_tile = (count%queues_per_tile)>0;
  uint32_t entire_tiles = count/queues_per_tile;
  uint32_t num_tiles = entire_tiles + partial_tile;

  uint32_t allocated_tiles;
  uint64_t conf_tlb_addr;

  uint32_t tileno;
  // SET THE LAYOUT OF TILES TO TARGET
  for (int i = 0; i < num_tiles; i++){
    tileno = i + 2;
#ifdef NO_DRIVER
    base[i] = BASE_MAPLE | ((tileno%WIDTH) << TILE_X) | ((tileno/WIDTH) << TILE_Y);
    mmub[i] = BASE_MMU   | ((tileno%WIDTH) << TILE_X) | ((tileno/WIDTH) << TILE_Y); 
    dream_base[i] = BASE_DREAM  | ((tileno%WIDTH) << TILE_X) | ((tileno / WIDTH) << TILE_Y); 
#endif

#ifdef DRIVER_EVALS
    base[i] = BASE_MAPLE | ((tileno%WIDTH) << TILE_X) | ((tileno/WIDTH) << TILE_Y);
#endif
    
  }
  allocated_tiles = num_tiles;
#ifndef BARE_METAL
  // ALLOCATE AND RESET TILE
#ifdef NO_DRIVER
  allocated_tiles = (uint64_t)alloc_tile(num_tiles,base);
  allocated_tiles = (uint64_t)alloc_tile(num_tiles,mmub);
  allocated_tiles = (uint64_t)alloc_tile(num_tiles,dream_base);
#endif

#ifdef DRIVER_EVALS
  allocated_tiles = (uint64_t)alloc_tile(num_tiles,base);
#endif

  if (!allocated_tiles) return 0;
  // IF VIRTUAL MEMORY, THEN GET THE PAGE TABLE BASE
#ifdef NO_DRIVER  
  conf_tlb_addr = syscall(258) >> 12;
  if (conf_tlb_addr == -1 ) {
      exit(-1);
  }
#endif

#endif // for nBARE_METAL

  //printDebug(dec_fifo_debug(0,2));
  dec_fifo_cleanup(allocated_tiles);
  __sync_synchronize; //Compiler should not reorder the next load
  //printDebug(dec_fifo_debug(0,2));
  uint32_t i = 0;
  uint32_t res_producer_conf;
  do { // Do the best allocation based on the number of tiles!
    // INIT_TILE: Target to allocate "len" queues per Tile
    uint64_t addr_maple = (base[i] | (size << FIFO));
    res_producer_conf = *(volatile uint32_t*)addr_maple;
    #ifdef PRI
    printf("Target MAPLE %d: address %p\n", i,(uint32_t*)addr_maple);
    printf("Target MAPLE %d: res %d, now config TLB\n", i, res_producer_conf);
    #endif
#ifdef BARE_METAL
    dec_disable_tlb(i);
#else
  #ifdef NO_DRIVER
    dec_set_tlb_ptbase(i,conf_tlb_addr);
  #endif
#endif
    i++;
  } while (i<allocated_tiles && res_producer_conf > 0);
  // count configured tiles
  uint32_t config_tiles = i;
  if (!res_producer_conf) config_tiles--;

  config_loop_unit(config_tiles, A,B,op);
  //printDebug(dec_fifo_debug(0,2));

  initialized = 1;
  uint32_t res = config_tiles*queues_per_tile;
  #ifdef PRI
  printf("INIT: res 0x%08x\n", ((uint32_t)res) & 0xFFFFFFFF);
  #endif
  return res;
}

uint32_t dec_fifo_init(uint32_t count, uint32_t size) {
#ifdef SWDAE
    dec2_fifo_init(1,DCP_SIZE_64);
#endif
    return dec_fifo_init_conf(count, size, 0, 0, 0);
}

//CLEANUP
uint32_t dec_fifo_cleanup(uint32_t tile) {
#if defined(DEC_DEBUG)
  //DEBUG_INIT;
#endif
  for (uint32_t i=0; i<tile;i++){
    volatile uint32_t res_reset = *(volatile uint32_t*)(destroy_tile_addr | base[i]);
    #ifdef PRI
    printf("RESET:%d\n",res_reset);
    #endif
  }
  return 1; //can this fail? security issues?
}

void dec_loop(uint64_t qid, uint32_t begin, uint32_t end) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  if (begin<end){
    uint64_t beg = (uint64_t)(begin) << 32;
    *(volatile uint64_t *)(push_loop | fpid[qid] ) = beg | (uint64_t) end;
  }
}
//STATS
uint64_t dec_fifo_stats(uint64_t qid, uint32_t stat, uint32_t* result) {
  //TODO adapt to fetch only stat (key)
  volatile uint64_t res_stat = *(volatile uint64_t*)(stats_addr | fpid[qid]);
  return res_stat;
}
//DEBUG
uint64_t dec_fifo_debug(uint64_t tile, uint32_t id) {
  volatile uint64_t res_debug = *(volatile uint64_t*)(debug_addr | base[tile] | id << FIFO);
  return res_debug;
}

// Produce/Consume
void dec_produce32(uint64_t qid, uint32_t data) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint32_t *)(prod_addr | fpid[qid] ) = data;
}

void dec_produce64(uint64_t qid, uint64_t data) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint64_t *)(prod_addr | fpid[qid])  = data;
}

uint32_t dec_consume32(uint64_t qid) {
#if defined(DEC_DEBUG)
  assert(fcid[qid] !=INVALID_FIFO);
#endif
#ifdef SWDAE
    uint32_t res = dec2_consume32(qid);
#else
  uint32_t res = *(volatile uint32_t *)(cons_addr | fcid[qid] );
#endif
  return res;
}

uint64_t dec_consume64(uint64_t qid) {
#if defined(DEC_DEBUG)
  assert(fcid[qid] !=INVALID_FIFO);
#endif
#ifdef SWDAE
    uint64_t res = dec2_consume64(qid);
#else
  uint64_t res = *(volatile uint64_t *)(cons_addr | fcid[qid] );
#endif
  return res;
}

void dec_set_base32(uint64_t qid, void *addr) {
    #ifdef PRI
    printf("BASE32:%p\n",addr);
    #endif
  *(volatile uint64_t*)(base_addr32 | fpid[qid]) = (uint64_t)addr;
}
void dec_set_base64(uint64_t qid, void *addr) {
    #ifdef PRI
    printf("BASE64:%p\n",addr);
    #endif
  *(volatile uint64_t*)(base_addr64 | fpid[qid]) = (uint64_t)addr;
}

// Async Loads 32 bits //
void dec_load32_asynci(uint64_t qid, uint64_t idx) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
#ifdef SWDAE
  dec2_produce32(qid,*(uint32_t *)idx);
#else
  *(volatile uint64_t *)(tload_addr32 | fpid[qid] ) = idx;
#endif
} void dec_load32_async(uint64_t qid, uint32_t *addr) {dec_load32_asynci(qid, (uint64_t)addr);}
// LLC Loads 32 bits //
void dec_load32_asynci_llc(uint64_t qid, uint64_t idx) {
  #if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
  #endif
  *(volatile uint64_t *)(llcload_addr32 | fpid[qid] ) = idx;
}void dec_load32_async_llc(uint64_t qid, uint32_t *addr) {dec_load32_asynci_llc(qid, (uint64_t)addr);}

// Async Loads 64 bits //
void dec_load64_asynci(uint64_t qid, uint64_t idx) {
  #if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
 //printf("Trans qid %d: into fifo%p\n", qid,(uint32_t*)fpid[qid]);
  #endif
  #ifdef SWDAE
    dec2_produce64(qid,*(uint64_t *)idx);
  #else
    *(volatile uint64_t *)(tload_addr64 | fpid[qid] ) = idx;
  #endif
}void dec_load64_async(uint64_t qid, uint64_t *addr) {dec_load64_asynci(qid, (uint64_t)addr);}

// LLC Loads 64 bits //
void dec_load64_asynci_llc(uint64_t qid, uint64_t idx) {
  #if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
  #endif
  *(volatile uint64_t *)(llcload_addr64 | fpid[qid] ) = idx;
}void dec_load64_async_llc(uint64_t qid, uint64_t *addr) {dec_load64_asynci_llc(qid, (uint64_t)addr);}

// Prefetch //
void dec_prefetchi(uint64_t qid, uint64_t idx) {
  #if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
  #endif
  *(volatile uint64_t *)(prefetch_addr | fpid[qid] ) = idx;
}void dec_prefetch(uint64_t qid, uint64_t *addr) {dec_prefetchi(qid, (uint64_t)addr);}

// Async RMWs
void dec_atomic_fetch_add_asynci(uint64_t qid, uint64_t idx, int val) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint64_t *)(add_addr | fpid[qid] ) = idx << 32 | ((uint64_t)val) & 0x0000FFFF;
} void dec_atomic_fetch_add_async(uint64_t qid, uint32_t *addr, int val) { dec_atomic_fetch_add_asynci(qid, ((uint64_t)addr),val);}

void dec_atomic_fetch_and_asynci(uint64_t qid, uint64_t idx, int val) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint64_t *)(and_addr | fpid[qid] ) = idx << 32 | (uint64_t)val;
} void dec_atomic_fetch_and_async(uint64_t qid, uint32_t *addr, int val) { dec_atomic_fetch_and_asynci(qid, ((uint64_t)addr),val);}


void dec_atomic_fetch_or_asynci(uint64_t qid, uint64_t idx, int val) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint64_t *)(or_addr | fpid[qid] ) = idx << 32 | (uint64_t)val;
} void dec_atomic_fetch_or_async(uint64_t qid, uint32_t *addr, int val) { dec_atomic_fetch_or_asynci(qid, ((uint64_t)addr),val);}

void dec_atomic_fetch_xor_asynci(uint64_t qid, uint64_t idx, int val) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint64_t *)(xor_addr | fpid[qid] ) = idx << 32 | (uint64_t)val;
} void dec_atomic_fetch_xor_async(uint64_t qid, uint32_t *addr, int val) { dec_atomic_fetch_xor_asynci(qid, ((uint64_t)addr),val);}

void dec_atomic_fetch_max_asynci(uint64_t qid, uint64_t idx, int val) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint64_t *)(max_addr | fpid[qid] ) = idx << 32 | (uint64_t)val;
} void dec_atomic_fetch_max_async(uint64_t qid, uint32_t *addr, int val) { dec_atomic_fetch_max_asynci(qid, ((uint64_t)addr),val);}

void dec_atomic_fetch_min_asynci(uint64_t qid, uint64_t idx, int val) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint64_t *)(min_addr | fpid[qid] ) = idx << 32 | (uint64_t)val;
} void dec_atomic_fetch_min_async(uint64_t qid, uint32_t *addr, int val) { dec_atomic_fetch_min_asynci(qid, ((uint64_t)addr),val);}


void dec_atomic_fetch_exchange_asynci(uint64_t qid, uint64_t idx, int val) {
#if defined(DEC_DEBUG)
  assert(fpid[qid] !=INVALID_FIFO);
#endif
  *(volatile uint64_t *)(swap_addr | fpid[qid] ) = idx << 32 | (uint64_t)val;
} void dec_atomic_fetch_exchange_async(uint64_t qid, uint32_t *addr, int val) { dec_atomic_fetch_exchange_asynci(qid, ((uint64_t)addr),val);}


void dec_atomic_compare_exchange_asynci(uint64_t qid, uint64_t idx, int data1, int data2) {
  uint64_t fifo = fpid[qid];
#if defined(DEC_DEBUG)
  assert(fifo !=INVALID_FIFO);
#endif
  if (data1<-32768 || data1>32767 || data2 <-32768 || data2>32767){ //Two messages
      *(volatile uint64_t *)(cas1_addr | fifo ) = ((uint64_t) data2) << 32 | (uint32_t)data1;
  } 
  *(volatile uint64_t *)(cas2_addr | fifo ) = (idx << 32 | data2 << 16 | (uint16_t)data1);  
} void dec_atomic_compare_exchange_async(uint64_t qid, uint32_t *addr, int data1, int data2){
  dec_atomic_compare_exchange_asynci(qid, (uint64_t)addr, data1, data2);
}

/// END OF OPERATIONS ///


void init_clock(){
    uint32_t res = dec_fifo_init(1,DCP_SIZE_64); 
    #ifdef PRI
    printf("ISTILE created res:%d\n",res);
    #endif
    dec_open_producer(0);
}
void print_old(uint32_t id){
    for (uint32_t j=0; j<4; j++){
        uint64_t stat = dec_fifo_stats(id,0,0);
        uint32_t stat_l = (uint32_t) stat;// / NNZ;
        uint32_t stat_h = (stat >> 32);// / NNZ;
        printf("%d st:%d, ld:%d\n",j,(int)stat_h, (int)stat_l);
    }
}
void print_st(uint32_t id){
    uint64_t stat = dec_fifo_stats(id,0,0);
    stat = dec_fifo_stats(id,0,0);
    uint32_t stat_c = (uint32_t) stat;
    uint32_t stat_p = (stat >> 32);
    stat = dec_fifo_stats(id,0,0);
    stat_c += (uint32_t) stat;
    stat_p += (stat >> 32);
    if (stat_c > stat_p) stat = stat_c; else stat = stat_p;
    printf("%d, ",(int)stat);
    stat = dec_fifo_stats(id,0,0);
}

void print_stats(){
    dec_close_producer(0); 
    print_st(0); 
}

void print_stats_fifos(uint32_t num){
    for (uint32_t fifo_id=0; fifo_id<num; fifo_id++)
    {
        #ifdef PRI
        printf("Stats for FIFO %d:\n", fifo_id);
        #endif
        print_st(fifo_id);    
    }
}

void init_tile(uint32_t num){
    uint32_t size = DCP_SIZE_64;
    if (num == 2) 
        size=DCP_SIZE_128;
    else if (num > 2)
        size=DCP_SIZE_32;
    uint32_t res = dec_fifo_init(num,size);
}

void touch(uint32_t * p, uint32_t len){
    uint32_t res = 0;
    for (int i = 0; i < len; i+=1024){
        res+=p[i];
    }
    res+=p[len-1];
    printf("T%d\n",res);
    #ifdef PRI
    printf("R%p\n",p);
    #endif
}
void touch64(uint64_t * p, uint32_t len){
    uint64_t res = 0;
    for (int i = 0; i < len; i+=512){
        res+=p[i];
    }
    res+=p[len-1];
    printf("T%d\n",res);
    #ifdef PRI
    printf("R%p\n",p);
    #endif
}

void print64(char * str,uint64_t s) {
    printf("%s: data 0x%08x 0x%08x\n", str, ((uint64_t)s)>>32,((uint64_t)s) & 0xFFFFFFFF);
}
void print32(char * str,uint32_t s) {
    printf("%s: data 0x%08x\n",str,((uint32_t)s));
}
void printDebug(uint64_t s) {
  printf("decr_entry:%x\n",    (s & 0x00000000000000FF) );
  printf("incr_entry:%x\n",    (s & 0x000000000000FF00) >> 8);
  printf("l0_val_r:%x\n",      (s & 0x00000000000F0000) >> 16);
  printf("fifo_not_empty:%x\n",(s & 0x0000000000F00000) >> 20);
  printf("l0_len_r:%x\n",      (s & 0x000000000F000000) >> 24);
  printf("l1_val,sel,fif:%x\n",(s & 0x00000000F0000000) >> 28);
  printf("s0_val_r:%x\n",      (s & 0x0000000F00000000) >> 32);
  printf("fifo_full:%x\n",     (s & 0x000000F000000000) >> 36);
  printf("s0_len_r:%x\n",      (s & 0x00000F0000000000) >> 40);
  printf("s1_val,sel,fif:%x\n",(s & 0x0000F00000000000) >> 44);
  printf("s3_val,no_ack,s2_val,s1_val_r:%x\n"  ,(s & 0x000F000000000000) >> 48);
  printf("noc2_rdy,noc2_ld,l2_val,noc2_st:%x\n",(s & 0x00F0000000000000) >> 52);
  printf("tlb_req,src,map,cr:%x\n",(s & 0x0F00000000000000) >> 56 );
  printf("fifo_val,inv:%x\n"      ,(s & 0xF000000000000000) >> 60);
}
void _kernel_(uint32_t id, uint32_t core_num);
void start_doall(uint32_t id, uint32_t core_num){
    #ifdef STAT
    volatile static uint32_t amo_cnt = 0;
    ATOMIC_OP(amo_cnt, 1, add, w);
    while(core_num != amo_cnt);
    if (id == 0) init_clock();
    ATOMIC_OP(amo_cnt, 1, add, w);
    while(2*core_num != amo_cnt);
    #endif
    _kernel_(id, core_num);
    #ifdef STAT
    ATOMIC_OP(amo_cnt, 1, add, w);
    // barrier to make sure all threads finished their jobs
    while((3*core_num) != amo_cnt);
    if (id == 0) print_stats(); 
    ATOMIC_OP(amo_cnt, 1, add, w);
    // barrier at the end to guarantee the stats are printed
    while((4*core_num) != amo_cnt);
    #endif
}

void aes_write(uint32_t tile, uint64_t addr, uint64_t value) {
  uint64_t write_addr = addr | dream_base[tile];
  *(volatile uint64_t*)write_addr = value;
  #ifdef PRI
  printf("Target AES write addr: %p, write config data: %llx\n", write_addr, (uint64_t)value);
  #endif
}

uint64_t aes_read(uint32_t tile, uint64_t addr) {
  uint64_t read_addr = addr | dream_base[tile];
  uint64_t read_value = *(volatile uint64_t*)read_addr;
  #ifdef PRI
  printf("Target AES read addr: %p, read config data: %llx\n", read_addr, read_value);
  #endif
  return read_value;
}


void baremetal_write(uint32_t tile, uint64_t addr, uint64_t value) {
  uint64_t write_addr = (addr << 3) | dream_base[tile];
  *(volatile uint64_t*)write_addr = value;
}
uint64_t uncached_read(uint32_t tile, uint64_t addr) {
  uint64_t read_addr = (addr << 3) | dream_base[tile];
  uint64_t read_value = *(volatile uint64_t*)read_addr;
  return read_value;
}

void cohort_ni_write(uint8_t c_id, uint64_t addr, uint64_t value) {
  uint8_t tile_no = c_id ;
  uint64_t write_addr = (addr * 8) | dream_base[tile_no];
  *(volatile uint64_t*)write_addr = value;
}

uint64_t cohort_ni_read(uint8_t c_id, uint64_t addr) {
  uint8_t tile_no = c_id;
  uint64_t read_addr = (addr * 8) | dream_base[c_id];
  uint64_t read_value = *(volatile uint64_t*)read_addr;
  return read_value;
}

void flush64(uint32_t tile, uint64_t addr, uint32_t size) {
  for (int k = 0; k < size; k+=8){ //8 elements is 8*64 (512 bits)
	      uint64_t res = *(volatile uint64_t *)addr; // Flush addr base, res=0
      addr += 64; // Address increases in 64 bytes
  }
}

// IF WE WANT TO READ THE 4 FIRST ENTRIES OF THE TLB
  // for (int i=0;i<4;i++){
  //     // Lower 4 bits {dirty bit, is_2M,is_1G,valid}
  //     // [31:4] is PPN
  //     // [59:32] is Asid, VPN
  //     // [63:60] Flag bits
  //     print64("Get TLB entry",dec_snoop_tlb_entry(0) );
  // }

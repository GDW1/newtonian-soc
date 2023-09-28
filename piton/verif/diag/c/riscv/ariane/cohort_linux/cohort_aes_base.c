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

#define NUM_QUEUES 3

#define BATCH_SIZE 18


extern uint64_t back_off_count;

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


struct _key_expansion_output {
uint64_t keys_out_higher[11];
uint64_t keys_out_lower[11];
};

typedef struct _key_expansion_output key_expansion_output;
key_expansion_output key_expansion(uint64_t key_higher, uint64_t key_lower) {
	uint64_t keys_out_higher[11];
	uint64_t keys_out_lower[11];
	// Combinational function
	uint64_t _temp0 = 32;
	uint64_t _temp1 = key_higher >> _temp0;
	uint64_t _temp2 = 4294967295;
	uint64_t _temp3 = _temp1 & _temp2;
	uint64_t w0 = _temp3;
	uint64_t _temp4 = 4294967295;
	uint64_t _temp5 = key_higher & _temp4;
	uint64_t w1 = _temp5;
	uint64_t _temp6 = 32;
	uint64_t _temp7 = key_lower >> _temp6;
	uint64_t _temp8 = 4294967295;
	uint64_t _temp9 = _temp7 & _temp8;
	uint64_t w2 = _temp9;
	uint64_t _temp10 = 4294967295;
	uint64_t _temp11 = key_lower & _temp10;
	uint64_t w3 = _temp11;
	uint64_t _temp12[256] = { 99, 124, 119, 123, 242, 107, 111, 197, 48, 1, 103, 43, 254, 215, 171, 118, 202, 130, 201, 125, 250, 89, 71, 240, 173, 212, 162, 175, 156, 164, 114, 192, 183, 253, 147, 38, 54, 63, 247, 204, 52, 165, 229, 241, 113, 216, 49, 21, 4, 199, 35, 195, 24, 150, 5, 154, 7, 18, 128, 226, 235, 39, 178, 117, 9, 131, 44, 26, 27, 110, 90, 160, 82, 59, 214, 179, 41, 227, 47, 132, 83, 209, 0, 237, 32, 252, 177, 91, 106, 203, 190, 57, 74, 76, 88, 207, 208, 239, 170, 251, 67, 77, 51, 133, 69, 249, 2, 127, 80, 60, 159, 168, 81, 163, 64, 143, 146, 157, 56, 245, 188, 182, 218, 33, 16, 255, 243, 210, 205, 12, 19, 236, 95, 151, 68, 23, 196, 167, 126, 61, 100, 93, 25, 115, 96, 129, 79, 220, 34, 42, 144, 136, 70, 238, 184, 20, 222, 94, 11, 219, 224, 50, 58, 10, 73, 6, 36, 92, 194, 211, 172, 98, 145, 149, 228, 121, 231, 200, 55, 109, 141, 213, 78, 169, 108, 86, 244, 234, 101, 122, 174, 8, 186, 120, 37, 46, 28, 166, 180, 198, 232, 221, 116, 31, 75, 189, 139, 138, 112, 62, 181, 102, 72, 3, 246, 14, 97, 53, 87, 185, 134, 193, 29, 158, 225, 248, 152, 17, 105, 217, 142, 148, 155, 30, 135, 233, 206, 85, 40, 223, 140, 161, 137, 13, 191, 230, 66, 104, 65, 153, 45, 15, 176, 84, 187, 2 };
	uint64_t sbox[256];
	memcpy(sbox, _temp12, sizeof(sbox));
	uint64_t _temp13 = 16;
	uint64_t _temp14 = w3 >> _temp13;
	uint64_t _temp15 = 255;
	uint64_t _temp16 = _temp14 & _temp15;
	uint64_t _temp17 = sbox[_temp16];
	uint64_t _temp18 = 24;
	uint64_t _temp19 = _temp17 << _temp18;
	uint64_t _temp20 = 8;
	uint64_t _temp21 = w3 >> _temp20;
	uint64_t _temp22 = 255;
	uint64_t _temp23 = _temp21 & _temp22;
	uint64_t _temp24 = sbox[_temp23];
	uint64_t _temp25 = 16;
	uint64_t _temp26 = _temp24 << _temp25;
	uint64_t _temp27 = _temp19 | _temp26;
	uint64_t _temp28 = 0;
	uint64_t _temp29 = w3 >> _temp28;
	uint64_t _temp30 = 255;
	uint64_t _temp31 = _temp29 & _temp30;
	uint64_t _temp32 = sbox[_temp31];
	uint64_t _temp33 = 8;
	uint64_t _temp34 = _temp32 << _temp33;
	uint64_t _temp35 = _temp27 | _temp34;
	uint64_t _temp36 = 24;
	uint64_t _temp37 = w3 >> _temp36;
	uint64_t _temp38 = 255;
	uint64_t _temp39 = _temp37 & _temp38;
	uint64_t _temp40 = sbox[_temp39];
	uint64_t _temp41 = _temp35 | _temp40;
	uint64_t subword1 = _temp41;
	uint64_t _temp42 = w0 ^ subword1;
	uint64_t _temp43 = 16777216;
	uint64_t _temp44 = _temp42 ^ _temp43;
	uint64_t w4 = _temp44;
	uint64_t _temp45 = w1 ^ w0;
	uint64_t _temp46 = _temp45 ^ subword1;
	uint64_t _temp47 = 16777216;
	uint64_t _temp48 = _temp46 ^ _temp47;
	uint64_t w5 = _temp48;
	uint64_t _temp49 = w2 ^ w1;
	uint64_t _temp50 = _temp49 ^ w0;
	uint64_t _temp51 = _temp50 ^ subword1;
	uint64_t _temp52 = 16777216;
	uint64_t _temp53 = _temp51 ^ _temp52;
	uint64_t w6 = _temp53;
	uint64_t _temp54 = w3 ^ w2;
	uint64_t _temp55 = _temp54 ^ w1;
	uint64_t _temp56 = _temp55 ^ w0;
	uint64_t _temp57 = _temp56 ^ subword1;
	uint64_t _temp58 = 16777216;
	uint64_t _temp59 = _temp57 ^ _temp58;
	uint64_t w7 = _temp59;
	uint64_t _temp60 = 16;
	uint64_t _temp61 = w7 >> _temp60;
	uint64_t _temp62 = 255;
	uint64_t _temp63 = _temp61 & _temp62;
	uint64_t _temp64 = sbox[_temp63];
	uint64_t _temp65 = 24;
	uint64_t _temp66 = _temp64 << _temp65;
	uint64_t _temp67 = 8;
	uint64_t _temp68 = w7 >> _temp67;
	uint64_t _temp69 = 255;
	uint64_t _temp70 = _temp68 & _temp69;
	uint64_t _temp71 = sbox[_temp70];
	uint64_t _temp72 = 16;
	uint64_t _temp73 = _temp71 << _temp72;
	uint64_t _temp74 = _temp66 | _temp73;
	uint64_t _temp75 = 0;
	uint64_t _temp76 = w7 >> _temp75;
	uint64_t _temp77 = 255;
	uint64_t _temp78 = _temp76 & _temp77;
	uint64_t _temp79 = sbox[_temp78];
	uint64_t _temp80 = 8;
	uint64_t _temp81 = _temp79 << _temp80;
	uint64_t _temp82 = _temp74 | _temp81;
	uint64_t _temp83 = 24;
	uint64_t _temp84 = w7 >> _temp83;
	uint64_t _temp85 = 255;
	uint64_t _temp86 = _temp84 & _temp85;
	uint64_t _temp87 = sbox[_temp86];
	uint64_t _temp88 = _temp82 | _temp87;
	uint64_t subword2 = _temp88;
	uint64_t _temp89 = w4 ^ subword2;
	uint64_t _temp90 = 33554432;
	uint64_t _temp91 = _temp89 ^ _temp90;
	uint64_t w8 = _temp91;
	uint64_t _temp92 = w5 ^ w4;
	uint64_t _temp93 = _temp92 ^ subword2;
	uint64_t _temp94 = 33554432;
	uint64_t _temp95 = _temp93 ^ _temp94;
	uint64_t w9 = _temp95;
	uint64_t _temp96 = w6 ^ w5;
	uint64_t _temp97 = _temp96 ^ w4;
	uint64_t _temp98 = _temp97 ^ subword2;
	uint64_t _temp99 = 33554432;
	uint64_t _temp100 = _temp98 ^ _temp99;
	uint64_t w10 = _temp100;
	uint64_t _temp101 = w7 ^ w6;
	uint64_t _temp102 = _temp101 ^ w5;
	uint64_t _temp103 = _temp102 ^ w4;
	uint64_t _temp104 = _temp103 ^ subword2;
	uint64_t _temp105 = 33554432;
	uint64_t _temp106 = _temp104 ^ _temp105;
	uint64_t w11 = _temp106;
	uint64_t _temp107 = 16;
	uint64_t _temp108 = w11 >> _temp107;
	uint64_t _temp109 = 255;
	uint64_t _temp110 = _temp108 & _temp109;
	uint64_t _temp111 = sbox[_temp110];
	uint64_t _temp112 = 24;
	uint64_t _temp113 = _temp111 << _temp112;
	uint64_t _temp114 = 8;
	uint64_t _temp115 = w11 >> _temp114;
	uint64_t _temp116 = 255;
	uint64_t _temp117 = _temp115 & _temp116;
	uint64_t _temp118 = sbox[_temp117];
	uint64_t _temp119 = 16;
	uint64_t _temp120 = _temp118 << _temp119;
	uint64_t _temp121 = _temp113 | _temp120;
	uint64_t _temp122 = 0;
	uint64_t _temp123 = w11 >> _temp122;
	uint64_t _temp124 = 255;
	uint64_t _temp125 = _temp123 & _temp124;
	uint64_t _temp126 = sbox[_temp125];
	uint64_t _temp127 = 8;
	uint64_t _temp128 = _temp126 << _temp127;
	uint64_t _temp129 = _temp121 | _temp128;
	uint64_t _temp130 = 24;
	uint64_t _temp131 = w11 >> _temp130;
	uint64_t _temp132 = 255;
	uint64_t _temp133 = _temp131 & _temp132;
	uint64_t _temp134 = sbox[_temp133];
	uint64_t _temp135 = _temp129 | _temp134;
	uint64_t subword3 = _temp135;
	uint64_t _temp136 = w8 ^ subword3;
	uint64_t _temp137 = 50331648;
	uint64_t _temp138 = _temp136 ^ _temp137;
	uint64_t w12 = _temp138;
	uint64_t _temp139 = w9 ^ w8;
	uint64_t _temp140 = _temp139 ^ subword3;
	uint64_t _temp141 = 50331648;
	uint64_t _temp142 = _temp140 ^ _temp141;
	uint64_t w13 = _temp142;
	uint64_t _temp143 = w10 ^ w9;
	uint64_t _temp144 = _temp143 ^ w8;
	uint64_t _temp145 = _temp144 ^ subword3;
	uint64_t _temp146 = 50331648;
	uint64_t _temp147 = _temp145 ^ _temp146;
	uint64_t w14 = _temp147;
	uint64_t _temp148 = w11 ^ w10;
	uint64_t _temp149 = _temp148 ^ w9;
	uint64_t _temp150 = _temp149 ^ w8;
	uint64_t _temp151 = _temp150 ^ subword3;
	uint64_t _temp152 = 50331648;
	uint64_t _temp153 = _temp151 ^ _temp152;
	uint64_t w15 = _temp153;
	uint64_t _temp154 = 16;
	uint64_t _temp155 = w15 >> _temp154;
	uint64_t _temp156 = 255;
	uint64_t _temp157 = _temp155 & _temp156;
	uint64_t _temp158 = sbox[_temp157];
	uint64_t _temp159 = 24;
	uint64_t _temp160 = _temp158 << _temp159;
	uint64_t _temp161 = 8;
	uint64_t _temp162 = w15 >> _temp161;
	uint64_t _temp163 = 255;
	uint64_t _temp164 = _temp162 & _temp163;
	uint64_t _temp165 = sbox[_temp164];
	uint64_t _temp166 = 16;
	uint64_t _temp167 = _temp165 << _temp166;
	uint64_t _temp168 = _temp160 | _temp167;
	uint64_t _temp169 = 0;
	uint64_t _temp170 = w15 >> _temp169;
	uint64_t _temp171 = 255;
	uint64_t _temp172 = _temp170 & _temp171;
	uint64_t _temp173 = sbox[_temp172];
	uint64_t _temp174 = 8;
	uint64_t _temp175 = _temp173 << _temp174;
	uint64_t _temp176 = _temp168 | _temp175;
	uint64_t _temp177 = 24;
	uint64_t _temp178 = w15 >> _temp177;
	uint64_t _temp179 = 255;
	uint64_t _temp180 = _temp178 & _temp179;
	uint64_t _temp181 = sbox[_temp180];
	uint64_t _temp182 = _temp176 | _temp181;
	uint64_t subword4 = _temp182;
	uint64_t _temp183 = w12 ^ subword4;
	uint64_t _temp184 = 67108864;
	uint64_t _temp185 = _temp183 ^ _temp184;
	uint64_t w16 = _temp185;
	uint64_t _temp186 = w13 ^ w12;
	uint64_t _temp187 = _temp186 ^ subword4;
	uint64_t _temp188 = 67108864;
	uint64_t _temp189 = _temp187 ^ _temp188;
	uint64_t w17 = _temp189;
	uint64_t _temp190 = w14 ^ w13;
	uint64_t _temp191 = _temp190 ^ w12;
	uint64_t _temp192 = _temp191 ^ subword4;
	uint64_t _temp193 = 67108864;
	uint64_t _temp194 = _temp192 ^ _temp193;
	uint64_t w18 = _temp194;
	uint64_t _temp195 = w15 ^ w14;
	uint64_t _temp196 = _temp195 ^ w13;
	uint64_t _temp197 = _temp196 ^ w12;
	uint64_t _temp198 = _temp197 ^ subword4;
	uint64_t _temp199 = 67108864;
	uint64_t _temp200 = _temp198 ^ _temp199;
	uint64_t w19 = _temp200;
	uint64_t _temp201 = 16;
	uint64_t _temp202 = w19 >> _temp201;
	uint64_t _temp203 = 255;
	uint64_t _temp204 = _temp202 & _temp203;
	uint64_t _temp205 = sbox[_temp204];
	uint64_t _temp206 = 24;
	uint64_t _temp207 = _temp205 << _temp206;
	uint64_t _temp208 = 8;
	uint64_t _temp209 = w19 >> _temp208;
	uint64_t _temp210 = 255;
	uint64_t _temp211 = _temp209 & _temp210;
	uint64_t _temp212 = sbox[_temp211];
	uint64_t _temp213 = 16;
	uint64_t _temp214 = _temp212 << _temp213;
	uint64_t _temp215 = _temp207 | _temp214;
	uint64_t _temp216 = 0;
	uint64_t _temp217 = w19 >> _temp216;
	uint64_t _temp218 = 255;
	uint64_t _temp219 = _temp217 & _temp218;
	uint64_t _temp220 = sbox[_temp219];
	uint64_t _temp221 = 8;
	uint64_t _temp222 = _temp220 << _temp221;
	uint64_t _temp223 = _temp215 | _temp222;
	uint64_t _temp224 = 24;
	uint64_t _temp225 = w19 >> _temp224;
	uint64_t _temp226 = 255;
	uint64_t _temp227 = _temp225 & _temp226;
	uint64_t _temp228 = sbox[_temp227];
	uint64_t _temp229 = _temp223 | _temp228;
	uint64_t subword5 = _temp229;
	uint64_t _temp230 = w16 ^ subword5;
	uint64_t _temp231 = 83886080;
	uint64_t _temp232 = _temp230 ^ _temp231;
	uint64_t w20 = _temp232;
	uint64_t _temp233 = w17 ^ w16;
	uint64_t _temp234 = _temp233 ^ subword5;
	uint64_t _temp235 = 83886080;
	uint64_t _temp236 = _temp234 ^ _temp235;
	uint64_t w21 = _temp236;
	uint64_t _temp237 = w18 ^ w17;
	uint64_t _temp238 = _temp237 ^ w16;
	uint64_t _temp239 = _temp238 ^ subword5;
	uint64_t _temp240 = 83886080;
	uint64_t _temp241 = _temp239 ^ _temp240;
	uint64_t w22 = _temp241;
	uint64_t _temp242 = w19 ^ w18;
	uint64_t _temp243 = _temp242 ^ w17;
	uint64_t _temp244 = _temp243 ^ w16;
	uint64_t _temp245 = _temp244 ^ subword5;
	uint64_t _temp246 = 83886080;
	uint64_t _temp247 = _temp245 ^ _temp246;
	uint64_t w23 = _temp247;
	uint64_t _temp248 = 16;
	uint64_t _temp249 = w23 >> _temp248;
	uint64_t _temp250 = 255;
	uint64_t _temp251 = _temp249 & _temp250;
	uint64_t _temp252 = sbox[_temp251];
	uint64_t _temp253 = 24;
	uint64_t _temp254 = _temp252 << _temp253;
	uint64_t _temp255 = 8;
	uint64_t _temp256 = w23 >> _temp255;
	uint64_t _temp257 = 255;
	uint64_t _temp258 = _temp256 & _temp257;
	uint64_t _temp259 = sbox[_temp258];
	uint64_t _temp260 = 16;
	uint64_t _temp261 = _temp259 << _temp260;
	uint64_t _temp262 = _temp254 | _temp261;
	uint64_t _temp263 = 0;
	uint64_t _temp264 = w23 >> _temp263;
	uint64_t _temp265 = 255;
	uint64_t _temp266 = _temp264 & _temp265;
	uint64_t _temp267 = sbox[_temp266];
	uint64_t _temp268 = 8;
	uint64_t _temp269 = _temp267 << _temp268;
	uint64_t _temp270 = _temp262 | _temp269;
	uint64_t _temp271 = 24;
	uint64_t _temp272 = w23 >> _temp271;
	uint64_t _temp273 = 255;
	uint64_t _temp274 = _temp272 & _temp273;
	uint64_t _temp275 = sbox[_temp274];
	uint64_t _temp276 = _temp270 | _temp275;
	uint64_t subword6 = _temp276;
	uint64_t _temp277 = w20 ^ subword6;
	uint64_t _temp278 = 100663296;
	uint64_t _temp279 = _temp277 ^ _temp278;
	uint64_t w24 = _temp279;
	uint64_t _temp280 = w21 ^ w20;
	uint64_t _temp281 = _temp280 ^ subword6;
	uint64_t _temp282 = 100663296;
	uint64_t _temp283 = _temp281 ^ _temp282;
	uint64_t w25 = _temp283;
	uint64_t _temp284 = w22 ^ w21;
	uint64_t _temp285 = _temp284 ^ w20;
	uint64_t _temp286 = _temp285 ^ subword6;
	uint64_t _temp287 = 100663296;
	uint64_t _temp288 = _temp286 ^ _temp287;
	uint64_t w26 = _temp288;
	uint64_t _temp289 = w23 ^ w22;
	uint64_t _temp290 = _temp289 ^ w21;
	uint64_t _temp291 = _temp290 ^ w20;
	uint64_t _temp292 = _temp291 ^ subword6;
	uint64_t _temp293 = 100663296;
	uint64_t _temp294 = _temp292 ^ _temp293;
	uint64_t w27 = _temp294;
	uint64_t _temp295 = 16;
	uint64_t _temp296 = w27 >> _temp295;
	uint64_t _temp297 = 255;
	uint64_t _temp298 = _temp296 & _temp297;
	uint64_t _temp299 = sbox[_temp298];
	uint64_t _temp300 = 24;
	uint64_t _temp301 = _temp299 << _temp300;
	uint64_t _temp302 = 8;
	uint64_t _temp303 = w27 >> _temp302;
	uint64_t _temp304 = 255;
	uint64_t _temp305 = _temp303 & _temp304;
	uint64_t _temp306 = sbox[_temp305];
	uint64_t _temp307 = 16;
	uint64_t _temp308 = _temp306 << _temp307;
	uint64_t _temp309 = _temp301 | _temp308;
	uint64_t _temp310 = 0;
	uint64_t _temp311 = w27 >> _temp310;
	uint64_t _temp312 = 255;
	uint64_t _temp313 = _temp311 & _temp312;
	uint64_t _temp314 = sbox[_temp313];
	uint64_t _temp315 = 8;
	uint64_t _temp316 = _temp314 << _temp315;
	uint64_t _temp317 = _temp309 | _temp316;
	uint64_t _temp318 = 24;
	uint64_t _temp319 = w27 >> _temp318;
	uint64_t _temp320 = 255;
	uint64_t _temp321 = _temp319 & _temp320;
	uint64_t _temp322 = sbox[_temp321];
	uint64_t _temp323 = _temp317 | _temp322;
	uint64_t subword7 = _temp323;
	uint64_t _temp324 = w24 ^ subword7;
	uint64_t _temp325 = 117440512;
	uint64_t _temp326 = _temp324 ^ _temp325;
	uint64_t w28 = _temp326;
	uint64_t _temp327 = w25 ^ w24;
	uint64_t _temp328 = _temp327 ^ subword7;
	uint64_t _temp329 = 117440512;
	uint64_t _temp330 = _temp328 ^ _temp329;
	uint64_t w29 = _temp330;
	uint64_t _temp331 = w26 ^ w25;
	uint64_t _temp332 = _temp331 ^ w24;
	uint64_t _temp333 = _temp332 ^ subword7;
	uint64_t _temp334 = 117440512;
	uint64_t _temp335 = _temp333 ^ _temp334;
	uint64_t w30 = _temp335;
	uint64_t _temp336 = w27 ^ w26;
	uint64_t _temp337 = _temp336 ^ w25;
	uint64_t _temp338 = _temp337 ^ w24;
	uint64_t _temp339 = _temp338 ^ subword7;
	uint64_t _temp340 = 117440512;
	uint64_t _temp341 = _temp339 ^ _temp340;
	uint64_t w31 = _temp341;
	uint64_t _temp342 = 16;
	uint64_t _temp343 = w31 >> _temp342;
	uint64_t _temp344 = 255;
	uint64_t _temp345 = _temp343 & _temp344;
	uint64_t _temp346 = sbox[_temp345];
	uint64_t _temp347 = 24;
	uint64_t _temp348 = _temp346 << _temp347;
	uint64_t _temp349 = 8;
	uint64_t _temp350 = w31 >> _temp349;
	uint64_t _temp351 = 255;
	uint64_t _temp352 = _temp350 & _temp351;
	uint64_t _temp353 = sbox[_temp352];
	uint64_t _temp354 = 16;
	uint64_t _temp355 = _temp353 << _temp354;
	uint64_t _temp356 = _temp348 | _temp355;
	uint64_t _temp357 = 0;
	uint64_t _temp358 = w31 >> _temp357;
	uint64_t _temp359 = 255;
	uint64_t _temp360 = _temp358 & _temp359;
	uint64_t _temp361 = sbox[_temp360];
	uint64_t _temp362 = 8;
	uint64_t _temp363 = _temp361 << _temp362;
	uint64_t _temp364 = _temp356 | _temp363;
	uint64_t _temp365 = 24;
	uint64_t _temp366 = w31 >> _temp365;
	uint64_t _temp367 = 255;
	uint64_t _temp368 = _temp366 & _temp367;
	uint64_t _temp369 = sbox[_temp368];
	uint64_t _temp370 = _temp364 | _temp369;
	uint64_t subword8 = _temp370;
	uint64_t _temp371 = w28 ^ subword8;
	uint64_t _temp372 = 134217728;
	uint64_t _temp373 = _temp371 ^ _temp372;
	uint64_t w32 = _temp373;
	uint64_t _temp374 = w29 ^ w28;
	uint64_t _temp375 = _temp374 ^ subword8;
	uint64_t _temp376 = 134217728;
	uint64_t _temp377 = _temp375 ^ _temp376;
	uint64_t w33 = _temp377;
	uint64_t _temp378 = w30 ^ w29;
	uint64_t _temp379 = _temp378 ^ w28;
	uint64_t _temp380 = _temp379 ^ subword8;
	uint64_t _temp381 = 134217728;
	uint64_t _temp382 = _temp380 ^ _temp381;
	uint64_t w34 = _temp382;
	uint64_t _temp383 = w31 ^ w30;
	uint64_t _temp384 = _temp383 ^ w29;
	uint64_t _temp385 = _temp384 ^ w28;
	uint64_t _temp386 = _temp385 ^ subword8;
	uint64_t _temp387 = 134217728;
	uint64_t _temp388 = _temp386 ^ _temp387;
	uint64_t w35 = _temp388;
	uint64_t _temp389 = 16;
	uint64_t _temp390 = w35 >> _temp389;
	uint64_t _temp391 = 255;
	uint64_t _temp392 = _temp390 & _temp391;
	uint64_t _temp393 = sbox[_temp392];
	uint64_t _temp394 = 24;
	uint64_t _temp395 = _temp393 << _temp394;
	uint64_t _temp396 = 8;
	uint64_t _temp397 = w35 >> _temp396;
	uint64_t _temp398 = 255;
	uint64_t _temp399 = _temp397 & _temp398;
	uint64_t _temp400 = sbox[_temp399];
	uint64_t _temp401 = 16;
	uint64_t _temp402 = _temp400 << _temp401;
	uint64_t _temp403 = _temp395 | _temp402;
	uint64_t _temp404 = 0;
	uint64_t _temp405 = w35 >> _temp404;
	uint64_t _temp406 = 255;
	uint64_t _temp407 = _temp405 & _temp406;
	uint64_t _temp408 = sbox[_temp407];
	uint64_t _temp409 = 8;
	uint64_t _temp410 = _temp408 << _temp409;
	uint64_t _temp411 = _temp403 | _temp410;
	uint64_t _temp412 = 24;
	uint64_t _temp413 = w35 >> _temp412;
	uint64_t _temp414 = 255;
	uint64_t _temp415 = _temp413 & _temp414;
	uint64_t _temp416 = sbox[_temp415];
	uint64_t _temp417 = _temp411 | _temp416;
	uint64_t subword9 = _temp417;
	uint64_t _temp418 = w32 ^ subword9;
	uint64_t _temp419 = 150994944;
	uint64_t _temp420 = _temp418 ^ _temp419;
	uint64_t w36 = _temp420;
	uint64_t _temp421 = w33 ^ w32;
	uint64_t _temp422 = _temp421 ^ subword9;
	uint64_t _temp423 = 150994944;
	uint64_t _temp424 = _temp422 ^ _temp423;
	uint64_t w37 = _temp424;
	uint64_t _temp425 = w34 ^ w33;
	uint64_t _temp426 = _temp425 ^ w32;
	uint64_t _temp427 = _temp426 ^ subword9;
	uint64_t _temp428 = 150994944;
	uint64_t _temp429 = _temp427 ^ _temp428;
	uint64_t w38 = _temp429;
	uint64_t _temp430 = w35 ^ w34;
	uint64_t _temp431 = _temp430 ^ w33;
	uint64_t _temp432 = _temp431 ^ w32;
	uint64_t _temp433 = _temp432 ^ subword9;
	uint64_t _temp434 = 150994944;
	uint64_t _temp435 = _temp433 ^ _temp434;
	uint64_t w39 = _temp435;
	uint64_t _temp436 = 16;
	uint64_t _temp437 = w39 >> _temp436;
	uint64_t _temp438 = 255;
	uint64_t _temp439 = _temp437 & _temp438;
	uint64_t _temp440 = sbox[_temp439];
	uint64_t _temp441 = 24;
	uint64_t _temp442 = _temp440 << _temp441;
	uint64_t _temp443 = 8;
	uint64_t _temp444 = w39 >> _temp443;
	uint64_t _temp445 = 255;
	uint64_t _temp446 = _temp444 & _temp445;
	uint64_t _temp447 = sbox[_temp446];
	uint64_t _temp448 = 16;
	uint64_t _temp449 = _temp447 << _temp448;
	uint64_t _temp450 = _temp442 | _temp449;
	uint64_t _temp451 = 0;
	uint64_t _temp452 = w39 >> _temp451;
	uint64_t _temp453 = 255;
	uint64_t _temp454 = _temp452 & _temp453;
	uint64_t _temp455 = sbox[_temp454];
	uint64_t _temp456 = 8;
	uint64_t _temp457 = _temp455 << _temp456;
	uint64_t _temp458 = _temp450 | _temp457;
	uint64_t _temp459 = 24;
	uint64_t _temp460 = w39 >> _temp459;
	uint64_t _temp461 = 255;
	uint64_t _temp462 = _temp460 & _temp461;
	uint64_t _temp463 = sbox[_temp462];
	uint64_t _temp464 = _temp458 | _temp463;
	uint64_t subword10 = _temp464;
	uint64_t _temp465 = w36 ^ subword10;
	uint64_t _temp466 = 167772160;
	uint64_t _temp467 = _temp465 ^ _temp466;
	uint64_t w40 = _temp467;
	uint64_t _temp468 = w37 ^ w36;
	uint64_t _temp469 = _temp468 ^ subword10;
	uint64_t _temp470 = 167772160;
	uint64_t _temp471 = _temp469 ^ _temp470;
	uint64_t w41 = _temp471;
	uint64_t _temp472 = w38 ^ w37;
	uint64_t _temp473 = _temp472 ^ w36;
	uint64_t _temp474 = _temp473 ^ subword10;
	uint64_t _temp475 = 167772160;
	uint64_t _temp476 = _temp474 ^ _temp475;
	uint64_t w42 = _temp476;
	uint64_t _temp477 = w39 ^ w38;
	uint64_t _temp478 = _temp477 ^ w37;
	uint64_t _temp479 = _temp478 ^ w36;
	uint64_t _temp480 = _temp479 ^ subword10;
	uint64_t _temp481 = 167772160;
	uint64_t _temp482 = _temp480 ^ _temp481;
	uint64_t w43 = _temp482;
	uint64_t _temp483 = 32;
	uint64_t _temp484 = w0 << _temp483;
	uint64_t _temp485 = _temp484 | w1;
	uint64_t key_s0_lower = _temp485;
	uint64_t _temp486 = 32;
	uint64_t _temp487 = w2 << _temp486;
	uint64_t _temp488 = _temp487 | w3;
	uint64_t key_s0_higher = _temp488;
	uint64_t _temp489 = 32;
	uint64_t _temp490 = w4 << _temp489;
	uint64_t _temp491 = _temp490 | w5;
	uint64_t key_s1_lower = _temp491;
	uint64_t _temp492 = 32;
	uint64_t _temp493 = w6 << _temp492;
	uint64_t _temp494 = _temp493 | w7;
	uint64_t key_s1_higher = _temp494;
	uint64_t _temp495 = 32;
	uint64_t _temp496 = w8 << _temp495;
	uint64_t _temp497 = _temp496 | w9;
	uint64_t key_s2_lower = _temp497;
	uint64_t _temp498 = 32;
	uint64_t _temp499 = w10 << _temp498;
	uint64_t _temp500 = _temp499 | w11;
	uint64_t key_s2_higher = _temp500;
	uint64_t _temp501 = 32;
	uint64_t _temp502 = w12 << _temp501;
	uint64_t _temp503 = _temp502 | w13;
	uint64_t key_s3_lower = _temp503;
	uint64_t _temp504 = 32;
	uint64_t _temp505 = w14 << _temp504;
	uint64_t _temp506 = _temp505 | w15;
	uint64_t key_s3_higher = _temp506;
	uint64_t _temp507 = 32;
	uint64_t _temp508 = w16 << _temp507;
	uint64_t _temp509 = _temp508 | w17;
	uint64_t key_s4_lower = _temp509;
	uint64_t _temp510 = 32;
	uint64_t _temp511 = w18 << _temp510;
	uint64_t _temp512 = _temp511 | w19;
	uint64_t key_s4_higher = _temp512;
	uint64_t _temp513 = 32;
	uint64_t _temp514 = w20 << _temp513;
	uint64_t _temp515 = _temp514 | w21;
	uint64_t key_s5_lower = _temp515;
	uint64_t _temp516 = 32;
	uint64_t _temp517 = w22 << _temp516;
	uint64_t _temp518 = _temp517 | w23;
	uint64_t key_s5_higher = _temp518;
	uint64_t _temp519 = 32;
	uint64_t _temp520 = w24 << _temp519;
	uint64_t _temp521 = _temp520 | w25;
	uint64_t key_s6_lower = _temp521;
	uint64_t _temp522 = 32;
	uint64_t _temp523 = w26 << _temp522;
	uint64_t _temp524 = _temp523 | w27;
	uint64_t key_s6_higher = _temp524;
	uint64_t _temp525 = 32;
	uint64_t _temp526 = w28 << _temp525;
	uint64_t _temp527 = _temp526 | w29;
	uint64_t key_s7_lower = _temp527;
	uint64_t _temp528 = 32;
	uint64_t _temp529 = w30 << _temp528;
	uint64_t _temp530 = _temp529 | w31;
	uint64_t key_s7_higher = _temp530;
	uint64_t _temp531 = 32;
	uint64_t _temp532 = w32 << _temp531;
	uint64_t _temp533 = _temp532 | w33;
	uint64_t key_s8_lower = _temp533;
	uint64_t _temp534 = 32;
	uint64_t _temp535 = w34 << _temp534;
	uint64_t _temp536 = _temp535 | w35;
	uint64_t key_s8_higher = _temp536;
	uint64_t _temp537 = 32;
	uint64_t _temp538 = w36 << _temp537;
	uint64_t _temp539 = _temp538 | w37;
	uint64_t key_s9_lower = _temp539;
	uint64_t _temp540 = 32;
	uint64_t _temp541 = w38 << _temp540;
	uint64_t _temp542 = _temp541 | w39;
	uint64_t key_s9_higher = _temp542;
	uint64_t _temp543 = 32;
	uint64_t _temp544 = w40 << _temp543;
	uint64_t _temp545 = _temp544 | w41;
	uint64_t key_s10_lower = _temp545;
	uint64_t _temp546 = 32;
	uint64_t _temp547 = w42 << _temp546;
	uint64_t _temp548 = _temp547 | w43;
	uint64_t key_s10_higher = _temp548;
	uint64_t _temp549[11] =  {key_s0_higher, key_s1_higher, key_s2_higher, key_s3_higher, key_s4_higher, key_s5_higher, key_s6_higher, key_s7_higher, key_s8_higher, key_s9_higher, key_s10_higher} ;
	memcpy(keys_out_higher, _temp549, sizeof(keys_out_higher));
	uint64_t _temp550[11] =  {key_s0_lower, key_s1_lower, key_s2_lower, key_s3_lower, key_s4_lower, key_s5_lower, key_s6_lower, key_s7_lower, key_s8_lower, key_s9_lower, key_s10_lower} ;
	memcpy(keys_out_lower, _temp550, sizeof(keys_out_lower));

	key_expansion_output output;
	memcpy(output.keys_out_higher, keys_out_higher, sizeof(output.keys_out_higher));
	memcpy(output.keys_out_lower, keys_out_lower, sizeof(output.keys_out_lower));
	return output;
}

struct _add_round_key_output {
uint64_t data_out_higher;
uint64_t data_out_lower;
};

typedef struct _add_round_key_output add_round_key_output;
add_round_key_output add_round_key(uint64_t data_in_lower, uint64_t data_in_higher, uint64_t key_in_lower, uint64_t key_in_higher) {
	uint64_t data_out_higher;
	uint64_t data_out_lower;
	// Combinational function
	uint64_t _temp0 = data_in_lower ^ key_in_lower;
	data_out_lower = _temp0;
	uint64_t _temp1 = data_in_higher ^ key_in_higher;
	data_out_higher = _temp1;

	add_round_key_output output;
	output.data_out_higher = data_out_higher;
	output.data_out_lower = data_out_lower;
	return output;
}

struct _mix_columns_output {
uint64_t data_out_lower;
uint64_t data_out_higher;
};

typedef struct _mix_columns_output mix_columns_output;
mix_columns_output mix_columns(uint64_t data_lower, uint64_t data_higher) {
	uint64_t data_out_lower;
	uint64_t data_out_higher;
	// Combinational function
	uint64_t _temp0 = 0xff;
	uint64_t _temp1 = data_lower & _temp0;
	uint64_t data0 = _temp1;
	uint64_t _temp2 = 0xff00;
	uint64_t _temp3 = data_lower & _temp2;
	uint64_t _temp4 = 0x8;
	uint64_t _temp5 = _temp3 >> _temp4;
	uint64_t data1 = _temp5;
	uint64_t _temp6 = 0xff0000;
	uint64_t _temp7 = data_lower & _temp6;
	uint64_t _temp8 = 0x10;
	uint64_t _temp9 = _temp7 >> _temp8;
	uint64_t data2 = _temp9;
	uint64_t _temp10 = 0xff000000;
	uint64_t _temp11 = data_lower & _temp10;
	uint64_t _temp12 = 0x18;
	uint64_t _temp13 = _temp11 >> _temp12;
	uint64_t data3 = _temp13;
	uint64_t _temp14 = 0xff00000000;
	uint64_t _temp15 = data_lower & _temp14;
	uint64_t _temp16 = 0x20;
	uint64_t _temp17 = _temp15 >> _temp16;
	uint64_t data4 = _temp17;
	uint64_t _temp18 = 0xff0000000000;
	uint64_t _temp19 = data_lower & _temp18;
	uint64_t _temp20 = 0x28;
	uint64_t _temp21 = _temp19 >> _temp20;
	uint64_t data5 = _temp21;
	uint64_t _temp22 = 0xff000000000000;
	uint64_t _temp23 = data_lower & _temp22;
	uint64_t _temp24 = 0x30;
	uint64_t _temp25 = _temp23 >> _temp24;
	uint64_t data6 = _temp25;
	uint64_t _temp26 = 0xff00000000000000;
	uint64_t _temp27 = data_lower & _temp26;
	uint64_t _temp28 = 0x38;
	uint64_t _temp29 = _temp27 >> _temp28;
	uint64_t data7 = _temp29;
	uint64_t _temp30 = 0xff;
	uint64_t _temp31 = data_higher & _temp30;
	uint64_t data8 = _temp31;
	uint64_t _temp32 = 0xff00;
	uint64_t _temp33 = data_higher & _temp32;
	uint64_t _temp34 = 0x8;
	uint64_t _temp35 = _temp33 >> _temp34;
	uint64_t data9 = _temp35;
	uint64_t _temp36 = 0xff0000;
	uint64_t _temp37 = data_higher & _temp36;
	uint64_t _temp38 = 0x10;
	uint64_t _temp39 = _temp37 >> _temp38;
	uint64_t data10 = _temp39;
	uint64_t _temp40 = 0xff000000;
	uint64_t _temp41 = data_higher & _temp40;
	uint64_t _temp42 = 0x18;
	uint64_t _temp43 = _temp41 >> _temp42;
	uint64_t data11 = _temp43;
	uint64_t _temp44 = 0xff00000000;
	uint64_t _temp45 = data_higher & _temp44;
	uint64_t _temp46 = 0x20;
	uint64_t _temp47 = _temp45 >> _temp46;
	uint64_t data12 = _temp47;
	uint64_t _temp48 = 0xff0000000000;
	uint64_t _temp49 = data_higher & _temp48;
	uint64_t _temp50 = 0x28;
	uint64_t _temp51 = _temp49 >> _temp50;
	uint64_t data13 = _temp51;
	uint64_t _temp52 = 0xff000000000000;
	uint64_t _temp53 = data_higher & _temp52;
	uint64_t _temp54 = 0x30;
	uint64_t _temp55 = _temp53 >> _temp54;
	uint64_t data14 = _temp55;
	uint64_t _temp56 = 0xff00000000000000;
	uint64_t _temp57 = data_higher & _temp56;
	uint64_t _temp58 = 0x38;
	uint64_t _temp59 = _temp57 >> _temp58;
	uint64_t data15 = _temp59;
	uint64_t _temp60 = 0x2;
	uint64_t _temp61 = data0 * _temp60;
	uint64_t _temp62 = 0x3;
	uint64_t _temp63 = data1 * _temp62;
	uint64_t _temp64 = _temp61 ^ _temp63;
	uint64_t _temp65 = _temp64 ^ data2;
	uint64_t _temp66 = _temp65 ^ data3;
	uint64_t data_out0 = _temp66;
	uint64_t _temp67 = 0x2;
	uint64_t _temp68 = data1 * _temp67;
	uint64_t _temp69 = data0 ^ _temp68;
	uint64_t _temp70 = 0x3;
	uint64_t _temp71 = data2 * _temp70;
	uint64_t _temp72 = _temp69 ^ _temp71;
	uint64_t _temp73 = _temp72 ^ data3;
	uint64_t data_out1 = _temp73;
	uint64_t _temp74 = data0 ^ data1;
	uint64_t _temp75 = 0x2;
	uint64_t _temp76 = data2 * _temp75;
	uint64_t _temp77 = _temp74 ^ _temp76;
	uint64_t _temp78 = 0x3;
	uint64_t _temp79 = data3 * _temp78;
	uint64_t _temp80 = _temp77 ^ _temp79;
	uint64_t data_out2 = _temp80;
	uint64_t _temp81 = 0x3;
	uint64_t _temp82 = data0 * _temp81;
	uint64_t _temp83 = _temp82 ^ data1;
	uint64_t _temp84 = _temp83 ^ data2;
	uint64_t _temp85 = 0x2;
	uint64_t _temp86 = data3 * _temp85;
	uint64_t _temp87 = _temp84 ^ _temp86;
	uint64_t data_out3 = _temp87;
	uint64_t _temp88 = 0x2;
	uint64_t _temp89 = data4 * _temp88;
	uint64_t _temp90 = 0x3;
	uint64_t _temp91 = data5 * _temp90;
	uint64_t _temp92 = _temp89 ^ _temp91;
	uint64_t _temp93 = _temp92 ^ data6;
	uint64_t _temp94 = _temp93 ^ data7;
	uint64_t data_out4 = _temp94;
	uint64_t _temp95 = 0x2;
	uint64_t _temp96 = data5 * _temp95;
	uint64_t _temp97 = data4 ^ _temp96;
	uint64_t _temp98 = 0x3;
	uint64_t _temp99 = data6 * _temp98;
	uint64_t _temp100 = _temp97 ^ _temp99;
	uint64_t _temp101 = _temp100 ^ data7;
	uint64_t data_out5 = _temp101;
	uint64_t _temp102 = data4 ^ data5;
	uint64_t _temp103 = 0x2;
	uint64_t _temp104 = data6 * _temp103;
	uint64_t _temp105 = _temp102 ^ _temp104;
	uint64_t _temp106 = 0x3;
	uint64_t _temp107 = data7 * _temp106;
	uint64_t _temp108 = _temp105 ^ _temp107;
	uint64_t data_out6 = _temp108;
	uint64_t _temp109 = 0x3;
	uint64_t _temp110 = data4 * _temp109;
	uint64_t _temp111 = _temp110 ^ data5;
	uint64_t _temp112 = _temp111 ^ data6;
	uint64_t _temp113 = 0x2;
	uint64_t _temp114 = data7 * _temp113;
	uint64_t _temp115 = _temp112 ^ _temp114;
	uint64_t data_out7 = _temp115;
	uint64_t _temp116 = 0x2;
	uint64_t _temp117 = data8 * _temp116;
	uint64_t _temp118 = 0x3;
	uint64_t _temp119 = data9 * _temp118;
	uint64_t _temp120 = _temp117 ^ _temp119;
	uint64_t _temp121 = _temp120 ^ data10;
	uint64_t _temp122 = _temp121 ^ data11;
	uint64_t data_out8 = _temp122;
	uint64_t _temp123 = 0x2;
	uint64_t _temp124 = data9 * _temp123;
	uint64_t _temp125 = data8 ^ _temp124;
	uint64_t _temp126 = 0x3;
	uint64_t _temp127 = data10 * _temp126;
	uint64_t _temp128 = _temp125 ^ _temp127;
	uint64_t _temp129 = _temp128 ^ data11;
	uint64_t data_out9 = _temp129;
	uint64_t _temp130 = data8 ^ data9;
	uint64_t _temp131 = 0x2;
	uint64_t _temp132 = data10 * _temp131;
	uint64_t _temp133 = _temp130 ^ _temp132;
	uint64_t _temp134 = 0x3;
	uint64_t _temp135 = data11 * _temp134;
	uint64_t _temp136 = _temp133 ^ _temp135;
	uint64_t data_out10 = _temp136;
	uint64_t _temp137 = 0x3;
	uint64_t _temp138 = data8 * _temp137;
	uint64_t _temp139 = _temp138 ^ data9;
	uint64_t _temp140 = _temp139 ^ data10;
	uint64_t _temp141 = 0x2;
	uint64_t _temp142 = data11 * _temp141;
	uint64_t _temp143 = _temp140 ^ _temp142;
	uint64_t data_out11 = _temp143;
	uint64_t _temp144 = 0x2;
	uint64_t _temp145 = data12 * _temp144;
	uint64_t _temp146 = 0x3;
	uint64_t _temp147 = data13 * _temp146;
	uint64_t _temp148 = _temp145 ^ _temp147;
	uint64_t _temp149 = _temp148 ^ data14;
	uint64_t _temp150 = _temp149 ^ data15;
	uint64_t data_out12 = _temp150;
	uint64_t _temp151 = 0x2;
	uint64_t _temp152 = data13 * _temp151;
	uint64_t _temp153 = data12 ^ _temp152;
	uint64_t _temp154 = 0x3;
	uint64_t _temp155 = data14 * _temp154;
	uint64_t _temp156 = _temp153 ^ _temp155;
	uint64_t _temp157 = _temp156 ^ data15;
	uint64_t data_out13 = _temp157;
	uint64_t _temp158 = data12 ^ data13;
	uint64_t _temp159 = 0x2;
	uint64_t _temp160 = data14 * _temp159;
	uint64_t _temp161 = _temp158 ^ _temp160;
	uint64_t _temp162 = 0x3;
	uint64_t _temp163 = data15 * _temp162;
	uint64_t _temp164 = _temp161 ^ _temp163;
	uint64_t data_out14 = _temp164;
	uint64_t _temp165 = 0x3;
	uint64_t _temp166 = data12 * _temp165;
	uint64_t _temp167 = _temp166 ^ data13;
	uint64_t _temp168 = _temp167 ^ data14;
	uint64_t _temp169 = 0x2;
	uint64_t _temp170 = data15 * _temp169;
	uint64_t _temp171 = _temp168 ^ _temp170;
	uint64_t data_out15 = _temp171;
	uint64_t _temp172 = 0x8;
	uint64_t _temp173 = data_out1 << _temp172;
	uint64_t _temp174 = data_out0 | _temp173;
	uint64_t _temp175 = 0x10;
	uint64_t _temp176 = data_out2 << _temp175;
	uint64_t _temp177 = _temp174 | _temp176;
	uint64_t _temp178 = 0x18;
	uint64_t _temp179 = data_out3 << _temp178;
	uint64_t _temp180 = _temp177 | _temp179;
	uint64_t _temp181 = 0x20;
	uint64_t _temp182 = data_out4 << _temp181;
	uint64_t _temp183 = _temp180 | _temp182;
	uint64_t _temp184 = 0x28;
	uint64_t _temp185 = data_out5 << _temp184;
	uint64_t _temp186 = _temp183 | _temp185;
	uint64_t _temp187 = 0x30;
	uint64_t _temp188 = data_out6 << _temp187;
	uint64_t _temp189 = _temp186 | _temp188;
	uint64_t _temp190 = 0x38;
	uint64_t _temp191 = data_out7 << _temp190;
	uint64_t _temp192 = _temp189 | _temp191;
	data_out_lower = _temp192;
	uint64_t _temp193 = 0x8;
	uint64_t _temp194 = data_out9 << _temp193;
	uint64_t _temp195 = data_out8 | _temp194;
	uint64_t _temp196 = 0x10;
	uint64_t _temp197 = data_out10 << _temp196;
	uint64_t _temp198 = _temp195 | _temp197;
	uint64_t _temp199 = 0x18;
	uint64_t _temp200 = data_out11 << _temp199;
	uint64_t _temp201 = _temp198 | _temp200;
	uint64_t _temp202 = 0x20;
	uint64_t _temp203 = data_out12 << _temp202;
	uint64_t _temp204 = _temp201 | _temp203;
	uint64_t _temp205 = 0x28;
	uint64_t _temp206 = data_out13 << _temp205;
	uint64_t _temp207 = _temp204 | _temp206;
	uint64_t _temp208 = 0x30;
	uint64_t _temp209 = data_out14 << _temp208;
	uint64_t _temp210 = _temp207 | _temp209;
	uint64_t _temp211 = 0x38;
	uint64_t _temp212 = data_out15 << _temp211;
	uint64_t _temp213 = _temp210 | _temp212;
	data_out_higher = _temp213;

	mix_columns_output output;
	output.data_out_lower = data_out_lower;
	output.data_out_higher = data_out_higher;
	return output;
}


struct _sub_bytes_output {
	uint64_t higher_out;
	uint64_t lower_out;
};

typedef struct _sub_bytes_output sub_bytes_output;
sub_bytes_output sub_bytes(uint64_t higher_bits, uint64_t lower_bits) {
	uint64_t higher_out;
	uint64_t lower_out;
	// Combinational function
	uint64_t _temp0 = 255;
	uint64_t _temp1 = lower_bits & _temp0;
	uint64_t data0 = _temp1;
	uint64_t _temp2 = 8;
	uint64_t _temp3 = lower_bits >> _temp2;
	uint64_t _temp4 = 255;
	uint64_t _temp5 = _temp3 & _temp4;
	uint64_t data1 = _temp5;
	uint64_t _temp6 = 16;
	uint64_t _temp7 = lower_bits >> _temp6;
	uint64_t _temp8 = 255;
	uint64_t _temp9 = _temp7 & _temp8;
	uint64_t data2 = _temp9;
	uint64_t _temp10 = 24;
	uint64_t _temp11 = lower_bits >> _temp10;
	uint64_t _temp12 = 255;
	uint64_t _temp13 = _temp11 & _temp12;
	uint64_t data3 = _temp13;
	uint64_t _temp14 = 32;
	uint64_t _temp15 = lower_bits >> _temp14;
	uint64_t _temp16 = 255;
	uint64_t _temp17 = _temp15 & _temp16;
	uint64_t data4 = _temp17;
	uint64_t _temp18 = 40;
	uint64_t _temp19 = lower_bits >> _temp18;
	uint64_t _temp20 = 255;
	uint64_t _temp21 = _temp19 & _temp20;
	uint64_t data5 = _temp21;
	uint64_t _temp22 = 48;
	uint64_t _temp23 = lower_bits >> _temp22;
	uint64_t _temp24 = 255;
	uint64_t _temp25 = _temp23 & _temp24;
	uint64_t data6 = _temp25;
	uint64_t _temp26 = 56;
	uint64_t _temp27 = lower_bits >> _temp26;
	uint64_t _temp28 = 255;
	uint64_t _temp29 = _temp27 & _temp28;
	uint64_t data7 = _temp29;
	uint64_t _temp30 = 255;
	uint64_t _temp31 = higher_bits & _temp30;
	uint64_t data8 = _temp31;
	uint64_t _temp32 = 8;
	uint64_t _temp33 = higher_bits >> _temp32;
	uint64_t _temp34 = 255;
	uint64_t _temp35 = _temp33 & _temp34;
	uint64_t data9 = _temp35;
	uint64_t _temp36 = 16;
	uint64_t _temp37 = higher_bits >> _temp36;
	uint64_t _temp38 = 255;
	uint64_t _temp39 = _temp37 & _temp38;
	uint64_t data10 = _temp39;
	uint64_t _temp40 = 24;
	uint64_t _temp41 = higher_bits >> _temp40;
	uint64_t _temp42 = 255;
	uint64_t _temp43 = _temp41 & _temp42;
	uint64_t data11 = _temp43;
	uint64_t _temp44 = 32;
	uint64_t _temp45 = higher_bits >> _temp44;
	uint64_t _temp46 = 255;
	uint64_t _temp47 = _temp45 & _temp46;
	uint64_t data12 = _temp47;
	uint64_t _temp48 = 40;
	uint64_t _temp49 = higher_bits >> _temp48;
	uint64_t _temp50 = 255;
	uint64_t _temp51 = _temp49 & _temp50;
	uint64_t data13 = _temp51;
	uint64_t _temp52 = 48;
	uint64_t _temp53 = higher_bits >> _temp52;
	uint64_t _temp54 = 255;
	uint64_t _temp55 = _temp53 & _temp54;
	uint64_t data14 = _temp55;
	uint64_t _temp56 = 56;
	uint64_t _temp57 = higher_bits >> _temp56;
	uint64_t _temp58 = 255;
	uint64_t _temp59 = _temp57 & _temp58;
	uint64_t data15 = _temp59;
	uint64_t _temp60[16] =  {data0, data1, data2, data3, data4, data5, data6, data7, data8, data9, data10, data11, data12, data13, data14, data15} ;
	uint64_t data[16];
	memcpy(data, _temp60, sizeof(data));
	uint64_t _temp61[256] = { 99, 124, 119, 123, 242, 107, 111, 197, 48, 1, 103, 43, 254, 215, 171, 118, 202, 130, 201, 125, 250, 89, 71, 240, 173, 212, 162, 175, 156, 164, 114, 192, 183, 253, 147, 38, 54, 63, 247, 204, 52, 165, 229, 241, 113, 216, 49, 21, 4, 199, 35, 195, 24, 150, 5, 154, 7, 18, 128, 226, 235, 39, 178, 117, 9, 131, 44, 26, 27, 110, 90, 160, 82, 59, 214, 179, 41, 227, 47, 132, 83, 209, 0, 237, 32, 252, 177, 91, 106, 203, 190, 57, 74, 76, 88, 207, 208, 239, 170, 251, 67, 77, 51, 133, 69, 249, 2, 127, 80, 60, 159, 168, 81, 163, 64, 143, 146, 157, 56, 245, 188, 182, 218, 33, 16, 255, 243, 210, 205, 12, 19, 236, 95, 151, 68, 23, 196, 167, 126, 61, 100, 93, 25, 115, 96, 129, 79, 220, 34, 42, 144, 136, 70, 238, 184, 20, 222, 94, 11, 219, 224, 50, 58, 10, 73, 6, 36, 92, 194, 211, 172, 98, 145, 149, 228, 121, 231, 200, 55, 109, 141, 213, 78, 169, 108, 86, 244, 234, 101, 122, 174, 8, 186, 120, 37, 46, 28, 166, 180, 198, 232, 221, 116, 31, 75, 189, 139, 138, 112, 62, 181, 102, 72, 3, 246, 14, 97, 53, 87, 185, 134, 193, 29, 158, 225, 248, 152, 17, 105, 217, 142, 148, 155, 30, 135, 233, 206, 85, 40, 223, 140, 161, 137, 13, 191, 230, 66, 104, 65, 153, 45, 15, 176, 84, 187, 2 };
	uint64_t sbox[256];
	memcpy(sbox, _temp61, sizeof(sbox));
	for(uint64_t i = 0; i < 16; i++) {
		uint64_t _temp62 = data[i];
		uint64_t _temp63 = sbox[_temp62];
		data[i] = _temp63;
	}
	uint64_t _temp65 = 0;
	uint64_t _temp66 = data[_temp65];
	uint64_t _temp67 = 1;
	uint64_t _temp68 = data[_temp67];
	uint64_t _temp69 = 8;
	uint64_t _temp70 = _temp68 << _temp69;
	uint64_t _temp71 = _temp66 | _temp70;
	uint64_t _temp72 = 2;
	uint64_t _temp73 = data[_temp72];
	uint64_t _temp74 = 16;
	uint64_t _temp75 = _temp73 << _temp74;
	uint64_t _temp76 = _temp71 | _temp75;
	uint64_t _temp77 = 3;
	uint64_t _temp78 = data[_temp77];
	uint64_t _temp79 = 24;
	uint64_t _temp80 = _temp78 << _temp79;
	uint64_t _temp81 = _temp76 | _temp80;
	uint64_t _temp82 = 4;
	uint64_t _temp83 = data[_temp82];
	uint64_t _temp84 = 32;
	uint64_t _temp85 = _temp83 << _temp84;
	uint64_t _temp86 = _temp81 | _temp85;
	uint64_t _temp87 = 5;
	uint64_t _temp88 = data[_temp87];
	uint64_t _temp89 = 40;
	uint64_t _temp90 = _temp88 << _temp89;
	uint64_t _temp91 = _temp86 | _temp90;
	uint64_t _temp92 = 6;
	uint64_t _temp93 = data[_temp92];
	uint64_t _temp94 = 48;
	uint64_t _temp95 = _temp93 << _temp94;
	uint64_t _temp96 = _temp91 | _temp95;
	uint64_t _temp97 = 7;
	uint64_t _temp98 = data[_temp97];
	uint64_t _temp99 = 56;
	uint64_t _temp100 = _temp98 << _temp99;
	uint64_t _temp101 = _temp96 | _temp100;
	higher_out = _temp101;
	uint64_t _temp102 = 8;
	uint64_t _temp103 = data[_temp102];
	uint64_t _temp104 = 9;
	uint64_t _temp105 = data[_temp104];
	uint64_t _temp106 = 8;
	uint64_t _temp107 = _temp105 << _temp106;
	uint64_t _temp108 = _temp103 | _temp107;
	uint64_t _temp109 = 10;
	uint64_t _temp110 = data[_temp109];
	uint64_t _temp111 = 16;
	uint64_t _temp112 = _temp110 << _temp111;
	uint64_t _temp113 = _temp108 | _temp112;
	uint64_t _temp114 = 11;
	uint64_t _temp115 = data[_temp114];
	uint64_t _temp116 = 24;
	uint64_t _temp117 = _temp115 << _temp116;
	uint64_t _temp118 = _temp113 | _temp117;
	uint64_t _temp119 = 12;
	uint64_t _temp120 = data[_temp119];
	uint64_t _temp121 = 32;
	uint64_t _temp122 = _temp120 << _temp121;
	uint64_t _temp123 = _temp118 | _temp122;
	uint64_t _temp124 = 13;
	uint64_t _temp125 = data[_temp124];
	uint64_t _temp126 = 40;
	uint64_t _temp127 = _temp125 << _temp126;
	uint64_t _temp128 = _temp123 | _temp127;
	uint64_t _temp129 = 14;
	uint64_t _temp130 = data[_temp129];
	uint64_t _temp131 = 48;
	uint64_t _temp132 = _temp130 << _temp131;
	uint64_t _temp133 = _temp128 | _temp132;
	uint64_t _temp134 = 15;
	uint64_t _temp135 = data[_temp134];
	uint64_t _temp136 = 56;
	uint64_t _temp137 = _temp135 << _temp136;
	uint64_t _temp138 = _temp133 | _temp137;
	lower_out = _temp138;

	sub_bytes_output output;
	output.higher_out = higher_out;
	output.lower_out = lower_out;
	return output;
}


struct _shift_rows_output {
	uint64_t data_out_higher;
	uint64_t data_out_lower;
};

typedef struct _shift_rows_output shift_rows_output;
shift_rows_output shift_rows(uint64_t higher_bits, uint64_t lower_bits) {
	uint64_t data_out_higher;
	uint64_t data_out_lower;
	// Combinational function
	uint64_t _temp0 = 255;
	uint64_t _temp1 = lower_bits & _temp0;
	uint64_t data0 = _temp1;
	uint64_t _temp2 = 8;
	uint64_t _temp3 = lower_bits >> _temp2;
	uint64_t _temp4 = 255;
	uint64_t _temp5 = _temp3 & _temp4;
	uint64_t data1 = _temp5;
	uint64_t _temp6 = 16;
	uint64_t _temp7 = lower_bits >> _temp6;
	uint64_t _temp8 = 255;
	uint64_t _temp9 = _temp7 & _temp8;
	uint64_t data2 = _temp9;
	uint64_t _temp10 = 24;
	uint64_t _temp11 = lower_bits >> _temp10;
	uint64_t _temp12 = 255;
	uint64_t _temp13 = _temp11 & _temp12;
	uint64_t data3 = _temp13;
	uint64_t _temp14 = 32;
	uint64_t _temp15 = lower_bits >> _temp14;
	uint64_t _temp16 = 255;
	uint64_t _temp17 = _temp15 & _temp16;
	uint64_t data4 = _temp17;
	uint64_t _temp18 = 40;
	uint64_t _temp19 = lower_bits >> _temp18;
	uint64_t _temp20 = 255;
	uint64_t _temp21 = _temp19 & _temp20;
	uint64_t data5 = _temp21;
	uint64_t _temp22 = 48;
	uint64_t _temp23 = lower_bits >> _temp22;
	uint64_t _temp24 = 255;
	uint64_t _temp25 = _temp23 & _temp24;
	uint64_t data6 = _temp25;
	uint64_t _temp26 = 56;
	uint64_t _temp27 = lower_bits >> _temp26;
	uint64_t _temp28 = 255;
	uint64_t _temp29 = _temp27 & _temp28;
	uint64_t data7 = _temp29;
	uint64_t _temp30 = 255;
	uint64_t _temp31 = higher_bits & _temp30;
	uint64_t data8 = _temp31;
	uint64_t _temp32 = 8;
	uint64_t _temp33 = higher_bits >> _temp32;
	uint64_t _temp34 = 255;
	uint64_t _temp35 = _temp33 & _temp34;
	uint64_t data9 = _temp35;
	uint64_t _temp36 = 16;
	uint64_t _temp37 = higher_bits >> _temp36;
	uint64_t _temp38 = 255;
	uint64_t _temp39 = _temp37 & _temp38;
	uint64_t data10 = _temp39;
	uint64_t _temp40 = 24;
	uint64_t _temp41 = higher_bits >> _temp40;
	uint64_t _temp42 = 255;
	uint64_t _temp43 = _temp41 & _temp42;
	uint64_t data11 = _temp43;
	uint64_t _temp44 = 32;
	uint64_t _temp45 = higher_bits >> _temp44;
	uint64_t _temp46 = 255;
	uint64_t _temp47 = _temp45 & _temp46;
	uint64_t data12 = _temp47;
	uint64_t _temp48 = 40;
	uint64_t _temp49 = higher_bits >> _temp48;
	uint64_t _temp50 = 255;
	uint64_t _temp51 = _temp49 & _temp50;
	uint64_t data13 = _temp51;
	uint64_t _temp52 = 48;
	uint64_t _temp53 = higher_bits >> _temp52;
	uint64_t _temp54 = 255;
	uint64_t _temp55 = _temp53 & _temp54;
	uint64_t data14 = _temp55;
	uint64_t _temp56 = 56;
	uint64_t _temp57 = higher_bits >> _temp56;
	uint64_t _temp58 = 255;
	uint64_t _temp59 = _temp57 & _temp58;
	uint64_t data15 = _temp59;
	uint64_t _temp60[16] =  {data0, data1, data2, data3, data5, data6, data7, data4, data10, data11, data8, data9, data15, data12, data13, data14} ;
	uint64_t data[16];
	memcpy(data, _temp60, sizeof(data));
	uint64_t _temp61 = 0;
	uint64_t _temp62 = data[_temp61];
	uint64_t _temp63 = 56;
	uint64_t _temp64 = _temp62 << _temp63;
	uint64_t _temp65 = 1;
	uint64_t _temp66 = data[_temp65];
	uint64_t _temp67 = 48;
	uint64_t _temp68 = _temp66 << _temp67;
	uint64_t _temp69 = 2;
	uint64_t _temp70 = data[_temp69];
	uint64_t _temp71 = 40;
	uint64_t _temp72 = _temp70 << _temp71;
	uint64_t _temp73 = 3;
	uint64_t _temp74 = data[_temp73];
	uint64_t _temp75 = 32;
	uint64_t _temp76 = _temp74 << _temp75;
	uint64_t _temp77 = 4;
	uint64_t _temp78 = data[_temp77];
	uint64_t _temp79 = 24;
	uint64_t _temp80 = _temp78 << _temp79;
	uint64_t _temp81 = 5;
	uint64_t _temp82 = data[_temp81];
	uint64_t _temp83 = 16;
	uint64_t _temp84 = _temp82 << _temp83;
	uint64_t _temp85 = 6;
	uint64_t _temp86 = data[_temp85];
	uint64_t _temp87 = 8;
	uint64_t _temp88 = _temp86 << _temp87;
	uint64_t _temp89 = 7;
	uint64_t _temp90 = data[_temp89];
	uint64_t _temp91 = _temp88 | _temp90;
	uint64_t _temp92 = _temp84 | _temp91;
	uint64_t _temp93 = _temp80 | _temp92;
	uint64_t _temp94 = _temp76 | _temp93;
	uint64_t _temp95 = _temp72 | _temp94;
	uint64_t _temp96 = _temp68 | _temp95;
	uint64_t _temp97 = _temp64 | _temp96;
	data_out_higher = _temp97;
	uint64_t _temp98 = 8;
	uint64_t _temp99 = data[_temp98];
	uint64_t _temp100 = 56;
	uint64_t _temp101 = _temp99 << _temp100;
	uint64_t _temp102 = 9;
	uint64_t _temp103 = data[_temp102];
	uint64_t _temp104 = 48;
	uint64_t _temp105 = _temp103 << _temp104;
	uint64_t _temp106 = 10;
	uint64_t _temp107 = data[_temp106];
	uint64_t _temp108 = 40;
	uint64_t _temp109 = _temp107 << _temp108;
	uint64_t _temp110 = 11;
	uint64_t _temp111 = data[_temp110];
	uint64_t _temp112 = 32;
	uint64_t _temp113 = _temp111 << _temp112;
	uint64_t _temp114 = 12;
	uint64_t _temp115 = data[_temp114];
	uint64_t _temp116 = 24;
	uint64_t _temp117 = _temp115 << _temp116;
	uint64_t _temp118 = 13;
	uint64_t _temp119 = data[_temp118];
	uint64_t _temp120 = 16;
	uint64_t _temp121 = _temp119 << _temp120;
	uint64_t _temp122 = 14;
	uint64_t _temp123 = data[_temp122];
	uint64_t _temp124 = 8;
	uint64_t _temp125 = _temp123 << _temp124;
	uint64_t _temp126 = 15;
	uint64_t _temp127 = data[_temp126];
	uint64_t _temp128 = _temp125 | _temp127;
	uint64_t _temp129 = _temp121 | _temp128;
	uint64_t _temp130 = _temp117 | _temp129;
	uint64_t _temp131 = _temp113 | _temp130;
	uint64_t _temp132 = _temp109 | _temp131;
	uint64_t _temp133 = _temp105 | _temp132;
	uint64_t _temp134 = _temp101 | _temp133;
	data_out_lower = _temp134;

	shift_rows_output output;
	output.data_out_higher = data_out_higher;
	output.data_out_lower = data_out_lower;
	return output;
}

add_round_key_output round (uint64_t data_in_lower, uint64_t data_in_higher, uint64_t key_lower, uint64_t key_higher) {
	// printf("key: 0x%lx_%lx\n", key_higher, key_lower);
	sub_bytes_output sub_bytes_out = sub_bytes(data_in_higher, data_in_lower);
	// printf("sub_bytes_out: 0x%lx_%lx\n", sub_bytes_out.higher_out, sub_bytes_out.lower_out);
	shift_rows_output shift_rows_out = shift_rows(sub_bytes_out.higher_out, sub_bytes_out.lower_out);
	// printf("shift_rows_out: 0x%lx_%lx\n", shift_rows_out.data_out_higher, shift_rows_out.data_out_lower);
	mix_columns_output mix_columns_out = mix_columns(shift_rows_out.data_out_lower, shift_rows_out.data_out_higher);
	// printf("mix_columns_out: 0x%lx_%lx\n", mix_columns_out.data_out_higher, mix_columns_out.data_out_lower);
	add_round_key_output add_round_key_out = add_round_key(mix_columns_out.data_out_lower, mix_columns_out.data_out_higher, key_lower, key_higher);
	return add_round_key_out;
}

add_round_key_output last_round (uint64_t data_in_lower, uint64_t data_in_higher, uint64_t key_lower, uint64_t key_higher) {
	// printf("key: 0x%lx_%lx\n", key_higher, key_lower);
	sub_bytes_output sub_bytes_out = sub_bytes(data_in_higher, data_in_lower);
	// printf("sub_bytes_out: 0x%lx_%lx\n", sub_bytes_out.higher_out, sub_bytes_out.lower_out);
	shift_rows_output shift_rows_out = shift_rows(sub_bytes_out.higher_out, sub_bytes_out.lower_out);
	// printf("shift_rows_out: 0x%lx_%lx\n", shift_rows_out.data_out_higher, shift_rows_out.data_out_lower);
	add_round_key_output add_round_key_out = add_round_key(shift_rows_out.data_out_lower, shift_rows_out.data_out_higher, key_lower, key_higher);
	return add_round_key_out;
}

struct _stage_0_output {
	uint64_t data_out_higher;
	uint64_t data_out_lower;
	key_expansion_output key_expansion_out;
};

typedef struct _stage_0_output stage_0_output;
stage_0_output stage_0(uint64_t data_in_lower, uint64_t data_in_higher, uint64_t key_lower, uint64_t key_higher) {
	key_expansion_output key_expansion_out = key_expansion(key_higher, key_lower);
	// for (int i = 0; i < 11; i++) {
	// 	printf("k_%d =  0x%lx_%lx\n", i, key_expansion_out.keys_out_higher[i], key_expansion_out.keys_out_lower[i]);
	// }
	add_round_key_output add_round_key_out = add_round_key(data_in_lower, data_in_higher, key_expansion_out.keys_out_lower[0], key_expansion_out.keys_out_higher[0]);
	// printf("add_round_key_out: 0x%lx_%lx\n", add_round_key_out.data_out_higher, add_round_key_out.data_out_lower);
	// round 1
	add_round_key_output add_key_out_1 = round(add_round_key_out.data_out_lower, add_round_key_out.data_out_higher, key_expansion_out.keys_out_lower[1], key_expansion_out.keys_out_higher[1]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_1.data_out_higher, add_key_out_1.data_out_lower);

	// round 2
	add_round_key_output add_key_out_2 = round(add_key_out_1.data_out_lower, add_key_out_1.data_out_higher, key_expansion_out.keys_out_lower[2], key_expansion_out.keys_out_higher[2]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_2.data_out_higher, add_key_out_2.data_out_lower);

	stage_0_output output;
	output.data_out_lower = add_key_out_2.data_out_lower;
	output.data_out_higher = add_key_out_2.data_out_higher;
	output.key_expansion_out = key_expansion_out;

	return output;
}

stage_0_output stage_1 (uint64_t data_in_lower, uint64_t data_in_higher, key_expansion_output key_expansion_out) {
	// round 3
	add_round_key_output add_key_out_1 = round(data_in_lower, data_in_higher, key_expansion_out.keys_out_lower[3], key_expansion_out.keys_out_higher[3]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_1.data_out_higher, add_key_out_1.data_out_lower);

	// round 4
	add_round_key_output add_key_out_2 = round(add_key_out_1.data_out_lower, add_key_out_1.data_out_higher, key_expansion_out.keys_out_lower[4], key_expansion_out.keys_out_higher[4]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_2.data_out_higher, add_key_out_2.data_out_lower);

	// round 5
	add_round_key_output add_key_out_3 = round(add_key_out_2.data_out_lower, add_key_out_2.data_out_higher, key_expansion_out.keys_out_lower[5], key_expansion_out.keys_out_higher[5]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_3.data_out_higher, add_key_out_3.data_out_lower);

	// round 6
	add_round_key_output add_key_out_4 = round(add_key_out_3.data_out_lower, add_key_out_3.data_out_higher, key_expansion_out.keys_out_lower[6], key_expansion_out.keys_out_higher[6]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_4.data_out_higher, add_key_out_4.data_out_lower);

	stage_0_output output;
	output.data_out_lower = add_key_out_4.data_out_lower;
	output.data_out_higher = add_key_out_4.data_out_higher;
	output.key_expansion_out = key_expansion_out;

	return output;
}

stage_0_output stage_2 (uint64_t data_in_lower, uint64_t data_in_higher, key_expansion_output key_expansion_out) {
	// round 3
	add_round_key_output add_key_out_5 = round(data_in_lower, data_in_higher, key_expansion_out.keys_out_lower[7], key_expansion_out.keys_out_higher[7]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_5.data_out_higher, add_key_out_5.data_out_lower);

	// round 4
	add_round_key_output add_key_out_6 = round(add_key_out_5.data_out_lower, add_key_out_5.data_out_higher, key_expansion_out.keys_out_lower[8], key_expansion_out.keys_out_higher[8]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_6.data_out_higher, add_key_out_6.data_out_lower);

	// round 5
	add_round_key_output add_key_out_7 = round(add_key_out_6.data_out_lower, add_key_out_6.data_out_higher, key_expansion_out.keys_out_lower[9], key_expansion_out.keys_out_higher[9]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_7.data_out_higher, add_key_out_7.data_out_lower);

	// round 6
	add_round_key_output add_key_out_8 = last_round(add_key_out_7.data_out_lower, add_key_out_7.data_out_higher, key_expansion_out.keys_out_lower[10], key_expansion_out.keys_out_higher[10]);
	// printf("data_out: 0x%lx_%lx\n", add_key_out_8.data_out_higher, add_key_out_8.data_out_lower);

	stage_0_output output;
	output.data_out_lower = add_key_out_8.data_out_lower;
	output.data_out_higher = add_key_out_8.data_out_higher;
	output.key_expansion_out = key_expansion_out;

	return output;
}

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

    uint64_t bypass[NUM_QUEUES] = {0b00, 0b10, 0b01};
    bool active[NUM_QUEUES] =     {false, true, true};
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

	// generate random data and key
	temp[0] = 0xCE5812A0A19CC7FA; // data_in_higher
	temp[1] = 0x737391C7F1443912; // data_in_lower

	temp[2] = 0x1b0356a6d986f350; // key_higher
	temp[3] = 0x1ea166d74afca90a; // key_lower

    printf("Starting NNHW process\n");

    unsigned long _c = -rdcycle();

	dec_open_producer(0);
	dec_open_consumer(0);


	// Push data
    if (active[0]) {
        if ((bypass[0] & 0x02) == 0x02) {
            // printf("Stage 0 push\n");
            fifo_push_64(temp[0], sw_to_cohort_fifo[0], 0); // push data_in_higher
            fifo_push_64(temp[1], sw_to_cohort_fifo[0], 1); // push data_in_lower
            fifo_push_64(temp[2], sw_to_cohort_fifo[0], 2); // push key_higher
            fifo_push_64(temp[3], sw_to_cohort_fifo[0], 3); // push key_lower

            fifo_push_sync(sw_to_cohort_fifo[0], 4); // push sync
        }

        if ((bypass[0] & 0x01) == 0x01) {
            // printf("Stage 0 pop\n");
            for (int i = 0; i < BATCH_SIZE; i++) {
                temp[i] = fifo_pop_64(cohort_to_sw_fifo[0], i);
            }

            fifo_pop_sync(cohort_to_sw_fifo[0], BATCH_SIZE);
        }
    } else {
        // use the SW version
        // take temp and run it through stage_0
		// printf("Stage 0 SW\n");

		stage_0_output stage_0_out = stage_0(temp[1], temp[0], temp[3], temp[2]);

		// marshall the data
		temp[0] = stage_0_out.data_out_lower;
		temp[1] = stage_0_out.data_out_higher;

		for (int i = 1; i < 9; i++) { // we want keys  2 - 10
			temp[ i * 2	  ] = stage_0_out.key_expansion_out.keys_out_lower[i + 2];
			temp[(i * 2) + 1] = stage_0_out.key_expansion_out.keys_out_higher[i + 2];
		}
    }


    // Push data
    if (active[1]) {
        if ((bypass[1] & 0x02) == 0x02) {
            // printf("Stage 1 push\n");
            // push 18 values
            for (int i = 0; i < BATCH_SIZE; i++) {
                fifo_push_64(temp[i], sw_to_cohort_fifo[1], i);
            }

            fifo_push_sync(sw_to_cohort_fifo[1], BATCH_SIZE);
        }

        if ((bypass[1] & 0x01) == 0x01) {
            // printf ("Stage 1 pop\n");
            for (int i = 0; i < 10; i++) {
                temp[i] = fifo_pop_64(cohort_to_sw_fifo[1], i);
            }

            fifo_pop_sync(cohort_to_sw_fifo[1], 10);
        }
    } else {
		// use the SW version
        // take temp and run it through stage_0
		// unmarshall temp into stage_0_input
		key_expansion_output key_expansion_input;
		// start at i = 7
		for (int i = 3; i < 11; i++) {
			key_expansion_input.keys_out_higher[i] = temp[((i - 2) * 2) + 1];
			key_expansion_input.keys_out_lower[i] = temp[((i - 2) * 2)];
		}

		// // print out the keys
		// for (int i = 0; i < 11; i++) {
		// 	printf("k_%d =  0x%lx_%lx\n", i, key_expansion_input.keys_out_higher[i], key_expansion_input.keys_out_lower[i]);
		// }

		stage_0_output stage_1_out = stage_1(temp[0], temp[1], key_expansion_input);

		// marshall the data
		temp[0] = stage_1_out.data_out_lower;
		temp[1] = stage_1_out.data_out_higher;

		for (int i = 1; i < 9; i++) { // we want keys  2 - 10
			temp[ i * 2	  ] = stage_1_out.key_expansion_out.keys_out_lower[i + 6];
			temp[(i * 2) + 1] = stage_1_out.key_expansion_out.keys_out_higher[i + 6];
		}
		// use the SW version
		// take temp and run it through stage_1
		// printf("Stage 1 SW\n");
    }

    // Stage 2
    if (active[2]) {
        // Push data
        if ((bypass[2] & 0x02) == 0x02) {
            // printf("Stage 2 push\n");
            // push 18 values
            for (int i = 0; i < 10; i++) {
                fifo_push_64(temp[i], sw_to_cohort_fifo[2], i);
            }

            fifo_push_sync(sw_to_cohort_fifo[2], 10);
        }

        if ((bypass[2] & 0x01) == 0x01) {
            // printf("Stage 2 pop\n");
            for (int i = 0; i < 2; i++) {
                temp[i] = fifo_pop_64(cohort_to_sw_fifo[2], i);
            }

            fifo_pop_sync(cohort_to_sw_fifo[2], 2);
        }
    } else {
		// use the SW version
		// take temp and run it through stage_2
		// use the SW version
        // take temp and run it through stage_0
		// unmarshall temp into stage_0_input
		key_expansion_output key_expansion_input;
		// start at i = 7
		for (int i = 7; i < 11; i++) {
			key_expansion_input.keys_out_higher[i] = temp[((i - 6) * 2) + 1];
			key_expansion_input.keys_out_lower[i] = temp[((i - 6) * 2)];
		}

		// // print out the keys
		// for (int i = 0; i < 11; i++) {
		// 	printf("k_%d =  0x%lx_%lx\n", i, key_expansion_input.keys_out_higher[i], key_expansion_input.keys_out_lower[i]);
		// }

		stage_0_output stage_2_out = stage_2(temp[0], temp[1], key_expansion_input);

		// marshall the data
		temp[0] = stage_2_out.data_out_lower;
		temp[1] = stage_2_out.data_out_higher;

		// use the SW version
		// take temp and run it through stage_1
		// printf("Stage 2 SW\n");
		
    }
    /*
        END PUSH DATA
    */
    
	dec_close_producer(0);
	dec_close_consumer(0);
    
    _c += rdcycle();

    // print out the number of cycles and instructions
    printf("Cycles: %ld\n", _c);

	printf("Accelerator Output: 0x%lx_%lx\n, ", temp[1], temp[0]);

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

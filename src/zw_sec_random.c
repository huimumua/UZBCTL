/** 
@file   zw_sec_random.c - Z-wave security layer pseudo-random number generator.
   
@author David Chow
  
@version    1.0 18-1-11  Initial release

version: 1.0
comments: Initial release
*/ 
#include <stddef.h>
#include <time.h>
#include "../include/zw_api_pte.h"
#include "../include/zw_security.h"


/**
zwsec_rand_output - Get pseudo-random number
@param[in,out]	random_ctx	    Random number generator context
@param[in]	    rand_num_cnt	Number of bytes required (maximum 16 bytes)
@param[out]	    rand_out	    Generated pseudo-random number
@return  number of bytes generated
*/
unsigned zwsec_rand_output(zwsec_random_t *random_ctx, uint8_t rand_num_cnt, uint8_t *rand_out)
{
    uint8_t     msg[16];
    uint8_t     tmp_buf[16];

    if (rand_num_cnt > 16)
    {
        return 0;
    }

    // Encrypt the value 0x5C (repeated 16 times) and use the least significant k bits of the result as PRNG output
    memset(msg, 0x5C, 16);
    zwsec_aes_encrypt(msg, random_ctx->inner_state, tmp_buf);
    memcpy(rand_out, tmp_buf, rand_num_cnt);

    //Encrypt the value 0x36 (repeated 16 times) and store the result as the new inner state in SRAM. 
    memset(msg, 0x36, 16);
    zwsec_aes_encrypt(msg, random_ctx->inner_state, tmp_buf);
    memcpy(random_ctx->inner_state, tmp_buf, 16);

    return rand_num_cnt;

}


/**
zwsec_rand_updt - Update the pseudo-random number generator inner state
@param[in,out]	random_ctx	Random number generator context
@return  0 on success; negative error number on failure
*/
static int zwsec_rand_updt(zwsec_random_t *random_ctx)
{
    int         result, i;
    uint8_t     rand_num[32];
    uint8_t     K1[16];
    uint8_t     K2[16];
    uint8_t     H0[16];
    uint8_t     H1[16];
    uint8_t     H2[16];
    uint8_t     S[16];
    uint8_t     rand_num_cnt;

    // Collect 256 bit (32 byte) data from hw generator
    rand_num_cnt = 32;
    result = zw_get_random_word(random_ctx->appl_ctx, &rand_num_cnt, rand_num);

    if (result != 0)
    {
        return result;
    }

    if (rand_num_cnt != 32)
    {
        return ZW_ERR_FAILED;
    }

    // Split these bits into the 128-bit encoded keys doneted K1 and K2, and set H0 to be the value 0xA5 (repeated 16 times).. 
    for (i = 0; i < 32; i++)
    {
        if (i < 16)
        {
            K1[i] = rand_num[i];
        }
        else
        {
            K2[i - 16] = rand_num[i];
        }
    }

    memset(H0, 0xA5, 16);

    // Compute H1 = AES(K1; H0) XOR H0
    zwsec_aes_encrypt(H0, K1, H1);

    for (i = 0; i < 16; i++)
    {
        H1[i] ^= H0[i];
    }

    // Compute H2 = AES(K2; H1) XOR H1
    zwsec_aes_encrypt(H1, K2, H2);
    for (i = 0; i < 16; i++)
    {
        H2[i] ^= H1[i];
    }

    // Compute S = Inner State XOR H2
    for (i = 0; i < 16; i++)
    {
        S[i] = (random_ctx->inner_state[i] ^ H2[i]);
    }

    // Use S as AES key and encrypt the value 0x36 (repeated 16 time)..
    // Store the result as the inner state in SRAM, and make sure to delete the old inner state all intermediate values.
    memset(H0, 0x36, 16);
    zwsec_aes_encrypt(H0, S, random_ctx->inner_state);

    return ZW_ERR_NONE;

}


/**
zwsec_rand_init - Initialize the pseudo-random number generator
@param[in,out]	random_ctx	Random number generator context
@return  0 on success; negative error number on failure
*/
int zwsec_rand_init(zwsec_random_t *random_ctx)
{
    memset(random_ctx->inner_state, 0, 16);

    //Update inner state
    return zwsec_rand_updt(random_ctx);
}



/**********************************************************************
* @file		fips_197_test_keys.c
* @brief	reference keys and buffers for the test mode
* @version	1.0
* @date		03.07.2012
* @author	nxp28536
*
* Copyright(C) 2012, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

#include "cypher.h"
#include "cypher_internal.h"


#ifdef TEST_MODE


#ifdef AES128_SUPPORT
/******************************************************************************
 * AES 128 specific keys
 *****************************************************************************/
const expKey_t referenceCryptKeyAes128_test1[KSCH_AES128_SIZE] __attribute__ ((aligned (4))) = {

0x2b7e1516,
0x28aed2a6,
0xabf71588,
0x09cf4f3c,
0xa0fafe17,
0x88542cb1,
0x23a33939,
0x2a6c7605,
0xf2c295f2,
0x7a96b943,
0x5935807a,
0x7359f67f,
0x3d80477d,
0x4716fe3e,
0x1e237e44,
0x6d7a883b,
0xef44a541,
0xa8525b7f,
0xb671253b,
0xdb0bad00,
0xd4d1c6f8,
0x7c839d87,
0xcaf2b8bc,
0x11f915bc,
0x6d88a37a,
0x110b3efd,
0xdbf98641,
0xca0093fd,
0x4e54f70e,
0x5f5fc9f3,
0x84a64fb2,
0x4ea6dc4f,
0xead27321,
0xb58dbad2,
0x312bf560,
0x7f8d292f,
0xac7766f3,
0x19fadc21,
0x28d12941,
0x575c006e,
0xd014f9a8,
0xc9ee2589,
0xe13f0cc8,
0xb6630ca6
};

const expKey_t referenceCryptKeyAes128_test2[KSCH_AES128_SIZE] __attribute__ ((aligned (4))) = {

0x00010203,
0x04050607,
0x08090a0b,
0x0c0d0e0f,
0xd6aa74fd,
0xd2af72fa,
0xdaa678f1,
0xd6ab76fe,
0xb692cf0b,
0x643dbdf1,
0xbe9bc500,
0x6830b3fe,
0xb6ff744e,
0xd2c2c9bf,
0x6c590cbf,
0x0469bf41,
0x47f7f7bc,
0x95353e03,
0xf96c32bc,
0xfd058dfd,
0x3caaa3e8,
0xa99f9deb,
0x50f3af57,
0xadf622aa,
0x5e390f7d,
0xf7a69296,
0xa7553dc1,
0x0aa31f6b,
0x14f9701a,
0xe35fe28c,
0x440adf4d,
0x4ea9c026,
0x47438735,
0xa41c65b9,
0xe016baf4,
0xaebf7ad2,
0x549932d1,
0xf0855768,
0x1093ed9c,
0xbe2c974e,
0x13111d7f,
0xe3944a17,
0xf307a78b,
0x4d2b30c5

};


const expKey_t referenceDecryptKeyAes128_test2[KSCH_AES128_SIZE] __attribute__ ((aligned (4))) = {

0x00010203,
0x04050607,
0x08090a0b,
0x0c0d0e0f,
0x8c56dff0,
0x825dd3f9,
0x805ad3fc,
0x8659d7fd,
0xa0db0299,
0x2286d160,
0xa2dc029c,
0x2485d561,
0xc7c6e391,
0xe54032f1,
0x479c306d,
0x6319e50c,
0xa8a2f504,
0x4de2c7f5,
0x0a7ef798,
0x69671294,
0x2ec41027,
0x6326d7d2,
0x6958204a,
0x003f32de,
0x72e3098d,
0x11c5de5f,
0x789dfe15,
0x78a2cccb,
0x8d82fc74,
0x9c47222b,
0xe4dadc3e,
0x9c7810f5,
0x1362a463,
0x8f258648,
0x6bff5a76,
0xf7874a83,
0x13aa29be,
0x9c8faff6,
0xf770f580,
0x00f7bf03,
0x13111d7f,
0xe3944a17,
0xf307a78b,
0x4d2b30c5

};


/******************************************************************************
 * FIPS-196 specification test patterns
 *****************************************************************************/

/* pattern set one, appendix B  */
aeskey_t aesTest_passPhraseAES128_test1[AES128_KEYSIZE] __attribute__ ((aligned (4)))
= { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

plainData_t plainText_AES128_test1[PLAIN_BSIZE] __attribute__ ((aligned (4))) 
=  { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30 ,0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};

cryptData_t cryptedText_AES128_test1[CRYPT_BSIZE] __attribute__ ((aligned (4))) 
=  { 0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09 ,0xfb, 0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32};

/* pattern set two, appendix C */
aeskey_t aesTest_passPhraseAES128_test2[AES128_KEYSIZE] __attribute__ ((aligned (4)))
= { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

plainData_t plainText_AES128_test2[PLAIN_BSIZE] __attribute__ ((aligned (4))) 
=  { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 ,0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

cryptData_t cryptedText_AES128_test2[CRYPT_BSIZE] __attribute__ ((aligned (4))) 
=  { 0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04 ,0x30, 0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};


/* placeholder for results */
cryptData_t cryptResult_AES128[CRYPT_BSIZE] __attribute__ ((aligned (4))) ;
plainData_t deCryptResult_AES128[PLAIN_BSIZE] __attribute__ ((aligned (4))) ;


CypherConfigStr testConfigAes128_test1 = {

	AES_128,
	&aesTest_passPhraseAES128_test1[0],
	&referenceCryptKeyAes128_test1[0],
	&cryptedText_AES128_test1[0]
};

CypherConfigStr testConfigAes128_test2 = {

	AES_128,
	&aesTest_passPhraseAES128_test2[0],
	&referenceCryptKeyAes128_test2[0],
	&cryptedText_AES128_test2[0]
};

CypherConfigStr testConfigAes128_test3 = {

	AES_128,
	&aesTest_passPhraseAES128_test2[0],
	&referenceDecryptKeyAes128_test2[0],
	&cryptedText_AES128_test2[0]
};


#endif

#ifdef AES192_SUPPORT
/******************************************************************************
 * AES 192 specific keys
 *****************************************************************************/
const expKey_t referenceCryptKeyAes192_test1[KSCH_AES192_SIZE] __attribute__ ((aligned (4))) = {

0x8e73b0f7,
0xda0e6452,
0xc810f32b,
0x809079e5,
0x62f8ead2, 
0x522c6b7b, 
0xfe0c91f7, 
0x2402f5a5, 
0xec12068e, 
0x6c827f6b, 
0x0e7a95b9, 
0x5c56fec2, 
0x4db7b4bd, 
0x69b54118, 
0x85a74796, 
0xe92538fd, 
0xe75fad44, 
0xbb095386, 
0x485af057, 
0x21efb14f, 
0xa448f6d9, 
0x4d6dce24, 
0xaa326360, 
0x113b30e6, 
0xa25e7ed5, 
0x83b1cf9a, 
0x27f93943, 
0x6a94f767, 
0xc0a69407, 
0xd19da4e1, 
0xec1786eb, 
0x6fa64971, 
0x485f7032, 
0x22cb8755, 
0xe26d1352, 
0x33f0b7b3, 
0x40beeb28, 
0x2f18a259, 
0x6747d26b,  
0x458c553e,  
0xa7e1466c,  
0x9411f1df,  
0x821f750a,  
0xad07d753,  
0xca400538,  
0x8fcc5006,
0x282d166a,
0xbc3ce7b5,
0xe98ba06f,
0x448c773c,
0x8ecc7204,
0x01002202

};

const expKey_t referenceCryptKeyAes192_test2[KSCH_AES192_SIZE] __attribute__ ((aligned (4))) = {

0x00010203,
0x04050607,
0x08090a0b,
0x0c0d0e0f,
0x10111213,
0x14151617,
0x5846f2f9,
0x5c43f4fe,
0x544afef5,
0x5847f0fa,
0x4856e2e9,
0x5c43f4fe,
0x40f949b3,
0x1cbabd4d,
0x48f043b8,
0x10b7b342,
0x58e151ab,
0x04a2a555,
0x7effb541,
0x6245080c,
0x2ab54bb4,
0x3a02f8f6,
0x62e3a95d,
0x66410c08,
0xf5018572,
0x97448d7e,
0xbdf1c6ca,
0x87f33e3c,
0xe5109761,
0x83519b69,
0x34157c9e,
0xa351f1e0,
0x1ea0372a,
0x99530916,
0x7c439e77,
0xff12051e,
0xdd7e0e88,
0x7e2fff68,
0x608fc842,
0xf9dcc154,
0x859f5f23,
0x7a8d5a3d,
0xc0c02952,
0xbeefd63a,
0xde601e78,
0x27bcdf2c,
0xa223800f,
0xd8aeda32,
0xa4970a33,
0x1a78dc09,
0xc418c271,
0xe3a41d5d

};


const expKey_t referenceDecryptKeyAes192_test2[KSCH_AES192_SIZE] __attribute__ ((aligned (4))) = {

0x00010203,
0x04050607,
0x08090a0b,
0x0c0d0e0f,
0x1a1f181d,
0x1e1b1c19,
0x4742c7d7,
0x4949cbde,
0x4b4ecbdb,
0x4d4dcfda,
0x5752d7c7,
0x4949cbde,
0x60dcef10,
0x299524ce,
0x62dbef15,
0x2f9620cf,
0x78c4f708,
0x318d3cd6,
0x9655b701,
0xbfc093cf,
0xdd1b7cda,
0xf28d5c15,
0x8a49ab1d,
0xbbc497cb,
0xc6deb0ab,
0x791e2364,
0xa4055fbe,
0x568803ab,
0xdcc1a8b6,
0x67053f7d,
0xcc5c194a,
0xb5423a2e,
0x11476590,
0x47cf663b,
0x9b0ece8d,
0xfc0bf1f0,
0xf77d6ec1,
0x423f54ef,
0x5378317f,
0x14b75744,
0x8fb999c9,
0x73b26839,
0xc7f9d89d,
0x85c68c72,
0xd6bebd0d,
0xc209ea49,
0x4db07380,
0x3e021bb9,
0xa4970a33,
0x1a78dc09,
0xc418c271,
0xe3a41d5d

};


/******************************************************************************
 * FIPS-196 specification test patterns
 *****************************************************************************/
/* pattern set one, appendix B  */
aeskey_t aesTest_passPhraseAES192_test1[AES192_KEYSIZE] __attribute__ ((aligned (4)))
= { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 
	0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };


/* pattern set two, appendix C */
aeskey_t aesTest_passPhraseAES192_test2[AES192_KEYSIZE] __attribute__ ((aligned (4)))
= { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 
	0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };

plainData_t plainText_AES192_test2[PLAIN_BSIZE] __attribute__ ((aligned (4))) 
=  { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 ,0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

cryptData_t cryptedText_AES192_test2[CRYPT_BSIZE] __attribute__ ((aligned (4))) 
=  { 0xdd, 0xa9, 0x7c, 0xa4, 0x86, 0x4c, 0xdf, 0xe0, 0x6e, 0xaf, 0x70, 0xa0, 0xec, 0x0d, 0x71, 0x91};


/* placeholder for results */
cryptData_t cryptResult_AES192[CRYPT_BSIZE] __attribute__ ((aligned (4))) ;
plainData_t deCryptResult_AES192[PLAIN_BSIZE] __attribute__ ((aligned (4))) ;


CypherConfigStr testConfigAes192_test1 = {

	AES_192,
	&aesTest_passPhraseAES192_test1[0],
	&referenceCryptKeyAes192_test1[0],
	0
};

CypherConfigStr testConfigAes192_test2 = {

	AES_192,
	&aesTest_passPhraseAES192_test2[0],
	&referenceCryptKeyAes192_test2[0],
	&cryptedText_AES192_test2[0]
};

CypherConfigStr testConfigAes192_test3 = {

	AES_192,
	&aesTest_passPhraseAES192_test2[0],
	&referenceDecryptKeyAes192_test2[0],
	&cryptedText_AES192_test2[0]
};


#endif

#ifdef AES256_SUPPORT
/******************************************************************************
 * AES 256 specific keys
 *****************************************************************************/
const expKey_t referenceCryptKeyAes256_test1[KSCH_AES256_SIZE] __attribute__ ((aligned (4))) = {

0x603deb10,
0x15ca71be,
0x2b73aef0,
0x857d7781,
0x1f352c07,
0x3b6108d7,
0x2d9810a3,
0x0914dff4,
0x9ba35411,
0x8e6925af,
0xa51a8b5f,
0x2067fcde,
0xa8b09c1a,
0x93d194cd,
0xbe49846e,
0xb75d5b9a,
0xd59aecb8,
0x5bf3c917,
0xfee94248,
0xde8ebe96,
0xb5a9328a,
0x2678a647,
0x98312229,
0x2f6c79b3,
0x812c81ad,
0xdadf48ba,
0x24360af2,
0xfab8b464,
0x98c5bfc9,
0xbebd198e,
0x268c3ba7,
0x09e04214,
0x68007bac,
0xb2df3316,
0x96e939e4,
0x6c518d80,
0xc814e204,
0x76a9fb8a,
0x5025c02d,
0x59c58239,
0xde136967,
0x6ccc5a71,
0xfa256395,
0x9674ee15,
0x5886ca5d,
0x2e2f31d7,
0x7e0af1fa,
0x27cf73c3,
0x749c47ab,
0x18501dda,
0xe2757e4f,
0x7401905a,
0xcafaaae3,
0xe4d59b34,
0x9adf6ace,
0xbd10190d,
0xfe4890d1,
0xe6188d0b,
0x046df344,
0x706c631e

};

const expKey_t referenceCryptKeyAes256_test2[KSCH_AES256_SIZE] __attribute__ ((aligned (4))) = {

0x00010203,
0x04050607,
0x08090a0b,
0x0c0d0e0f,
0x10111213,
0x14151617,
0x18191a1b,
0x1c1d1e1f,
0xa573c29f,
0xa176c498,
0xa97fce93,
0xa572c09c,
0x1651a8cd,
0x0244beda,
0x1a5da4c1,
0x0640bade,
0xae87dff0,
0x0ff11b68,
0xa68ed5fb,
0x03fc1567,
0x6de1f148,
0x6fa54f92,
0x75f8eb53,
0x73b8518d,
0xc656827f,
0xc9a79917,
0x6f294cec,
0x6cd5598b,
0x3de23a75,
0x524775e7,
0x27bf9eb4,
0x5407cf39,
0x0bdc905f,
0xc27b0948,
0xad5245a4,
0xc1871c2f,
0x45f5a660,
0x17b2d387,
0x300d4d33,
0x640a820a,
0x7ccff71c,
0xbeb4fe54,
0x13e6bbf0,
0xd261a7df,
0xf01afafe,
0xe7a82979,
0xd7a5644a,
0xb3afe640,
0x2541fe71,
0x9bf50025,
0x8813bbd5,
0x5a721c0a,
0x4e5a6699,
0xa9f24fe0,
0x7e572baa,
0xcdf8cdea,
0x24fc79cc,
0xbf0979e9,
0x371ac23c,
0x6d68de36

};


const expKey_t referenceDecryptKeyAes256_test2[KSCH_AES256_SIZE] __attribute__ ((aligned (4))) = {

0x00010203,
0x04050607,
0x08090a0b,
0x0c0d0e0f,
0x1a1f181d,
0x1e1b1c19,
0x12171015,
0x16131411,
0x2a2840c9,
0x24234cc0,
0x26244cc5,
0x202748c4,
0x7fd7850f,
0x61cc9916,
0x73db8903,
0x65c89d12,
0x15c668bd,
0x31e5247d,
0x17c168b8,
0x37e6207c,
0xaed55816,
0xcf19c100,
0xbcc24803,
0xd90ad511,
0xde69409a,
0xef8c64e7,
0xf84d0c5f,
0xcfab2c23,
0xf85fc4f3,
0x374605f3,
0x8b844df0,
0x528e98e1,
0x3ca69715,
0xd32af3f2,
0x2b67ffad,
0xe4ccd38e,
0x74da7ba3,
0x439c7e50,
0xc81833a0,
0x9a96ab41,
0xb5708e13,
0x665a7de1,
0x4d3d824c,
0xa9f151c2,
0xc8a30580,
0x8b3f7bd0,
0x43274870,
0xd9b1e331,
0x5e1648eb,
0x384c350a,
0x7571b746,
0xdc80e684,
0x34f1d1ff,
0xbfceaa2f,
0xfce9e25f,
0x2558016e,
0x24fc79cc,
0xbf0979e9,
0x371ac23c,
0x6d68de36

};


/******************************************************************************
 * FIPS-196 specification test patterns
 *****************************************************************************/
/* pattern set one, appendix B  */
aeskey_t aesTest_passPhraseAES256_test1[AES256_KEYSIZE] __attribute__ ((aligned (4)))
= { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 
	0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };


/* pattern set two, appendix C */
aeskey_t aesTest_passPhraseAES256_test2[AES256_KEYSIZE] __attribute__ ((aligned (4)))
= { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

plainData_t plainText_AES256_test2[PLAIN_BSIZE] __attribute__ ((aligned (4))) 
=  { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 ,0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

cryptData_t cryptedText_AES256_test2[CRYPT_BSIZE] __attribute__ ((aligned (4))) 
=  { 0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf, 0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89 };


/* placeholder for results */
cryptData_t cryptResult_AES256[CRYPT_BSIZE] __attribute__ ((aligned (4))) ;
plainData_t deCryptResult_AES256[PLAIN_BSIZE] __attribute__ ((aligned (4))) ;


CypherConfigStr testConfigAes256_test1 = {

	AES_256,
	&aesTest_passPhraseAES256_test1[0],
	&referenceCryptKeyAes256_test1[0],
	0
};

CypherConfigStr testConfigAes256_test2 = {

	AES_256,
	&aesTest_passPhraseAES256_test2[0],
	&referenceCryptKeyAes256_test2[0],
	&cryptedText_AES256_test2[0]
};

CypherConfigStr testConfigAes256_test3 = {

	AES_256,
	&aesTest_passPhraseAES256_test2[0],
	&referenceDecryptKeyAes256_test2[0],
	&cryptedText_AES256_test2[0]
};


#endif

void runFips197Test(void) {

	#ifdef AES128_SUPPORT
	/******************************************************************************
	* first FIPS-197 test according to appendix B, AES128
	*****************************************************************************/
	/* checking is done on the key generation*/
	initCrypt(&testConfigAes128_test1);	

	/* checking is done on the crypted text */
	crypt(&plainText_AES128_test1[0], PLAIN_BSIZE, &cryptResult_AES128[0]);
	
	/******************************************************************************
	* second FIPS-197 test according to appendix C, AES128 
	*****************************************************************************/
	/* checking is done on the key generation*/	
	initCrypt(&testConfigAes128_test2);	
	
	/* checking is done on the crypted text */
	crypt(&plainText_AES128_test2[0], PLAIN_BSIZE, &cryptResult_AES128[0]);

	/* checking is done on the decrypt key */
	initDeCrypt(&testConfigAes128_test3);
	
	/* checking is done on the decrypted text */	
	deCrypt(&cryptResult_AES128[0], CRYPT_BSIZE, &deCryptResult_AES128[0]);
	
	#endif /* AES-128 support */

	
	#ifdef AES192_SUPPORT
	/******************************************************************************
	* first FIPS-197 test according to appendix A, AES192 
	*****************************************************************************/
	/* checking is done on the key generation only */	
	initCrypt(&testConfigAes192_test1);	
	
	/******************************************************************************
	* second FIPS-197 test according to appendix C, AES192 
	*****************************************************************************/
	initCrypt(&testConfigAes192_test2);	

	crypt(&plainText_AES192_test2[0], PLAIN_BSIZE, &cryptResult_AES192[0]);

	initDeCrypt(&testConfigAes192_test3);
	
	deCrypt(&cryptResult_AES192[0], CRYPT_BSIZE, &deCryptResult_AES192[0]);
	
	#endif /* AES-192 support */


	#ifdef AES256_SUPPORT
	/******************************************************************************
	* first FIPS-197 test according to appendix A, AES256 
	*****************************************************************************/
	/* checking is done on the key generation only */	
	initCrypt(&testConfigAes256_test1);	

	/******************************************************************************
	* second FIPS-197 test according to appendix C, AES256 
	*****************************************************************************/
	/* checking is done on the key generation*/	
	initCrypt(&testConfigAes256_test2);	
	
	/* checking is done on the crypted text */
	crypt(&plainText_AES256_test2[0], PLAIN_BSIZE, &cryptResult_AES256[0]);

	/* checking is done on the decrypt key */
	initDeCrypt(&testConfigAes256_test3);
	
	/* checking is done on the decrypted text */	
	deCrypt(&cryptResult_AES256[0], CRYPT_BSIZE, &deCryptResult_AES256[0]);
	
	#endif /* AES-256 support */
			
}



#endif /* #ifdef TEST_MODE */


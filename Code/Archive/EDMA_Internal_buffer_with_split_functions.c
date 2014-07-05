#include <stdio.h>
#include <stdlib.h>
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"

/////////////////////////////////////////////////////////////////
//         DON'T change the following setting for EDMA         //
/////////////////////////////////////////////////////////////////
/* EDMA Registers for 6437*/
#define		PaRAM_OPT	0	// Channel Options Parameter
#define		PaRAM_SRC	1	// Channel Source Address
#define		PaRAM_BCNT	2	// Count for 2nd Dimension (BCNT) | Count for 1st Dimension (ACNT)
#define		PaRAM_DST	3	// Channel Destination Address
#define		PaRAM_BIDX	4	// Destination BCNT Index | Source BCNT Index
#define		PaRAM_RDL	5	// BCNT Reload (BCNTRLD) | Link Address (LINK)
#define		PaRAM_CIDX	6	// Destination CCNT Index | Source CCNT Index
#define		PaRAM_CCNT	7	// Count for 3rd Dimension (CCNT)

/* EDMA Registers for 6437*/
#define		EDMA_IPR	*(volatile int *)0x01C01068	// EDMA Channel interrupt pending low register
#define		EDMA_IPRH	*(volatile int *)0x01C0106C	// EDMA Channel interrupt pending high register
#define		EDMA_IER	*(volatile int *)0x01C01050	// EDMA Channel interrupt enable low register
#define		EDMA_IERH	*(volatile int *)0x01C01054	// EDMA Channel interrupt enable high register
#define		EDMA_ER 	*(volatile int *)0x01C01000	// EDMA Event low register
#define		EDMA_ERH	*(volatile int *)0x01C01004	// EDMA Event high register
#define		EDMA_EER	*(volatile int *)0x01C01020	// EDMA Event enable low register
#define		EDMA_EERH	*(volatile int *)0x01C01024	// EDMA Event enable high register
#define		EDMA_ECR	*(volatile int *)0x01C01008	// EDMA Event clear low register
#define		EDMA_ECRH	*(volatile int *)0x01C0100C	// EDMA Event clear high register
#define		EDMA_ESR	*(volatile int *)0x01C01010	// EDMA Event set low register
#define		EDMA_ESRH	*(volatile int *)0x01C01014	// EDMA Event set high register

/////////////////////////////////////////////////////////////////

extern Int16 video_loopback_test();

// size for buffer_in: 720 * 480 / 2, the reason is explained below. 
#define Pixels 172800

// Resolution 720 * 480 (NTSC mode)
#define vWidth 720
#define vHeight 480

// CAN change the internal blocksize here, the example is 60 * 120
#define INTERNAL_BLK_WIDTH 60
#define INTERNAL_BLK_HEIGHT 120


// Define a space on memory for save the information input and output (Interface data)
Uint32 buffer_out[Pixels]; //from 0x80000000
Uint32 buffer_in[Pixels]; //from 0x800A8C00, which is the same as 4 (bytes for integer) * Pixels

// Declare the internal buffer 
Uint32 internal_buffer_2D[INTERNAL_BLK_HEIGHT][INTERNAL_BLK_WIDTH / 2];


// Define the position of the data (refer to linker.cmd)
// Internal memory L2RAM ".l2ram" 
// External memory DDR2 ".ddr2"
#pragma DATA_SECTION(buffer_out,".ddr2")
#pragma DATA_SECTION(buffer_in,".ddr2")


// buffer_in represents one input frame which consists of two interleaved frames.
// Each 32 bit data has the information for two adjacent pixels in a row.
// Thus, the buffer holds 720/2 integer data points for each row of 2D image and there exist 480 rows.
//
// Format: yCbCr422 ( y1 | Cr | y0 | Cb )
// Each of y1, Cr, y0, Cb has 8 bits
// For each pixel in the frame, it has y, Cb, Cr components
//
// You can generate a lookup table for color conversion if you want to convert to different color space such as RGB.
// Could refer to http://www.fourcc.org/fccyvrgb.php for conversion between yCbCr and RGB
// 

// Using EDMA, copy data from input buffer in the external memory to internal buffer in the internal memory.
void BlockProcessingEDMA_InputBufferToInternalMemory(void){
	int i;
	int blkIdx, blkNum = 0;
	int *Event1;

	// DON'T change the following setting except the source and destination address
	// Event[PaRAM_SRC] is the source data
	// Event[PaRAM_DST] is the destination data
	//
	// Setup a channel for EDMA transfer from External to Internal
	Event1 = (int *)(0x01C04000 + 32 * 9);
	Event1[PaRAM_OPT] = 0x0010000C;
	Event1[PaRAM_SRC] = (int)buffer_in;				// Source address
	Event1[PaRAM_BCNT] = ((INTERNAL_BLK_HEIGHT) << 16) | (INTERNAL_BLK_WIDTH / 2 * 4);
	Event1[PaRAM_DST] = (int)internal_buffer_2D;  	// Destination address
	Event1[PaRAM_BIDX] = ((INTERNAL_BLK_WIDTH / 2 * 4) << 16) | (vWidth / 2 * 4);
	Event1[PaRAM_RDL] = 0x0000FFFF;
	Event1[PaRAM_CIDX] = 0x00000000;
	Event1[PaRAM_CCNT] = 0x00000001;


	/* Block-based processing */
	blkNum = vHeight * vWidth / (INTERNAL_BLK_WIDTH * INTERNAL_BLK_HEIGHT); // Total number of blocks
	// 'blkIdx' indicates the index of block you're processing.
	// The block with zero 'blkIdx' is the block at the top-left corner.
	// The following code retrives the blocks in the same order of raster scan.

	for (i = 0; i<500; i++)
	{
		if (EDMA_IPR & 0x400)
			break; // Waiting for EDMA channel 10 transfer complete
	}

	for (blkIdx = 0; blkIdx < blkNum; ++blkIdx)
	{

		// DON'T change the following setting
		// DMA transfer of a block from external memeory to internal memrory
		Event1[PaRAM_SRC] = (int)(&buffer_in[(blkIdx % (vWidth / INTERNAL_BLK_WIDTH)) * INTERNAL_BLK_WIDTH / 2 + ((int)(blkIdx / (vWidth / INTERNAL_BLK_WIDTH))) * vWidth / 2 * INTERNAL_BLK_HEIGHT]);
		Event1[PaRAM_DST] = (int)(&internal_buffer_2D[(blkIdx % (vWidth / INTERNAL_BLK_WIDTH)) * INTERNAL_BLK_WIDTH / 2 + ((int)(blkIdx / (vWidth / INTERNAL_BLK_WIDTH))) * vWidth / 2 * INTERNAL_BLK_HEIGHT]);

		for (i = 0; i<50; i++)
		{
		 if (EDMA_IPR & 0x200==0)
			 break; // Waiting for EDMA channel 9 transfer complete
		}
		EDMA_IPR = 0x200;             // Clear CIP9
		EDMA_ESR = EDMA_ESR | 0x200;    // Start channel 9 EDMA transfer
		
		
	}
}

//Using EDMA, copy data from the internal memory to the external memory's output buffer
void BlockProcessingEDMA_InternalMemoryToExternalOutputBuffer(void){
	int i;
	int blkIdx, blkNum = 0;
	int *Event2;

	// DON'T change the following setting except the source and destination address
	// Event[PaRAM_SRC] is the source data
	// Event[PaRAM_DST] is the destination data
	//

	// Setup a channel for EDMA transfer from Internal to External
	Event2 = (int *)(0x01C04000 + 32 * 10);
	Event2[PaRAM_OPT] = 0x0010000C;
	Event2[PaRAM_SRC] = (int)internal_buffer_2D;	// Source address
	Event2[PaRAM_BCNT] = ((INTERNAL_BLK_HEIGHT) << 16) | (INTERNAL_BLK_WIDTH / 2 * 4);
	Event2[PaRAM_DST] = (int)buffer_out;  			// Destination address
	Event2[PaRAM_BIDX] = ((vWidth / 2 * 4) << 16) | (INTERNAL_BLK_WIDTH / 2 * 4);
	Event2[PaRAM_RDL] = 0x0000FFFF;
	Event2[PaRAM_CIDX] = 0x00000000;
	Event2[PaRAM_CCNT] = 0x00000001;

	/* Block-based processing */
	blkNum = vHeight * vWidth / (INTERNAL_BLK_WIDTH * INTERNAL_BLK_HEIGHT); // Total number of blocks
	// 'blkIdx' indicates the index of block you're processing.
	// The block with zero 'blkIdx' is the block at the top-left corner.
	// The following code retrives the blocks in the same order of raster scan.

	for (i = 0; i<500; i++)
	{
		if (EDMA_IPR & 0x200) break;
	}// Waiting for EDMA channel 9 transfer complete

	for (blkIdx = 0; blkIdx < blkNum; ++blkIdx)
	{

		// DON'T change the following setting
		// DMA transfer of a block from internal memeory to external memrory
		Event2[PaRAM_SRC] = (int)(&internal_buffer_2D[(blkIdx % (vWidth / INTERNAL_BLK_WIDTH)) * INTERNAL_BLK_WIDTH / 2 + ((int)(blkIdx / (vWidth / INTERNAL_BLK_WIDTH))) * vWidth / 2 * INTERNAL_BLK_HEIGHT]);
		Event2[PaRAM_DST] = (int)(&buffer_out[(blkIdx % (vWidth / INTERNAL_BLK_WIDTH)) * INTERNAL_BLK_WIDTH / 2 + ((int)(blkIdx / (vWidth / INTERNAL_BLK_WIDTH))) * vWidth / 2 * INTERNAL_BLK_HEIGHT]);
		int cnt = 0;
		for (i = 0; i<50; i++)
		{
		 if (EDMA_IPR & 0x400==0)
			 break;
		} // Waiting for EDMA channel 10 transfer complete

		EDMA_IPR = 0x400;              // Clear CIP10
		EDMA_ESR = EDMA_ESR | 0x400;    // Start channel 10 EDMA transfer
		
	}
}

// Using EDMA, copy data from input buffer in the external memory to internal buffer in the internal memory.
//and then internal memory to external memory - all using block transfer
void BlockProcessingEDMA(void){
	int i;
	int blkIdx, blkNum = 0;
	int *Event1, *Event2;

	// DON'T change the following setting except the source and destination address
	// Event[PaRAM_SRC] is the source data
	// Event[PaRAM_DST] is the destination data
	//
	// Setup a channel for EDMA transfer from External to Internal
	Event1 = (int *)(0x01C04000 + 32 * 9);
	Event1[PaRAM_OPT] = 0x0010000C;
	Event1[PaRAM_SRC] = (int)buffer_in;				// Source address
	Event1[PaRAM_BCNT] = ((INTERNAL_BLK_HEIGHT) << 16) | (INTERNAL_BLK_WIDTH / 2 * 4);
	Event1[PaRAM_DST] = (int)internal_buffer_2D;  	// Destination address
	Event1[PaRAM_BIDX] = ((INTERNAL_BLK_WIDTH / 2 * 4) << 16) | (vWidth / 2 * 4);
	Event1[PaRAM_RDL] = 0x0000FFFF;
	Event1[PaRAM_CIDX] = 0x00000000;
	Event1[PaRAM_CCNT] = 0x00000001;

	// Setup a channel for EDMA transfer from Internal to External
	Event2 = (int *)(0x01C04000 + 32 * 10);
	Event2[PaRAM_OPT] = 0x0010000C;
	Event2[PaRAM_SRC] = (int)internal_buffer_2D;	// Source address
	Event2[PaRAM_BCNT] = ((INTERNAL_BLK_HEIGHT) << 16) | (INTERNAL_BLK_WIDTH / 2 * 4);
	Event2[PaRAM_DST] = (int)buffer_out;  			// Destination address
	Event2[PaRAM_BIDX] = ((vWidth / 2 * 4) << 16) | (INTERNAL_BLK_WIDTH / 2 * 4);
	Event2[PaRAM_RDL] = 0x0000FFFF;
	Event2[PaRAM_CIDX] = 0x00000000;
	Event2[PaRAM_CCNT] = 0x00000001;

	/* Block-based processing */
	blkNum = vHeight * vWidth / (INTERNAL_BLK_WIDTH * INTERNAL_BLK_HEIGHT); // Total number of blocks
	// 'blkIdx' indicates the index of block you're processing.
	// The block with zero 'blkIdx' is the block at the top-left corner.
	// The following code retrives the blocks in the same order of raster scan.  
	for (blkIdx = 0; blkIdx < blkNum; ++blkIdx)
	{

		// DON'T change the following setting
		// DMA transfer of a block from external memeory to internal memrory
		Event1[PaRAM_SRC] = (int)(&buffer_in[(blkIdx % (vWidth / INTERNAL_BLK_WIDTH)) * INTERNAL_BLK_WIDTH / 2 + ((int)(blkIdx / (vWidth / INTERNAL_BLK_WIDTH))) * vWidth / 2 * INTERNAL_BLK_HEIGHT]);
		for (i = 0; i<500; i++)
		if (EDMA_IPR & 0x400 == 0) break; // Waiting for EDMA channel 10 transfer complete		
		EDMA_IPR = 0x200;             // Clear CIP9
		EDMA_ESR = EDMA_ESR | 0x200;    // Start channel 9 EDMA transfer

		// Do your processing based on the blocks in internal memory
		// CAN change the size of blocks (check the DEFINE statement at the beginning of this file)
		//


		// DON'T change the following setting
		// DMA transfer of a block from internal memeory to external memrory
		Event2[PaRAM_DST] = (int)(&buffer_out[(blkIdx % (vWidth / INTERNAL_BLK_WIDTH)) * INTERNAL_BLK_WIDTH / 2 + ((int)(blkIdx / (vWidth / INTERNAL_BLK_WIDTH))) * vWidth / 2 * INTERNAL_BLK_HEIGHT]);
		for (i = 0; i<500; i++)
		if (EDMA_IPR & 0x200 == 0) break; // Waiting for EDMA channel 9 transfer complete		

		EDMA_IPR = 0x400;              // Clear CIP10
		EDMA_ESR = EDMA_ESR | 0x400;    // Start channel 10 EDMA transfer
	}
}

// Flip vertically the image
// Same as that in the first template
void Inv(void){
	int i, j;
	unsigned int temp;

	i = 0;
	j = Pixels - Pixels / vHeight;

	do{
		temp = buffer_in[j];
		buffer_out[i] = temp;
		i++;
		j++;

		if (i % (Pixels / vHeight) == 0){ j = Pixels - i - Pixels / vHeight; }
	} while (i < Pixels);
}

void main(void)
{
	//Int16 dip0, dip1, dip2, dip3;

	/* Initialize BSL */
	EVMDM6437_init();

	/* Initialize the DIP Switches & LEDs if needed */
	EVMDM6437_DIP_init();
	EVMDM6437_LED_init();

	video_loopback_test();
	int i = 0, j=0;
	while (1){

		/* Will return DIP_DOWN or DIP_UP */
		/*dip0 = EVMDM6437_DIP_get(DIP_0);
		dip1 = EVMDM6437_DIP_get(DIP_1);
		dip2 = EVMDM6437_DIP_get(DIP_2);
		dip3 = EVMDM6437_DIP_get(DIP_3);*/

		// Run different procedures depending on the DIP switches pressed.

		//BlockProcessingEDMA();
		BlockProcessingEDMA_InputBufferToInternalMemory();
		for(i=0; i<vHeight; ++i)
				for (j=0; j<vWidth/2; ++j) {
					//internal_buffer_2D[i * vWidth/2 + j] = buffer_in[i * vWidth/2 + j];
					if ( i >= 220 && i < 260 && j >= 170 && j < 190 )
					{
						internal_buffer_2D[i][j] = 0;
					}
				}
		BlockProcessingEDMA_InternalMemoryToExternalOutputBuffer();
		
		/*else if ( dip2 == DIP_DOWN )
		Copy();
		else if ( dip3 == DIP_DOWN )
		Inv();*/
	}
}

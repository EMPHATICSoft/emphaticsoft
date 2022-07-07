#ifndef _SSDCONSTS_
#define _SSDCONSTS_

#define CBC_EVENT_SIZE_32 9			// 9 32bit words per CBC
#define EVENT_HEADER_TDC_SIZE_32 6	// total of 6 32 bit words for HEADER + TDC
#define EVENT_HEADER_SIZE_32 5		// 5 words for the header
#define OFFSET_CBCSTUBDATA 264 + 23 //LAST BIT
    //#define IC_OFFSET_CBCSTUBDATA     276  //BIT 12
#define WIDTH_CBCSTUBDATA 12
#define OFFSET_CBCDATA 2 + 8 //OFFSET_PIPELINE_ADDRESS + WIDTH_PIPELINE_ADDRESS
#define WIDTH_CBCDATA 254	 //NCHANNELS

#endif

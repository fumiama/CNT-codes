#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lnklib.h"

void save_send_buf(U8* buf, size_t size) {
	is_waiting = 1;
	memcpy(prev_send_buf, buf, size);
	prev_send_bufsz = size;
}

void check_recv(U8* buf, size_t len, int is_bit_arr) {
	unsigned int head_len = is_bit_arr ? FRAME_HEAD_LEN : (FRAME_HEAD_LEN / 8);;
	unsigned int info_len = is_bit_arr ? FRAME_INFO_LEN : (FRAME_INFO_LEN / 8);;
	switch (unpack_frame(buf, len, is_bit_arr)) {
	case IS_ACK_FRAME:
		go_next_frame();
		break;
	case IS_DAT_FRAME:
		if (is_bit_arr) {
			size_t sz = len / 8 + 1;
			U8* tmp = (U8*)malloc(sz);
			BitArrayToByteArray(buf, len, tmp, sz);
			SendtoUpper(&tmp[FRAME_HEAD_LEN / 8], sz - FRAME_INFO_LEN / 8);
		} else SendtoUpper(&buf[head_len], len - info_len);
		break;
	case IS_ERR_FRAME:
		puts("ÒÑ¶ªÆú´íÎóÖ¡");
		break;
	default:
		break;
	}
}
void go_next_frame() {
	is_waiting = 0;
	prev_send_bufsz = 0;
}

int unpack_frame(U8* buf, size_t len, int is_bit_arr) {
	return IS_ERR_FRAME;
}

U8* pack_frame(U8* buf, size_t len, int is_bit_arr) {
	return NULL;
}
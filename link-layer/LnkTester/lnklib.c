#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lnklib.h"

void save_send_buf(U8* buf, size_t size) {
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
		SendtoUpper(&buf[head_len], len - info_len);
		break;
	case IS_ERR_FRAME:
		puts("ÒÑ¶ªÆú´íÎóÖ¡");
		break;
	default:
		break;
	}
}
static int dnxhd_find_frame_end(DNXHDParserContext *dctx,
const uint8_t *buf, int buf_size)
{
ParseContext *pc = &dctx->pc;
uint64_t state = pc->state64;
int pic_found = pc->frame_start_found;
int i = 0;

if (!pic_found) {
for (i = 0; i < buf_size; i++) {
state = (state << 8) | buf[i];
if (ff_dnxhd_check_header_prefix(state & 0xffffffffff00LL) != 0) {
i++;
pic_found = 1;
dctx->cur_byte = 0;
dctx->remaining = 0;
break;
}
}
}

if (pic_found && !dctx->remaining) {
if (!buf_size) /* EOF considered as end of frame */
return 0;
for (; i < buf_size; i++) {
dctx->cur_byte++;
state = (state << 8) | buf[i];

if (dctx->cur_byte == 24) {
dctx->h = (state >> 32) & 0xFFFF;
} else if (dctx->cur_byte == 26) {
dctx->w = (state >> 32) & 0xFFFF;
} else if (dctx->cur_byte == 42) {
int cid = (state >> 32) & 0xFFFFFFFF;

if (cid <= 0)
continue;

                dctx->remaining = avpriv_dnxhd_get_frame_size(cid);
                if (dctx->remaining <= 0) {
                    dctx->remaining = dnxhd_get_hr_frame_size(cid, dctx->w, dctx->h);
                    if (dctx->remaining <= 0)
                        return dctx->remaining;
}
if (buf_size - i + 47 >= dctx->remaining) {
int remaining = dctx->remaining;

pc->frame_start_found = 0;
pc->state64 = -1;
dctx->cur_byte = 0;
dctx->remaining = 0;
return remaining;
} else {
dctx->remaining -= buf_size;
}
}
}
} else if (pic_found) {
if (dctx->remaining > buf_size) {
dctx->remaining -= buf_size;
} else {
int remaining = dctx->remaining;

pc->frame_start_found = 0;
pc->state64 = -1;
dctx->cur_byte = 0;
dctx->remaining = 0;
return remaining;
}
}
pc->frame_start_found = pic_found;
pc->state64 = state;
return END_NOT_FOUND;
}

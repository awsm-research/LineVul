uint64_t esp_reg_read(ESPState *s, uint32_t saddr)
{
uint32_t old_val;

trace_esp_mem_readb(saddr, s->rregs[saddr]);
switch (saddr) {
case ESP_FIFO:
        if (s->ti_size > 0) {
s->ti_size--;
            if ((s->rregs[ESP_RSTAT] & STAT_PIO_MASK) == 0) {
                /* Data out.  *
                qemu_log_mask(LOG_UNIMP,
                              "esp: PIO data read not implemented\n");
                s->rregs[ESP_FIFO] = 0;
            } else {
                s->rregs[ESP_FIFO] = s->ti_buf[s->ti_rptr++];
            }
esp_raise_irq(s);
}
        if (s->ti_size == 0) {
s->ti_rptr = 0;
s->ti_wptr = 0;
}
s->ti_wptr = 0;
}
break;
case ESP_RINTR:
/* Clear sequence step, interrupt register and all status bits
except TC */
old_val = s->rregs[ESP_RINTR];
s->rregs[ESP_RINTR] = 0;
s->rregs[ESP_RSTAT] &= ~STAT_TC;
s->rregs[ESP_RSEQ] = SEQ_CD;
esp_lower_irq(s);

return old_val;
case ESP_TCHI:
/* Return the unique id if the value has never been written */
if (!s->tchi_written) {
return s->chip_id;
}
default:
break;
}

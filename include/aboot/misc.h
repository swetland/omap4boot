void sr32(u32 addr, u32 start_bit, u32 num_bits, u32 value);
u32 wait_on_value(u32 read_bit_mask, u32 match_value, u32 read_addr, u32 bound);
void sdelay(unsigned long loops);


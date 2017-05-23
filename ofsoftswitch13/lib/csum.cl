__kernel void recalcCsum16(__global uint16_t * old_csum, __global uint16_t * old_u16, __global uint16_t * new_u16, const uint16_t hc_prime_complement)
{
	uint16_t hc_complement = ~old_csum;
	uint16_t m_complement = ~old_u16;
	uint16_t m_prime = new_u16;
	uint32_t sum = hc_complement + m_complement + m_prime;
	uint16_t hc_prime_comp = sum + (sum >> 16);
	hc_prime_complement = ~hc_prime_comp;
}

__kernel void createOldValue(__global uint8_t  * ipv4ihlver, __global uint8_t * ipv4tos, const uint16_t old_val)
{
	old_val = htons((ipv4ihlver << 8) + ipv4tos);
}

__kernel void createNewValue(__global uint8_t  * ipv4ihlver, __global uint8_t * tos, const uint16_t new_val)
{
	new_val = htons((ipv4ihlver << 8) + tos);
}

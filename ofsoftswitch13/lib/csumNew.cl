/*typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
*/
__kernel void negateOldCsum(__global ushort* old_csum, __global ushort* hc_complement)
{
	hc_complement[0] = ~old_csum[0];
}

__kernel void negateOldValue(__global ushort* old_u16, __global ushort* m_complement)
{
	m_complement[0] = ~old_u16[0];
}

__kernel void computeNewCsum16(__global ushort* hc_complement, __global ushort* m_complement, __global ushort* new_u16, __global ushort* hc_prime_complement)
{
	uint sum = hc_complement[0]+m_complement[0]+new_u16[0];
	ushort hc_prime_comp = sum+(sum>>16);
	hc_prime_complement[0] = ~hc_prime_comp;
}

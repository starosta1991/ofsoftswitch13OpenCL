/* Copyright (c) 2008 The Board of Trustees of The Leland Stanford
 * Junior University
 *
 * We are making the OpenFlow specification and associated documentation
 * (Software) available for public use and benefit with the expectation
 * that others will use, modify and enhance the Software and contribute
 * those enhancements back to the community. However, since we would
 * like to make the Software available for broadest use, with as few
 * restrictions as possible permission is hereby granted, free of
 * charge, to any person obtaining a copy of this Software to deal in
 * the Software under the copyrights without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * The name and trademarks of copyright holder(s) may NOT be used in
 * advertising or publicity pertaining to the Software or any
 * derivatives without specific, written prior permission.
 */

//added by michael DP start
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
/*errno and strerror*/
#include <errno.h>
#include <string.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
//#include "check_opencl.h"
#define MAX_SOURCE_SIZE (0x100000)
//#define val_size 10000
//added by michael DP end
#include "udatapath/datapath.h"
#include <config.h>
#include "csum.h"
#include "oflib/ofl.h"
#include "oflib/ofl-actions.h"
#include "oflib/ofl-log.h"

#define LOG_MODULE VLM_dp_acts

//static struct vlog_rate_limit rl = VLOG_RATE_LIMIT_INIT(60, 60);


/* Returns the IP checksum of the 'n' bytes in 'data'. */
uint16_t
csum(const void *data, size_t n)
{
    return csum_finish(csum_continue(0, data, n));
}

/* Adds the 16 bits in 'new' to the partial IP checksum 'partial' and returns
 * the updated checksum.  (To start a new checksum, pass 0 for 'partial'.  To
 * obtain the finished checksum, pass the return value to csum_finish().) */
uint32_t
csum_add16(uint32_t partial, uint16_t new)
{
    return partial + new;
}

/* Adds the 32 bits in 'new' to the partial IP checksum 'partial' and returns
 * the updated checksum.  (To start a new checksum, pass 0 for 'partial'.  To
 * obtain the finished checksum, pass the return value to csum_finish().) */
uint32_t
csum_add32(uint32_t partial, uint32_t new)
{
    return partial + (new >> 16) + (new & 0xffff);
}


/* Adds the 'n' bytes in 'data' to the partial IP checksum 'partial' and
 * returns the updated checksum.  (To start a new checksum, pass 0 for
 * 'partial'.  To obtain the finished checksum, pass the return value to
 * csum_finish().) */
uint32_t
csum_continue(uint32_t partial, const void *data_, size_t n)
{
    const uint16_t *data = data_;

    for (; n > 1; n -= 2) {
        partial = csum_add16(partial, *data++);
    }
    if (n) {
        partial += *(uint8_t *) data;
    }
    return partial;
}

/* Returns the IP checksum corresponding to 'partial', which is a value updated
 * by some combination of csum_add16(), csum_add32(), and csum_continue(). */
uint16_t
csum_finish(uint32_t partial)
{
    return ~((partial & 0xffff) + (partial >> 16));
}

/* Returns the new checksum for a packet in which the checksum field previously
 * contained 'old_csum' and in which a field that contained 'old_u16' was
 * changed to contain 'new_u16'. */
//uint16_t
//recalc_csumcl16(uint16_t old_csum, uint16_t old_u16, uint16_t new_u16)
//{	

    /* Ones-complement arithmetic is endian-independent, so this code does not
     * use htons() or ntohs().
     *
     * See RFC 1624 for formula and explanation. 
	
     cl_ushort hc_complement;
     cl_ushort m_complement;
     cl_ushort hc_prime_complement;
     cl_program program = NULL;
     cl_kernel kernel[3] = {NULL, NULL, NULL};
     cl_mem old_csumMem = NULL;
     cl_mem old_u16Mem = NULL;
     cl_mem new_u16Mem = NULL;
     cl_mem hc_complementMem = NULL;
     cl_mem m_complementMem = NULL;
     cl_mem hc_prime_complementMem = NULL;
     cl_event events[2];

                 
     //create memory buffers for each argument
     old_csumMem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_ushort), NULL, &ret);
     old_u16Mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_ushort), NULL, &ret);
     new_u16Mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_ushort), NULL, &ret);
     hc_complementMem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_ushort), NULL, &ret);
     m_complementMem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_ushort), NULL, &ret);
     hc_prime_complementMem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_ushort), NULL, &ret);

     //copy the all lists to their respective memory buffers (input data)
     ret = clEnqueueWriteBuffer(command_queue, old_csumMem, CL_TRUE, 0, sizeof(cl_ushort), (void *)&old_csum, 0, NULL, NULL);
     ret = clEnqueueWriteBuffer(command_queue, old_u16Mem, CL_TRUE, 0, sizeof(cl_ushort), (void *)&old_u16, 0, NULL, NULL);
     ret = clEnqueueWriteBuffer(command_queue, hc_complementMem, CL_TRUE, 0, sizeof(cl_ushort), (void *)&hc_complement, 0, NULL, NULL);
     ret = clEnqueueWriteBuffer(command_queue, m_complementMem, CL_TRUE, 0, sizeof(cl_ushort), (void *)&m_complement, 0, NULL, NULL);
     ret = clEnqueueWriteBuffer(command_queue, new_u16Mem, CL_TRUE, 0, sizeof(cl_ushort), (void *)&new_u16, 0, NULL, NULL);
     
     //create kernel program from source
     program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
            //free(source_str);//do not uncomment

     //build program
     ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

     //create task parallel openCL kernels
     kernel[0] = clCreateKernel(program, "negateOldCsum", &ret);
     kernel[1] = clCreateKernel(program, "negateOldValue", &ret);
     kernel[2] = clCreateKernel(program, "computeNewCsum16", &ret);

     //openCL kernel arguments setting
     ret = clSetKernelArg(kernel[0], 0, sizeof(cl_ushort), (void *)&old_csumMem);
     ret = clSetKernelArg(kernel[0], 1, sizeof(cl_ushort), (void *)&hc_complementMem);
     ret = clSetKernelArg(kernel[1], 0, sizeof(cl_ushort), (void *)&old_u16Mem);
     ret = clSetKernelArg(kernel[1], 1, sizeof(cl_ushort), (void *)&m_complementMem);
     ret = clSetKernelArg(kernel[2], 0, sizeof(cl_ushort), (void *)&hc_complementMem);
     ret = clSetKernelArg(kernel[2], 1, sizeof(cl_ushort), (void *)&m_complementMem);
     ret = clSetKernelArg(kernel[2], 2, sizeof(cl_ushort), (void *)&new_u16Mem);
     ret = clSetKernelArg(kernel[2], 3, sizeof(cl_ushort), (void *)&hc_prime_complementMem);

     //execute them as task parallel
     ret = clEnqueueTask(command_queue, kernel[0], 0, NULL, &events[0]);
     ret = clEnqueueTask(command_queue, kernel[1], 0, NULL, &events[1]);
     ret = clEnqueueTask(command_queue, kernel[2], 2, events, NULL);

     //copy results to the host
     ret = clEnqueueReadBuffer(command_queue, hc_prime_complementMem, CL_TRUE, 0, sizeof(cl_ushort), (void *)&hc_prime_complement, 0, NULL, NULL);

     //finalization
     ret = clFlush(command_queue);
     ret = clFinish(command_queue);
//VLOG_WARN_RL(LOG_MODULE, &rl, "Dostal som sa nakoniec v recalc csum '%hu'", hc_prime_complement);

     return hc_prime_complement;*/	
//}

uint16_t
recalc_csum16(uint16_t old_csum, uint16_t old_u16, uint16_t new_u16)
{
    /* Ones-complement arithmetic is endian-independent, so this code does not
     * use htons() or ntohs().
     *
     * See RFC 1624 for formula and explanation. */
    uint16_t hc_complement = ~old_csum;
    uint16_t m_complement = ~old_u16;
    uint16_t m_prime = new_u16;
    uint32_t sum = hc_complement + m_complement + m_prime;
    uint16_t hc_prime_complement = sum + (sum >> 16);
    return ~hc_prime_complement;
}

/* Returns the new checksum for a packet in which the checksum field previously
 * contained 'old_csum' and in which a field that contained 'old_u32' was
 * changed to contain 'new_u32'. */
uint16_t
recalc_csum32(uint16_t old_csum, uint32_t old_u32, uint32_t new_u32)
{
    return recalc_csum16(recalc_csum16(old_csum, old_u32, new_u32),
                         old_u32 >> 16, new_u32 >> 16);
}

/* Returns the new checksum for a packet in which the checksum field previously
 * contained 'old_csum' and in which a field that contained 'old_u64' was
 * changed to contain 'new_u64'. */
uint16_t
recalc_csum64(uint16_t old_csum, uint64_t old_u64, uint64_t new_u64)
{
    return recalc_csum32(recalc_csum32(old_csum, old_u64, new_u64),
                         old_u64 >> 32, new_u64 >> 32);
}


/* Returns the new checksum for a packet in which the checksum field previously
 * contained 'old_csum' and in which a field that contained 'old_u32' was
 * changed to contain 'new_u12'. */
uint16_t
recalc_csum128(uint16_t old_csum, uint8_t old_u128[16], uint8_t new_u128[16])
{
    uint64_t old_left, old_right;
    uint64_t new_left, new_right;
    old_right = *((uint64_t*) ( old_u128 + 8 ));
    new_right = *((uint64_t*) ( new_u128 + 8 ));
    old_left = *((uint64_t*) ( old_u128 ));
    new_left = *((uint64_t*) ( new_u128 ));
    return recalc_csum64(recalc_csum64(old_csum, old_right, new_right),
                         old_left, new_left);
}

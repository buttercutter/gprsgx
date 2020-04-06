/*
 * Copyright (C) 2011-2018 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */

#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

#include "thread_data.h"
#include "arch.h"

//extern "C" void* get_thread_data();

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */

void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}

void printf_helloworld(int flag)
{
    printf("xxxHello World\n");

	// https://web.cse.ohio-state.edu/~lin.3021/file/RAID17.pdf#page=10
	// %gs:0x20 always points to the GPRSGX region

	// Layout of GPRSGX Portion of the State Save Area :
	// https://www.intel.com/content/dam/www/public/emea/xe/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3d-part-4-manual.pdf#page=15

    uintptr_t offset = 144; // 32 bytes + 144 bytes = 0x20 + 0x90 = 0xB0
    long long ursp;

	//asm ("mov %%gs:%1, %0" : "=r"(ursp) : "m"(*(char *)offset)); // 2338810765860929536
    //asm ("mov %%gs:(%1), %0" : "=r"(ursp) : "r"(offset)); // -4290487885628440576
	//asm ("mov %%gs:0xB0, %0" : "=r"(ursp)); // 0
	asm ("mov %%gs:0x28 , %0" : "=r"(ursp)); // 140306321915720
/*
	printf("divisor = %d\n", divisor);

	if(flag == 0) divisor = 0;

//	else divisor = 1;

	dividend = 10;

	printf("Before divison operation\n");

	printf("divisor = %d\n", divisor);
	printf("dividend = %d\n", dividend);

	try {
		if(divisor == 0) throw "Division by zero condition!\n";
		
		else 
		{
			quotient = dividend / divisor;
			printf("inside the else\n");
			printf("ursp = %p\n", ursp);
			printf("quotient = %d\n", quotient);
		}
	} 
	catch (const char* msg) {

		printf("Inside catch()\n");			

		printf("ursp = %p\n", ursp);

		printf("divisor = %d\n", divisor);

		divisor = 1; // test the value persistence of enclave variables after enclave re-entry
		
		printf(msg);

		printf("After divison operation\n");
		printf("divisor = %d\n", divisor);
	}

	printf("Outside of throw-catch exception block\n");
	printf("divisor = %d\n", divisor);
*/
	printf("ursp = %p\n", ursp);

    // read the outside stack address from current SSA
    thread_data_t *thread_data = get_thread_data();

	printf("thread_data = %p\n", thread_data);

	long long stack_guard;
	printf("stack_guard = %p\n", thread_data->stack_guard);

    ssa_gpr_t *ssa_gpr = reinterpret_cast<ssa_gpr_t *>(thread_data->first_ssa_gpr);

	// https://github.com/intel/linux-sgx/blob/master/common/inc/internal/arch.h#L129-L155
    size_t rax_addr = ssa_gpr->REG(ax); // RAX
	size_t ursp_addr = ssa_gpr->REG(sp_u); // URSP
	size_t urbp_addr = ssa_gpr->REG(bp_u); // URBP

	// rax_addr and ursp_addr do not match the value of %%gs:0x20 and host's ESP respectively
	printf("using API call, start of GPRSGX region = %p\n", rax_addr); 
	printf("using API call, URSP of GPRSGX region = %p\n", ursp_addr);
	printf("using API call, URBP of GPRSGX region = %p\n", urbp_addr);

	//printf("using API call, start of GPRSGX region = %lld\n", *(long long*) (p+0x20));
}

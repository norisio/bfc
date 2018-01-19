#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stack>
#include <string>

namespace put{
void open_main(std::ostream& ostr){
	ostr << R"( .text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
)";
}
void close_main(std::ostream& ostr){
	ostr << R"(
	movq $0, %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.5) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
)";
}


void alloc(std::ostream& ostr){
	ostr << R"(
	movq $30000, %rdi
	movq $12, %rax
	syscall
	movq %rax, %r15
)";
}

void incptr(std::ostream& ostr){
	ostr << R"(	incq %r15
)";
}
void decptr(std::ostream& ostr){
	ostr << R"(	decq %r15
)";
}
void incval(std::ostream& ostr){
	ostr << R"(	incq (%r15)
)";
}
void decval(std::ostream& ostr){
	ostr << R"(	decq (%r15)
)";
}
void putchar(std::ostream& ostr){
	ostr << R"(	movq $1, %rax
	movq $1, %rdi
	movq (%r15), %rsi
	movq $1, %rdx	
	syscall
)";
}
void getchar(std::ostream& ostr){
	ostr << R"(	movq $0, %rax	
	movq %r15, %rdi 
	movq $1, %rsi	
	syscall
)";
}
std::stack<int> loop_stack;
void open_loop(std::ostream& ostr){
	static int loop_serial_num = 0;
	ostr << R"(	cmpq $0, (%r15)
	je loop_close_)" << loop_serial_num << R"(
loop_open_)" << loop_serial_num << R"(:
)";
	loop_stack.push(loop_serial_num);
	loop_serial_num ++;
}
void close_loop(std::ostream& ostr){
	int const loop_serial_num = loop_stack.top();
	loop_stack.pop();
	ostr << R"(	cmpq $0, (%r15)
	jne loop_open_)" << loop_serial_num << R"(
loop_close_)" << loop_serial_num << R"(:
)";
}
} //namespace put
/*
 * r15: 配列へのポインタ
 */
int main(int argc, char** argv){
	if(argc <= 1 || argc >= 3){
		std::cerr << "No sufficient args" << std::endl;
		exit(-1);
	}
	std::unordered_map<char, void(*)(std::ostream&)> funcs;
	funcs['>'] = put::incptr;
	funcs['<'] = put::decptr;
	funcs['+'] = put::incval;
	funcs['-'] = put::decval;
	funcs['.'] = put::putchar;
	funcs[','] = put::getchar;
	funcs['['] = put::open_loop;
	funcs[']'] = put::close_loop;


	std::ofstream ofs("out.s", std::ios::out);
	put::open_main(ofs);
	put::alloc(ofs);
	while(*argv[1] != '\0'){
		(funcs[*argv[1]])(ofs);
		argv[1]++;
	}
	put::close_main(ofs);
	return 0;
}


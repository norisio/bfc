#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stack>
#include <string>

namespace put{
void open_main(std::ostream& ostr){
	ostr << R"( .text
	.globl	_start
_start:
)";
}
void close_main(std::ostream& ostr){
	ostr << R"(# exit syscall
	movq $60, %rax
	movq $0, %rdi
	syscall
)";
}


void alloc(std::ostream& ostr){
	ostr << R"(
	movq $12, %rax	# brk() syscall
	movq $0, %rdi	# to get current break point
	syscall
	movq %rax, %r15	# save the current break point
	movq $12, %rax 	# brk() syscall
	movq %r15, %rdi
	addq $30000, %rdi
	syscall
)";
}
void clear_array(std::ostream& ostr){
	ostr << R"(
	movq %r15, %r14		#obtain the first pointer
	movl $30000, %ecx	#num of loop
clear_element:
	movb $0, (%r14)
	incq %r14
	loop clear_element
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
	ostr << R"(	incb (%r15)
)";
}
void decval(std::ostream& ostr){
	ostr << R"(	decb (%r15)
)";
}
void putchar(std::ostream& ostr){
	ostr << R"(	movq $1, %rax
	movq $1, %rdi
	movq %r15, %rsi
	movq $1, %rdx	
	syscall
)";
}
void getchar(std::ostream& ostr){
	ostr << R"(	movq $0, %rax	
	movq $0, %rdi
	movq %r15, %rsi 
	movq $1, %rdx	
	syscall
)";
}
std::stack<int> loop_stack;
void open_loop(std::ostream& ostr){
	static int loop_serial_num = 0;
	ostr << R"(	cmpb $0, (%r15)
	je loop_close_)" << loop_serial_num << R"(
loop_open_)" << loop_serial_num << R"(:
)";
	loop_stack.push(loop_serial_num);
	loop_serial_num ++;
}
void close_loop(std::ostream& ostr){
	int const loop_serial_num = loop_stack.top();
	loop_stack.pop();
	ostr << R"(	cmpb $0, (%r15)
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
	put::clear_array(ofs);
	while(*argv[1] != '\0'){
		(funcs[*argv[1]])(ofs);
		argv[1]++;
	}
	put::close_main(ofs);
	return 0;
}


#include "tuple.h"
#include <iostream>

using namespace zstl;

int main(){
	Tuple<std::string, std::string> t1("1", "2");
	std::cout << t1 << std::endl;
	
	Tuple<std::string, std::string> t3;
	auto t2 = STL_MOVE(t1);	
	t3 = t2;	
	std::cout << t1 << std::endl;
	std::cout << t2 << std::endl;


}

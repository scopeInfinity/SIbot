#include<iostream>
#include<cstdlib>
#include<time.h>
int main(int argc, char const *argv[])
{
	
	int a;
	std::cin>>a;
	for(int i=0;i<10;i++)
		std::cout<<((a&(1<<i))!=0)<<' ';
	std::cout<<std::endl;
		return 0;
}


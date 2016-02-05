#include<iostream>
#include<cstdlib>
#include<time.h>
int getRandom()
{
	return (int)(rand()*1.0/RAND_MAX*(1<<8));
}
int main(int argc, char const *argv[])
{
	srand(time(NULL)*clock());
	int a=getRandom(),x;
	for(int i=0;i<8;i++)
		std::cout<<((a&(1<<i))!=0)<<' ';
	std::cout<<std::endl;
	for(int i=0;i<8;i++)
		std::cout<<((a&(1<<i))!=0)<<' ';
	std::cout<<std::endl;
	return 0;
}


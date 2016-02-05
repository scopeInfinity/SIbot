#include<iostream>
#include<cstdlib>
#include<time.h>
int getRandom()
{
	return rand()*1.0/RAND_MAX*5;
}
int main(int argc, char const *argv[])
{
	srand(time(NULL)*clock());
	int a=getRandom(),b=getRandom();
	std::cout<<a<<' '<<b<<std::endl;
	for(int i=0;i<10;i++)
	if(a+b>i)
		std::cout<<"1 ";
	else
		std::cout<<"0 ";
	return 0;
}


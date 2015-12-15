#include<iostream>
#include<cstdlib>
#include<time.h>
int getRandom()
{
	if(rand()*1.0/RAND_MAX>0.5)
		return 1;
	else
		return 0;
}
int main(int argc, char const *argv[])
{
	srand(time(NULL));
	int a=getRandom(),b=getRandom();
	std::cout<<a<<' '<<b<<std::endl;
	if(a&&b)
		std::cout<<"1"<<std::endl;
	else
		std::cout<<"0"<<std::endl;
	return 0;
}


#include<iostream>
#include<cstdlib>
#include<time.h>
int getRandom()
{
	return (rand()*1.0/RAND_MAX*1000);
}
int main(int argc, char const *argv[])
{
	srand(time(NULL)*clock());
	int a=getRandom();
	for(int i=0;i<10;i++)
		std::cout<<((a&(1<<i))!=0)<<' ';
	std::cout<<std::endl;
	
	if(a>=500)
		std::cout<<"1"<<std::endl;
	else
		std::cout<<"0"<<std::endl;
	return 0;
}


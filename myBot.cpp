#include <iostream>
#include <vector>
#include "SIbot2.cpp"

using namespace std;

int main(int argc,char *argv[])
{
	
	
	SIbot<int> mybot(argc,argv);
	
	if(mybot.isLiveMode())
	{
		/// Live Mode
		std::vector<int> v(mybot.getNumberInputs());
		cout<<"Enter Inputs : ";
		for(int i=0;i<v.size();i++)
			cin>>v[i];
		mybot.findOutput(v);
		std::vector<int> Result = mybot.giveNormalisedOutput();
		cout<<"Result : ";
		for(int i=0;i<Result.size();i++)
			cout<<Result[i]<<' ';
		cout<<endl;
	}
	else
	{
		//Learn Mode
		std::vector<int> v(mybot.getNumberInputs());
		cout<<"Enter Inputs : ";
		for(int i=0;i<mybot.getNumberInputs();i++)
			cin>>v[i];
		std::vector<double> vO(mybot.getNumberOutputs());
		cout<<"Enter Outputs : ";
		for(int i=0;i<mybot.getNumberOutputs();i++)
			cin>>vO[i];
		mybot.learnThing(v,vO);
	}
}
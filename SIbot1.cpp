
/************************************************************************
		-----------
		|SIBot1   |
		-----------
		
			By scopeInfinity 

Usage : 

SIbot1<InputType=int> mybot(argc,argv);		//To Be Called From Main
mybot.findOutput(vector<InputType>input);	//To Find Expected Output
mybot.giveNormalisedOutput();				//Return [0/1] vector of Node Output
mybot.getOutputs();							//Return Exact Node Output vector
mybot.getBestOutput();						//Return Index of Max Output Node

Execute :

./compiled 0 <name>							//To Find Expected Output 	
./compiled 1 <name>							//Train name

Example
	./compiled 1 and
	./compiled 0 and
or
	while true;do ./andProgram | ./compiled 1 and;sleep 1;done;
	Ctrl^C to Stop Training..

<name>.conf
Desc-
	<NumberOfInput> <NumberOfOutput>
	<UpdateFactor for Weight>
	<Threshold Of Output>
	<MinRandom Weight> <MaxRandom Weight>
Sample-
	2 2
	0.05
	0.5
	-1 1




/********************* Sample myBot.cpp ************************

		#include <iostream>
		#include <vector>
		#include "SIbot1.cpp"

		using namespace std;

		int main(int argc,char *argv[])
		{
			
			
			SIbot1<int> mybot(argc,argv);
			if(mybot.isLiveMode())
			{
				/// Live Mode
				std::vector<int> v(mybot.getNumberInputs());
				cout<<"Enter Inputs : ";
				for(int i=0;i<mybot.getNumberInputs();i++)
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




**************************************************************************/
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <time.h>


using namespace std;

template<typename T> class NodeOutput;
template<typename T> class NodeInput;
template<typename T> class SIbot1;
template<typename T>

class NodeOutput
{
	SIbot1<T>* bot; 
	std::vector<double> weights;
	double expectedOutput,targetOutput;
	double mycontant;
	double threshold;
	double getRandom()
	{
		return bot->getMinRand() + rand()*1.0/RAND_MAX*(bot->getMaxRand()-bot->getMinRand());
	}

	public:
	NodeOutput(SIbot1<T> *bot,double threshold)
	{
		this->bot = bot;
		this->threshold = threshold;
		mycontant = 0;
	}
	void setConstant(double c)
	{
		mycontant = c;
	}
	double getConstant()
	{
		return mycontant;
	}
	void setTargetOutput(double output)
	{
		targetOutput = output;
	}
	void loadWeights(std::vector<double> &v)
	{
		weights.clear();
		for(std::vector<double>::iterator it=v.begin();it!=v.end();it++)
		weights.push_back(*it);
	}
	void loadWeights(int size)
	{
		weights.clear();
		for(int i=0;i<size;i++)
		weights.push_back(getRandom());	
	}
	double getOutputFromWeight()
	{
		expectedOutput = 0;
		int  c = 0;
		for(std::vector<double>::iterator it=weights.begin();it!=weights.end();it++)
			expectedOutput +=  (*it)*(bot->getInput(c++));
		expectedOutput+=mycontant;
		return expectedOutput;
	}
	double getExpectedOutput()
	{
		return expectedOutput;
	}
	int getNormalisedOutput()
	{
		if(expectedOutput>threshold)
			return 1;
		return 0;

	}
	std::vector<double> &getLoadWeight()
	{
		return weights;
	}
	void changeWeights()
	{
		if(targetOutput>threshold)
		{
			//More
			if(expectedOutput>threshold)
				return;
		}
		else
		{
			//Less
			if(expectedOutput<threshold)
				return;
		}
		for(int i=0;i<weights.size();i++)
		{
			weights[i] += (bot->getLearningFactor())*(targetOutput-expectedOutput)*bot->getInput(i);
		}
		mycontant+=(bot->getLearningFactor())*(targetOutput-expectedOutput);
	}
	
};
template<typename T>
class NodeInput
{
	T input;
	SIbot1<T>* bot; 
	public:
	NodeInput(SIbot1<T> *bot)
	{
		this->bot = bot;
	}
	
	void setInput(T input)
	{
		this->input = input;
	}
	T getInput()
	{
		return input;
	}


};
template<typename T>
class SIbot1
{
private:
	bool isLive;
	int no_of_inputs,no_of_outputs;
	double learn_fact;
	double SIBot1_minRan,SIBot1_maxRan;
	int bestOutputNode;

	char filename_memory[100];
	char filename_conf[100];

	class SIbotMAIN;
	
	std::vector<NodeInput<T>*> inputs;
	std::vector<NodeOutput<T>*> outputs;
	bool readfile()
	{
		if (FILE *fp = fopen(filename_memory, "r"))
		{
			for(int c=0;c<no_of_outputs;c++)
			{
				std::vector<double> v;
				double d;
				fscanf(fp,"%lf",&d);
				outputs[c]->setConstant(d);
					
				for(int i=0;i<no_of_inputs;i++)
				{
					fscanf(fp,"%lf",&d);
					v.push_back(d);
				}
				outputs[c]->loadWeights(v);
			}
			fclose(fp);
			return true;
		}
		for(int i=0;i<no_of_outputs;i++)
			{
				outputs[i]->loadWeights(no_of_inputs);
				outputs[i]->setConstant(0);
			}
		return false;
	}
	bool writefile()
	{
		int c=0;
		if (FILE *fp = fopen(filename_memory, "w"))
		{
			for(int c=0;c<no_of_outputs;c++)
			{
				std::vector<double> v = outputs[c]->getLoadWeight() ;
				fprintf(fp,"%lf ",outputs[c]->getConstant());
				
				for(int i=0;i<no_of_inputs;i++)
					fprintf(fp,"%lf ",v[i]);
				fprintf(fp,"\n");
			}
			fclose(fp);
			return true;
		}
		return false;
		
	}
	SIbotMAIN botMain;
public:
	double getMinRand()
	{
		return SIBot1_minRan;
	}
	double getMaxRand()
	{
		return SIBot1_maxRan;
	}

	bool isLiveMode()
	{
		return isLive;
	}
	double getLearningFactor()
	{
		return learn_fact;
	}
	SIbot1(int argc,char *argv[]):botMain(argc,argv)	//botMain constructor will be called
	{
		this->no_of_inputs = botMain.no_of_inputs;//1 is Constant Input 1
		this->no_of_outputs = botMain.no_of_outputs;
		learn_fact = botMain.learning_fact;
		strcpy(this->filename_memory,botMain.filename_memory);
		strcpy(this->filename_conf,botMain.filename_conf);
		this->isLive = botMain.isLive;
		this->SIBot1_minRan = botMain.rand_min;
		this->SIBot1_maxRan = botMain.rand_max;

		for(int i=0;i<no_of_inputs;i++)
			inputs.push_back(new NodeInput<T>(this));
		for(int i=0;i<no_of_outputs;i++)
			outputs.push_back(new NodeOutput<T>(this,botMain.threshold));
		readfile();
	}
	void findOutput(std::vector<T> &newInputs)
	{
		for(int i=0;i<newInputs.size();i++)
			inputs[i]->setInput(newInputs[i]); 
		bestOutputNode = -1;
		double x,lmax;
		for(int i=0;i<no_of_outputs;i++)
		{
			x=outputs[i]->getOutputFromWeight();
			if(bestOutputNode==-1 || x>lmax)
			{
				lmax = x;
				bestOutputNode = i;
			}
		}
		
	}
	int getBestOutput()
	{
		return bestOutputNode;
	}
	std::vector<int> &giveNormalisedOutput()			//Memory Leak Neglected here... :pos
	{
		std::vector<int> *v = new std::vector<int>(no_of_outputs);
		for(int i=0;i<no_of_outputs;i++)
		{
			(*v)[i]=outputs[i]->getNormalisedOutput();
		}
		return *v;
	}
	std::vector<double> &getOutputs()
	{
		std::vector<double> v(no_of_outputs);
		for(int i=0;i<no_of_outputs;i++)
		{
			v[i]=outputs[i]->getExpectedOutput();
		}
		return v;	
	}

	T getInput(int pos)
	{
		return inputs[pos]->getInput();
	}
	~SIbot1()
	{
		writefile();
	}
	void learnThing(std::vector<T> &lInput,std::vector<double> &lOutput)
	{
		cout<<"Learning...\n";
		findOutput(lInput);
		for(int i=0;i<no_of_outputs;i++)
			outputs[i]->setTargetOutput(lOutput[i]);
		for(int i=0;i<no_of_outputs;i++)
			outputs[i]->changeWeights();

	}

	int getNumberInputs()
	{
		return no_of_inputs;
	}
	int getNumberOutputs()
	{
		return no_of_outputs;
	}


private:
	class SIbotMAIN{
	public:
		double learning_fact;
		double threshold;
		int no_of_inputs,no_of_outputs;
		char filename_memory[100],filename_conf[100];
		double rand_min,rand_max;
		bool isLive;
		SIbotMAIN(int argc,char *argv[])
		{
			learning_fact = 0.1;
			threshold = 0.5;
			no_of_inputs = 2;
			no_of_outputs = 1;
			rand_min = -1;
			rand_max = -1;

			if(argc>2)
			{
				strcpy(filename_memory,argv[2]);
				strcpy(filename_conf,argv[2]);
				strcat(filename_memory,".mem");
				strcat(filename_conf,".conf");
			}
			else
			{
				strcpy(filename_memory,	"SIbot1.mem");
				strcpy(filename_conf,"SIbot1.conf");
			}
			

			srand (time(NULL));
			if (FILE *fp = fopen(filename_conf, "r"))
			{
				fscanf(fp,"%d %d",&no_of_inputs,&no_of_outputs);
				fscanf(fp,"%lf",&learning_fact);
				fscanf(fp,"%lf",&threshold);
				fscanf(fp,"%lf %lf",&rand_min,&rand_max);
				fclose(fp);
			}
			else
			{
				cout<<"No Conf File Found!!!!\n";
				if (FILE *fp = fopen(filename_conf, "w"))
				{
					fprintf(fp,"2 1\n0.1\n0.5\n-1 1\n");
					fclose(fp);
					cout<<"Default Conf Created\n";
				}
			
			}

			if(argc<=1 || strcmp("0",argv[1])==0)
				isLive = true;
			else isLive = false;
		}
	};
};




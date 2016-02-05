
/************************************************************************
		-----------
		|SIBot2   |
		-----------
		
			By scopeInfinity 

Usage : 

SIbot2<InputType=int> mybot(argc,argv);		//To Be Called From Main
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
	<NumberOfInput> <NumberOfOutput> <NoOfHiddenLevelUnit>
	<Node in HiddenUnit 1> [<Node in HiddenUnit 2> <Node in HiddenUnit 3> <Node in HiddenUnit 4> ...]
	<UpdateFactor for Weight>
	<Threshold Of Output>
	<MinRandom Weight> <MaxRandom Weight>
Sample-


/********************* Sample myBot.cpp ************************



**************************************************************************/
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <time.h>


using namespace std;

template<typename T> class NodeOutput;
template<typename T> class NodeInput;
template<typename T> class SIbot;
template<typename T>

class NodeOutput
{
	SIbot<T>* bot; 
	std::vector<double> weights;
	std::vector<void*> previousLayer;	//Need To TypeCast to NodeOutput* or NodeInput*
	double expectedOutput,targetOutput;
	double mycontant;
	double threshold;
	double error;//delta
	int hiddenLevelID,id;



	double delta;
	double getRandom()
	{
		return bot->getMinRand() + rand()*1.0/RAND_MAX*(bot->getMaxRand()-bot->getMinRand());
	}

	public:
	NodeOutput(SIbot<T> *bot,double threshold,int id,int hiddenLevelID)
	{
		this->bot = bot;
		this->threshold = threshold;
		mycontant = 0;
		this->hiddenLevelID = hiddenLevelID;
		this->id = id;
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

	void loadInputs(std::vector<NodeOutput<T>*> &v)
	{
		previousLayer.clear();
		for(typename std::vector<NodeOutput<T>*>::iterator it=v.begin();it!=v.end();it++)
		previousLayer.push_back((void*)*it);
	}
	void loadInputs(std::vector<NodeInput<T>*> &v)
	{
		previousLayer.clear();
		for(typename std::vector<NodeInput<T>*>::iterator it=v.begin();it!=v.end();it++)
		previousLayer.push_back((void*)*it);
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
		{
			if(hiddenLevelID==0)
				expectedOutput +=  (*it)*((NodeInput<T>*)(previousLayer[c++]))->getInput();
			else
				expectedOutput +=  (*it)*((NodeOutput<T>*)(previousLayer[c++]))->getOutput();
		}
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
	double sigmod(double t)
	{
		return 1/(1+exp(-t));
	}
	double getOutput()
	{
		double d=sigmod(expectedOutput);	
		if(d<0.001)
			d=0.001;
		else if(d>0.999)
			d=0.999;
		return d;
	}
	void calculateError()
	{
		if(hiddenLevelID==bot->getHiddenLevels()-1)
			error = (targetOutput-getOutput() )*(getOutput())*(1-getOutput());
		else
		{
			double d = 0;
			std::vector<NodeOutput<T>* > nextLayer = bot->getOutputsVector(hiddenLevelID+1);
			for(int i=0;i<nextLayer.size();i++)
			{
				d+=nextLayer[i]->getLoadWeight()[id]*nextLayer[i]->getError();
			}
			error = d*(getOutput())*(1-getOutput());
		}
	}
	double getError()
	{
		return error;
	}
	double getCostError()
	{
		return (targetOutput-getOutput())*(targetOutput-getOutput() )/2;
	}
	void changeWeights()
	{
		/*if(targetOutput>threshold)
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
		*/
		for(int i=0;i<weights.size();i++)
		{
			weights[i] += (bot->getLearningFactor())*(error)*bot->getInput(i,hiddenLevelID-1);////////////////////
		}
		mycontant+=(bot->getLearningFactor())*(error);
	}
	
};
template<typename T>
class NodeInput
{
	T input;
	SIbot<T>* bot; 
	public:
	NodeInput(SIbot<T> *bot)
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
class SIbot
{
private:
	int learnType;
	int no_of_inputs;
	int hiddenLevels;
	std::vector<int> hiddenLevelUnits;
		
	double learn_fact;
	double SIBot_minRan,SIBot_maxRan;
	int bestOutputNode;

	char filename_memory[100];
	char filename_conf[100];

	class SIbotMAIN;
	
	std::vector<NodeInput<T>*> inputs;
	std::vector<std::vector<NodeOutput<T>*> > outputs;
	bool readfile()
	{
		if (FILE *fp = fopen(filename_memory, "r"))
		{
			for(int k=0;k<hiddenLevels;k++)
			{
				for(int j=0;j<hiddenLevelUnits[k];j++)
				{
					std::vector<double> v;
					double d;
					fscanf(fp,"%lf",&d);
					outputs[k][j]->setConstant(d);
					int s = no_of_inputs;
					if(k>0)
						s = hiddenLevelUnits[k-1];
					for(int i=0;i<s;i++)
					{
						fscanf(fp,"%lf",&d);
						v.push_back(d);
					}
					outputs[k][j]->loadWeights(v);
			
				}
				
			}
			fclose(fp);
			return true;
		}
		for(int k=0;k<hiddenLevels;k++)
		for(int i=0;i<hiddenLevelUnits[k];i++)
		{
			int s = no_of_inputs;
			if(k>0)
				s = hiddenLevelUnits[k-1];
			outputs[k][i]->loadWeights(s);
			outputs[k][i]->setConstant(0);
		}
		return false;
	}
	bool writefile()
	{
		int c=0;
		if (FILE *fp = fopen(filename_memory, "w"))
		{
			for(int k=0;k<hiddenLevels;k++)
			{
				for(int c=0;c<hiddenLevelUnits[k];c++)
				{
					std::vector<double> v = outputs[k][c]->getLoadWeight() ;
					fprintf(fp,"%lf ",outputs[k][c]->getConstant());
					
					for(int i=0;i<v.size();i++)
						fprintf(fp,"%lf ",v[i]);
					fprintf(fp,"\n");
				}
				fprintf(fp,"\n");
				
			}
			fclose(fp);
			return true;
		}
		return false;
		
	}
	SIbotMAIN botMain;
public:
	std::vector<NodeOutput<T>*> &getOutputsVector(int level)
	{
		return outputs[level];
	}
	int getHiddenLevels()
	{
		return hiddenLevels;
	}
	double getMinRand()
	{
		return SIBot_minRan;
	}
	double getMaxRand()
	{
		return SIBot_maxRan;
	}

	bool isLiveMode()
	{
		return learnType==0;
	}
	double getLearningFactor()
	{
		return learn_fact;
	}
	SIbot(int argc,char *argv[]):botMain(argc,argv)	//botMain constructor will be called
	{
		this->no_of_inputs = botMain.no_of_inputs;//1 is Constant Input 1
		learn_fact = botMain.learning_fact;
		strcpy(this->filename_memory,botMain.filename_memory);
		strcpy(this->filename_conf,botMain.filename_conf);
		this->learnType = botMain.learnType;
		this->SIBot_minRan = botMain.rand_min;
		this->SIBot_maxRan = botMain.rand_max;
		this->hiddenLevels = botMain.hiddenLevels;
		hiddenLevelUnits = std::vector<int>(botMain.hiddenLevelUnits);

		for(int i=0;i<no_of_inputs;i++)
			inputs.push_back(new NodeInput<T>(this));
		for(int i=0;i<hiddenLevels;i++)
		{
			std::vector<NodeOutput<T>*> level;
			for(int j=0;j<hiddenLevelUnits[i];j++)
				level.push_back(new NodeOutput<T>(this,botMain.threshold,j,i));
			
			outputs.push_back(level);
		}

		for(int j=0;j<hiddenLevelUnits[0];j++)
				outputs[0][j]->loadInputs(inputs);
		for( int i = 1; i < hiddenLevels ; i++ )
			for(int j=0;j<hiddenLevelUnits[i];j++)
					outputs[i][j]->loadInputs(outputs[i-1]);
		readfile();
	}
	void findOutput(std::vector<T> &newInputs)
	{

		for(int i=0;i<newInputs.size();i++)
			inputs[i]->setInput(newInputs[i]); 
		bestOutputNode = -1;
		double x,lmax;
		for(int k=0;k<hiddenLevels;k++)
		{
			bestOutputNode = -1;
			for(int i=0;i<hiddenLevelUnits[k];i++)
			{
				x=outputs[k][i]->getOutputFromWeight();
				if(bestOutputNode==-1 || x>lmax)
				{
					lmax = x;
					bestOutputNode = i;
				}
			}
		}		
	}
	int getBestOutput()
	{
		return bestOutputNode;
	}
	std::vector<int> &giveNormalisedOutput()			//Memory Leak Neglected here... :pos
	{
		std::vector<int> *v = new std::vector<int>(hiddenLevelUnits[hiddenLevels-1]);
		for(int i=0;i<v->size();i++)
		{
			(*v)[i]=outputs[hiddenLevels-1][i]->getNormalisedOutput();
		}
		return *v;
	}
	std::vector<double> &getOutputs()
	{
		std::vector<double> *v = new std::vector<double>(hiddenLevelUnits[hiddenLevels-1]);
		for(int i=0;i<v->size();i++)
		{
			(*v)[i]=outputs[hiddenLevels-1][i]->getOutputs();
		}
		return *v;
	}

	T getInput(int pos)
	{
		return inputs[pos]->getInput();
	}
	double getInput(int pos,int level)
	{
		if(level<0)
			return getInput(pos);
		return outputs[level][pos]->getOutput();
	}
	~SIbot()
	{
		writefile();
	}
	void learnThing(std::vector<T> &lInput,std::vector<double> &lOutput)
	{
		double old_error,new_error=1e100;
		double fError;
		bool f=false;
		cout<<"Learning...\n";
		do
		{
			old_error=new_error;
			findOutput(lInput);
			new_error = 0;
			for(int i=0;i<outputs[hiddenLevels-1].size();i++)
			{
				outputs[hiddenLevels-1][i]->setTargetOutput(lOutput[i]);
				outputs[hiddenLevels-1][i]->calculateError();
				new_error+=outputs[hiddenLevels-1][i]->getCostError();
			}		
			if(!f)
			{
				f=true;
				fError = new_error;
			}
			for(int k=hiddenLevels-2;k>=0;k--)
				for(int i=0;i<outputs[k].size();i++)
					outputs[k][i]->calculateError();

				
			for(int k=hiddenLevels-1;k>=0;k--)
			{
				////////////////////////////////////////////////////
				for(int i=0;i<outputs[k].size();i++)
					outputs[k][i]->changeWeights();
			}

		}while(old_error-new_error>0.000001);
		cout<<"Error from "<<fError<<" to "<<new_error<<endl;
		
	}

	void learnThing(std::vector<std::vector<T> > &lInputs,std::vector<std::vector<double> > &lOutputs)
	{
		cout<<"Learning...Multiple\n";
		double finalError;
		do
		{
			finalError = 0;
			for(int testNumber = 0;testNumber<lInputs.size();testNumber++)
			{
				std::vector<T> &lInput = lInputs[testNumber];
				std::vector<double> &lOutput = lOutputs[testNumber];

						findOutput(lInput);
						
						for(int i=0;i<outputs[hiddenLevels-1].size();i++)
						{
							outputs[hiddenLevels-1][i]->setTargetOutput(lOutput[i]);
							outputs[hiddenLevels-1][i]->calculateError();
							finalError+=fabs(outputs[hiddenLevels-1][i]->getError());
						}

						for(int k=hiddenLevels-2;k>=0;k--)
							for(int i=0;i<outputs[k].size();i++)
								outputs[k][i]->calculateError();

							
						for(int k=hiddenLevels-1;k>=0;k--)
						{
							////////////////////////////////////////////////////
							for(int i=0;i<outputs[k].size();i++)
								outputs[k][i]->changeWeights();
						}
			}
			break;///////////////////// NOT WORKING
		}while(finalError>0.01);
	}

	int getNumberInputs()
	{
		return no_of_inputs;
	}
	int getNumberOutputs()
	{
		return outputs[hiddenLevels-1].size();
	}
	


private:
	class SIbotMAIN{
	public:
		double learning_fact;
		double threshold;
		int no_of_inputs;
		char filename_memory[100],filename_conf[100];
		double rand_min,rand_max;
		int learnType;
		int hiddenLevels;
		std::vector<int> hiddenLevelUnits;
		SIbotMAIN(int argc,char *argv[])
		{
			learning_fact = 0.1;
			threshold = 0.5;
			no_of_inputs = 2;
			rand_min = -1;
			hiddenLevels = 2;
			hiddenLevelUnits.push_back(2);
			hiddenLevelUnits.push_back(1);
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
				strcpy(filename_memory,	"SIbot.mem");
				strcpy(filename_conf,"SIbot.conf");
			}
			

			srand (time(NULL));
			if (FILE *fp = fopen(filename_conf, "r"))
			{
				fscanf(fp,"%d %d",&no_of_inputs,&hiddenLevels);
				hiddenLevelUnits = std::vector<int>(hiddenLevels);
				for(int i=0;i<hiddenLevels;i++)
					fscanf(fp,"%d",&hiddenLevelUnits[i]);
				
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
					fprintf(fp,"2 2\n2 1\n0.05\n0.5\n-0.07 0.07\n");
					fclose(fp);
					cout<<"Default Conf Created\n";
				}
			
			}

			if(argc<=1 || strcmp("0",argv[1])==0)
				learnType = 0;
			else if(strcmp("1",argv[1])==0) learnType = 1;
			else learnType = 2;
		}
	};
};




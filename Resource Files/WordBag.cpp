#include "provided.h"
#include <string>
#include "MyMap.h"
using namespace std;

class WordBagImpl
{
public:
    WordBagImpl(const string& text);
    bool getFirstWord(string& word, int& count);
    bool getNextWord(string& word, int& count);
private:
	MyMap<string, int> mapster;
};
//Constructor takes in a string and uses the Tokenizer to parse it into distinct words with a count of how many times each appears.
// The word and its count are put into a MyMap object.
WordBagImpl::WordBagImpl(const string& text)
{
	Tokenizer t(text);
	string w;
	while(t.getNextToken(w))
	{
		strToLower(w);
		if(mapster.find(w)!=NULL)
		{
			int* numOf=mapster.find(w);
			mapster.associate(w,*numOf+1);
		}
		else
			mapster.associate(w,1);
	}
}

//This returns the first word (or root node) of the Map
bool WordBagImpl::getFirstWord(string& word, int& count)
{
    int* counter=mapster.getFirst(word);
	if(counter==NULL)
		return false;
	else
	{
		count=*counter;
		return  true;
	}
}

//Returns the next word in the tree implementation of MyMap
bool WordBagImpl::getNextWord(string& word, int& count)
{
     int* counter=mapster.getNext(word);
	if(counter==NULL)
		return false;
	else
	{
		count=*counter;
		return  true;
	}
}

//******************** WordBag functions *******************************

// These functions simply delegate to WordBagImpl's functions.
// You probably don't want to change any of this code.

WordBag::WordBag(const string& text)
{
    m_impl = new WordBagImpl(text);
}

WordBag::~WordBag()
{
    delete m_impl;
}

bool WordBag::getFirstWord(string& word, int& count)
{
    return m_impl->getFirstWord(word, count);
}

bool WordBag::getNextWord(string& word, int& count)
{
    return m_impl->getNextWord(word, count);
}

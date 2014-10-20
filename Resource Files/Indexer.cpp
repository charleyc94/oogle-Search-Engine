#include "provided.h"
#include <string>
#include <vector>
#include "MyMap.h"
#include <fstream>
#include <iostream>
using namespace std;
struct idToUrl;

class IndexerImpl
{
public:
    bool incorporate(string url, WordBag& wb);
    vector<UrlCount> getUrlCounts(string word);
    bool save(string filenameBase);
    bool load(string filenameBase);
private:
	MyMap <string, vector<idToUrl>> word2Url;    //Map of words in the index to a vector of URL(IDs) and a count of how many times that word appears in the URL
	MyMap <string, int> url2Int;                  //Mapping of URL names to URL ID
	MyMap <int, string> int2Url;                 //Mapping of URL ID to URL name
	int countOfUrl;
};

struct idToUrl         //This struct holds the ID of the URL and how many times the word associated with it appears in the URL
{
	int ID;
	int counter;
};

//Puts the words from the wordBag into the index along with the URL they appear in
bool IndexerImpl::incorporate(string url, WordBag& wb)
{
	countOfUrl=url2Int.size();      //Get how many URLs have been seen
	int* findUrl=url2Int.find(url);       //Find out if URL has been seen before
	if(findUrl!=NULL)                       //If URL has been seen, return false
	{
		return false;
	}
	if(countOfUrl%2==0)                        //If there is an even number of URLS seen, make its ID the number of URLS currently being held
	{
		int2Url.associate(countOfUrl,url);      //Map the ID to URL name
		url2Int.associate(url,countOfUrl);      //Map the URL name to ID
		countOfUrl++;                          //Increment number of IDs
	}
	else
	{   
		int2Url.associate(-1*countOfUrl,url);     //If there is an odd number of URLS seen, make the ID negative
		url2Int.associate(url,-1*countOfUrl);
		countOfUrl++;
	}

	string w;
	int numOf;
	bool gotAWord=wb.getFirstWord(w,numOf);        //Iterate through each word 
	while(gotAWord)
    {
		idToUrl urlPush;
		urlPush.counter=numOf;
		int* urlHolder=url2Int.find(url);          //Find the corresponding ID of the URL
		if(urlHolder!=NULL)
			urlPush.ID=*urlHolder;                 //Put the ID and count into idToUrl struct

		vector<idToUrl>* v=word2Url.find(w);       //Find if there already is a vector holding the URLs associated with a word
		if(v!=NULL)
		{
			v->push_back(urlPush);                  //Push the idToUrl struct onto the vector
			word2Url.associate(w,*v);               //Put this vector with the word into the MyMap
		}
		else                                       //This word has not been put into the MyMap yet
		{
			vector<idToUrl> emptyVec;              //Make an empty vector, push the idToUrl struct on and put it into the MyMap with the word 
			emptyVec.push_back(urlPush);         
			word2Url.associate(w,emptyVec);
		}
		gotAWord=wb.getNextWord(w,numOf);           //Get next word from the wordBag
	}
	return true;
}

vector<UrlCount> IndexerImpl::getUrlCounts(string word)
{
	strToLower(word);                  //Make sure word is case insensitive
	vector<UrlCount> vec;
	vector<idToUrl>* v=word2Url.find(word);            //Find id the word is in the Index
	if(v!=NULL)
	{
		for(size_t i=0;i<v->size();i++)               //If it is, get each URL ID and find the corresponding name from the int2Url map
		{
			string* urlHold=int2Url.find((*v)[i].ID);      
			if(urlHold!=NULL)                          //If the URL is in the Index
			{
				UrlCount temp;                         //Convert the ID and count into a URL name and count in UrlCount struct
			    temp.url=*urlHold;
				temp.count=(*v)[i].counter;
				vec.push_back(temp);                    //Push this onto the vector
			}
		}
		return vec;                                           //return the vector
	}
	else 
	{
		vector<UrlCount> empty;                       //Else return an empty vector
		return empty;
	} 
}

//This is an overloaded function that will save the MyMap data structure holding words mapped to URL ID and count
bool saveMyFiles(string filenameBase, MyMap<string,vector<idToUrl>>& m )
{
	string key;
	vector<idToUrl>* val;
	ofstream outfile(filenameBase);
	if(outfile)                              //If opened the file successfully
	{
		for(val=m.getFirst(key);val!=NULL;val=m.getNext(key))       //Go through the MyMap of words to idToUrl
		{
			outfile<<key<<endl;                                     //Save the word into the file
			size_t i=0;
			outfile<<val->size()<<endl;                             //Save how many URLs are associated with this word
			while(i<val->size())                                           //Then go through the vector of idToUrl structs
			{
				outfile<<(*val)[i].counter<<endl;                       //Save the count into the file
				outfile<<(*val)[i].ID<<endl;                            //Save the ID into the file
				i++;
			}
            
		}
		return true;              //Return saving file was successful
	}
	else
		return false;               //Return otherwise
}

template <class KeyType, class ValueType>
bool saveMyFiles(string filenameBase, MyMap<KeyType,ValueType>& m )
{
	KeyType key;
	ValueType* val;
	ofstream outfile(filenameBase);
	if(outfile)                         //If openeing the file was successful
	{
		for(val=m.getFirst(key);val!=NULL;val=m.getNext(key))       //Get each ID or URL name, depending on function parameters
		{
			outfile<<key<<endl;                                       //Save the ID or URL name into the file
			outfile<<*val<<endl;                                     //Save the ID or URl name into the file
		}
		return true;
	}
	else 
		return false;
}
bool IndexerImpl::save(string filenameBase)   //Call the saveMyFiles, giving the appropriate parameters and appending the right name onto the fileNameBase
{
    return saveMyFiles(filenameBase+"w2u.txt", word2Url) && saveMyFiles(filenameBase+"i2u.txt", int2Url) && saveMyFiles(filenameBase+"u2i.txt",url2Int);
}

bool IndexerImpl::load(string filenameBase)
{
	this->int2Url.clear();         //Clear all the Index's current data
	this->url2Int.clear();
	this->word2Url.clear();
    ifstream infile(filenameBase+"w2u.txt");        //Create ifstream variables from each respective file
	ifstream cinFile(filenameBase+"i2u.txt");
	ifstream intFile(filenameBase+"u2i.txt");
	if(infile && cinFile && intFile)                     //If opening them was successful
	{
		string word;
		int num;
		while(getline(intFile,word))                   //read in the URL mapped to URL ID data file. this calls getline read the URL name into word
		{
			intFile>>num;                              //read in the ID associated with the URL
			intFile.ignore(1000,'\n');                 //Ignore the end of line so as to read in the next line of words
			this->url2Int.associate(word,num);         //put this into the MyMap associating URL names to ID
			
		}
		int storeNums;
		string storeUrls;
		while(cinFile>>storeNums)        //Get the ID of the URl and keep going while this reading the int in is successful
		{
			cinFile.ignore(1000,'\n');     //Ignore rest of line and endline to get the next word
			getline(cinFile,storeUrls);     //Read in the URL name
			this->int2Url.associate(storeNums,storeUrls);    //Store this in the MyMap associating ID to URL name
		}
		string wordsToStore;
		int howMany;
		vector<idToUrl> vec;
		idToUrl uCount;
		while(getline(infile,wordsToStore))    //Get the word from getline, if this is successful continue loop
		{
			infile>>howMany;             //Read in how many items to push into vector of idToUrl structs
			vec.clear();                  //Clear the temporary vector of idToUrl structs that will be associated with the word so that it can be used during this iteration
			while(howMany>0 && infile)     //While there are still idToUrl data to convert and push into the vector
			{
				infile>>uCount.counter;     //Get the count of how many times the word appears in the URL
				infile.ignore(10000,'\n');   
				infile>>uCount.ID;             //Get the ID of the URL
				infile.ignore(10000,'\n');
				vec.push_back(uCount);          //Push this onto the vector
				howMany--;                     
			}
			this->word2Url.associate(wordsToStore,vec);   //Associate this vector of URL ID's and counts with the word
		}
		return true;             //Return true, loading was successful
	}
	else 
		return false;
}

//******************** Indexer functions *******************************

// These functions simply delegate to IndexerImpl's functions.
// You probably don't want to change any of this code.

Indexer::Indexer()
{
    m_impl = new IndexerImpl;
}

Indexer::~Indexer()
{
    delete m_impl;
}

bool Indexer::incorporate(string url, WordBag& wb)
{
    return m_impl->incorporate(url, wb);
}

vector<UrlCount> Indexer::getUrlCounts(string word)
{
    return m_impl->getUrlCounts(word);
}

bool Indexer::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool Indexer::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}

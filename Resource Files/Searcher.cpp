#include "provided.h"
#include <vector>
#include <algorithm>
#include <string>
#include "MyMap.h"
using namespace std;

class SearcherImpl
{
public:
    vector<string> search(string terms);
    bool load(string filenameBase);
private:
	Indexer searchIndex;
};

struct matchScore      //Holds score and number of matches to pages 
{
	int score;
	int matchNum;
};

struct urlScore             //Holds url and score for the page
{
	string url;
	int pageScore;
};

bool sortUrls(const urlScore& s1, const urlScore& s2)     //Used to sort pages in greatest to least order
{
	return s1.pageScore>s2.pageScore;
}

vector<string> getSearchTerms(string terms)     //Returns a vector of all the search terms as distinct strings, with no repeating words
{
	strToLower(terms);
	vector<string> search;
	Tokenizer t(terms);
	string w;
	while(t.getNextToken(w))
	{
		vector<string>::iterator a,b,itr;
		a=search.begin();
		b=search.end();
		itr=find(a,b,w);                       //Uses STL find algorithm
		if(itr==b)                             //If the word is not already in the vector of search terms, add it
		{
			search.push_back(w);
		}
	}

	return search;
}

vector<string> SearcherImpl::search(string terms)
{
	int N=0;
	int T=0;
	vector<string> searchTerms;
	searchTerms=getSearchTerms(terms);
	N=searchTerms.size();                    //Get amount of search terms
	T=(int(0.7*N)>1)?int(0.7*N):1;           //Calculate match threshold
	vector<UrlCount> urlVec;
	MyMap<string, matchScore> urlMatchNum;
	for(size_t i=0;i<searchTerms.size();i++)      //Find all the URLs that have matches with search terms and record them in a map
	{
		urlVec.clear();
		urlVec=searchIndex.getUrlCounts(searchTerms[i]);
		for(size_t j=0;j<urlVec.size();j++)
		{
			matchScore* scoreFound=urlMatchNum.find(urlVec[j].url);     //If the URL already is in the map, just increment the match count and score appropriately
			if(scoreFound!=NULL)
			{
				matchScore temp;
				temp.matchNum=scoreFound->matchNum+1;
				temp.score=scoreFound->score+urlVec[j].count;
				urlMatchNum.associate(urlVec[j].url,temp);
			}
			else                                                         //Make the match count one and the score the amount of times the word appears on the URL
			{
				matchScore hold;
				hold.matchNum=1;
				hold.score=urlVec[j].count;
				urlMatchNum.associate(urlVec[j].url,hold);
			}
		}
	}

	string termKey;
	matchScore* iter=urlMatchNum.getFirst(termKey);
	vector<urlScore> pageMatches;
	while(iter!=NULL)                       //Store each URL name with score that is equal to or greater than the minimum threshold for page matches in a vector of such data structs so that they can later be sorted
	{
		if(iter->matchNum>=T)
		{
			urlScore trivial;
			trivial.url=termKey;
			trivial.pageScore=iter->score;
			pageMatches.push_back(trivial);
		}
		iter=urlMatchNum.getNext(termKey);
	}

	searchTerms.clear();                                   //Clear searchTerms vector instead of making a new vector and use this as the returned vector
	searchTerms.resize(pageMatches.size());
	sort(pageMatches.begin(),pageMatches.end(), sortUrls);    //Sort all of the pages with decreasing order
	for(size_t m=0;m<pageMatches.size();m++)                      //Copy all the URL names into the final vector to be returned
		searchTerms[m]=pageMatches[m].url;
    return searchTerms;  
}

bool SearcherImpl::load(string filenameBase)
{
	return searchIndex.load(filenameBase);                    //Call the Indexer's load
}

//******************** Searcher functions *******************************

// These functions simply delegate to SearcherImpl's functions.
// You probably don't want to change any of this code.

Searcher::Searcher()
{
    m_impl = new SearcherImpl;
}

Searcher::~Searcher()
{
    delete m_impl;
}

vector<string> Searcher::search(string terms)
{
    return m_impl->search(terms);
}

bool Searcher::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}

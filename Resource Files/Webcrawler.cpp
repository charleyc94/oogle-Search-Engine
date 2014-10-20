#include "provided.h"
#include <string>
#include "http.h"
#include <list>
using namespace std;

class WebCrawlerImpl
{
public:
    void addUrl(string url);
    int getNumberOfUrls() const;
    void crawl(void (*callback)(string url, bool success));
    bool save(string filenameBase);
    bool load(string filenameBase);
private:
	vector<string> urls;
	Indexer webIndex;      
	int counter;       //Count of how many URLs have been crawled
};

void WebCrawlerImpl::addUrl(string url)
{
	if(urls.empty())    //If this is the first call to addUrl's make counter equal 0
		counter=0;
	urls.push_back(url);   //Push the url onto the urls vector
}

int WebCrawlerImpl::getNumberOfUrls() const
{
    return urls.size()-counter;     //return how many urls have not been crawled, which is the size of urls minus how many have been crawled
}

void WebCrawlerImpl::crawl(void (*callback)(string url, bool success))
{
	string webPage;
	bool filler;
	int it=counter;              //Start from the URL that has not been crawled
	while(it!=urls.size())        //Iterate through all the vectors that have not been crawled
	{
		vector<string>::iterator a,b,itr;
		a=urls.begin();
		b=urls.begin()+counter;
		itr=find(a,b,urls[it]);     //Use STL find algorithm to find the URL
		if(itr!=b)                  //If the URL iterator is equal to one past the last URL crawled
		{
			callback(urls[it],false);      //Call the callback function passing in a false boolean since this URL has been crawled before
			counter++;                     //Move the crawled counter up one
			it++;
			continue;                     //Go to the next URL to be crawled
		}
		if(HTTP().get(urls[it],webPage))       //If the URL was never crawled, get the words on the URL 
		{
			WordBag bags(webPage);                         //Put these words into a wordBag
			filler=webIndex.incorporate(urls[it],bags);    //Incorporate these words with the URL and check if the incorporation was successful 
		}
		else 
			filler=false;                                 //If downloading was not successful, set the boolean to false
		callback(urls[it],filler);                  //call callback function with the current URL and whether downloading and incorporating it was successful
		counter++;
		it++;
	}

}

bool WebCrawlerImpl::save(string filenameBase)
{
	return webIndex.save(filenameBase);  //Call Indexer's save
}

bool WebCrawlerImpl::load(string filenameBase)
{
	return webIndex.load(filenameBase);    //Call Indexer's load
}

//******************** WebCrawler functions *******************************

// These functions simply delegate to WebCrawlerImpl's functions.
// You probably don't want to change any of this code.

WebCrawler::WebCrawler()
{
    m_impl = new WebCrawlerImpl;
}

WebCrawler::~WebCrawler()
{
    delete m_impl;
}

void WebCrawler::addUrl(string url)
{
    m_impl->addUrl(url);
}

int WebCrawler::getNumberOfUrls() const
{
    return m_impl->getNumberOfUrls();
}

void WebCrawler::crawl(void (*callback)(string url, bool success))
{
    m_impl->crawl(callback);
}

bool WebCrawler::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool WebCrawler::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}

/*#include "provided.h"
#include <vector>
#include "http.h"
#include <iostream>
using namespace std;

void reportStatus(string url, bool success)
{
	if(success)
	cout<<"web download of "<<url<<" success"<<endl;
	else
		cout<<"web download of "<<url<<" not success"<<endl;
}

int main()
{
    WordBag wb("<html>I’m livin’ 2 love hash tables, I say.</html>");
    Indexer i;
	i.incorporate("http:/www.losers.com",wb);
	vector<UrlCount> urls=i.getUrlCounts("I");
	for(size_t k=0; k<urls.size();k++)
	{
		cout<<urls[k].url<<" "<<urls[k].count<<endl;
	}
	WordBag wb2("<html>I’m a faggot who loves loves loves hash Tables say nigs.</html>");
	WordBag wb3("<html>I’m in loves with hash tables say 2.</html>");
	cout<<boolalpha<<i.incorporate("http:/www.goodsers.com",wb2)<<endl;
	cout<<boolalpha<<i.incorporate("http:/www.lopssers.com",wb3)<<endl;
	vector<UrlCount> urls2=i.getUrlCounts("loves");
	for(size_t k=0; k<urls2.size();k++)
	{
		cout<<urls2[k].url<<" "<<urls2[k].count<<endl;
	}
	i.save("C:/Temp/oogleDocs/text");
	Indexer i2;
	i2.load("C:/Temp/oogleDocs/text");
	i2.save("C:/Temp/oogleDocs/textssss");

	string syllabus;
	WebCrawler syl;
	syl.addUrl("http://cs.ucla.edu/classes/winter13/cs32/syllabus.html");
	syl.crawl(reportStatus);
	syl.save("C:/Temp/oogleDocs/webTest");
	HTTP().set("http:/a.com", "aaa bbb ccc ddd ddd");
	HTTP().set("http:/b.com", "bbb ccc ddd eee");
	HTTP().set("http:/c.com", "ccc ddd ddd eee xxx");
	WebCrawler cp;
	cp.addUrl("http:/a.com");
	cp.addUrl("http:/b.com");
	cp.addUrl("http:/c.com");
	cp.crawl(reportStatus);
	cp.save("C:/Temp/oogleDocs/webdocs");

	Searcher s;
	cout<<boolalpha<<s.load("C:/Temp/oogleDocs/webdocs")<<endl;
	vector<string> pages=s.search(" aaa ddd");
	for(size_t sizeT=0; sizeT<pages.size();sizeT++)
	{
		cout<<pages[sizeT]<<endl;
	}
	cout<<s.load("C:/Temp/oogleDocs/webTest")<<endl;
	vector<string> pag=s.search(" Smalleberg Nachenberg");
	for(size_t sit=0; sit<pag.size();sit++)
	{
		cout<<pag[sit]<<endl;
	}

}*/


#include "provided.h"
#include "http.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const string INDEX_PREFIX = "oogleIndex";

void crawlerCallback(string url, bool success);
void addFromFile(string filename, WebCrawler& wc);
void doSearches();
void help();

int main()
{
    cout << "oogle - the amazing search engine" << endl;
    help();
    const string validCommands("afcsqhp");
    WebCrawler wc;
    for (;;)
    {
        cout << "Command: ";
        string line;
        if (!getline(cin,line))
            break;
        istringstream lineStream(line);
        string cmd;
	if (!(lineStream >> cmd))  // empty line
            continue;
        if (cmd.size() != 1  ||  validCommands.find(cmd[0]) == string::npos)
        {
            cout << "***** Unknown command \"" << cmd << "\" *****" << endl;
            help();
            continue;
        }
        bool requiresArg = (cmd[0] == 'a'  ||  cmd[0] == 'f');
        string arg;
        if (lineStream >> arg)
        {
            if (!requiresArg)
            {
                cout << "***** The \"" << cmd << "\" command takes no arguments *****" << endl;
                help();
                continue;
            }
        }
        else if (requiresArg)
        {
            cout << "***** The \"" << cmd << "\" command requires an argument *****" << endl;
            help();
            continue;
        }
        switch (cmd[0])
        {
          case 'a':
            wc.addUrl(arg);
            cout << "Added " << arg << endl;
            break;
          case 'f':
            addFromFile(arg, wc);
            break;
          case 'c':
            cout << "Processing " << wc.getNumberOfUrls() << " URLs:" << endl;
            wc.crawl(crawlerCallback);
            break;
          case 's':
            if (!wc.save(INDEX_PREFIX))
            {
                cout << "***** Error saving crawler index" << endl;
                continue;
            }
            doSearches();
            break;
          case 'q':
            return 0;
            break;
          case 'h':
            help();
            break;
          case 'p':
            HTTP().set("http://a.com", "aaa bbb ccc ddd ddd");
            HTTP().set("http://b.com", "bbb ccc ccc ddd eee");
            HTTP().set("http://c.com", "ddd eee fff fff ggg");
            cout << "Using the pseudoweb (The h command shows the 3 pages)" << endl;
            break;
        }
    }
}

void crawlerCallback(string url, bool success)
{
    if (success)
        cout << "Processed " << url << endl;
    else
        cout << "***** Error processing " << url << endl;
}

void addFromFile(string filename, WebCrawler& wc)
{
    ifstream urlFile(filename.c_str());
    if (!urlFile)
    {
        cout << "***** Error opening " << filename << endl;
        return;
    }
    string url;
    while (getline(urlFile, url))
    {
        cout << "Added " << url << endl;
        wc.addUrl(url);
    }
}

void doSearches()
{
    Searcher s;
    if (!s.load(INDEX_PREFIX))
    {
        cout << "***** Error loading searcher index" << endl;
        return;
    }
    cout << "Entering an empty line will end searching" << endl;
    for (;;)
    {
        cout << "Enter search terms: ";
        string terms;
        if (!getline(cin, terms)  ||  terms.empty())
            break;
        vector<string> urls = s.search(terms);
        if (urls.empty())
            cout << "There are no matches" << endl;
        else
        {
            cout << "There are " << urls.size() << " matches:" << endl;
            for (size_t k = 0; k != urls.size(); k++)
                cout << "   " << urls[k] << endl;
        }
    }
}

void help()
{
    cout << "Commands available:" << endl
         << "   a someUrl  -- add a URL to be crawled later" << endl
         << "   f filename -- add URLs from a file (one URL per line)" << endl
         << "   c          -- crawl the added URLs, building an index" << endl
         << "   s          -- start perfroming various searches" << endl
         << "   q          -- quit" << endl
         << "   h          -- help:  show this list of commands" << endl
         << "   p          -- use this pseudo-web instead of the real Web:" << endl
         << "                   http://a.com -> \"aaa bbb ccc ddd ddd\"" << endl
         << "                   http://b.com -> \"bbb ccc ccc ddd eee\"" << endl
         << "                   http://c.com -> \"ddd eee fff fff ggg\"" << endl;
}
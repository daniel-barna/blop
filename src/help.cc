#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <vector>
#include <map>
#include <sstream>
#include <string.h>
#include <stdlib.h>

#include "bloputils.h"
#include "blop_bookkeeper.h"
#include "help.h"
#include "pstream.h"
#include "config.h"

namespace blop
{

    using namespace std;



    string get_title(string fullfilename)
    {
	string entry = fullfilename;
	{
	    string::size_type hashmark = fullfilename.find('#');
	    if(hashmark != string::npos) fullfilename.erase(hashmark,string::npos);
	    ifstream fff(fullfilename.c_str());
	    string line;
	    while(getline(fff,line))
	    {
		if(line.find("<center class=title><b>") != string::npos)
		{
		    getline(fff,line);
		    entry = line;
		    break;
		}
	    }
	}
	return entry;
    }

    void fill_helpindex(const string &filename, map<string,vector<pair<string,string> > > &help_index,
			const string &docdir)
    {
	ifstream indexfile(filename.c_str());
	string line;
	while(getline(indexfile,line))
	{
	    istringstream linestr(line);
	    vector<string> keywords,fns;

	    string str;
	    int what = 0;
	    while(linestr>>str)
	    {
		if(str == ":") ++what;
		else
		{
		    if(what == 0) keywords.push_back(str);
		    else fns.push_back(str);
		}
	    }

	    for(vector<string>::size_type i=0; i<keywords.size(); ++i)
	    {
		for(vector<string>::size_type j=0; j<fns.size(); ++j)
		{
		    string fullfilename;
		    if(fns[j][0] != '/') fullfilename = docdir + "/" + fns[j];
		    else fullfilename = fns[j];
		    
		    pair<string,string> entry;
		    entry.first = get_title(fullfilename);
		    entry.second = fullfilename;
		    help_index[keywords[i]].push_back(entry);
		}
	    }
	}
    }


    void show_helpfile(const string &helpfilename)
    {
	system((string("blop-helpviewer ") + helpfilename).c_str());
/*
	bool could_show = false;
	var tried_cmds;
	static bool first_call = true;
	if(html_viewers.empty() || first_call) find_html_viewers();
	first_call = false;
	for(int i=-1; i<(int)(html_viewers.size()); ++i)
	{
	    var cmd;
	    if(i == -1)
	    {
		if(const char *helpviewer = getenv("BLOP_HELPVIEWER"))
		    cmd = helpviewer;
	    }
	    else
	    {
		cmd = html_viewers[i];
	    }
	    if(cmd.str() != "")
	    {
		cmd.replace("%f",helpfilename);
		if(system(cmd) == 0) 
		{
		    could_show = true;
		    break;
		}
		else
		{
		    tried_cmds &= cmd;
		    tried_cmds &= "\n";
		}
	    }
	}
	if(!could_show)
	{
	    cerr<<"I tried to run the following commands, but all of them failed:"<<endl;
	    cerr<<tried_cmds.str()<<endl;
	    cerr<<"-------------------------"<<endl;
	    cerr<<"Look at the file '"<<helpfilename<<"' by another browser"<<endl;
	}
*/
    }



    void help(var topic, var file_name)
    {
	bool make_search = false;
	if(topic[0] == '?')
	{
	    make_search = true;
	    topic(0,0) = "";
	}

	const char *docdir = getenv("BLOP_DOCDIR");
	if(docdir == 0) docdir = BLOP_DOCDIR;

	static map<string,vector<pair<string,string> > > help_index;
	if(help_index.empty())  // fill the help-index
	{
	    char indexfilename[1000];
	    sprintf(indexfilename,"%s/help-index",docdir);
	    fill_helpindex(indexfilename,help_index,docdir);

	    if(const char *home = getenv("HOME"))
	    {
		string hi = string(home) + "/.blop/help-index";
		fill_helpindex(hi,help_index,docdir);
	    }
	    
	}

	if(topic == "keywords")
	{
	    cout<<"Available keywords: "<<endl;
	    cout<<"^^^^^^^^^^^^^^^^^^^ "<<endl;
	    for(map<string,vector<pair<string,string> > >::iterator i=help_index.begin();
		i != help_index.end(); ++i)
	    {
		cout<<"  "<<(*i).first<<endl;
	    }
	    return;
	}
	

	string topic_str = topic.str();
	for(unsigned int i=0; i<topic_str.size(); ++i)
	{
	    if(topic_str[i] == '\'') topic_str[i] = ' ';
	}

	{
	    string fn = string(docdir) + "/documentation/" + topic_str;
	    if( (topic_str.find(".h") == topic_str.size()-2 ||
		 topic_str.find(".cc") == topic_str.size()-3) &&
		file_exists(fn+".html") )
	    {
		show_helpfile(fn+".html");
		return;
	    }
	    if(topic_str.find(".html") == topic_str.size()-5 && file_exists(fn))
	    {
		show_helpfile(fn);
		return;
	    }
	}

	// found in help index (only once...) and no search is forced
	if(help_index.find(topic_str) != help_index.end() &&
	   help_index[topic_str].size() == 1 && !make_search)
	{
	    show_helpfile(help_index[topic_str][0].second);
	    return;
	}
	
	string helpfilename;
	if(file_name.str() == "") helpfilename = BLOP.tmpfile("XXXXXX.html");
	else helpfilename = file_name.str();
	ofstream helpfile(helpfilename.c_str());
	helpfile<<"<html><body>"<<endl;

	bool found_in_index = false;
	if(help_index.find(topic_str) != help_index.end())
	{
	    helpfile<<"<h1>The following files are dedicated for the keyword '"<<topic_str<<"':</h1><br>"<<endl;
	    helpfile<<"<ul>"<<endl;
	    for(unsigned int i=0; i<help_index[topic_str].size(); ++i)
	    {
		helpfile<<"<li><a href=\""<<help_index[topic_str][i].second
			<<"\">"<<help_index[topic_str][i].first<<"</a>"<<endl;
	    }
	    helpfile<<"</ul>"<<endl;
	    found_in_index = true;
	}
	else
	{
	    helpfile<<"<h1>No docfiles are dedicated for the keyword '"<<topic_str<<"'</h1><br>"<<endl;
	}
	helpfile<<"<hr>"<<endl;

	bool found_by_grep = false;
	if(make_search || !found_in_index)
	{
	    // then search all files in the docdir
	    vector<string> subdirs;
	    subdirs.push_back(string(docdir) + "/documentation");
	    subdirs.push_back(string(docdir) + "/tutorial");
	    subdirs.push_back(string(docdir) + "/user-examples");
	    
	    for(unsigned int i=0; i<subdirs.size(); ++i)
	    {
		DIR *dir = opendir(subdirs[i].c_str());
		if(dir == 0)
		{
		    cerr<<"Could not open directory \""<<subdirs[i]<<"\""<<endl;
		    continue;
		}
		
		dirent *f;
		while( (f=readdir(dir)) != 0)
		{
		    bool ok = false;
		    if(strstr(f->d_name,".html") != 0) ok = true;
		    if(subdirs[i] == string(docdir) + "/user-examples") ok = true;
		    if(!ok) continue;
		    string fullfilename = subdirs[i] + "/" + string(f->d_name);
		    string grepcmd = string("grep -E '") + topic_str + string("' ") + fullfilename;
		    
		    ipstream grep(grepcmd);
		    string line, result = "<pre>";
		    while(getline(grep,line))
		    {
			string lineno = "";
			string::size_type beg;
			if((beg = line.find("<a name=\"__line")) != string::npos)
			{
			    string::size_type end = line.find('>',beg);
			    lineno = line.substr(beg+9,end-beg-10);
			    
			    // cut the first part
			    line.replace(beg,end+5-beg,"");
			}
			
			
			// replace < with &lt;, > with &gt; 
			for(unsigned int i=0; i<line.length(); ++i)
			{
			    if     (line[i] == '<') { line.replace(i,1,"&lt;"); i += 3; }
			    else if(line[i] == '>') { line.replace(i,1,"&gt;"); i += 3; }
			}
			
			
			// highlight the matching word
			{
			    string::size_type pos = 0, res;
			    while((res = line.find(topic_str,pos)) != string::npos)
			    {
				string newstr = "<span style=\"color: #FF0000;\">" + topic_str + "</span>"; 
				line.replace(res,topic_str.length(),newstr);
				pos = res + newstr.length();
			    }
			}
			
			
			if(lineno != "")
			{
			    result += "<a href=\"" + fullfilename + "#" + lineno + "\">=&gt;</a>  " + line+ "\n";
			}
			else
			{
			    result += "=&gt;  " + line + "\n";
			}
		    }
		    result += "</pre>";
		    if(grep.close() == 0)
		    {
			if(found_by_grep == false)
			{
			    helpfile << "<h1>The keyword '"<<topic_str<<"' was found in the following files: </h1><br>"
				     << endl;
			    helpfile << "<ul>"<<endl;
			}
			found_by_grep = true;
			
			string entry = get_title(fullfilename);
			helpfile<<"<li><dl><dt><a href=\""<<fullfilename<<"\"> "<<entry;
			if(entry != fullfilename) helpfile<<" ("<<fullfilename<<")";
			helpfile<<"</a></dt>"<<endl;
			helpfile<<"        <dd>\n"<<result<<"</dd></dl>"<<endl;
		    }
		}
	    }
	    if(found_by_grep) helpfile<<"</ul>"<<endl;
	    else helpfile<<"<h1>The keyword '"<<topic_str<<"' was not found in any other files</h1>"<<endl;
	}
	helpfile << "</body></html>"<<endl;
	helpfile.close();
	
	if(found_in_index == false && found_by_grep == false)
	{
	    cerr<<"Your topic '"<<topic_str<<"' was not found"<<endl;
	    return;
	}

	show_helpfile(helpfilename);
    }
}

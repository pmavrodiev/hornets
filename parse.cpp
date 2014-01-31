#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include "data_file.yy.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/greg_month.hpp>
#include <boost/date_time/gregorian/formatters.hpp>
#include <boost/date_time/gregorian/greg_year.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <map>
#include <set>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cctype>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#include "classes.h"


using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

/* =========================== GLOBAL VARS ======================= */
string version="0.0";
vector<string> readings;
map<string,Hornet> hornets_map;

/* =========================== GLOBAL FUNCTIONS ======================= */

/* ==================================================================== */
/* ============================   MAIN   ============================== */
/* ===                                                              === */
/* ===
 *                                                                      */
int main(int argc, char**argv) {
    argv++;
    argc--;
    
    if (argc != 1) {
        cout<<"Version: "<<version<<endl<<endl;
        cout<<"Usage: hornets <data_file>"<<endl<<endl;        
        cout<<"<data_file>\t full path to a file containing reading information"<<endl<<endl;
        return 0;
    }
    else  {      	        
        const char* file_name = argv[0];
	cout<<endl<<"Reading data file ... ";
        data_in = fopen(file_name,"r");
        if (data_in == NULL) {
            perror(file_name);
            exit(1);
        }
        data_lex();	
	fclose(data_in);
	cout<<"DONE"<<endl<<flush;
	/*init the proper hornet objects*/
	char_separator<char> sep("/");
	cout<<endl<<"Initializing hornet objects ... ";
	for (unsigned i=0; i<readings.size(); i++) {
	  tokenizer< char_separator<char> > tokens(readings[i], sep);
	  //if (tokens.size() !=3) {
	    //cerr<<"Error: Something is wrong in the readings vector"<<endl;
	    //exit(1);
	  //}
	  string time, antenna_type, hornet_id;  
	  unsigned counter=0;
	  BOOST_FOREACH (const string& t, tokens) {	    
	    if (counter == 0)
	      time=t;
	    else if (counter == 1)
	      antenna_type=t;
	    else if (counter == 2)
	      hornet_id=t;
	    counter++;
	  }	  
	  int a = (antenna_type[antenna_type.size()-1]=='a' ? 1 : 0);
	  int n = (antenna_type[0]=='2' ? 1 : 0);  	  
	  //cout<<hornet_id<<endl;
	  Hornet &hornet_ref = hornets_map[hornet_id];
	  hornet_ref.hexid = hornet_id;
	  hornet_ref.my_events.push_back(event(ptime(from_iso_string(time)),a,n));          
	}
	cout<<"DONE"<<endl;	
	/*CONTINUE FROM HERE*/  
	hornets_map["D223"].print(&cout);
	hornets_map["D004"].print(&cout);
	//hornets_map["D223"].print2(&cout,14);
	//hornets_map["D223"].decode_events();
	//hornets_map["D223"].construct_valid_indeces();
	//cout<<hornets_map["D223"].valid_indeces<<endl;
	
        return 0;
    }    
}


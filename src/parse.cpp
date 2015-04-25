
#include "classes.h"

#include <fstream>
#include <sstream>

#include "data_file.yy.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <map>

/* =========================== GLOBAL VARS ======================= */
string version="0.0";
vector<string> readings;
map<string,Hornet> hornets_map;
map<date, vector<pair<Hornet_ptr,int> > > hornets_data_D;
map<date, vector<pair<Hornet_ptr,int> > > hornets_data_2D;

string output_dir = "../output/";

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
	boost::char_separator<char> sep("/");
	cout<<endl<<"Initializing hornet objects ... ";
	for (unsigned i=0; i<readings.size(); i++) {
	  boost::tokenizer< boost::char_separator<char> > tokens(readings[i], sep);
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
          int n = 0;
          //handle the case when the data contains only 1 antenna.
          //in this case we don't enter the 'if' block below and n=1
          //otherwise, parse the antennas as normal
          if (antenna_type.size() > 1)
               n = (antenna_type[0]=='2' ? 1 : 0);  	  
	  //cout<<hornet_id<<endl;
	  Hornet &hornet_ref = hornets_map[hornet_id];
	  hornet_ref.hexid = hornet_id;
	  ptime date_time(from_iso_string(time));
	  hornet_ref.my_events.push_back(event(ptime(date_time),a,n));
	  //get only the date
	  date date_only = date_time.date();
	  vector<pair<Hornet_ptr,int> > *ref = (n == 0 ? &hornets_data_D[date_only]:
							 &hornets_data_2D[date_only]);
	  //search for hornet_ref
	  bool found=false;
	  unsigned j;	  
	  for (j=0; j<ref->size(); j++) {
	    if ((*ref)[j].first.h_ptr == NULL)
	      continue;
	    if ((*ref)[j].first.h_ptr->hexid == hornet_ref.hexid) {found = true; break;}
	  }
	  if (found)
	    (*ref)[j].second++;
	  else 
	    ref->push_back(pair<Hornet_ptr,int>(&hornet_ref,1));	  
	}
	cout<<"DONE"<<endl;	
	/*CONTINUE FROM HERE*/  
	//hornets_map["D223"].print(&cout);	
	
	ofstream output,output2;
	string filename = output_dir +  "inside.dat";
	string filename2 = output_dir + "outside.dat";
	string filename5 = output_dir + "inside_D.dat";
	string filename6 = output_dir + "inside_2D.dat";
	string filename3 = output_dir + "nest_statistics.dat";
	string filename4 = output_dir + "hornet_longevity.dat";
	string filename7 = output_dir + "nest_transitions.dat";
	
	/*output hornet longevity*/
	cout<<"Outputting hornet longevity...";
	output.open(filename4.c_str(),ios::out);
	if (!output) {
	    perror(filename4.c_str());
	    exit(1);
	}
	output<<"Hornet\tFirst Occurrence\tLast Occurrence\t\tDifference(Days:Hours:Minutes:Seconds)"<<endl; 
	for (map<string,Hornet>::iterator itr=hornets_map.begin(); itr!=hornets_map.end();itr++) {
	  if (itr->second.my_events.size() > 0) {
	    time_duration td = itr->second.my_events.back().eventtime-itr->second.my_events.front().eventtime;
	    long h = td.hours();
	    long m = td.minutes();
	    long s = td.seconds();
	    long days = h / 24;
	    h = h % 24;
	    output<<itr->second.hexid<<"\t";
	    output<<to_simple_string(itr->second.my_events.front().eventtime)<<"\t";
	    output<<to_simple_string(itr->second.my_events.back().eventtime)<<"\t\t";
	    output<<days<<":"<<h<<":"<<m<<":"<<s<<endl;
	  }
	}
	output.close();
	cout<<"DONE"<<endl;
	
	cout<<"Outputting nest visitations per day ...";
	output.open(filename3.c_str(),ios::out);
	if (!output) {
	    perror(filename3.c_str());
	    exit(1);
	}
	output<<"NEST D"<<endl;
	map<date,vector<pair<Hornet_ptr,int> > >::iterator itr;
	for (itr=hornets_data_D.begin(); itr!=hornets_data_D.end();itr++) {
	  output<<to_simple_string(itr->first)<<"\t";
	  for (unsigned gg=0; gg<itr->second.size(); gg++) {
	    output<<itr->second[gg].first.h_ptr->hexid<<":"<<itr->second[gg].second;
	    output<<"  ";
	  }
	  output<<endl;	  
	}
	output<<"NEST 2D"<<endl;
	for (itr=hornets_data_2D.begin(); itr!=hornets_data_2D.end();itr++) {
	  output<<to_simple_string(itr->first)<<"\t";
	  for (unsigned gg=0; gg<itr->second.size(); gg++) {
	    output<<itr->second[gg].first.h_ptr->hexid<<":"<<itr->second[gg].second;
	    output<<"  ";
	  }
	  output<<endl;	  
	}
	output.close();	
	cout<<"DONE"<<endl;
	
	cout<<"Outputting inside statistics for each nest separately...";
	map<string,Hornet>::iterator i;
	output.open(filename5.c_str(),ios::out);
	output2.open(filename6.c_str(),ios::out);
	if (!output) {
	    perror(filename5.c_str());
	    exit(1);
	}	
	if (!output2) {
	    perror(filename6.c_str());
	    exit(1);
	}
	
	for (i=hornets_map.begin(); i != hornets_map.end(); i++) {
	  i->second.decode_events();
	  i->second.print_inside(&output,&output2);
	}
	output.close();
	output2.close();
	cout<<"DONE"<<endl;
	cout<<"Outputting outside statistics...";
	output.open(filename2.c_str(),ios::out);	
	if (!output) {
	    perror(filename2.c_str());
	    exit(1);
	}		
	for (i=hornets_map.begin(); i != hornets_map.end(); i++) {	  
	  i->second.print_outside(&output);
	}
	output.close();
	cout<<"DONE"<<endl;
	cout<<"Outputting nest transitions ...";
	output.open(filename7.c_str(),ios::out);
	if (!output) {
	    perror(filename7.c_str());
	    exit(1);
	}
	output<<"Hornet\t\tTime_Start\t\tTime_End\tDuration\tD->2D"<<endl;
	
	for (i=hornets_map.begin(); i != hornets_map.end(); i++) {	
	  for (unsigned j=0; j < i->second.transitions.size(); j++) {	   
	    output<<i->second.hexid<<"\t"<<to_simple_string(i->second.transitions[j].start_time);
	    output<<"\t"<<to_simple_string(i->second.transitions[j].end_time)<<"\t";
	    //output<<to_simple_string(i->second.transitions[j].patternduration)<<"\t";
	    output<<(double) i->second.transitions[j].patternduration.total_seconds() / 60.0<<"\t";    
	    output<<i->second.transitions[j].nest<<endl;
	  }	
	}
	
	output.close();
	cout<<"DONE"<<endl;
	//hornets_map["D223"].print2(&cout,10000);
	//hornets_map["D223"].decode_events();
	//hornets_map["D223"].print_patterns(&cout);
	//hornets_map["D004"].print(&cout);
	//hornets_map["30D9"].print2(&cout,14);
	//pair<int,int> bubu = hornets_map["D223"].find_repetitive_sequence("1001300001111",3);
	
	//hornets_map["D223"].decode_events();
	//hornets_map["D223"].construct_valid_indeces();
	//cout<<hornets_map["D223"].valid_indeces<<endl;
	
        return 0;
    }    
}


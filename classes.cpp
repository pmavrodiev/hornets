
/* ======================== CLASS DECLARATIONS ========================= */
//class Box; //represents a box
//class Bat; //represents a bat
//class BatEntry; //represents an entry of a bat into a box
//class Lf_pair; //represents a leading following pair
/* ===================================================================== */
#include "classes.h"
#include <iostream>
#include <math.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/greg_month.hpp>
#include <boost/date_time/gregorian/formatters.hpp>

using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;

/* ======================== CLASS DEFINITIONS ========================== */

event::event() {
  eventtime = not_a_date_time;
  antenna=-1;
  nest=-1;
}

event::event(ptime whichtime, int a, int n) {
  eventtime = whichtime;
  antenna=a;
  nest=n;
  encoded = antenna | nest<<1;
}

decoded_pattern::decoded_pattern() {  
  name="";
}

decoded_pattern::decoded_pattern(string name, pair<int,int> ptr, time_duration td) {
 patternduration = td;
 this->name = name;
 ptr_to_event_vector = ptr;
}

Hornet::Hornet(string Id) {
  hexid=Id;
  valid_indeces="";
}
Hornet::Hornet() {
  hexid="";
  valid_indeces="";
}

bool Hornet::check_valid_event(unsigned int idx) {
  if (idx<0 && idx>my_events.size())
    return false;
  if (my_events[idx] != 1 && my_events[idx] != 3)
    return false;
  if (idx>0 && idx<my_events.size()) {
    if ((my_events[idx-1] == 1 || my_events[idx-1] == 3) &&
	(my_events[idx+1] == 1 || my_events[idx+1] == 3))
      return true;
    else 
      return false;
  }
  //idx=0 or idx=my_events.size()
  return (my_events[idx]==1 ? true: (my_events[idx]==3 ? true : false));  
}


int Hornet::decode_events() {
  int start_outside=0,end_outside=0;
  int start_inside=0,end_inside=0;  
  bool into_pattern=false;
  for (unsigned i=0; i<my_events.size();i++) {        
    //need to get to the first 1 or 3, that is the first
    //reading of the outside antenna
    if (check_valid_event(i)) {
      if (!into_pattern) {
	//sanity check
	if (end_inside-start_inside >= 0) {
	  time_duration td = my_events[end_inside].eventtime-my_events[start_inside].eventtime;
	  decoded_pattern dp_inside("inside",pair<int,int>(start_inside,end_inside),td);
	  my_decoded_patterns.push_back(dp_inside);	
	}
	start_inside = i;
	into_pattern = true;
	if (end_outside-start_outside >=0) {
	  time_duration td = my_events[end_outside].eventtime-my_events[start_outside].eventtime;
	  decoded_pattern dp_outside("outside",pair<int,int>(start_outside,end_outside),td);
	  my_decoded_patterns.push_back(dp_outside);	
	}	  	
      }
      else {
	into_pattern = false;
	end_inside = i;
	start_outside = i;
      }
    }
    else {     
      if (!into_pattern) {
	end_outside=i;		
      }
    }
  }
}

int Hornet::construct_valid_indeces() {
  stringstream index_stream;
  //the first index is always valid
  index_stream<<0;
  for (unsigned i=1; i<my_events.size()-1;i++) {
    //if 1 (3) is preceeded and followed by 0 (2) - not valid
    //all else is valid
    if (my_events[i].encoded == 0 || my_events[i].encoded == 2)
       index_stream<<i;
    else if (my_events[i].encoded == 1   &&
            (my_events[i-1].encoded != 1 ||
	     my_events[i+1].encoded != 1)) {
       index_stream<<i;
    }
    else if (my_events[i].encoded == 3   &&
            (my_events[i-1].encoded != 3 ||
	     my_events[i+1].encoded != 3)) {
       index_stream<<i;
    }
  }
  //the last index is always valid
  index_stream<<my_events.size()-1;
  index_stream>>valid_indeces;
  return 0;
}

void Hornet::print(ostream *out) {
  (*out)<<hexid<<endl;
  for (unsigned i=0; i<my_events.size(); i++)
    (*out)<<my_events[i].encoded;
  cout<<endl;
}

void Hornet::print2(ostream *out,unsigned limit) {
  (*out)<<hexid<<endl;
  limit = (limit<my_events.size() ? limit : my_events.size());
  for (unsigned i=0; i<limit; i++)
    (*out)<<my_events[i].encoded<<":"<<to_simple_string(my_events[i].eventtime)<<endl;
  cout<<endl;

}
/* ==================================================================== */

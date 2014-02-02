
/* ======================== CLASS DECLARATIONS ========================= */
//class Box; //represents a box
//class Bat; //represents a bat
//class BatEntry; //represents an entry of a bat into a box
//class Lf_pair; //represents a leading following pair
/* ===================================================================== */
#include "classes.h"


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
 start_time = not_a_date_time;
 end_time = not_a_date_time;
 patternduration = pos_infin;
 name = "";
}

decoded_pattern::decoded_pattern(string n, time_duration td, ptime start, ptime end) {
  name = n;
  patternduration = td;
  start_time = start;
  end_time = end;
}

decoded_pattern::decoded_pattern(string n, time_duration td, ptime start, ptime end, int nest) {
  name = n;
  patternduration = td;
  start_time = start;
  end_time = end;
  this->nest = nest;
}

void decoded_pattern::print(ostream *out) {
  (*out)<<name<<"-"<<to_simple_string(start_time)<<"-"<<to_simple_string(end_time);
  (*out)<<"-"<<to_simple_string(patternduration)<<endl;
}

Hornet::Hornet(string Id) {
  hexid=Id;
  
}
Hornet::Hornet() {
  hexid="";
  
}

/*look for a sequence of output antenna readings, i.e. aaaaaa, where a could be either 1 or 3*/
std::pair< int, int > Hornet::find_repetitive_sequence(string s,int begin_pos) {
  pair<int,int> return_pair(-1,-1);
  size_t first_pos_D = s.find(sD,(size_t) begin_pos);
  size_t first_pos_2D = s.find(sD2,(size_t) begin_pos);
  
  int start_pos = (first_pos_D == -1 ? (first_pos_2D == -1 ? -1 : first_pos_2D) : 
		  (first_pos_2D == -1 ? first_pos_D: (first_pos_D < first_pos_2D ? first_pos_D : first_pos_2D)));
  
  if (start_pos == -1)
    return return_pair;
  
  int end_pos = start_pos;
  while (s[end_pos] == cD || s[end_pos] == cD2) {
    end_pos++;
    if (s[end_pos] != cD && s[end_pos] != cD2)
      break;
    if (s[end_pos] != s[end_pos-1]) {
      //transition from one nest to another
      time_duration td = my_events[end_pos].eventtime-my_events[end_pos-1].eventtime;
      //from_to = 0-1 = -1 if transition from D to 2D, 1-0 = 1 if transition from 2D to D
      int from_to = my_events[end_pos].nest - my_events[end_pos-1].nest;
      transitions.push_back(decoded_pattern("transition",td,my_events[end_pos-1].eventtime,
					    my_events[end_pos].eventtime,from_to));
    }
  }
  
  return_pair.first=start_pos;
  return_pair.second=end_pos-1;
  return return_pair;  
}

bool Hornet::is_good(int* start_pos, int* first,int *second) {
  int nest_a = my_events[*first-1].nest;
  int nest_b = my_events[*start_pos+1].nest;
  if (nest_a != nest_b) {
	//cerr<<hexid<<"\t"<<start_pos<<"\t"<<p.first<<endl;
	//cerr<<"decode_events(): Warning - inconsistent inside nests "<<nest_a<<" and "<<nest_b<<". Investigate"<<endl;
	//print2(&cerr,1287,1320);
	//print(&cerr);
	//exit(1);	
	return false;
  }
  if (*first-*start_pos <= 2)
    return false;
  return true;
}


int Hornet::decode_events() {
  /*first convert the encoding of each event to string*/
  stringstream ss;
  for (unsigned i=0; i<my_events.size(); i++)
    ss<<my_events[i].encoded;
  my_events_as_strings = ss.str();
  ss.str(""); //clear it  
  /**/  
  pair<int,int> p;
  int start_pos;
  /*first get time outside*/
  //1. find the first reading of an outside antenna, that is a 1 or a 3
  //string nestD="1";
  //string nest2D="3";
  size_t nestD_pos = my_events_as_strings.find(sD,0);
  size_t nest2D_pos = my_events_as_strings.find(sD2,0);  
  
  p=find_repetitive_sequence(my_events_as_strings,0);    
  
  start_pos = p.second;  
  
  if (start_pos == -1) {
    cerr<<"decode_events(): wrong pattern for hornet "<<hexid<<endl;
    exit(1);
  }    
  while (start_pos < my_events_as_strings.size()-1) {
      p=find_repetitive_sequence(my_events_as_strings,start_pos+1);           
      if (p.first == -1 || p.second == -1)
	break;
      int nest_a = my_events[p.first-1].nest;
      int nest_b = my_events[start_pos+1].nest;
      bool good = is_good(&start_pos,&p.first,&p.second);
      if (good) {
	//cout<<"SUCCESS"<<endl;
	decoded_pattern dp_inside("inside",my_events[p.first].eventtime-my_events[start_pos].eventtime,
	  my_events[start_pos].eventtime,my_events[p.first].eventtime,nest_a);      
	inside.push_back(dp_inside);
      }
      decoded_pattern dp_outside("outside",my_events[p.second].eventtime-my_events[p.first].eventtime,
	 		 my_events[p.first].eventtime,my_events[p.second].eventtime);
      if (dp_outside.patternduration.total_seconds() > 0 ) {
	  outside.push_back(dp_outside);
      }
      start_pos=p.second;    
    
  }    
}



void Hornet::print_inside(ostream* out1, ostream* out2) {
  
  for (unsigned i=0; i<inside.size(); i++) {
    if (inside[i].nest == 0) {
      (*out1)<<to_simple_string(inside[i].start_time)<<"\t";
      (*out1)<<to_simple_string(inside[i].end_time)<<"\t";
      (*out1)<<to_simple_string(inside[i].patternduration)<<"\t";
      (*out1)<<(double) inside[i].patternduration.total_seconds() / 60.0<<endl;
    }
    else if (inside[i].nest == 1) {
      (*out2)<<to_simple_string(inside[i].start_time)<<"\t";
      (*out2)<<to_simple_string(inside[i].end_time)<<"\t";
      (*out2)<<to_simple_string(inside[i].patternduration)<<"\t";
      (*out2)<<(double) inside[i].patternduration.total_seconds() / 60.0<<endl;
    }
    else {
      cerr<<"Hornet::print_inside() - invalid nest "<<inside[i].nest<<endl;
      exit(1);
    }
    
  }
  
}


void Hornet::print_outside(ostream* out1) {
  
  for (unsigned i=0; i<outside.size(); i++) {  
      (*out1)<<to_simple_string(outside[i].start_time)<<"\t";
      (*out1)<<to_simple_string(outside[i].end_time)<<"\t";
      (*out1)<<to_simple_string(outside[i].patternduration)<<"\t";
      (*out1)<<(double) outside[i].patternduration.total_seconds() / 60.0<<endl;   
  }  
}


void Hornet::print(ostream *out) {
  (*out)<<hexid<<endl;
  for (unsigned i=0; i<my_events.size(); i++)
    (*out)<<my_events[i].encoded;
  cout<<endl;
}

void Hornet::print2(ostream *out,unsigned start,unsigned end) {
  (*out)<<hexid<<endl;
  end = (end<my_events.size() ? end : my_events.size());
  start = (start < end ? start : end);
  for (unsigned i=start; i<end; i++)
    (*out)<<my_events[i].encoded<<":"<<to_simple_string(my_events[i].eventtime)<<endl;
  cout<<endl;

}


void Hornet::print3(ostream* out) {
  (*out)<<hexid<<endl;
  cout<<"Outside events"<<endl;
  for (unsigned i=0; i<outside.size(); i++) {
    cout<<to_simple_string(outside[i].start_time)<<"\t";
    cout<<to_simple_string(outside[i].end_time)<<"\t";
    cout<<to_simple_string(outside[i].patternduration)<<endl;
  }
}

Hornet_ptr::Hornet_ptr() {
  h_ptr = NULL;
}
Hornet_ptr::Hornet_ptr(Hornet* h) {
  h_ptr = h;
}


/* ==================================================================== */

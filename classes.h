#ifndef _CLASSES_H
#define _CLASSES_H

#include <iostream>
#include <math.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/greg_month.hpp>
#include <boost/date_time/gregorian/formatters.hpp>


//using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;

const int iD = 1; const char cD = '1'; const string sD = "1"; 
const int iD2 = 3; const char cD2 = '3'; const string sD2 = "3";


/* ======================== CLASS DEFINITIONS ========================== */
/*antenna = 0 - inside, 1 - outside*/
/*nest = 0 - D, 1 - 2D
 encoding = antenna<<1 | nest. Example:
 
 nest  nest<<1 antenna encoding (decimal)
   00     00     00        00 (0)
   00     00     01        01 (1)
   01     10     00        10 (2)
   01     10     01        11(3)
   
   In this way, the first nest gets 0-1, and 
   the second 2-3
 */
class event {
public:
  ptime eventtime;
  int antenna;
  int nest;
  int encoded;
  event();
  event(ptime whichtime, int a, int n);
};


class decoded_pattern {
public:
  time_duration patternduration;
  ptime start_time;
  ptime end_time;
  string name;
  int nest; //for which nest is this pattern? only for "inside" patterns  
  decoded_pattern();
  decoded_pattern(string n,time_duration td, ptime start, ptime end);
  decoded_pattern(string n,time_duration td, ptime start, ptime end, int nest);
  void print(ostream *);
};


class Hornet {
private:
    bool is_good(int* start_pos, int* first, int* second);
public:
    string hexid;
    string my_events_as_strings;
    vector<event> my_events;
    vector<decoded_pattern> inside;
    vector<decoded_pattern> outside;
    vector<decoded_pattern> transitions;
    //vector<decoded_pattern> my_decoded_patterns;
    pair<int,int> find_repetitive_sequence(string s, int start_pos);
    Hornet(string Id);
    Hornet();   
    int decode_events();   
    void print(ostream *out);
    void print2(ostream *out,unsigned start,unsigned end);
    void print3(ostream *out);  
    void print_inside(ostream *out1,ostream *out2);
    void print_outside(ostream *out1);

};

class Hornet_ptr {
public:
  Hornet *h_ptr;
  Hornet_ptr();
  Hornet_ptr(Hornet *);
};


#endif
#ifndef _CLASSES_H
#define _CLASSES_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/greg_month.hpp>
#include <boost/date_time/gregorian/formatters.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cctype>
#include <algorithm>


using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;


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
  time_duration patternduration;
  pair<int,int> ptr_to_event_vector;
  string name;
  decoded_pattern();
  decoded_pattern(string name,pair<int,int> ptr, time_duration td);
};

class Hornet {
public:
    string hexid;
    string valid_indeces;
    vector<event> my_events;
    vector<decoded_pattern> my_decoded_patterns;
    Hornet(string Id);
    Hornet();
    int construct_valid_indeces();
    int decode_events();
    bool check_valid_event(unsigned idx);
    void print(ostream *out);
    void print2(ostream *out,unsigned limit);
};

#endif
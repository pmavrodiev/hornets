%option noyywrap
%option nounput
%option prefix="data_"
%{
#include <iostream>
#include <vector>
#include <utility>
#include <cctype>
#include <algorithm>
#include <sstream>


using namespace std;

string Date; string Time; string HexId; string Antenna; 
string local_month,local_day,local_year;
char *pch2;

extern vector<string> readings;


bool date_flag=false, time_flag=false, hexid_flag=false;
bool antenna_flag=false;
%}

DIGIT [0-9]
HEXDIGIT [a-fA-F0-9]
LETTER [a-zA-Z]
SPACE [" ","\t"]

%%


<INITIAL>{DIGIT}{1,2}"/"{DIGIT}{1,2}"/"{DIGIT}{4} {    
  stringstream ss;
  pch2 = strtok(data_text,"/");  
  local_month = pch2;
  if (local_month.length() == 1)
    local_month = "0"+local_month;
  pch2 = strtok(NULL,"/");
  local_day = pch2;
  if (local_day.length() == 1)
    local_day = "0"+local_day;
  pch2 = strtok(NULL,"/");
  local_year = pch2;
  Date = local_year+local_month+local_day;
  date_flag = true;   
}


<INITIAL>{DIGIT}{1,2}":"{DIGIT}{1,2}":"{DIGIT}{1,2} {    
  string hours,minutes,seconds;
  pch2 = strtok(data_text,":");
  hours=pch2;
  if (hours.length() == 1)
    hours = "0"+hours;
  pch2 = strtok(NULL,":");
  minutes=pch2;
  if (minutes.length() == 1)
    minutes = "0"+minutes;    
  pch2 = strtok(NULL,":");
  seconds=pch2;
  if (seconds.length() == 1)
    seconds="0"+seconds;  
  Time = hours+minutes+seconds;
  time_flag=true;
}

<INITIAL>"2D"{SPACE}+"i"|"2D"{SPACE}+"a" {
  pch2=strtok(data_text,".");
  Antenna = pch2;
  antenna_flag=true;
}

<INITIAL>"D"{SPACE}+"i"|"D"{SPACE}+"a" {
  pch2=strtok(data_text,".");
  Antenna = pch2;
  antenna_flag=true;
}

<INITIAL>"i"|"a" {
  pch2=strtok(data_text,".");
  Antenna = pch2;
  antenna_flag=true;
}

<INITIAL>{HEXDIGIT}{4,10} {  
  pch2 = strtok(data_text,".");
  HexId = pch2;
  hexid_flag = true;
}


<INITIAL>\n|\r|\r\n {/*ignore*/
  if (date_flag && time_flag && hexid_flag && antenna_flag) {
    string r = Date+"T"+Time+"/"+Antenna+"/"+HexId;    
    readings.push_back(r);
  }
  date_flag=false;
  time_flag=false;
  hexid_flag=false;
  antenna_flag=false;
}

<*>. //{printf("%s",data_text);}/* ignore this token in any start condition*/


%%



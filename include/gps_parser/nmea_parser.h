#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include <string.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class NmeaParser
{
  public:
    string gpgga;
    string gprmc;
    
    bool parse_gpgga();
    bool parse_gprmc();
    
    
    void get_latlong(double &latitude,char &latdir, double &longitude, char &longdir);
    int get_fix();
    int get_n_satellites();
    float get_altitude();
    float get_heading();

  private:
  double latitude, longitude;
  char latitude_cardinal, longitude_cardinal;
  int n_satellites;
  float altitude;
  int fix_indicator;
  float heading;
  string time_of_fix;
};

#endif // NMEA_PARSER_H

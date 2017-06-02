#include "nmea_parser.h"

using namespace std;

bool NmeaParser::parse_gpgga()
{
  if(this->gpgga.size() < 1)
    return false;

  char* res = new char[this->gpgga.size()];
  strcpy(res, gpgga.c_str());
  string tmp;
  
  char *tok = strtok(res,",."); // header
  if(tok == NULL)
  {
  	return false;
  }
  else
  {
  	char temp[6];
  	sscanf(tok,"%s", &temp);
  	string prefix(temp);
  	if(!(prefix == "$GPGGA"))
  	{
    		cout << "Invalid prefix" << endl;
    		return false;
  	}
  }	

  tok = strtok(NULL, ".,"); // time hhmmss
  /*if(tok == NULL)
  {
  	this->time_of_fix = "xx:xx:xx";
  }
  else
  {
  	char time[6];
  	sscanf(tok, "%s", &time);
  	tmp = string(time);
  	if(tmp.size() != 6)
  		this->time_of_fix = "xx:xx:xx";
  	else
  		this->time_of_fix = tmp.substr(0,2) + ":" + tmp.substr(2,2) + ":" + tmp.substr(4,2);
  }*/
  
  tok = strtok(NULL, ".,"); //decimal of seconds

	double coord[2];
	char dir[2];
	
	int i = 0;
	while(i < 2)
	{
		tok = strtok(NULL, ".,"); // coordinate degrees and minutes  		
  		if(tok == NULL)
  		{
  			coord[i] = 0.0;
  		}
		else
  		{
  			tmp.clear();
  			int degrees;
  			string min;
  			if(i == 0) // latitude
  			{
  				char dm[4];
				sscanf(tok, "%s", &dm);
  				tmp = string(dm);
  				degrees = atoi(tmp.substr(0,2).c_str());
  				min = tmp.substr(2,2);
        		}
        		if(i == 1) // longitude
  			{
  				char dm[5];
				sscanf(tok, "%s", &dm);
  				tmp = string(dm);
  				degrees = atoi(tmp.substr(0,3).c_str());
  				min = tmp.substr(3,2);
        		}
        		tok = strtok(NULL, ".,"); // coordinate degree decimal
  			tmp.clear();
  			char degdec[4];
  			sscanf(tok, "%s", &degdec);
  			tmp = string(degdec);
  			double minutes = atof((min + "." + tmp).c_str());
  			coord[i] = degrees + (minutes/60.0);
   		}
	
  	
  		tok = strtok(NULL,".,");
  		if(tok == NULL)
  		{
  			dir[i] = ' ';
  		}
  		else
  		{
  			char dr;
  			sscanf(tok, "%s", &dr);
  			dir[i] = dr;
  		}

		i++;
	}

	this->latitude = coord[0];
	this->latitude_cardinal = dir[0];
	
	this->longitude = coord[1];
	this->longitude_cardinal = dir[1];
  
  tok = strtok(NULL,".,");  
  if(tok == NULL)
  {
  	this->fix_indicator = 0;
  }
  else
  {
  	sscanf(tok, "%d", &this->fix_indicator);
  }   
  
  tok = strtok(NULL,".,");  
  if(tok == NULL)
  {
  	this->n_satellites = 0;
  }
  else
  {
  	sscanf(tok, "%d", &this->n_satellites);
  }

  tok = strtok(NULL,","); // HDOP
  tok = strtok(NULL,","); // altitude  
  if(tok == NULL)
  {
  	this->altitude = 0;
  }
  else
  {
  	sscanf(tok, "%f", &this->altitude);
  }

  tok = strtok(NULL,","); // letter M
  tok = strtok(NULL,","); // wgs84 ellipsoid ~ mean sea level altitude
  tok = strtok(NULL,","); // letter M
  tok = strtok(NULL,","); // nothing 
  return true;
}

bool NmeaParser::parse_gprmc()
{
    if(this->gprmc.size() < 1)
    	return false;

  char* res = new char[this->gprmc.size()];
  strcpy(res, gprmc.c_str());
  string tmp;
  
  char *tok = strtok(res,",."); // header
  if(tok == NULL)
  {
  	return false;
  }
  else
  {
  	char temp[6];
  	sscanf(tok,"%s", &temp);
  	string prefix(temp);
  	if(!(prefix == "$GPRMC"))
  	{
    		cout << "Invalid prefix" << endl;
    		return false;
  	}
  }	

	tok = strtok(NULL,"."); // hhmmss
	tok = strtok(NULL,","); // decimal part of seconds  
	tok = strtok(NULL,","); // status indicator
	tok = strtok(NULL,"."); // latitude degree minutes
	tok = strtok(NULL,","); // latitude decimal part of minutes
	tok = strtok(NULL,","); // latitude cardinal
	tok = strtok(NULL,"."); // longitude degree minutes
	tok = strtok(NULL,","); // longitude decimal part of minutes
	tok = strtok(NULL,","); // longitude cardinal	
	tok = strtok(NULL,","); // speed in knots
	
	tok = strtok(NULL,","); // heading
	if(tok == NULL)
  	{
  		this->heading = 0;
  	}
  	else
  	{
  		sscanf(tok, "%f", &this->heading);
  	}
	
	tok = strtok(NULL,","); // ddmmyy
	tok = strtok(NULL,","); // magnetic variation
	tok = strtok(NULL,","); // direction of magnetic variation
	tok = strtok(NULL,","); // Mode indicator
	return true;
}

float NmeaParser::get_heading()
{
	return this->heading;
}


float NmeaParser::get_altitude()
{
	return this->altitude;
}

int NmeaParser::get_n_satellites()
{
	return this->n_satellites;
}

int NmeaParser::get_fix()
{
	return this->fix_indicator;
}

void NmeaParser::get_latlong(double &latitude, char &latdir,  double &longitude, char &longdir)
{
  latitude = this->latitude;
  latdir = this->latitude_cardinal;
  longitude = this->longitude;
  longdir = this->longitude_cardinal;
}

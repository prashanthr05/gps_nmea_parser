#include <iostream>
#include <string>

#include "nmea_parser.h"

#include "ros/ros.h"
#include "libserial/SerialPort.h"
#include "gps_common/GPSFix.h"

using namespace std;

SerialPort* gps_serial;
NmeaParser* gps_parser;
gps_common::GPSFix gps_msg;
ros::Publisher gps_pub;

void gpsReadCallback(const ros::TimerEvent&)
{
	string res;
	string gpgga, gprmc;
	do
	{
		res = "";
		try
		{
			res = gps_serial->ReadLine(100);
		}
		catch(...)
		{
			cout << "Timeout 1" << endl;
		}
	} while(!(res.substr(0,6) == "$GPGGA"));
	
	gpgga = res;
	
	do
	{
		res = "";
		try
		{
			res = gps_serial->ReadLine(100);
		}
		catch(...)
		{
			cout << "Timeout 2" << endl;
		}
	} while(!(res.substr(0,6) == "$GPRMC"));
	
	gprmc = res;

	cout << gpgga << endl;
	cout << gprmc << endl;
	gps_parser->gpgga.assign(gpgga);
	gps_parser->gprmc.assign(gprmc);
}

void gpsWriteCallback(const ros::TimerEvent&)
{
	int fix, n_satellites;
	double latitude = 0.0, longitude = 0.0;
	char latitude_cardinal, longitude_cardinal;
	double altitude = 0.0, heading = 0.0;
	if(gps_parser->parse_gpgga() || gps_parser->parse_gprmc())
	{
		gps_parser->get_latlong(latitude, latitude_cardinal, longitude, longitude_cardinal);
		altitude = gps_parser->get_altitude();
		heading = gps_parser->get_heading();
		fix = gps_parser->get_fix();
		n_satellites = gps_parser->get_n_satellites();
		
		if((fix == 0) || (n_satellites == 0) || (latitude < -90) || (latitude > 90) || (longitude < -180) || (longitude > 180))
		{
			latitude = 0.0;
			longitude = 0.0;
			altitude = 0.0;
			heading = 0.0;
		}
	}
	else
	{
		latitude = 0.0;
		longitude = 0.0;
		altitude = 0.0;
		heading = 0.0;
		fix = 0;
		n_satellites = 0;
	}

	gps_msg.header.stamp = ros::Time::now();
	gps_msg.header.frame_id = "/gps";
	gps_msg.status.status = fix;
	gps_msg.status.satellites_visible = n_satellites;	
	gps_msg.latitude = latitude;
	gps_msg.longitude = longitude;
	gps_msg.altitude = altitude;
	gps_msg.dip = heading;

	
	gps_pub.publish(gps_msg);
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "gps_parser");
	ros::NodeHandle nh;
	
	gps_serial = new SerialPort("/dev/ttyACM0");
	gps_serial->Open(SerialPort::BAUD_19200, SerialPort::CHAR_SIZE_8, SerialPort::PARITY_NONE, SerialPort::STOP_BITS_1, SerialPort::FLOW_CONTROL_NONE);
	if(!gps_serial->IsOpen())
	{
		cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << "GPS Serial port could not be opened." << endl;
		exit(1);
	}

	gps_parser = new NmeaParser;

	gps_pub = nh.advertise<gps_common::GPSFix>("/ocu/gpsFused", 1);
	ros::Timer gps_read = nh.createTimer(ros::Duration(1.5), gpsReadCallback);
	ros::Timer gps_write = nh.createTimer(ros::Duration(1), gpsWriteCallback);

	ros::spin();
	ros::shutdown();
	return 0;
}

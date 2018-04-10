#include <vector>
#include "Drone.h"
#include <math.h>
#include <boost/asio/io_service.hpp>
#include <Fullnavdata.h>
#include <gnuplot_iostream.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
/*
 * PRIVATE HEADER
 */

#define DRONE_IP                    "192.168.42.1"
#define DRONE_MAX_ALTITUDE          1.0
#define DRONE_MAX_HORIZONTAL_SPEED  0.3
#define DRONE_MAX_VERTICAL_SPEED    0.3
#define LAND_AFTER_LAST_WAYPOINT    true
#define CALIBRATION_FILE            "res/calib_bd2.xml"
#define HULLPROTECTIONON            true
#define LOOK_FOR_CHESSBOARD         false


#define MAX_COORD 10000000

using namespace std;

void ReadyToReadScan();
void ReadScanFile(string fn);
double * CheckSmallestCoord(double x, double y, int check_x_or_y, double current_min[], int negative);

/* TODO: Need to uncomment drone code if trying to use drone;
          currently just trying to make a droneless code   */

//Drone bebop;

/* Signal catches; if a process dies, we need drone to emergency stop */
/*
void kill_handler (int junk)
{
	signal(SIGINT, kill_handler);
	cout << endl << "AHHHHHHHHHHHH, WHAT ARE YOU DOING" << endl;
	bebop.emergency();
	exit(1);
}
*/

int main(int argc, char *argv[]) {
	
	cout << argc << endl;	
	if( argc != 2 )
	{
		//need to put the file name of where the scans will go;
		// this assumes all scans will go to the same file
		cout << "PUT IN A FILE NAME, FOOL!" << endl;
 		exit(0);
	}
	

		
	/* SETUP FOR DONE */	
/*	signal(SIGINT, kill_handler);
	bebop.connect();

	while(!bebop.isRunning()){ sleep(1); }

	bebop.setMaxAltitude(DRONE_MAX_ALTITUDE);
   bebop.setMaxHorizontalSpeed(DRONE_MAX_HORIZONTAL_SPEED);
   bebop.setMaxVerticalSpeed(DRONE_MAX_VERTICAL_SPEED);

	if(bebop.blockingFlatTrim()) 
	{
		std::cout << "FLAT TRIM IS OK" << std::endl;
	}
	else
	{
		std::cerr << "FLAT TRIM NOT OK" << std::endl;
		return 1;
	}
*/	/*END SETUP*/
	
	/*
	   This is very rudimentary; the drone cannot currently fly with
		everything on it.
	*/
	
	string file_name = argv[1];
	
	bool getting_info_loop = true;
	
	while(getting_info_loop)
	{	
		ReadyToReadScan();
		
		ReadScanFile(file_name);
		
	}


	exit(0);
}

/*
	This just makes sure the user is
*/
void ReadyToReadScan()
{
	char scan_ready;
	cout << "Ready for next scan (y for yes; e for exit)" << endl;
	bool scan_ready_loop = true;
	while(scan_ready_loop)
	{
		cin >> scan_ready;
		switch(scan_ready)
		{
			case 'y':
				scan_ready_loop = false;
				break;
			
			case 'e':
				exit(0);
				break;
			
			default:
				cout << "FOOL, DO NOT TEST ME!" << endl;
		}
	}
}

void ReadScanFile(string fn)
{
	ifstream fin;
	double x_info;
	double y_info;
	double z_info;
	
/*
	double smallest_forward_x[2] = {MAX_COORD, MAX_COORD};
	double smallest_backward_x[2] = {MAX_COORD, MAX_COORD};
	double smallest_left_y[2] = {MAX_COORD, MAX_COORD};
	double smallest_right_y[2] = {MAX_COORD, MAX_COORD};
*/

	double *smallest_forward_x = NULL;
	double *smallest_backward_x = NULL;
	double *smallest_left_y = NULL;
	double *smallest_right_y = NULL;

	string info_line;

	fin.open(fn);
	
	
	//the first line is the "NODE" line, which should be discarded here	
	getline(fin, info_line);
	
	//getting each x and y pair of data
	while( getline(fin, info_line) )
	{
		cout << info_line << endl;
		
		istringstream iss(info_line);

		iss >> x_info;
		iss >> y_info;
		iss >> z_info;
		
		
		if( x_info < 0)
		{
			smallest_backward_x = CheckSmallestCoord(x_info, y_info, 0, smallest_backward_x, 1);
		}
		if( x_info >= 0 )
		{
			smallest_forward_x = CheckSmallestCoord(x_info, y_info, 0, smallest_forward_x, 0);
		}	
		//TODO: THIS MIGHT BE BACKWARDS, need to check!!!!!!!!!!!!!!!!!!!!
		if( y_info < 0 )
		{
			smallest_left_y = CheckSmallestCoord(x_info, y_info, 1, smallest_left_y, 1);
		}
		if( y_info >= 0 )
		{
			smallest_right_y = CheckSmallestCoord(x_info, y_info, 1, smallest_right_y, 0);
		}
	} 
	
	cout << "SBX: " << smallest_backward_x[0] << " " << smallest_backward_x[1] << endl;
	cout << "SFX: " << smallest_forward_x[0] << " " << smallest_forward_x[1] << endl;
	cout << "SLY: " << smallest_left_y[0] << " " << smallest_left_y[1] << endl;
	cout << "SRY: " << smallest_right_y[0] << " " << smallest_right_y[1] << endl;
	fin.close();
}

/*
	check_x_or_y means which index it should be checking in the array:
		0 for x, 1 for y
*/
double * CheckSmallestCoord(double x, double y, int check_x_or_y, double current_min[], int negative)
{
	double * new_min_array = new double[2];
	
	//if it is the first loop, need to set the min to current point
	if( current_min == NULL )
	{
		new_min_array[0] = x;
		new_min_array[1] = y;
		return new_min_array;
	}
	
	double check_min = current_min[check_x_or_y];

	new_min_array[0] = current_min[0];
	new_min_array[1] = current_min[1];

	//checking x
	if( check_x_or_y == 0 ) 
	{
		if( check_min > x  && negative == 0)
		{
			new_min_array[0] = x;
			new_min_array[1] = y;
		}
		else if( check_min < x && negative == 1)
		{
			new_min_array[0] = x;
			new_min_array[1] = y;
		}	
	}
	//checking y
	if( check_x_or_y == 1 ) 
	{
		if( check_min > y && negative == 0 )
		{
			new_min_array[0] = x;
			new_min_array[1] = y;
		}
		else if( check_min < y && negative == 1)
		{ 
			new_min_array[0] = x;
			new_min_array[1] = y;
		}
	}
	delete[] current_min;
	
	return new_min_array;
}

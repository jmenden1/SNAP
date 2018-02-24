#include <vector>
#include "Drone.h"
#include <math.h>
#include <boost/asio/io_service.hpp>
#include <Fullnavdata.h>
#include <gnuplot_iostream.h>
#include <deque>


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

using namespace std;

/* TODO:declaring drone globaly; useful for functions?.... */
Drone bebop;

/* Signal catches; if a process dies, we need drone to emergency stop */
void kill_handler (int junk)
{
	signal(SIGINT, kill_handler);
	cout << endl << "AHHHHHHHHHHHH, WHAT ARE YOU DOING" << endl;
	bebop.emergency();
	exit(1);
}

int main() {
	
	signal(SIGINT, kill_handler);
	
	cout << "Houston, we have take off" << endl;
	
	/* SETUP FOR DONE */	
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
	/*END SETUP*/

	bebop.takeOff();
	sleep(6);
	bebop.modifyPitch(30);
	sleep(2);
	bebop.modifyPitch(0);
	sleep(2);
	bebop.land();

	exit(0);
}




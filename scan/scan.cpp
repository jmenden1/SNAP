/*Author: Chad Davidson
Date: 1/18/2018
Description: Implentation of Scan.h.
 */
#include "scan.h"
#include <vector>
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <math.h>
#include <fstream>

/*The constructor reads the configuration file and fills in the class values. If you want to add a 
  configuration option, make sure it's in the correct format in the .config file and then add another else
  if statement to this terrible looking constructor*/
Scan::Scan(){

    FILE *config = fopen("config/scan.config", "r");

    char var [100];
    char val [1000];

    /*Use this when the val is a number. Convert to string first and then convert to something else
      Just for ease*/
    std::string val_string;

    if (config != NULL){

        while (fscanf(config, "%s %s\n", var, val) != EOF){


            /*Contains the path to the Sweep device i.e. ttyUSB0 probably*/
            if (strcmp(var,"devicepath") == 0){

                devicepath.assign(val, strlen(val));
                continue;                

                /*Number of scans for each call to scan. Default is 10*/        
            }else if (strcmp(var, "numscansper") == 0){

                val_string.assign(val, strlen(val)); 
                num_scans_per = stoi(val_string, NULL);
                continue;

            }else if (strcmp(var, "motorspeed") == 0){

                val_string.assign(val, strlen(val));
                motor_speed = stoi(val_string, NULL);
                continue;

            }else if (strcmp(var, "samplerate") == 0){

                val_string.assign(val, strlen(val));
                sample_rate = stoi(val_string, NULL);
                continue; 


            }else{

                fprintf(stderr, "Configuration File Error @ Line:\n%s %s\n", var, val);
                exit(1);

            }                         
        }

    }else{

        fprintf(stderr, "Error opening configuration file\n");
        exit(1);

    }

}

/*Can set device path outside of configuration file*/
void Scan::set_device_path(std::string input_path){

    devicepath = input_path;

}

/*Simple get stuff here*/
std::string Scan::get_device_path(){

    return devicepath;

}

double Scan::DegreestoRadians(double degrees){

    return ((degrees/180.0) * M_PI);

}
int Scan::perform_scan(std::ofstream& of){

    int quadrant;
    double temp_angle;
    double angle;
    double y;
    double x;

    try{

        /*Initalize sweep device and start the scanning process. Scanning is assured not to start until
          the device is calibrated*/

        sweep::sweep device{devicepath.c_str()};

        if (device.get_motor_speed() != motor_speed) device.set_motor_speed(motor_speed);
        if (device.get_sample_rate() != sample_rate) device.set_sample_rate(sample_rate);


        device.start_scanning();

        for (int i = 0; i < num_scans_per; i++){

            const sweep::scan scan = device.get_scan();

            for (const sweep::sample& sample : scan.samples){

//                std::cout << "angle " << (double)sample.angle/1000 << " distance " << sample.distance << " strength " << sample.signal_strength << std::endl;
                
                angle = (double)sample.angle/1000;

                quadrant = (angle/90) + 1;

                temp_angle = angle - (90 * (quadrant-1));
                switch(quadrant){

                    case 1:
                        if (temp_angle < 45){

                            y = (double) (abs(sample.distance * sin(DegreestoRadians(temp_angle))));
                            x = (double) (abs(sample.distance * cos(DegreestoRadians(temp_angle))));            

                        }else{

                            y = (double) (abs(sample.distance * cos(DegreestoRadians(90 - temp_angle))));
                            x = (double) (abs(sample.distance * sin(DegreestoRadians(90 - temp_angle))));

                        }
                        break;

                    case 2:
                        if (temp_angle < 45){

                            y = (double) (abs(sample.distance * cos(DegreestoRadians(temp_angle))));
                            x = (double) -(abs(sample.distance * sin(DegreestoRadians(temp_angle))));            

                        }else{

                            y = (double) (abs(sample.distance * sin(DegreestoRadians(90 - temp_angle))));
                            x = (double) -(abs(sample.distance * cos(DegreestoRadians(90 - temp_angle))));

                        }
                        break;


                    case 3:

                        if (temp_angle < 45){

                            y = (double) -(abs(sample.distance * sin(DegreestoRadians(temp_angle))));
                            x = (double) -(abs(sample.distance * cos(DegreestoRadians(temp_angle))));            

                        }else{

                            y = (double) -(abs(sample.distance * cos(DegreestoRadians(90 - temp_angle))));
                            x = (double) -(abs(sample.distance * sin(DegreestoRadians(90 - temp_angle))));

                        }
                        break;


                    case 4:
                        if (temp_angle < 45){

                            y = (double) -(abs(sample.distance * cos(DegreestoRadians(temp_angle))));
                            x = (double) (abs(sample.distance * sin(DegreestoRadians(temp_angle))));            

                        }else{

                            y = (double) -(abs(sample.distance * sin(DegreestoRadians(90 - temp_angle))));
                            x = (double) (abs(sample.distance * cos(DegreestoRadians(90 - temp_angle))));

                        }
                        break;


                }   
                
                if ((x/100.0 < 6.0 && x/100.0 > -6.0) && (y/100.00 < 6.0 && y/100.0 > -6.0)){                of << x/100.0;
                of << " " << y/100.0 << " 0" << std::endl; 
                }
            }

        }

        device.stop_scanning();

    } catch (const sweep::device_error& e){

        std::cerr << "Error: " << e.what() << std::endl;
    }



    return 0; 

}

/*
int main (int argc, char** argv){

    int scan_stat;
    Scan *S = new Scan();
    scan_stat = S->perform_scan();

}
*/

#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <sweep/sweep.hpp>

class Scan{
    
    public:
        Scan();
        int perform_scan();
        void set_device_path(std::string input_path);
        std::string get_device_path();
        
    private:
       std::string devicepath;
       std::string sweeppath;
       std::vector <int> angles;
       std::vector <int> distance;
       int num_scans_per;
       int motor_speed;
       int sample_rate;
};


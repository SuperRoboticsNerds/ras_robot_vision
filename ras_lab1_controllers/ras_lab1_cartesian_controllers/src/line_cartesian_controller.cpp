#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "ras_arduino_msgs/PWM.h"
#include "ras_arduino_msgs/Encoders.h"
#include "geometry_msgs/Twist.h"
#include <sstream>
#include <math.h>





class LineCartesianControllerNode
{
public:

    ros::NodeHandle n;
    ros::Publisher twist_pub_;

    LineCartesianControllerNode()
    {
        n = ros::NodeHandle("~");
        twist_pub_ = n.advertise<geometry_msgs::Twist>("/motor_controller/twist", 1000);
	
    }

    ~LineCartesianControllerNode()
    {
        //delete motor_controller_;
    }


void twist_function()
    {
	twist_msg.linear.x=0.2;
	twist_msg.angular.z=0;
	twist_pub_.publish(twist_msg);
     }

private:


    geometry_msgs::Twist twist_msg;
    double pwm;
    double alpha;
    double desired_w_1;
    double desired_w_2;
};


int main(int argc, char **argv)
{
    
    ros::init(argc, argv, "line_cartesian_controller");
    LineCartesianControllerNode Line_Cartesian_Controller_Node;

    // Control @ 10 Hz
    double control_frequency = 10.0;

    ros::Rate loop_rate(control_frequency);
	// while (ros::ok())
    while(Line_Cartesian_Controller_Node.n.ok())
    {
    Line_Cartesian_Controller_Node.twist_function();

		
 

    ros::spinOnce();
    loop_rate.sleep();
    }

    return 0;
}

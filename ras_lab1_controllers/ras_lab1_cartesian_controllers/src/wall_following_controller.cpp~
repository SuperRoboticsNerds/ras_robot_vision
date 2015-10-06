#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "ras_arduino_msgs/PWM.h"
#include "ras_arduino_msgs/Encoders.h"
#include "ras_arduino_msgs/ADConverter.h"
#include "geometry_msgs/Twist.h"
#include <sstream>
#include <math.h>





class WallFollowingControllerNode
{
public:

    ros::NodeHandle n;
    ros::Publisher twist_pub_;
    ros::Subscriber distance_sub_;

    WallFollowingControllerNode()
    {
        n = ros::NodeHandle("~");
        twist_pub_ = n.advertise<geometry_msgs::Twist>("/motor_controller/twist", 1000);
	distance_sub_ = n.subscribe<ras_arduino_msgs::ADConverter>("/kobuki/adc",1,&WallFollowingControllerNode::distance_sensor_function,this);
    
	alpha=0.01;
	
    }

    ~WallFollowingControllerNode()
    {
        //delete motor_controller_;
    }


void distance_sensor_function(const ras_arduino_msgs::ADConverter distance_sensor_msg)
{

	distance_sensor1 = distance_sensor_msg.ch1;
	distance_sensor2 = distance_sensor_msg.ch2;
	difference=distance_sensor2 - distance_sensor1;
}

void twist_function()
    {
	twist_msg.linear.x=0.2;
	twist_msg.angular.z=alpha*difference;

	//twist_msg.linear.x=distance_sensor1;
	//twist_msg.angular.z=distance_sensor2;
	twist_pub_.publish(twist_msg);
     }

private:


    geometry_msgs::Twist twist_msg;
    double pwm;
    double alpha;
    double desired_w_1;
    double desired_w_2;
    double distance_sensor1;
    double distance_sensor2;
    double difference;
};


int main(int argc, char **argv)
{
    
    ros::init(argc, argv, "line_cartesian_controller");
    WallFollowingControllerNode Wall_Following_Controller_Node;

    // Control @ 10 Hz
    double control_frequency = 10.0;

    ros::Rate loop_rate(control_frequency);
	// while (ros::ok())
    while(Wall_Following_Controller_Node.n.ok())
    {
    Wall_Following_Controller_Node.twist_function();

		
 

    ros::spinOnce();
    loop_rate.sleep();
    }

    return 0;
}

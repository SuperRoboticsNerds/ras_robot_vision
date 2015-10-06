#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "ras_arduino_msgs/PWM.h"
#include "ras_arduino_msgs/Encoders.h"
#include <sstream>
void function(const ras_arduino_msgs::Encoders encoders_msg)
{


}
int main(int argc, char **argv)
{

  ros::init(argc, argv, "open_loop_controller");
  ros::NodeHandle n;
  ros::Subscriber encoders_sub_ = n.subscribe("/kobuki/encoders",1000,function);
  ros::Publisher pwm_pub_ = n.advertise<ras_arduino_msgs::PWM>("/kobuki/pwm", 1000);

  ros::Rate loop_rate(10);



  while (ros::ok())
  {
   	
   ras_arduino_msgs::PWM pwm_msg;


    
    pwm_msg.PWM1 = 255;
    pwm_msg.PWM2 = 255;

   

	
    pwm_pub_.publish(pwm_msg);

    ros::spinOnce();

    loop_rate.sleep();
    
  }


  return 0;
}

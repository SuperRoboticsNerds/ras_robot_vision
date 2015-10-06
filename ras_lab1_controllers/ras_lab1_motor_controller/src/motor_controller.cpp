#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "ras_arduino_msgs/PWM.h"
#include "ras_arduino_msgs/Encoders.h"
#include "geometry_msgs/Twist.h"
#include <sstream>
#include <math.h>





class MotorcontrollerNode
{
public:

    ros::NodeHandle n;
    ros::Publisher pwm_pub_;
    ros::Subscriber encoders_sub_;
    ros::Subscriber twist_sub_;

    MotorcontrollerNode()
    {
        n = ros::NodeHandle("~");
        //motor_controller_ = new KobukiMotors();

        wheel_radius_ = 0.05;
        base_ = 0.26;
	
	alpha1=0.1;
	beta1 = 0.2;

	alpha2=0.6;
	beta2 = 0.4;
        //estimated_w_1;
        //estimated_w_2;
        encoders_sub_ = n.subscribe("/arduino/encoders",1,&MotorcontrollerNode::encoder_function,this);
        twist_sub_ = n.subscribe<geometry_msgs::Twist>("/motor_controller/twist",1,&MotorcontrollerNode::twist_function,this);
        pwm_pub_ = n.advertise<ras_arduino_msgs::PWM>("/arduino/pwm", 1000);
    }

    ~MotorcontrollerNode()
    {
        //delete motor_controller_;
    }

void encoder_function(const ras_arduino_msgs::Encoders encoders_msg)
    {// degree/millisecond
	//Left
	  estimated_w_1 = (encoders_msg.delta_encoder1*2*M_PI*10)/(360);

	//Right
	  estimated_w_2 = (encoders_msg.delta_encoder2*2*M_PI*10)/(360);

	//estimated_w= ((estimated_w_2-estimated_w_1)/base_)*wheel_radius_;
    }

void PWM_function()
    {
//wheel one i gthe right one. Wheel two is the left
	//desired_w
	i_1 += beta1*(desired_w_1 - estimated_w_1);
	i_2 += beta2*(desired_w_2 - estimated_w_2);
	pwm_msg.PWM1 =-1*( alpha1*(desired_w_1 - estimated_w_1) + i_1 );
	pwm_msg.PWM2 = alpha2*(desired_w_2 - estimated_w_2) + i_2;
	//pwm_msg.PWM1 = estimated_w_2;
    	//pwm_msg.PWM2 = desired_w_2;


	ROS_INFO("%i,%i",pwm_msg.PWM1, pwm_msg.PWM2);
	pwm_pub_.publish(pwm_msg);
    }
void twist_function(const geometry_msgs::Twist twist_msg)
    {
	desired_w_1 = ((twist_msg.linear.x)+(base_/2)*twist_msg.angular.z)/wheel_radius_;

	desired_w_2 = ((twist_msg.linear.x)-(base_/2)*twist_msg.angular.z)/wheel_radius_;
     }

private:
  //  KobukiMotors *motor_controller_;

    // [0] corresponds to left wheel, [1] corresponds to right wheel

    double wheel_radius_;
    double base_;
    double estimated_w;
    double estimated_w_1;
    double estimated_w_2;
    ras_arduino_msgs::PWM pwm_msg;
    double alpha1;
    double alpha2;
    double desired_w_1;
    double desired_w_2;
    double i_1;
    double i_2;
    double beta1;
    double beta2;
};


int main(int argc, char **argv)
{
    
    ros::init(argc, argv, "closed_loop_controller");
    MotorcontrollerNode motor_controller_node;

    // Control @ 10 Hz
    double control_frequency = 10.0;

    ros::Rate loop_rate(control_frequency);
	// while (ros::ok())
    while(motor_controller_node.n.ok())
    {
    motor_controller_node.PWM_function();

		
 

    ros::spinOnce();
    loop_rate.sleep();
    }

    return 0;
}

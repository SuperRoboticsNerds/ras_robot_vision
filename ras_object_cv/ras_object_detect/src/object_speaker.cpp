#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <ras_msgs/Object_id.h>


#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include "std_msgs/String.h"


using namespace std;



const int LOOP_RATE = 10;
ros::Publisher  speech_publish;


void speechCallback(const ras_msgs::Object_id& obj_msg){
	int color_code = obj_msg.color;
	int shape_code = obj_msg.shape;
	std_msgs::String message;

	if(color_code == 3){
		message.data = "I see an orange star.";
	}

	if(color_code == 0){
		message.data = "I see a dark green cube.";
	}

	if(color_code == 1){
		message.data = "I see a red object";
	}

	if(color_code == 2){
		message.data = "I see a blue cube";
	}

	if(color_code == 4){
		message.data = "I see a light green hollow cube.";
	}

	if(color_code == 5){
		message.data = "I see a purple object";
	}

	if(color_code == 6){
		message.data = "I see a yellow ball";
	}

	speech_publish.publish(message);


}



int main(int argc, char ** argv){

	ros::init(argc, argv, "object_speaker_node");
	ros::NodeHandle node("~");
	ros::Subscriber  sub = node.subscribe("/object_pub", 1, speechCallback);
	speech_publish = node.advertise<std_msgs::String>("/espeak/string", 1);


	ros::Rate loop_rate(LOOP_RATE);
	ros::spin();
	return 0;

}
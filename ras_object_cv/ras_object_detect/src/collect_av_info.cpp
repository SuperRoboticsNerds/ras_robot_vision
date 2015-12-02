
#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <ras_object_lib/Image_Transfer.h>
#include <ras_msgs/Object_id.h>
#include <ras_msgs/Shape.h>
#include <ras_msgs/RAS_Evidence.h>


#include <string>
#include <sstream>
#include "std_msgs/String.h"


#define LOOP_RATE 10

class ObjectInfo{

public:
	ros::NodeHandle n;
	ros::Publisher speech_publish;	
	ros::Publisher obj_publish;
	ros::Subscriber  sub_color;
	ros::Subscriber  sub_shape;
	int shape_code;
	int color_code;
	int object_id;
	std::string obj_id;
	int group_num = 8;
	std_msgs::String message;
	
	
	ObjectInfo(){
		sub_color = n.subscribe("/object/color", 1, &ObjectInfo::color_cb, this);
		sub_shape = n.subscribe("/object/shape", 1, &ObjectInfo::shape_cb, this);
		speech_publish = n.advertise<std_msgs::String>("/espeak/string", 1);
		obj_publish = n.advertise<ras_msgs::Object_id>("/object/object", 1);

		// clusters_pub = n.advertise<std_msgs::String>("", 1);

	}


	void color_cb(const ras_msgs::Object_id& obj_msg){
		color_code = obj_msg.color;
		decisionRule(color_code, shape_code);
		// shape_code = obj_msg.shape;

	}


	void shape_cb(const ras_msgs::Shape& shape_msg){
		shape_code = shape_msg.shape;
		// decisionRule(color_code, shape_code);

	}


	void decisionRule(int color_id, int shape_id){
		// std_msgs::String message;

		object_id = 100;
		if(color_id == 2 && shape_id == 1){
			object_id = 8;
			message.data = "I see a blue triangle";
		}

		if(color_id == 2 && shape_id == 2){
			object_id = 2;
			message.data = "I see a blue cube";
		}

		if(color_id == 4 || (color_id == 0 && shape_id != 2)){
			object_id = 7;
			message.data = "I see a light green cylinder";
		}

		if(color_id == 0 ){
			object_id = 3;
			message.data = "I see a dark green cube";
		}

		if(color_id == 6 && shape_id == 5){
			object_id = 5;
			message.data = "I see a yellow ball";
		}

		if(color_id == 6 && shape_id != 5){
			object_id = 4;
			message.data = "I see a yellow cube";
		}

		if(color_id == 5){
			object_id = 9;
			message.data = "I see a purple cross";
		}

		if(color_id == 1 && shape_id == 5){
			object_id = 6;
			message.data = "I see a red ball";
		}

		if(color_code == 3) {
			object_id = 10;
			message.data = "I see an orange star.";
		}

		ras_msgs::Object_id obj;
		obj.object = object_id;
		obj.color = color_code;
		// obj.shape = shape_code;
		// ros::Time begin = ros::Time::now();
		// obj.time = begin;

		speech_publish.publish(message);
		obj_publish.publish(obj);

	}

};



int main(int argc, char ** argv){

	ros::init(argc, argv, "objinfo");
	// ros::NodeHandle node("~");
	ObjectInfo  oi;

	ros::Rate loop_rate(LOOP_RATE);

	while(oi.n.ok()){
		ros::spinOnce();
    	loop_rate.sleep();
	}

	ros::spin();
	return 0;

}


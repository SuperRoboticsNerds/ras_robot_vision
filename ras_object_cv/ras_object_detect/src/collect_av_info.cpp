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
	ros::Publisher ras_evidence_publish;
	ros::Subscriber  sub_color;
	ros::Subscriber  sub_shape;
	ros::Subscriber sub_small_img;
	int shape_code;
	int color_code;
	int object_id;
	float material;
	ras_msgs::Object_id obj;
	double x_pos;
	double y_pos;
	std::string obj_id;
	int group_num = 8;
	std_msgs::String message;
	ras_msgs::RAS_Evidence evidence;
	
	
	ObjectInfo(){
		sub_small_img = n.subscribe("/object/small_img", 1, &ObjectInfo::img_cb, this);
		sub_color = n.subscribe("/object/color", 1, &ObjectInfo::color_cb, this);
		sub_shape = n.subscribe("/object/shape", 1, &ObjectInfo::shape_cb, this);


		speech_publish = n.advertise<std_msgs::String>("/espeak/string", 1);
		obj_publish = n.advertise<ras_msgs::Object_id>("/object/object", 1);
		ras_evidence_publish = n.advertise<ras_msgs::RAS_Evidence>("/evidence", 1);
		// clusters_pub = n.advertise<std_msgs::String>("", 1);

	}



	void img_cb(const sensor_msgs::Image &img1){
		evidence.image_evidence = img1;
		evidence.group_number = 8;
		evidence.stamp = ros::Time::now();

	}



	void color_cb(const ras_msgs::Object_id& obj_msg){
		color_code = obj_msg.color;
		decisionRule(color_code, shape_code);
		// shape_code = obj_msg.shape;

	}



	void shape_cb(const ras_msgs::Shape& shape_msg){
		shape_code = shape_msg.shape;
		x_pos = shape_msg.x;
		y_pos = shape_msg.y;
		material = shape_msg.material;
		// decisionRule(color_code, shape_code);
	}


	void publishEverything(){
		speech_publish.publish(message);
		obj_publish.publish(obj);
		ras_evidence_publish.publish(evidence);

	}




	void decisionRule(int color_id, int shape_id){
		// std_msgs::String message;
		object_id = 100;
		obj.object = object_id;
		obj.color = color_code;
		obj.shape = shape_code;
		obj.x = x_pos;
		obj.y = y_pos;
		
		ros::Time begin = ros::Time::now();		

		if(color_id == 2 && shape_id != 2){
			object_id = 8;
			message.data = "I see a blue triangle";
			obj_id = "Blue Triangle";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;

		}


		if(color_id == 2 && shape_id == 2 ){
			object_id = 2;
			message.data = "I see a blue cube";
			obj_id = "Blue Cube";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;
		}


		if(color_id == 4 || (color_id == 0 && shape_id != 2) || (color_id == 4 && material == 0) || (color_id == 0 && material == 0) ){
			object_id = 7;
			message.data = "I see a light green cylinder";
			obj_id = "Green Cylinder";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;
		}


		if(color_id == 0 ){
			object_id = 3;
			message.data = "I see a dark green cube";
			obj_id = "Green Cube";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;

		}

		// if(color_id == 6 && shape_id == 5){
		// 	object_id = 5;
		// 	message.data = "I see a yellow ball";
		// 	obj_id = "Yellow Ball";
		// }

		if(color_id == 6 && shape_id != 5){
			object_id = 4;
			message.data = "I see a yellow cube";
			obj_id = "Yellow Cube";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;

		}


		if(color_id == 5){
			object_id = 9;
			message.data = "I see a purple cross";
			obj_id = "Purple Cross";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;
		}

		if((color_id == 1 && shape_id == 2) || (color_id == 1 && material ==1)){
			object_id = 1;
			message.data = "I see a red cube";
			obj_id = "Red Cube";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;
		}

		if(color_id == 1 && material == 0){
			object_id = 6;
			message.data = "I see a red hollow cube";
			obj_id = "Red Hollow Cube";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;

		}

		if(color_code == 3) {
			object_id = 10;
			message.data = "I see an orange star.";
			obj_id = "Patric";
			evidence.object_id = obj_id;
			obj.object = object_id;
			publishEverything();
			return;
		}

		object_id = 100;
		evidence.object_id = obj_id;
		obj.object = object_id;
		publishEverything();
		return;
		// obj.time = begin;


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
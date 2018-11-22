#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <vector>
#include <math.h>
#include <visualization_msgs/Marker.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Int32.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <geometry_msgs/Quaternion.h>
#include <sensor_msgs/LaserScan.h>


class Rotation {

public:
    ros::NodeHandle nh;

	ros::Subscriber sub_pose;
	ros::Subscriber sub_brain;
	ros::Subscriber sub_goal;

	ros::Publisher pub_velocity;
	ros::Publisher pub_has_reached_orientation;

    Rotation() {
        nh = ros::NodeHandle("~");

		sub_pose = nh.subscribe<geometry_msgs::Pose2D>("/pose", 1, &Rotation::poseCallBack, this);
		sub_goal = nh.subscribe<geometry_msgs::Pose2D>("/global_desired_pose", 10, &Rotation::globalDesiredPoseCallBack, this);

		pub_velocity = nh.advertise<geometry_msgs::Twist>("/motor_controller/velocity", 1);
		pub_has_reached_orientation = nh.advertise<std_msgs::Bool>("/has_reached_orientation", 1);

        reached_orientation_msg.data = false;
        previous_has_reached_orientation = true;
        newInfoAboutGoal = false;
        stopped = false;

        current_angle = M_PI / 2;
        desired_angle = M_PI / 2;

        double threshold_deg = 1;
        angle_threshold = degToRad(threshold_deg);
    }

	void poseCallBack(const geometry_msgs::Pose2D::ConstPtr& pose_msg) {
		current_angle = pose_msg->theta;
	}

	void globalDesiredPoseCallBack(const geometry_msgs::Pose2D::ConstPtr& global_desired_pose_msg) {
		desired_angle = global_desired_pose_msg->theta;
	}

    double degToRad(double degrees) {
		return degrees * M_PI / 180;
	}

    double getErrorAngle() {
        double error_angle = desired_angle - current_angle;

        // Remap to [0, 2pi]
        if (error_angle <= -M_PI) {
            error_angle += 2*M_PI;
        }
        else if (error_angle > M_PI) {
            error_angle -= 2*M_PI;
        }

        return error_angle;
	}

    void rotate() {

        reached_orientation_msg.data = false;

        double error_angle = getErrorAngle();
		double sign = error_angle / fabs(error_angle);

        if (fabs(error_angle) > angle_threshold) {
            ROS_INFO("Giving signal to motor");
            velocity_msg.linear.x = 0;
            velocity_msg.angular.z = sign * 1.2; // 1.2 is a okay value
            pub_velocity.publish(velocity_msg);
        }

        else {
            ROS_INFO("stopping, i.e. close enough");
            stop();
            // return the server bool
        }

        updateGoalInfo();
        if(newInfoAboutGoal){
			pub_has_reached_orientation.publish(reached_orientation_msg);
		}
		
	}

    void updateGoalInfo() {
		if(reached_orientation_msg.data != previous_has_reached_orientation){
			newInfoAboutGoal = true;
		}
		else{
			newInfoAboutGoal = false;
		}
		previous_has_reached_orientation = reached_orientation_msg.data;

	}
    
    void stop() {
		velocity_msg.linear.x = 0;
		velocity_msg.angular.z = 0;
        pub_velocity.publish(velocity_msg);
        stopped = true;
        reached_orientation_msg.data = true;
        
	}


private:

    int brain_state;
    bool stopped;
    double current_angle;
    double desired_angle;
    double angle_threshold;
    bool previous_has_reached_orientation;
    bool newInfoAboutGoal;

    geometry_msgs::Twist velocity_msg;
    std_msgs::Bool reached_orientation_msg;

};

int main(int argc, char** argv) {

	int control_frequency = 125;

	ros::init(argc, argv, "rotation");
    Rotation rotation;
	ros::Rate rate(control_frequency);

	while (rotation.nh.ok()) {
		ros::spinOnce();
        ROS_INFO("Going into rotation.rotate()!");
        rotation.rotate();
        rate.sleep();
		
	}

	return 0;
}
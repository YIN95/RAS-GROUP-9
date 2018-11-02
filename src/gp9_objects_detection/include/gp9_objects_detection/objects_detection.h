#include "ros/ros.h"
#include <math.h>
#include <vector>
#include <std_msgs/Float32.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#include <geometry_msgs/Pose2D.h>
#include <tf/transform_broadcaster.h>
#include <iostream>
#include <stdio.h>
#include <gp9_objects_detection/detection_util.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/PointStamped.h>
#include <string>
#include <std_msgs/Int32.h>


using namespace cv;
using namespace std;
class ObjectDetection{
public: /* ros */
	ros::NodeHandle nh;
    ros::Subscriber sub_image_rgb;
    ros::Subscriber sub_image_depth;
    ros::Subscriber sub_tensorflow_state;
    ros::Subscriber sub_classification_shape;

    ros::Publisher pub_object_pose;
    ros::Publisher pub_object_marker;
    ros::Publisher pub_object_marker_array;
    ros::Publisher pose_pub;
    //ros::Publisher pub_classification_target;

    visualization_msgs::Marker marker;
    visualization_msgs::MarkerArray marker_array;

    tf::TransformListener listener;
    
public:
    ObjectDetection();
    void imageRGBCallback(const sensor_msgs::ImageConstPtr &msg);
    void imageDepthCallback(const sensor_msgs::ImageConstPtr &msg);
    void detectAndDisplay(cv_bridge::CvImagePtr ptr);
    void colorFilter(Mat frame);
    void removeBackground(Mat frame);
    int colorFilter_is_object(Mat frame, int x, int y);
    int colorClassifier(int h, int s, int v, int b, int g, int r);
    void showresult(int txt);
    void showResult(int index, int obi);
    int getDepth(int x, int y);
    void pubPose(double x, double y, int type);
    void listen_obj_map(double x, double y, int type);
    bool check_pre_object(int temp);
    int getTrueDepth(int depth);
    Mat cropTarget(int x, int y);
    void saveTrainingData(Mat target);
    void publishClassificationTarget(Mat target);
    void stateCallback(const std_msgs::Int32ConstPtr &msg);
    void shapeCallback(const std_msgs::Int32ConstPtr &msg);
    int check_now_object();

public:
    static const int img_rgb_height = 480;
    static const int img_rgb_width = 640;
    static const double fx = 619.7237548828125;
    static const double cx = 304.5382995605469;
    int preDetectColor;
    int object_depth;
    int tensorflowState;
    cv_bridge::CvImagePtr cv_rgb_ptr;
    cv_bridge::CvImagePtr cv_depth_ptr;
    string cascade_name;
    CascadeClassifier cascade;
    std::vector<Rect> objects;
    ObjectColorShape obj;
    geometry_msgs::Pose2D pose;
    Mat origin_frame;
    int now_color;
    int now_shape;
    int now_object;

};


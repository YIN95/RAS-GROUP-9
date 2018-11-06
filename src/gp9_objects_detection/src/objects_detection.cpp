#include <gp9_objects_detection/objects_detection.h>
// roslaunch realsense_camera sr300_nodelet_rgbd.launch
// the following one is better
// roslaunch ras_camera ras_camera_nodelet.launch

int main(int argc, char** argv) 
{	

	ros::init(argc, argv, "object_detection");
	ObjectDetection objectDetection;

	ros::Rate rate(2);
	
	while (objectDetection.nh.ok()) {
        
		ros::spinOnce();
		rate.sleep();
	}
	return 0;
}

ObjectDetection::ObjectDetection(){
    
    object_depth = 99999;
    tensorflowState = 0;
    preDetectColor = 0;
    now_color = -1;
    now_shape = -1;
    now_object = -1;

	sub_image_rgb = nh.subscribe<sensor_msgs::Image>("/camera/rgb/image_rect_color", 1, &ObjectDetection::imageRGBCallback, this);
	// sub_image_depth = nh.subscribe<sensor_msgs::Image>("/camera/depth_registered/sw_registered/image_rect", 1, &ObjectDetection::imageDepthCallback, this);
   	sub_image_depth = nh.subscribe<sensor_msgs::Image>("/camera/depth/image_raw", 1, &ObjectDetection::imageDepthCallback, this);
    sub_tensorflow_state = nh.subscribe<std_msgs::Int32>("/classification/state", 1, &ObjectDetection::stateCallback, this);
    sub_classification_shape = nh.subscribe<std_msgs::Int32>("/classification/shape", 1, &ObjectDetection::shapeCallback, this);
    sub_rob_position = nh.subscribe<geometry_msgs::Pose2D>("/pose", 1, &ObjectDetection::robotCallback, this);
    sub_moving_state = nh.subscribe<std_msgs::Int32>("/state/ismoving", 1, &ObjectDetection::stateMovingCallback, this);

    pub_object_pose = nh.advertise<geometry_msgs::Pose2D>("/object/pose", 1);
    pub_object_marker = nh.advertise<visualization_msgs::Marker>("/object/marker", 1);
    pub_object_marker_array = nh.advertise<visualization_msgs::MarkerArray>("/object/marker_array", 10);
    pose_pub = nh.advertise<geometry_msgs::Pose2D>("/global_pose/object", 1);
    pub_speak = nh.advertise<std_msgs::String>("/espeak/string", 30);
    pub_evidence = nh.advertise<ras_msgs::RAS_Evidence>("/evidence", 5);
    //pub_classification_target = nh.advertise<geometry_msgs::Pose2D>("/classification/target", 1);

    char *buffer;
    buffer = getcwd(NULL, 0);
    string path = buffer;
    string filePath = "/gp9_objects_detection/src/cascade.xml";
    string fullPath = path + filePath;
    
    fullPath = "/home/ras19/catkin_ws/src/gp9_objects_detection/src/cascade2.xml";
    cascade_name = fullPath;
    //cascade.mode = ;
    if(!cascade.load(cascade_name)){ 
        ROS_ERROR("Error loading cascade!"); 
    };
}

void ObjectDetection::robotCallback(const geometry_msgs::Pose2D::ConstPtr &msg){
    robot_x = msg->x;
    robot_y = msg->y;
    return;
}

void ObjectDetection::stateCallback(const std_msgs::Int32ConstPtr &msg){
    tensorflowState = msg->data;

    ROS_INFO("tensorflow_state: %d", tensorflowState);
}

void ObjectDetection::stateMovingCallback(const std_msgs::Int32ConstPtr &msg){
    movingState = msg->data;

}

void ObjectDetection::shapeCallback(const std_msgs::Int32ConstPtr &msg){
    now_shape = msg->data;
    ROS_INFO("shape: %d", now_shape);
}

void ObjectDetection::imageRGBCallback(const sensor_msgs::ImageConstPtr &msg){
    try{
        cv_rgb_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        Mat image_hsv;
        Mat frame_threshold;
        cvtColor(cv_rgb_ptr->image, image_hsv, CV_BGR2HSV);
        inRange(image_hsv, Scalar(0, 90, 0), Scalar(180, 255, 255), frame_threshold);
        cvtColor(frame_threshold, frame_threshold, COLOR_GRAY2BGR);
        origin_frame = (cv_rgb_ptr->image).clone();
        bitwise_and((cv_rgb_ptr->image), frame_threshold, origin_frame_masked);
                
        //imshow("image_hsv", origin_frame_masked);
        //cv::waitKey(3);
        
        detectAndDisplay(cv_rgb_ptr);
    }
    catch (...){
        return;
    }
    // cv::imshow("RGB_WINDOW", cv_rgb_ptr->image);
    // cv::waitKey(3);    
}

void ObjectDetection::imageDepthCallback(const sensor_msgs::ImageConstPtr &msg){
    try{
        cv_depth_ptr = cv_bridge::toCvCopy(msg);
        // , sensor_msgs::image_encodings::mono8
    }
    catch (...){
        return;
    }
    // cv::imshow("DEPTH_WINDOW", cv_depth_ptr->image);
    // cv::waitKey(3);  
}

void ObjectDetection::detectAndDisplay(cv_bridge::CvImagePtr ptr)
{
    try{
        bool only_detect_one = true;
        int detect_size = 5;
        Mat frame = ptr->image;
        Mat frame_gray;
        Mat frame_target;
        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
        equalizeHist(frame_gray, frame_gray);
        //-- Detect objects
        cascade.detectMultiScale(frame_gray, objects, 1.1, 3, 0|CASCADE_SCALE_IMAGE, Size(90, 90), Size(170, 170));
        detect_size = std::min(int(objects.size()), detect_size);


        if (only_detect_one && objects.size()>0){
            int i;
            for (i=0; i<detect_size; i++){
                int center_x = objects[i].x + objects[i].width/2;
                int center_y = objects[i].y + objects[i].height/2;
                Point center(center_x, center_y);
                // ellipse(frame, center, Size(objects[0].width/2, objects[0].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
                int color_result = colorFilter_is_object(origin_frame_masked, center_x, center_y);
                object_depth = getDepth(center_x, center_y);
                object_depth = getTrueDepth(object_depth);
                ROS_INFO("DEPTH: %d", object_depth);
                // if (color_result > 0 && object_depth >= 0){

                if (color_result > 0){

                    ellipse(ptr->image, center, Size(4, 4), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
                    Point pt1(objects[i].x, objects[i].y);
                    Point pt2(objects[i].x + objects[i].width, objects[i].y + objects[i].height);
                    
                    
                    rectangle(ptr->image, pt1, pt2, Scalar( 255, 0, 255 ), 4, 8, 0 );
                    showResult(color_result, i+1);

                    pose.x = 1.0 * object_depth / 1000;
                    pose.y = -1.0 * pose.x / fx * (center_x - cx);
                    pose.theta = 0;
                    pub_object_pose.publish(pose);

                    frame_target = cropTarget(center_x, center_y);

                    if (tensorflowState == 0){
                        if (movingState == 0){
                            now_color = color_result;
                            publishClassificationTarget(frame_target);
                            //imshow("target", frame_target);
                            char keyt = (char)waitKey(1);
                        }
                        

                        // listen_obj_map(pose.x, pose.y, color_result);
                        // preDetectColor = color_result;

                        // if (preDetectColor != color_result){
                    }
                    int now_see = check_now_object();
                    // int now_see = check_now_object_color_shape();
                    
                    if ((now_see > 0)){
                        bool is_new_object = check_pre_object(now_object);
                        bool is_new_object_p = check_pre_object_by_position(now_object, pose.x, pose.y);
                        if (is_new_object_p){
                        // if (is_new_object && (object_depth >= 50) && (object_depth <=650)){
                            //frame_target = cropTarget(pose.x, pose.y);
                            speakResult();
                            evidence_frame = frame_target;
                            listen_obj_map(pose.x, pose.y, now_object);
                            evidence_id = getEvidenceID(now_object);
                            publishEvidence(evidence_id, evidence_frame, evidence_x, evidence_y);
                            //ROS_INFO("now::: %d", now_object);
                            //preDetectColor = now_object;
                        }
                    }
                    now_shape = -1;
                                        
                }
            }

        }
        else{
            for (size_t i = 0; i < objects.size(); i++ ){
                Point center(objects[i].x + objects[i].width/2, objects[i].y + objects[i].height/2);
                ellipse(frame, center, Size(objects[i].width/2, objects[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
            }
        }
        imshow("RESULT", frame);
        char keyr = (char)waitKey(1);
       
    }
    catch(...){
        return;
    }
}

void ObjectDetection::listen_obj_map(double x, double y, int type){
    geometry_msgs::PointStamped obj_tf_camera;
    obj_tf_camera.header.frame_id = "camera";
    obj_tf_camera.header.stamp = ros::Time();
    obj_tf_camera.point.x = x;
    obj_tf_camera.point.y = y;
    obj_tf_camera.point.z = 0.0;
    try{
        geometry_msgs::PointStamped obj_tf_map;
        listener.transformPoint("map", obj_tf_camera, obj_tf_map);

        ROS_INFO("camera: (%.2f, %.2f. %.2f) -----> map: (%.2f, %.2f, %.2f) at time %.2f",
            obj_tf_camera.point.x, obj_tf_camera.point.y, obj_tf_camera.point.z,
            obj_tf_map.point.x, obj_tf_map.point.y, obj_tf_map.point.z, obj_tf_map.header.stamp.toSec());
        geometry_msgs::Pose2D pose_;
        pose_.x = obj_tf_map.point.x;
        pose_.y = obj_tf_map.point.y;
        evidence_x = pose_.x;
        evidence_y = pose_.y;
        pose_pub.publish(pose_);
        pubPose(pose_.x, pose_.y, type);
    }
    catch(tf::TransformException& ex){
        ROS_ERROR("Received an exception trying to transform a point");
    }

    
}

void ObjectDetection::removeBackground(Mat frame){
    
    // OBJECTS_MIN = cv.Scalar(0, 40, 90)
    // OBJECTS_MAX = cv.Scalar(255, 255, 255)
}

void ObjectDetection::pubPose(double x, double y, int type){
    int id;
    id = marker_array.markers.size();
    ROS_INFO("ID:::: %d", id);
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time();
    marker.ns = "space";
    marker.id = id;
    marker.type = visualization_msgs::Marker::SPHERE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.x = x;
    marker.pose.position.y = y;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = robot_x;
    marker.pose.orientation.y = robot_y;
    marker.pose.orientation.z = type;
    marker.pose.orientation.w = 1.0;
    marker.scale.x = 0.1;
    marker.scale.y = 0.1;
    marker.scale.z = 0.1;
    marker.color.a = 1.0; // Don't forget to set the alpha!
    marker.color.r = 0.0;
    marker.color.g = 1.0;
    marker.color.b = 0.0;
    // //only if using a MESH_RESOURCE marker type:
    // marker.mesh_resource = "package://pr2_description/meshes/base_v0/base.dae";
    // pub_object_marker.publish(marker);

    marker_array.markers.push_back(marker);
    pub_object_marker_array.publish(marker_array);
        
    // ROS_INFO("SIZE: %d", xx);
    // vis_pub.publish(marker);
}

int ObjectDetection::colorFilter_is_object(Mat frame, int x, int y){
     
    try{
        Mat img_hsv;
        cvtColor(frame, img_hsv, CV_BGR2HSV);
        //!!!!!!!!!!!!!!!!!  BGR
        uchar* d_ = frame.ptr<uchar>(y); 
        uchar* d = img_hsv.ptr<uchar>(y); 

        int h = d[3 * x];            
        int s = d[x * 3 + 1];            
        int v = d[x * 3 + 2];
        int b = d_[3 * x];    
        int g = d_[x * 3 + 1];            
        int r = d_[x * 3 + 2];  

        if ((b==0)&&(g==0)&&(r==0)){
            return -1;
        }

        if (s < 40){
            ROS_INFO("-----------");
            ROS_INFO("center: (%d, %d)", x, y);
            ROS_INFO("BGR: (%d, %d, %d)", b, g, r);
            ROS_INFO("HSV: (%d, %d, %d)", h, s, v);
           
            return colorClassifier(h, s, v, b, g, r);
            return -1;
            
        }
        else{
            ROS_INFO("-----------");
            ROS_INFO("center: (%d, %d)", x, y);
            ROS_INFO("BGR: (%d, %d, %d)", b, g, r);
            ROS_INFO("HSV: (%d, %d, %d)", h, s, v);
           
            return colorClassifier(h, s, v, b, g, r);
        }
    }
    catch (...){
        ROS_ERROR("error on hsv");
    }
    return -1;  
}

int ObjectDetection::colorClassifier(int h, int s, int v, int b, int g, int r){
    bool use_rgb = false;
    bool use_bgr_hsv = true;

    if (use_bgr_hsv){
        if (s <40){
            return -1;
        }

        if (0 <= h && h <= 7 && 0 <= s && s <= 255 && 0 <= v && v <= 255){   
            ROS_INFO("COLOR_RED");
            return obj.COLOR_RED;
        }
        else if ((156 <= h && h <= 180) && (0 <= s && s <= 255) && (0 <= v && v <= 255)){
            ROS_INFO("COLOR_RED");
            return obj.COLOR_RED;
        }
        else if ((8 <= h && h <= 16) && (0 <= s && s <= 255) && (0 <= v && v <= 255)){
            ROS_INFO("COLOR_ORANGE");
            return obj.COLOR_ORANGE;
        }
        else if (17 <= h && h <= 34 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_YELLOW");
            return obj.COLOR_YELLOW;
        }
        else if ((35 <= h && h <= 66) && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_LIGHT_GREEN");
            return obj.COLOR_LIGHT_GREEN;
        }
        
        else if (67 <= h && h < 86 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_GREEN");
            return obj.COLOR_GREEN;
        }
        else if (87 <= h && h <= 128 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            if (v>150){
                ROS_INFO("COLOR_LIGHT_BLUE");
                return obj.COLOR_LIGHT_BLUE;
            }
            
        }
        else if (87 <= h && h <= 128 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            if (v<150){
                ROS_INFO("COLOR_BLUE");
                return obj.COLOR_BLUE;
            }
        }
        else if (129 <= h && h < 155 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_PURPLE");
            return obj.COLOR_PURPLE;
        }

        return -1;   
    }

    if (use_rgb){
        if (10 < b && b < 70 && 50 < g && g < 120 && 0 < r && r < 40){
            ROS_INFO("COLOR_GREEN");
            return obj.COLOR_GREEN;
        }
        else if ((0 <= b && b < 20) && (70 < g && g < 180) && (143 < r && r < 255)){
            ROS_INFO("COLOR_YELLOW");
            return obj.COLOR_YELLOW;
        }
        else if ((5 <= b && b < 30) && 80 < g && g < 160 && 50 < r && r < 125){
            ROS_INFO("COLOR_LIGHT_GREEN");
            return obj.COLOR_LIGHT_GREEN;
        }
        else if (0 <= b && b < 10 && 30 < g && g < 70 && 150 < r && r < 220){
            ROS_INFO("COLOR_ORANGE");
            return obj.COLOR_ORANGE;
        }
        else if (0 <= b && b < 30 && 0 <= g && g < 40 && 125 < r && r < 185){
            ROS_INFO("COLOR_RED");
            return obj.COLOR_RED;
        }
        else if (80 <= b && b < 160 && 79 < g && g < 130 && 0 <= r && r < 30){
            ROS_INFO("COLOR_LIGHT_BLUE");
            return obj.COLOR_LIGHT_BLUE;
        }
        else if (30 <= b && b < 79 && 40 < g && g < 110 && 0 <= r && r < 30){
            ROS_INFO("COLOR_BLUE");
            return obj.COLOR_BLUE;
        }
        else if (50 <= b && b < 180 && 40 < g && g < 100 && 90 < r && r < 180){
            ROS_INFO("COLOR_PURPLE");
            return obj.COLOR_PURPLE;
        }
        return -1;
    }
    else{
        // return 1;
        if (s <40){
            return -1;
        }
        if (35 <= h && h <= 54 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_GREEN");
            return obj.COLOR_GREEN;
        }
        else if ((90 <= h && h <= 107) && (0 <= s && s <= 255) && (0 <= v && v <= 255)){
            ROS_INFO("COLOR_YELLOW");
            return obj.COLOR_YELLOW;
        }
        else if ((55 <= h && h <= 89) && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_LIGHT_GREEN");
            return obj.COLOR_LIGHT_GREEN;
        }
        else if (108 <= h && h <= 117 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_ORANGE");
            return obj.COLOR_ORANGE;
        }
        else if (4 <= h && h < 17 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_RED");
            return obj.COLOR_RED;
        }
        else if (118 <= h && h <= 128 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_RED");
            return obj.COLOR_RED;
        }
        else if (18 <= h && h <= 35 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_LIGHT_BLUE");
            return obj.COLOR_LIGHT_BLUE;
        }
        else if (19 <= h && h <= 35 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_BLUE");
            return obj.COLOR_BLUE;
        }
        else if (129 <= h && h < 170 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_PURPLE");
            return obj.COLOR_PURPLE;
        }
        else if (0 <= h && h < 4 && 0 <= s && s <= 255 && 0 <= v && v <= 255){
            ROS_INFO("COLOR_PURPLE");
            return obj.COLOR_PURPLE;
        }
        return -1;        
    }
    
}

void ObjectDetection::showResult(int index, int obi){
    Point org(30, obi*50);
    String text;
    switch(index) {
        case 1:
            text = "YELLOW";
            break; 
        case 2:
            text = "GREEN";
            break;
        case 3:
            text = "LIGHT_GREEN";
            break; 
        case 4:
            text = "ORANGE";
            break;
        case 5:
            text = "RED";
            break; 
        case 6:
            text = "BLUE";
            break;
        case 7:
            text = "BLUE";
            break; 
        case 8:
            text = "PURPLE";
            break;
        default : //Optional
            return;
    }
    putText(cv_rgb_ptr->image, text, org, 1, 2.5, Scalar( 255, 0, 255 ), 4, 8, 0);
    
    return;
}

int ObjectDetection::getDepth(int x, int y){
    int i, j, depth, temp, count;
    depth = 0;
    count = 0;
    Mat frame_depth = cv_depth_ptr->image;
    for (i=x-10; i<x+10; i++){
        for (j=y-10; j<y+10; j++){
            uchar* d = frame_depth.ptr<uchar>(j);
            temp = d[2*i] + 255*d[2*i+1];
            // depth = max(depth, temp);
            if (temp > 50 && temp < 650){
                depth += temp;
                count++;
            }
        }
    }
    if (count > 0){
        depth = depth / count;
    }
    
    
    // uchar* d = frame_depth.ptr<uchar>(y); 
    // int depth = d[2*x] + 255*d[2*x+1];            
    return depth;
}

bool ObjectDetection::check_pre_object(int temp){
    int size = marker_array.markers.size();
    int index = 1;
    //ROS_INFO("SIZE: %d", size);

    if (size == 0){
        return true;
    }

    

    while((index <= 3) && (size - index >= 0)){
        // ROS_INFO("!!!%d , %s", temp, arker_array.markers[size-index].ns);
        if (marker_array.markers[size-index].pose.orientation.z == temp){
            return false;
        }
        index++;
    }
    return true;
}

bool ObjectDetection::check_pre_object_by_position(int temp, int x, int y){
    double gx;
    double gy;
    double Dis;
    double nx, ny;
    gx = robot_x;
    gy = robot_y;
    int size = marker_array.markers.size();
    int index = 1;
    
    if (size == 0){
        return true;
    }

    while (size - index >= 0){
        if(marker_array.markers[size-index].pose.orientation.z == temp){
            nx = marker_array.markers[size-index].pose.orientation.x;
            ny = marker_array.markers[size-index].pose.orientation.y;
            ROS_INFO("X1, %f, X2, %f", nx, gx);
            ROS_INFO("y1, %f, y2, %f", ny, gy);
            Dis = calculateDiatance(gx, gy, nx, ny);
            ROS_INFO("Dis, %f", Dis);
            if (Dis < 0.5){
                return false;
            }
        }
        index++;
    }

    return true;
}

int ObjectDetection::getTrueDepth(int depth){
    double TD;
    int height = 180;
    TD = sqrt(depth*depth - height*height);
    TD = std::max(int(TD), 0);
    return int(TD);
}

Mat ObjectDetection::cropTarget(int x, int y){
    Mat frame_global = cv_rgb_ptr->image;
    Mat frame_target;
    int sx, sy;
    int cropsize = 128;
    sx = x-(cropsize/2);
    sy = y-(cropsize/2);
    
    if ((x-(cropsize/2))<0){
        sx = 0;
    }

    if ((y-(cropsize/2))<0){
        sy = 0;
    }

    if ((x+(cropsize/2))>640){
        sx = 640-cropsize;
    }

    if ((y+(cropsize/2))>480){
        sy = 480-cropsize;
    }
    ROS_INFO("x, %d ; y, %d", x, y);
    ROS_INFO("sx, %d ; sy, %d", sx, sy);
    Rect rect(sx, sy, cropsize, cropsize);
    frame_target = origin_frame(rect);

    return frame_target;
}

void ObjectDetection::saveTrainingData(Mat target){
    
}

void ObjectDetection::publishClassificationTarget(Mat target){
    static image_transport::ImageTransport it(nh);
    static image_transport::Publisher pub_classification_target = it.advertise("/classification/image", 1);
    sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", target).toImageMsg();
    pub_classification_target.publish(msg);
    
    
    // try{
    //     cv_bridge::CvImagePtr ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    //     Mat frame = ptr->image;
    //     cv::imshow("target-pub", frame);
    //     cv::waitKey(3); 
    // }
    // catch (...){
    //     return;
    // }
    return;
}

int ObjectDetection::check_now_object_color_shape(){
    switch(now_shape) {
        case 0 :
            now_object = obj.Blue_Cube;
            ROS_INFO("I SEE BLUE CUBE");
            break;
        
        case 1 :
            now_object = obj.Blue_Triangle;
            ROS_INFO("I SEE BLUE TRIANGLE");
            break;
        
        case 2 :
            now_object = obj.Green_Cube;
            ROS_INFO("I SEE GREEN CUBE");
            break;
        
        case 3 :
            now_object = obj.Green_Cylinder;
            ROS_INFO("I SEE GREEN CYLINDER");
            break;
        
        case 4 :
            now_object = obj.Green_Hollow_Cube;
            ROS_INFO("I SEE GREEN HOLLOW CUBE");
            break;
        
        case 5 :
            now_object = obj.Orange_Cross;
            ROS_INFO("I SEE ORANGE CROSS");
            break;
        
        case 6 :
            now_object = obj.UNKNOWN;
            ROS_INFO("I SEE BLUE CUBE");
            break;
        
        case 7 :
            now_object = obj.Patric;
            ROS_INFO("I SEE PATRIC");
            break;
        
        case 8 :
            now_object = obj.Purple_Cross;
            ROS_INFO("I SEE PURPLE CROSS");
            break;
        
        case 9 :
            now_object = obj.Purple_Star;
            ROS_INFO("I SEE PURPLE STAR");
            break;
        
        case 10 :
            now_object = obj.Red_Ball;
            ROS_INFO("I SEE RED BA;;");
            break;
        
        case 11 :
            now_object = obj.Red_Cylinder;
            ROS_INFO("I SEE RED CYLINDER");
            break;
        
        case 12 :
            now_object = obj.Red_Hollow_Cube;
            ROS_INFO("I SEE RED HOLLOW CUBE");
            break;
        
        case 13 :
            now_object = obj.Yellow_Ball;
            ROS_INFO("I SEE YELLOW BALL");
            break;
        
        case 14 :
            now_object = obj.Yellow_Cube;
            ROS_INFO("I SEE YELLOW CUBE");
            break;
        
        default :
            ROS_INFO("classification error");
    }
 
    return now_object;
}

int ObjectDetection::check_now_object(){
    bool color_first = true;
    if (color_first){
        switch(now_color) {
        
            // yellow
            case 1 :
                if (now_shape == obj.SHAPE_BALL){
                    now_object = obj.Yellow_Ball;
                    ROS_INFO("I SEE YELLOW BALL");
                }
                else if (now_shape == obj.SHAPE_CUBE){
                    now_object = obj.Yellow_Cube;
                    ROS_INFO("I SEE YELLOW CUBE");
                }
                break;
            
            // green
            case 2 :
                if ((now_shape >= 0) &&(now_shape <999)){
                    now_object = obj.Green_Cube;
                    ROS_INFO("I SEE GREEN CUBE");
                }
                
         
                break;

            // light green
            case 3 :
                if (now_shape == obj.SHAPE_CYLINDER){
                    now_object = obj.Green_Cylinder;
                    ROS_INFO("I SEE GREEN CYLINDER");
                }
                else if (now_shape == obj.SHAPE_HOLLOW_CUBE){
                    now_object = obj.Green_Hollow_Cube;
                    ROS_INFO("I SEE GREEN HOLLOW CUBE");
                }
                // else if (now_shape == obj.SHAPE_CUBE){
                //     now_object = obj.Green_Hollow_Cube;
                //     ROS_INFO("I SEE GREEN HOLLOW CUBE");
                // }
                break;

            // orange
            case 4 :
                if (now_shape == obj.SHAPE_STAR){
                    now_object = obj.Patric;
                    ROS_INFO("I SEE PATRIC");
                }
                else if (now_shape == obj.SHAPE_CROSS){
                    now_object = obj.Orange_Cross;
                    ROS_INFO("I SEE ORANGE CROSS");
                }
               
                break;

            // red
            case 5 :
                if (now_shape == obj.SHAPE_BALL){
                    now_object = obj.Red_Ball;
                    ROS_INFO("I SEE RED BALL");
                }
                else if (now_shape == obj.SHAPE_CYLINDER){
                    now_object = obj.Red_Cylinder;
                    ROS_INFO("I SEE RED CYLINDER");
                }
                else if (now_shape == obj.SHAPE_HOLLOW_CUBE){
                    now_object = obj.Red_Hollow_Cube;
                    ROS_INFO("I SEE RED HOLLOW CUBE");
                }
                else if (now_shape == obj.SHAPE_CUBE){
                    now_object = obj.Red_Hollow_Cube;
                    ROS_INFO("I SEE RED HOLLOW CUBE");
                }
                
                break;

            // light blue
            case 6 :
                if ((now_shape >= 0) &&(now_shape <999)){
                    now_object = obj.Blue_Triangle;
                    ROS_INFO("I SEE BLUE TRIANGLE");
                }
                
         
                break;

            // blue
            case 7 :
                if ((now_shape >= 0) &&(now_shape <999)){
                    now_object = obj.Blue_Cube;
                    ROS_INFO("I SEE BLUE CUBE");
                }
                
         
                break;

            // purple
            case 8 :
                if (now_shape == obj.SHAPE_CROSS){
                    now_object = obj.Purple_Cross;
                    ROS_INFO("I SEE PURPLE CROSS");
                }
                else if (now_shape == obj.SHAPE_STAR){
                    now_object = obj.Purple_Star;
                    ROS_INFO("I SEE PURPLE STAR");
                }
                break;

            default :
                now_object = 99;
                ROS_INFO("classification error");
            
        }

    }
    else{
        switch(now_shape) {
        // BALL
        case 0 :
            if (now_color == obj.COLOR_YELLOW){
                now_object = obj.Yellow_Ball;
                ROS_INFO("I SEE YELLOW BALL");
            }
            else if (now_color == obj.COLOR_RED){
                now_object = obj.Red_Ball;
                ROS_INFO("I SEE RED BALL");
            }
            else{
                now_object = obj.UNKNOWN;
            }
            break;

        // CROSS    
        case 1 :
            if (now_color == obj.COLOR_ORANGE){
                now_object = obj.Orange_Cross;
                ROS_INFO("I SEE ORANGE CROSS");
            }
            else if (now_color == obj.COLOR_PURPLE){
                now_object = obj.Purple_Cross;
                ROS_INFO("I SEE PURPLE CROSS");
            }
            else{
                now_object = obj.UNKNOWN;
            }  
            break;
        
        // CUBE        
        case 2 :
            if (now_color == obj.COLOR_YELLOW){
                now_object = obj.Yellow_Cube;
                ROS_INFO("I SEE YELLOW CUBE");
            }
            else if (now_color == obj.COLOR_GREEN){
                now_object = obj.Green_Cube;
                ROS_INFO("I SEE GREEN CUBE");
            }
            else if (now_color == obj.COLOR_BLUE) {
                now_object = obj.Blue_Cube;
                ROS_INFO("I SEE BLUE CUBE");
            }
            else if (now_color == obj.COLOR_LIGHT_BLUE) {
                now_object = obj.Blue_Cube;
                ROS_INFO("I SEE BLUE CUBE");
            }
            else{
                now_object = obj.UNKNOWN;
            }    
            break;

        // CYLINDER        
        case 3 :
            if (now_color == obj.COLOR_GREEN || now_color == obj.COLOR_LIGHT_GREEN){
                now_object = obj.Green_Cylinder;
                ROS_INFO("I SEE GREEN CYLINDER");
            }
            else if (now_color == obj.COLOR_RED){
                now_object = obj.Red_Cylinder;
                ROS_INFO("I SEE RED CYLINDER");
            }
            else{
                now_object = obj.UNKNOWN;
            }    
            break;

        // HOLLOW CUBE        
        case 4 :
            if (now_color == obj.COLOR_LIGHT_GREEN){
                now_object = obj.Green_Hollow_Cube;
                ROS_INFO("I SEE GREEN HOLLOW CUBE");
            }
            else if (now_color == obj.COLOR_RED){
                now_object = obj.Red_Hollow_Cube;
                ROS_INFO("I SEE RED HOLLOW CUBE");
            }
            else if (now_color == obj.COLOR_GREEN){
                now_object = obj.Green_Cube;
                ROS_INFO("I SEE GREEN CUBE");
            }
            else if (now_color == obj.COLOR_BLUE){
                now_object = obj.Blue_Cube;
                ROS_INFO("I SEE BLUE CUBE");
            }
            else if (now_color == obj.COLOR_LIGHT_BLUE){
                now_object = obj.Blue_Triangle;
                ROS_INFO("I SEE BLUE TRIANGLE");
            }
            else{
                now_object = obj.UNKNOWN;
            }    
            break;

        // STAR
        case 5 :
            ROS_INFO("others");
            now_object = -1;
            break;

        // STAR
        case 6 :
            if (now_color == obj.COLOR_ORANGE){
                now_object = obj.Patric;
                ROS_INFO("I SEE PATRIC");
            }
            else if (now_color == obj.COLOR_PURPLE){
                now_object = obj.Purple_Star;
                ROS_INFO("I SEE PURPLE STAR");
            }
            else{
                now_object = obj.UNKNOWN;
            }  
            break;

        // TRIANGLE
        case 7 :
            if (now_color == obj.COLOR_BLUE || now_color == obj.COLOR_LIGHT_BLUE){
                now_object = obj.Blue_Triangle;
                ROS_INFO("I SEE BLUE TRIANGLE");
            }
            else if (now_color == obj.COLOR_RED){
                now_object = obj.Red_Hollow_Cube;
                ROS_INFO("I SEE Red HOLLOW CUBE");
            }
            else{
                now_object = obj.UNKNOWN;
            }
            break;
        

        default :
            now_object = 99;
            ROS_INFO("classification error");
        }
    }
    
 
    return now_object;
}

void ObjectDetection::speakResult(){
    String result;
    std_msgs::String msg;
    
    switch(now_object) {
        case 1 :
            result = "I see a yellow ball";
            break;

        case 2 :
            result = "I see a yellow cube";
            break;
        
        case 3 :
            result = "I see a green cube";
            break;

        case 4 :
            result = "I see a green cylinder";
            break;
        
        case 5 :
            result = "I see a green hollow cube";
            break;

        case 6 :
            result = "I see a orange cross";
            break;
        
        case 7 :
            result = "I see a Patric";
            break;

        case 8 :
            result = "I see a red cylinder";
            break;

        case 9 :
            result = "I see a red hollow cube";
            break;
        
        case 10 :
            result = "I see a red ball";
            break;

        case 11 :
            result = "I see a blue cube";
            break;
        
        case 12 :
            result = "I see a blue triangle";
            break;

        case 13 :
            result = "I see a purple cross";
            break;
        
        case 14 :
            result = "I see a purple star";
            break;
        
        case 99 :
            // result = "I see an object";
            result = "";
            break;
        default :
            result = "";
            ROS_INFO("speak error");
    }
    msg.data = result;
    pub_speak.publish(msg);
}

void ObjectDetection::publishEvidence(String object_id, Mat image, int x, int y){
    cv_bridge::CvImage img;
    img.header.stamp = ros::Time::now();
    img.encoding = sensor_msgs::image_encodings::BGR8;
    img.image = image;

    ras_msgs::RAS_Evidence msg;
    msg.stamp = ros::Time::now();
    msg.group_number = 9;
    msg.image_evidence = *img.toImageMsg();
    msg.object_id = object_id;

    geometry_msgs::TransformStamped location;
    location.transform.translation.x = x;
    location.transform.translation.y = y;
    pub_evidence.publish(msg);
}

String ObjectDetection::getEvidenceID(int index){
    switch(index) {
        case 1 :
            return "Yellow Ball";

        case 2 :
            return "Yellow Cube";
        
        case 3 :
            return "Green Cube";
        
        case 4 :
            return "Green Cylinder";
        
        case 5 :
            return "Green Hollow Cube";

        case 6 :
            return "Orange Cross";

        case 7 :
            return "Patric";

        case 8 :
            return "Red Cylinder";

        case 9 :
            return "Red Hollow Cube";
        
        case 10 :
            return "Red Ball";
        
        case 11 :
            return "Blue Cube";
        
        case 12 :
            return "Blue Triangle";

        case 13 :
            return "Purple Cross";

        case 14 :
            return "Purple Star";
        
        default :
            return "error";
    }
}

double ObjectDetection::calculateDiatance(double x1, double y1, double x2, double y2){
    double dis;
    double l1 = x1 - x2;
    double l2 = y1 - y2;
    dis = sqrt(l1*l1 + l2*l2);
    return dis;
}
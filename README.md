# RAS-GROUP-9
RAS-GROUP-9

* [RAS-GROUP-9](#ras-group-9)
    * [Milestone 1](#milestone-1)
        * [motor control command](#motor-control-command)

## Milestone 1

1. Robot be able to drive in the maze.  
2. Keyboard control.  
3. Have a dead reckoning system (odometry) on the robot. 
4. Use tf (ROS Transform library) to display the position of the robot 
and the current laser scan from the laser in rviz. 

*The commands for milestone*
0. To be able to connect, be on the same wifi as the NUC with your computer  
ssh ras19@192.168.1.18

1. First, roscore and source
``` c++
roscore  
```
2. motor controller
``` c++
roslaunch gp9_motor_controller motor_controller.launch 
```
3. Transforms
``` c++
roslaunch transforms transforms.launch 
```

4. Rviz
``` c++
rviz
```


### motor control command
First, roscore and source
``` c++
roscore  
source ./devel/setup.bash  
```
run the command in terminal
``` c++
rosrun phidgets motor _serial:=<SERIAL_NUMBER> __name:=<NODE_NAME> _name:=<NAME>
rosrun phidgets motor _serial:=482685 __name:=motor_left _name:=motor_left
rosrun phidgets motor _serial:=490601 __name:=motor_right _name:=motor_right 
```
To rotate motor, can publish /<NAME>/cmd_vel, run following command in terminal:
``` c++
rostopic pub -r 10 /motor_left/cmd_vel std_msgs/Float32 "data: 30.0"     
rostopic pub -r 10 /motor_right/cmd_vel std_msgs/Float32 "data: 30.0"   
```
To rotate motor with the given speed, run following command in terminal:
``` c++
rosrun gp9_motor_controller velocity_controller 
rosrun gp9_motor_controller motor_controller 
```
To read motor encoder you need to echo a topic by:
``` c++
rostopic echo /<NAME>/encoder
```
To change frequency of the encoder publishing by initiating motor node with frequency argument by:
``` c++
rosrun phidgets motor _serial:=<SERIAL_NUMBER> _name:=<NAME> _frequency:=<FREQUENCY>
```
Also can launch with the launch file
``` 
roslaunch gp9_motor_controller motor_controller.launch 
```
### Realsense command
roslaunch realsense_camera sr300_nodelet_rgbd.launch

frame: camera_rgb_frame  
topic: /camera/depth_registered/points  
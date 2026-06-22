Package ball_detector
=====================

Description
-----------

This package implements the green-ball detector used in Assignment 6.
It receives a ROS image topic, converts the image to OpenCV, thresholds the image in HSV
colour space, removes small noise with morphological operations, and publishes the detected
ball centroid and blob area.


Inputs
------

/output/moving_camera
Type: sensor_msgs/msg/Image
Used in the simulation workflow.

/image
Type: sensor_msgs/msg/Image
Used in the direct camera / hardware workflow.


Outputs
-------

/ball_position
Type: geometry_msgs/msg/Point

Point meaning:
- x = horizontal ball-centre coordinate in pixels
- y = vertical ball-centre coordinate in pixels
- z = detected blob area in pixels


Parameters
----------

image_topic
Type: string
Default: /image
Image topic to subscribe to.

point_topic
Type: string
Default: /ball_position
Topic used to publish the detected ball position.


Run
---

Simulation use:

ros2 run ball_detector detector_node --ros-args -p image_topic:=/output/moving_camera

Direct camera / hardware use:

ros2 run ball_detector detector_node --ros-args -p image_topic:=/image


Core components
---------------

BallDetector()
Creates the subscriptions, publisher, and configurable topic names.

image_callback()
Converts the ROS image to OpenCV, thresholds green pixels in HSV space, computes image
moments, and publishes the detected centroid and area if the blob is large enough.


Location in code
----------------

- src/ball_detector.cpp


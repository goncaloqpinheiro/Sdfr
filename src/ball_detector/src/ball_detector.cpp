//=============================================================================
// Authors : Goncalo Pinheiro, Ceyhun Ceylan
// Group : 17
// License : LGPL open source license
//
// Brief : Detects a green ball in the image stream and publishes its centroid.
//
//=============================================================================

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "geometry_msgs/msg/point.hpp"
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <string>

class BallDetector : public rclcpp::Node
{
public:
    BallDetector() : Node("ball_detector")
    {
        const auto image_topic = this->declare_parameter<std::string>("image_topic", "/image");
        const auto point_topic = this->declare_parameter<std::string>("point_topic", "/ball_position");

        image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            image_topic, 10,
            std::bind(&BallDetector::image_callback, this, std::placeholders::_1));

        point_pub_ = this->create_publisher<geometry_msgs::msg::Point>(point_topic, 10);

        RCLCPP_INFO(
            this->get_logger(),
            "Ball detector started (OpenCV HSV mode). image_topic=%s point_topic=%s",
            image_topic.c_str(),
            point_topic.c_str());
    }

private:
    static constexpr int    H_MIN    = 35,    H_MAX    = 85;
    static constexpr int    S_MIN    = 70,    S_MAX    = 255;
    static constexpr int    V_MIN    = 70,    V_MAX    = 255;
    static constexpr double MIN_AREA = 500.0;

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
    rclcpp::Publisher<geometry_msgs::msg::Point>::SharedPtr  point_pub_;

    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        // 1. Convert ROS image → OpenCV Mat
        cv::Mat frame;
        try {
            frame = cv_bridge::toCvShare(msg, "bgr8")->image;
        } catch (const cv_bridge::Exception & e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge error: %s", e.what());
            return;
        }

        // 2. BGR → HSV
        cv::Mat hsv;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        // 3. Threshold green pixels
        cv::Mat mask;
        cv::inRange(hsv,
            cv::Scalar(H_MIN, S_MIN, V_MIN),
            cv::Scalar(H_MAX, S_MAX, V_MAX),
            mask);

        // 4. Morphological cleanup
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN,  kernel);
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

        // 5. Centroid via moments
        cv::Moments m = cv::moments(mask, true);
        if (m.m00 < MIN_AREA) {
            RCLCPP_DEBUG(this->get_logger(), "No ball detected (area=%.0f)", m.m00);
            return;
        }

        // 6. Publish
        geometry_msgs::msg::Point ball_pos;
        ball_pos.x = m.m10 / m.m00;
        ball_pos.y = m.m01 / m.m00;
        ball_pos.z = m.m00;  // blob area — grows as ball gets closer

        RCLCPP_INFO(this->get_logger(),
            "Ball at X: %.1f  Y: %.1f  Area: %.0f px",
            ball_pos.x, ball_pos.y, ball_pos.z);

        point_pub_->publish(ball_pos);
    }
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BallDetector>());
    rclcpp::shutdown();
    return 0;
}

//=============================================================================
// Authors : Goncalo Pinheiro, Ceyhun Ceylan
// Group : 17
// License : LGPL open source license
//
// Brief : Implements the closed-loop controller that follows the detected ball.
//
//=============================================================================

#include "steering_sim.hpp"
#include <algorithm>
#include <cmath>

SteerSim::SteerSim()
: Node("relbot_sequence_controller"),
  last_ball_time_(0, 0, RCL_SYSTEM_TIME)
{
    image_width_ = static_cast<float>(this->declare_parameter("image_width", DEFAULT_WIDTH));
    target_ball_area_ = static_cast<float>(this->declare_parameter("target_ball_area", DEFAULT_TARGET_AREA));
    center_deadband_px_ =
        static_cast<float>(this->declare_parameter("center_deadband_px", DEFAULT_CENTER_DEADBAND));
    ball_timeout_ = this->declare_parameter("ball_timeout", DEFAULT_TIMEOUT);
    sim_sign_fix_ = this->declare_parameter("sim_sign_fix", false);
    const auto image_topic = this->declare_parameter<std::string>("image_topic", "/output/moving_camera");

    ball_x_ = image_width_ * 0.5f;

    RCLCPP_INFO(
        this->get_logger(),
        "Init - closed-loop ball tracking. image_topic=%s image_width=%.1f target_ball_area=%.1f timeout=%.2f",
        image_topic.c_str(),
        image_width_,
        target_ball_area_,
        ball_timeout_);
    last_ball_time_ = this->now();

    left_wheel_pub_ = this->create_publisher<example_interfaces::msg::Float64>(
        "/input/left_motor/setpoint_vel", 1);
    right_wheel_pub_ = this->create_publisher<example_interfaces::msg::Float64>(
        "/input/right_motor/setpoint_vel", 1);

    ball_sub_ = this->create_subscription<geometry_msgs::msg::Point>(
        "/ball_position", 10,
        std::bind(&SteerSim::ball_callback, this, std::placeholders::_1));
    image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
        image_topic, 10,
        std::bind(&SteerSim::image_callback, this, std::placeholders::_1));

    timer_ = this->create_wall_timer(
        std::chrono::duration<double>(1.0 / LOOP_HZ),
        std::bind(&SteerSim::calculate_velocity, this));
}

void SteerSim::ball_callback(const geometry_msgs::msg::Point::SharedPtr msg)
{
    ball_x_         = msg->x;
    ball_y_         = msg->y;
    ball_area_      = msg->z;
    search_direction_ = (ball_x_ >= (image_width_ * 0.5f)) ? 1.0f : -1.0f;
    ball_seen_      = true;
    last_ball_time_ = this->now();
}

void SteerSim::image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
{
    image_width_ = static_cast<float>(msg->width);
}

void SteerSim::calculate_velocity()
{
    if (!ball_seen_ || (this->now() - last_ball_time_).seconds() > ball_timeout_) {
        search_for_ball();
        return;
    }

    const float image_cx = image_width_ * 0.5f;
    float orientation = 0.0f;
    if (image_cx > 0.0f) {
        orientation = (ball_x_ - image_cx) / image_cx;
    }
    orientation = std::clamp(orientation, -1.0f, 1.0f);

    if (std::abs(ball_x_ - image_cx) < center_deadband_px_) {
        orientation = 0.0f;
    }

    const float area_ratio =
        target_ball_area_ > 0.0f ? std::clamp(ball_area_ / target_ball_area_, 0.0f, 2.0f) : 0.0f;

    if (area_ratio >= 1.0f) {
        RCLCPP_INFO_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000, "Ball reached - stopping.");
        publish(0.0, 0.0);
        return;
    }

    float v = K_V * (1.0f - area_ratio);
    v = std::clamp(v, V_MIN, V_MAX);

    float omega = K_STEER * orientation;
    if (sim_sign_fix_) {
        omega = -omega;
    }
    omega = std::clamp(omega, -OMEGA_MAX, OMEGA_MAX);

    v *= std::max(0.25f, 1.0f - TURN_SLOWDOWN * std::abs(orientation));

    float left_vel  =  v - omega;
    float right_vel =  v + omega;

    left_vel  = std::clamp(left_vel,  -V_MAX, V_MAX);
    right_vel = std::clamp(right_vel, -V_MAX, V_MAX);

    RCLCPP_INFO_THROTTLE(
        this->get_logger(),
        *this->get_clock(),
        500,
        "track: x=%.1f y=%.1f area=%.1f orient=%.2f area_ratio=%.2f left=%.2f right=%.2f",
        ball_x_,
        ball_y_,
        ball_area_,
        orientation,
        area_ratio,
        -left_vel,
        right_vel);

    if (sim_sign_fix_) {
        publish(left_vel, right_vel);
    } else {
        publish(-left_vel, right_vel);
    }
}

void SteerSim::search_for_ball()
{
    const float search_command = SEARCH_SPEED * search_direction_;

    RCLCPP_WARN_THROTTLE(
        this->get_logger(),
        *this->get_clock(),
        1000,
        "No recent ball detection - searching. search_command=%.2f",
        search_command);

    publish(search_command, search_command);
}

void SteerSim::publish(float left, float right)
{
    example_interfaces::msg::Float64 l, r;

    // --> Apply the hardware flip here! <--
    /* If the RIGHT wheel is the one driving backward, use -right:
    l.data = left;
    r.data = -right; 
    */

    // If you test it and realize it was actually the LEFT wheel 
    //that was backward, swap it to this instead:
    l.data = -left;
    r.data = right;

    // ADD THIS LINE TO PROVE IT WORKS:
    RCLCPP_INFO(this->get_logger(), "PUBLISHING FLIPPED: L=%.2f, R=%.2f", l.data, r.data);
    

    left_wheel_pub_->publish(l);
    right_wheel_pub_->publish(r);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SteerSim>());
    rclcpp::shutdown();
    return 0;
}

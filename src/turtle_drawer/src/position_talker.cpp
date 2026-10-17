#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"

class PositionTalker : public rclcpp::Node {
public:
    PositionTalker() : Node("position_talker") {
        subscription_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            [this](const turtlesim::msg::Pose::SharedPtr msg) {
                RCLCPP_INFO(
                    this->get_logger(),
                    "Posisi turtle: x = %.2f, y = %.2f, theta = %.2f rad",
                    msg->x,
                    msg->y,
                    msg->theta
                );
            }
        );
    }

private:
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<PositionTalker>();

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
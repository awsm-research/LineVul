GesturePoint::GesturePoint()
: first_touch_time_(0.0),
last_touch_time_(0.0),
last_tap_time_(0.0),
      velocity_calculator_(kBufferedPoints) {
}

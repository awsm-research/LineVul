bool GesturePoint::IsSecondClickInsideManhattanSquare(
const TouchEvent& event) const {
int manhattanDistance = abs(event.x() - last_tap_position_.x()) +
abs(event.y() - last_tap_position_.y());
  return manhattanDistance < kMaximumTouchMoveInPixelsForClick;
}

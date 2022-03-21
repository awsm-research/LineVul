void PartialMagnificationController::OnWindowDestroying(aura::Window* window) {
CloseMagnifierWindow();

aura::Window* new_root_window = GetCurrentRootWindow();
if (new_root_window != window)
    SwitchTargetRootWindow(new_root_window);
}

void WallpaperManager::SetDefaultWallpaperPath(
const base::FilePath& default_small_wallpaper_file,
std::unique_ptr<gfx::ImageSkia> small_wallpaper_image,
const base::FilePath& default_large_wallpaper_file,
std::unique_ptr<gfx::ImageSkia> large_wallpaper_image) {
default_small_wallpaper_file_ = default_small_wallpaper_file;
default_large_wallpaper_file_ = default_large_wallpaper_file;

ash::WallpaperController* controller =
ash::Shell::Get()->wallpaper_controller();

// |need_update_screen| is true if the previous default wallpaper is visible
// now, so we need to update wallpaper on the screen.
//
// Layout is ignored here, so wallpaper::WALLPAPER_LAYOUT_CENTER is used
// as a placeholder only.
const bool need_update_screen =
default_wallpaper_image_.get() &&
controller->WallpaperIsAlreadyLoaded(default_wallpaper_image_->image(),
false /* compare_layouts */,
wallpaper::WALLPAPER_LAYOUT_CENTER);

default_wallpaper_image_.reset();
if (GetAppropriateResolution() == WALLPAPER_RESOLUTION_SMALL) {
if (small_wallpaper_image) {
default_wallpaper_image_.reset(
new user_manager::UserImage(*small_wallpaper_image));
default_wallpaper_image_->set_file_path(default_small_wallpaper_file);
}
} else {
if (large_wallpaper_image) {
default_wallpaper_image_.reset(
new user_manager::UserImage(*large_wallpaper_image));
default_wallpaper_image_->set_file_path(default_large_wallpaper_file);
}
}

  if (need_update_screen)
    DoSetDefaultWallpaper(EmptyAccountId(), MovableOnDestroyCallbackHolder());
}

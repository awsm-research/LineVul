void WallpaperManager::DoSetDefaultWallpaper(
const AccountId& account_id,
MovableOnDestroyCallbackHolder on_finish) {
// There is no visible wallpaper in kiosk mode.
if (user_manager::UserManager::Get()->IsLoggedInAsKioskApp())
return;
wallpaper_cache_.erase(account_id);

WallpaperResolution resolution = GetAppropriateResolution();
const bool use_small = (resolution == WALLPAPER_RESOLUTION_SMALL);

const base::FilePath* file = NULL;

const user_manager::User* user =
user_manager::UserManager::Get()->FindUser(account_id);

if (user_manager::UserManager::Get()->IsLoggedInAsGuest()) {
file =
use_small ? &guest_small_wallpaper_file_ : &guest_large_wallpaper_file_;
} else if (user && user->GetType() == user_manager::USER_TYPE_CHILD) {
file =
use_small ? &child_small_wallpaper_file_ : &child_large_wallpaper_file_;
} else {
file = use_small ? &default_small_wallpaper_file_
: &default_large_wallpaper_file_;
}
wallpaper::WallpaperLayout layout =
use_small ? wallpaper::WALLPAPER_LAYOUT_CENTER
: wallpaper::WALLPAPER_LAYOUT_CENTER_CROPPED;
DCHECK(file);
if (!default_wallpaper_image_.get() ||
default_wallpaper_image_->file_path() != *file) {
default_wallpaper_image_.reset();
if (!file->empty()) {
loaded_wallpapers_for_test_++;
      StartLoadAndSetDefaultWallpaper(*file, layout, std::move(on_finish),
&default_wallpaper_image_);
return;
}

CreateSolidDefaultWallpaper();
}
  // 1x1 wallpaper is actually solid color, so it should be stretched.
  if (default_wallpaper_image_->image().width() == 1 &&
      default_wallpaper_image_->image().height() == 1)
    layout = wallpaper::WALLPAPER_LAYOUT_STRETCH;
  WallpaperInfo info(default_wallpaper_image_->file_path().value(), layout,
                     wallpaper::DEFAULT, base::Time::Now().LocalMidnight());
  SetWallpaper(default_wallpaper_image_->image(), info);
}

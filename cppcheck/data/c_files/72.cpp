void TranslateInfoBarBase::Layout() {
// Layout the close button.
  InfoBar::Layout();

// Layout the icon on left of bar.
gfx::Size icon_ps = icon_->GetPreferredSize();
  icon_->SetBounds(InfoBar::kHorizontalPadding, InfoBar::OffsetY(this, icon_ps),
      icon_ps.width(), icon_ps.height());
}

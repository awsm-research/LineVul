void ObserverOnLogoAvailable(LogoObserver* observer,
                             bool from_cache,
                             LogoCallbackReason type,
                             const base::Optional<Logo>& logo) {
  switch (type) {
    case LogoCallbackReason::DISABLED:
    case LogoCallbackReason::CANCELED:
    case LogoCallbackReason::FAILED:
      break;
    case LogoCallbackReason::REVALIDATED:
      // TODO(sfiera): double-check whether we should inform the observer of the
      // fresh metadata.
      break;
    case LogoCallbackReason::DETERMINED:
      observer->OnLogoAvailable(logo ? &logo.value() : nullptr, from_cache);
      break;
  }
  if (!from_cache) {
    observer->OnObserverRemoved();
  }
}

void HTMLImportsController::Dispose() {
  for (const auto& loader : loaders_)
    loader->Dispose();
  loaders_.clear();

if (root_) {
root_->Dispose();
root_.Clear();
}
}

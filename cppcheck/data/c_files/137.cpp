bool Extension::HasAPIPermission(const std::string& function_name) const {
base::AutoLock auto_lock(runtime_data_lock_);
return runtime_data_.GetActivePermissions()->
      HasAccessToFunction(function_name);
}

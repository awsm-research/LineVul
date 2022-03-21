String InspectorPageAgent::CachedResourceTypeJson(
const Resource& cached_resource) {
  return ResourceTypeJson(CachedResourceType(cached_resource));
}

void LocalFileSystem::deleteFileSystem(ExecutionContext* context, FileSystemType type, PassOwnPtr<AsyncFileSystemCallbacks> callbacks)
{
RefPtrWillBeRawPtr<ExecutionContext> contextPtr(context);
ASSERT(context);
ASSERT_WITH_SECURITY_IMPLICATION(context->isDocument());

    RefPtr<CallbackWrapper> wrapper = adoptRef(new CallbackWrapper(callbacks));
requestFileSystemAccessInternal(context,
bind(&LocalFileSystem::deleteFileSystemInternal, this, contextPtr, type, wrapper),
bind(&LocalFileSystem::fileSystemNotAllowedInternal, this, contextPtr, wrapper));
}

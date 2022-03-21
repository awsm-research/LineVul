virtual void scheduleBeginFrameAndCommit()
{
        CCMainThread::postTask(m_proxy->createBeginFrameAndCommitTaskOnCCThread());
}

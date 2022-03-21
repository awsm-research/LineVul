void ProcessControlLaunched() {
base::ScopedAllowBlockingForTesting allow_blocking;
base::ProcessId service_pid;
EXPECT_TRUE(GetServiceProcessData(NULL, &service_pid));
EXPECT_NE(static_cast<base::ProcessId>(0), service_pid);
#if defined(OS_WIN)
service_process_ =
base::Process::OpenWithAccess(service_pid,
SYNCHRONIZE | PROCESS_QUERY_INFORMATION);
#else
service_process_ = base::Process::Open(service_pid);
#endif
EXPECT_TRUE(service_process_.IsValid());
    // Quit the current message. Post a QuitTask instead of just calling Quit()
    // because this can get invoked in the context of a Launch() call and we
    // may not be in Run() yet.
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::RunLoop::QuitCurrentWhenIdleClosureDeprecated());
}

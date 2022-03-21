FileStream::FileStream(base::File file,
const scoped_refptr<base::TaskRunner>& task_runner)
    : context_(base::MakeUnique<Context>(std::move(file), task_runner)) {}

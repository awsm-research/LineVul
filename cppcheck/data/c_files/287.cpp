ServiceWorkerHandler::ServiceWorkerHandler()
: DevToolsDomainHandler(ServiceWorker::Metainfo::domainName),
enabled_(false),
      process_(nullptr),
weak_factory_(this) {}

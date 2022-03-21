void WebProcessProxy::addExistingWebPage(WebPageProxy* webPage, uint64_t pageID)
{
m_pageMap.set(pageID, webPage);
globalPageMap().set(pageID, webPage);
#if PLATFORM(MAC)
    if (pageIsProcessSuppressible(webPage));
m_processSuppressiblePages.add(pageID);
updateProcessSuppressionState();
#endif
}

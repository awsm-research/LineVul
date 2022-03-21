void WebPagePrivate::didComposite()
{
if (!m_page->settings()->developerExtrasEnabled())
return;
    InspectorInstrumentation::didComposite(m_page);
}

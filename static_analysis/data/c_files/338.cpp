PassRefPtr<DocumentFragment> Range::createContextualFragment(const String& markup, ExceptionCode& ec)
{
if (!m_start.container()) {
ec = INVALID_STATE_ERR;
return 0;
}

Node* element = m_start.container()->isElementNode() ? m_start.container() : m_start.container()->parentNode();
if (!element || !element->isHTMLElement()) {
ec = NOT_SUPPORTED_ERR;
return 0;
}

    RefPtr<DocumentFragment> fragment = createDocumentFragmentForElement(markup, toElement(element), AllowScriptingContentAndDoNotMarkAlreadyStarted);

if (!fragment) {
ec = NOT_SUPPORTED_ERR;
return 0;
}

return fragment.release();
}

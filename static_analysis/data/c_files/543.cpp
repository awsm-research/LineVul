void LinkChangeSerializerMarkupAccumulator::appendAttribute(StringBuilder& result, Element* element, const Attribute& attribute, Namespaces* namespaces)
{
    if (m_replaceLinks && element->isURLAttribute(attribute) && !element->isJavaScriptURLAttribute(attribute)) {
        String completeURL = m_document->completeURL(attribute.value());
        if (m_replaceLinks->contains(completeURL)) {
            // FIXME: Refactor MarkupAccumulator so it is easier to append an attribute like this.
            result.append(' ');
            result.append(attribute.name().toString());
            result.appendLiteral("=\"");
            if (!m_directoryName.isEmpty()) {
                result.appendLiteral("./");
                result.append(m_directoryName);
                result.append('/');
            }
            result.append(m_replaceLinks->get(completeURL));
            result.appendLiteral("\"");
            return;
        }
    }
    MarkupAccumulator::appendAttribute(result, element, attribute, namespaces);
}

Node::InsertionNotificationRequest HTMLBodyElement::insertedInto(ContainerNode* insertionPoint)
{
HTMLElement::insertedInto(insertionPoint);
    if (insertionPoint->inDocument()) {
        // FIXME: It's surprising this is web compatible since it means a marginwidth
        // and marginheight attribute can magically appear on the <body> of all documents
        // embedded through <iframe> or <frame>.
        Element* ownerElement = document().ownerElement();
        if (isHTMLFrameElementBase(ownerElement)) {
            HTMLFrameElementBase& ownerFrameElement = toHTMLFrameElementBase(*ownerElement);
            int marginWidth = ownerFrameElement.marginWidth();
            if (marginWidth != -1)
                setIntegralAttribute(marginwidthAttr, marginWidth);
            int marginHeight = ownerFrameElement.marginHeight();
            if (marginHeight != -1)
                setIntegralAttribute(marginheightAttr, marginHeight);
        }
    }
    return InsertionDone;
}

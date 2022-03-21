PassRefPtr<DocumentFragment> XSLTProcessor::transformToFragment(Node* sourceNode, Document* outputDoc)
{
String resultMIMEType;
String resultString;
String resultEncoding;

// If the output document is HTML, default to HTML method.
if (outputDoc->isHTMLDocument())
resultMIMEType = "text/html";

if (!transformToString(sourceNode, resultMIMEType, resultString, resultEncoding))
return 0;
    return createFragmentFromSource(resultString, resultMIMEType, outputDoc);
}

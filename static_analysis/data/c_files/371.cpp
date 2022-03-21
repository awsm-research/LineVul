xmlParseConditionalSections(xmlParserCtxtPtr ctxt) {
int id = ctxt->input->id;

SKIP(3);
SKIP_BLANKS;
if (CMP7(CUR_PTR, 'I', 'N', 'C', 'L', 'U', 'D', 'E')) {
SKIP(7);
SKIP_BLANKS;
if (RAW != '[') {
xmlFatalErr(ctxt, XML_ERR_CONDSEC_INVALID, NULL);
} else {
if (ctxt->input->id != id) {
xmlValidityError(ctxt, XML_ERR_ENTITY_BOUNDARY,
"All markup of the conditional section is not in the same entity\n",
NULL, NULL);
}
NEXT;
}
if (xmlParserDebugEntities) {
if ((ctxt->input != NULL) && (ctxt->input->filename))
xmlGenericError(xmlGenericErrorContext,
"%s(%d): ", ctxt->input->filename,
ctxt->input->line);
xmlGenericError(xmlGenericErrorContext,
"Entering INCLUDE Conditional Section\n");
}

	while ((RAW != 0) && ((RAW != ']') || (NXT(1) != ']') ||
	       (NXT(2) != '>'))) {
const xmlChar *check = CUR_PTR;
unsigned int cons = ctxt->input->consumed;

if ((RAW == '<') && (NXT(1) == '!') && (NXT(2) == '[')) {
xmlParseConditionalSections(ctxt);
} else if (IS_BLANK_CH(CUR)) {
NEXT;
} else if (RAW == '%') {
xmlParsePEReference(ctxt);
} else
xmlParseMarkupDecl(ctxt);

/*
* Pop-up of finished entities.
*/
while ((RAW == 0) && (ctxt->inputNr > 1))
xmlPopInput(ctxt);

if ((CUR_PTR == check) && (cons == ctxt->input->consumed)) {
xmlFatalErr(ctxt, XML_ERR_EXT_SUBSET_NOT_FINISHED, NULL);
break;
}
}
if (xmlParserDebugEntities) {
if ((ctxt->input != NULL) && (ctxt->input->filename))
xmlGenericError(xmlGenericErrorContext,
"%s(%d): ", ctxt->input->filename,
ctxt->input->line);
xmlGenericError(xmlGenericErrorContext,
"Leaving INCLUDE Conditional Section\n");
}

} else if (CMP6(CUR_PTR, 'I', 'G', 'N', 'O', 'R', 'E')) {
int state;
xmlParserInputState instate;
int depth = 0;

SKIP(6);
SKIP_BLANKS;
if (RAW != '[') {
xmlFatalErr(ctxt, XML_ERR_CONDSEC_INVALID, NULL);
} else {
if (ctxt->input->id != id) {
xmlValidityError(ctxt, XML_ERR_ENTITY_BOUNDARY,
"All markup of the conditional section is not in the same entity\n",
NULL, NULL);
}
NEXT;
}
if (xmlParserDebugEntities) {
if ((ctxt->input != NULL) && (ctxt->input->filename))
xmlGenericError(xmlGenericErrorContext,
"%s(%d): ", ctxt->input->filename,
ctxt->input->line);
xmlGenericError(xmlGenericErrorContext,
"Entering IGNORE Conditional Section\n");
}

/*
* Parse up to the end of the conditional section
* But disable SAX event generating DTD building in the meantime
*/
state = ctxt->disableSAX;
instate = ctxt->instate;
if (ctxt->recovery == 0) ctxt->disableSAX = 1;
ctxt->instate = XML_PARSER_IGNORE;

	while ((depth >= 0) && (RAW != 0)) {
if ((RAW == '<') && (NXT(1) == '!') && (NXT(2) == '[')) {
depth++;
SKIP(3);
continue;
}
if ((RAW == ']') && (NXT(1) == ']') && (NXT(2) == '>')) {
if (--depth >= 0) SKIP(3);
continue;
}
NEXT;
continue;
}

ctxt->disableSAX = state;
ctxt->instate = instate;

if (xmlParserDebugEntities) {
if ((ctxt->input != NULL) && (ctxt->input->filename))
xmlGenericError(xmlGenericErrorContext,
"%s(%d): ", ctxt->input->filename,
ctxt->input->line);
xmlGenericError(xmlGenericErrorContext,
"Leaving IGNORE Conditional Section\n");
}

} else {
xmlFatalErr(ctxt, XML_ERR_CONDSEC_INVALID_KEYWORD, NULL);
}

if (RAW == 0)
SHRINK;

if (RAW == 0) {
xmlFatalErr(ctxt, XML_ERR_CONDSEC_NOT_FINISHED, NULL);
} else {
if (ctxt->input->id != id) {
xmlValidityError(ctxt, XML_ERR_ENTITY_BOUNDARY,
"All markup of the conditional section is not in the same entity\n",
NULL, NULL);
}
SKIP(3);
}
}

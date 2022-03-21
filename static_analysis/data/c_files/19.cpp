xmlParseAttValueComplex(xmlParserCtxtPtr ctxt, int *attlen, int normalize) {
xmlChar limit = 0;
xmlChar *buf = NULL;
xmlChar *rep = NULL;
int len = 0;
int buf_size = 0;
int c, l, in_space = 0;
xmlChar *current = NULL;
xmlEntityPtr ent;

if (NXT(0) == '"') {
ctxt->instate = XML_PARSER_ATTRIBUTE_VALUE;
limit = '"';
NEXT;
} else if (NXT(0) == '\'') {
limit = '\'';
ctxt->instate = XML_PARSER_ATTRIBUTE_VALUE;
NEXT;
} else {
xmlFatalErr(ctxt, XML_ERR_ATTRIBUTE_NOT_STARTED, NULL);
return(NULL);
}

/*
* allocate a translation buffer.
*/
buf_size = XML_PARSER_BUFFER_SIZE;
buf = (xmlChar *) xmlMallocAtomic(buf_size * sizeof(xmlChar));
if (buf == NULL) goto mem_error;

/*
* OK loop until we reach one of the ending char or a size limit.
*/
c = CUR_CHAR(l);
    while ((NXT(0) != limit) && /* checked *
           (IS_CHAR(c)) && (c != '<')) {
if (c == 0) break;
if (c == '&') {
in_space = 0;
if (NXT(1) == '#') {
int val = xmlParseCharRef(ctxt);

if (val == '&') {
if (ctxt->replaceEntities) {
if (len > buf_size - 10) {
growBuffer(buf, 10);
}
buf[len++] = '&';
} else {
/*
* The reparsing will be done in xmlStringGetNodeList()
* called by the attribute() function in SAX.c
*/
if (len > buf_size - 10) {
growBuffer(buf, 10);
}
buf[len++] = '&';
buf[len++] = '#';
buf[len++] = '3';
buf[len++] = '8';
buf[len++] = ';';
}
} else if (val != 0) {
if (len > buf_size - 10) {
growBuffer(buf, 10);
}
len += xmlCopyChar(0, &buf[len], val);
}
} else {
ent = xmlParseEntityRef(ctxt);
ctxt->nbentities++;
if (ent != NULL)
ctxt->nbentities += ent->owner;
if ((ent != NULL) &&
(ent->etype == XML_INTERNAL_PREDEFINED_ENTITY)) {
if (len > buf_size - 10) {
growBuffer(buf, 10);
}
if ((ctxt->replaceEntities == 0) &&
(ent->content[0] == '&')) {
buf[len++] = '&';
buf[len++] = '#';
buf[len++] = '3';
buf[len++] = '8';
buf[len++] = ';';
} else {
buf[len++] = ent->content[0];
}
} else if ((ent != NULL) &&
(ctxt->replaceEntities != 0)) {
if (ent->etype != XML_INTERNAL_PREDEFINED_ENTITY) {
rep = xmlStringDecodeEntities(ctxt, ent->content,
XML_SUBSTITUTE_REF,
0, 0, 0);
if (rep != NULL) {
current = rep;
while (*current != 0) { /* non input consuming */
if ((*current == 0xD) || (*current == 0xA) ||
(*current == 0x9)) {
buf[len++] = 0x20;
current++;
} else
buf[len++] = *current++;
if (len > buf_size - 10) {
growBuffer(buf, 10);
}
}
xmlFree(rep);
rep = NULL;
}
} else {
if (len > buf_size - 10) {
growBuffer(buf, 10);
}
if (ent->content != NULL)
buf[len++] = ent->content[0];
}
} else if (ent != NULL) {
int i = xmlStrlen(ent->name);
const xmlChar *cur = ent->name;

/*
* This may look absurd but is needed to detect
* entities problems
*/
if ((ent->etype != XML_INTERNAL_PREDEFINED_ENTITY) &&
(ent->content != NULL)) {
rep = xmlStringDecodeEntities(ctxt, ent->content,
XML_SUBSTITUTE_REF, 0, 0, 0);
if (rep != NULL) {
xmlFree(rep);
rep = NULL;
}
}

/*
* Just output the reference
*/
buf[len++] = '&';
while (len > buf_size - i - 10) {
growBuffer(buf, i + 10);
}
for (;i > 0;i--)
buf[len++] = *cur++;
buf[len++] = ';';
}
}
} else {
if ((c == 0x20) || (c == 0xD) || (c == 0xA) || (c == 0x9)) {
if ((len != 0) || (!normalize)) {
if ((!normalize) || (!in_space)) {
COPY_BUF(l,buf,len,0x20);
while (len > buf_size - 10) {
growBuffer(buf, 10);
}
}
in_space = 1;
}
} else {
in_space = 0;
COPY_BUF(l,buf,len,c);
if (len > buf_size - 10) {
growBuffer(buf, 10);
}
}
NEXTL(l);
}
GROW;
c = CUR_CHAR(l);
}
if ((in_space) && (normalize)) {
while ((len > 0) && (buf[len - 1] == 0x20)) len--;
}
buf[len] = 0;
if (RAW == '<') {
xmlFatalErr(ctxt, XML_ERR_LT_IN_ATTRIBUTE, NULL);
} else if (RAW != limit) {
if ((c != 0) && (!IS_CHAR(c))) {
xmlFatalErrMsg(ctxt, XML_ERR_INVALID_CHAR,
"invalid character in attribute value\n");
} else {
xmlFatalErrMsg(ctxt, XML_ERR_ATTRIBUTE_NOT_FINISHED,
"AttValue: ' expected\n");
}
} else
NEXT;
if (attlen != NULL) *attlen = len;
return(buf);

mem_error:
xmlErrMemory(ctxt, NULL);
if (buf != NULL)
xmlFree(buf);
if (rep != NULL)
xmlFree(rep);
return(NULL);
}

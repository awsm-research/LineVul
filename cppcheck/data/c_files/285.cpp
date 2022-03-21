NO_INLINE JsVar *jspeArrowFunction(JsVar *funcVar, JsVar *a) {
assert(!a || jsvIsName(a));
JSP_ASSERT_MATCH(LEX_ARROW_FUNCTION);
funcVar = jspeAddNamedFunctionParameter(funcVar, a);

bool expressionOnly = lex->tk!='{';
jspeFunctionDefinitionInternal(funcVar, expressionOnly);
if (execInfo.thisVar) {
jsvObjectSetChild(funcVar, JSPARSE_FUNCTION_THIS_NAME, execInfo.thisVar);
}
return funcVar;
}

// parse expressions with commas, maybe followed by an arrow function (bracket already matched)
NO_INLINE JsVar *jspeExpressionOrArrowFunction() {
JsVar *a = 0;
JsVar *funcVar = 0;
bool allNames = true;
while (lex->tk!=')' && !JSP_SHOULDNT_PARSE) {
if (allNames && a) {
// we never get here if this isn't a name and a string
funcVar = jspeAddNamedFunctionParameter(funcVar, a);
}
jsvUnLock(a);
a = jspeAssignmentExpression();
if (!(jsvIsName(a) && jsvIsString(a))) allNames = false;
if (lex->tk!=')') JSP_MATCH_WITH_CLEANUP_AND_RETURN(',', jsvUnLock2(a,funcVar), 0);
}
JSP_MATCH_WITH_CLEANUP_AND_RETURN(')', jsvUnLock2(a,funcVar), 0);
// if arrow is found, create a function
if (allNames && lex->tk==LEX_ARROW_FUNCTION) {
funcVar = jspeArrowFunction(funcVar, a);
jsvUnLock(a);
return funcVar;
} else {
jsvUnLock(funcVar);
return a;
}
}

/// Parse an ES6 class, expects LEX_R_CLASS already parsed
NO_INLINE JsVar *jspeClassDefinition(bool parseNamedClass) {
JsVar *classFunction = 0;
JsVar *classPrototype = 0;
JsVar *classInternalName = 0;

bool actuallyCreateClass = JSP_SHOULD_EXECUTE;
if (actuallyCreateClass)
classFunction = jsvNewWithFlags(JSV_FUNCTION);

if (parseNamedClass && lex->tk==LEX_ID) {
if (classFunction)
classInternalName = jslGetTokenValueAsVar(lex);
JSP_ASSERT_MATCH(LEX_ID);
}
if (classFunction) {
JsVar *prototypeName = jsvFindChildFromString(classFunction, JSPARSE_PROTOTYPE_VAR, true);
jspEnsureIsPrototype(classFunction, prototypeName); // make sure it's an object
classPrototype = jsvSkipName(prototypeName);
jsvUnLock(prototypeName);
}
if (lex->tk==LEX_R_EXTENDS) {
JSP_ASSERT_MATCH(LEX_R_EXTENDS);
JsVar *extendsFrom = actuallyCreateClass ? jsvSkipNameAndUnLock(jspGetNamedVariable(jslGetTokenValueAsString(lex))) : 0;
JSP_MATCH_WITH_CLEANUP_AND_RETURN(LEX_ID,jsvUnLock4(extendsFrom,classFunction,classInternalName,classPrototype),0);
if (classPrototype) {
if (jsvIsFunction(extendsFrom)) {
jsvObjectSetChild(classPrototype, JSPARSE_INHERITS_VAR, extendsFrom);
// link in default constructor if ours isn't supplied
jsvObjectSetChildAndUnLock(classFunction, JSPARSE_FUNCTION_CODE_NAME, jsvNewFromString("if(this.__proto__.__proto__)this.__proto__.__proto__.apply(this,arguments)"));
} else
jsExceptionHere(JSET_SYNTAXERROR, "'extends' argument should be a function, got %t", extendsFrom);
}
jsvUnLock(extendsFrom);
}
JSP_MATCH_WITH_CLEANUP_AND_RETURN('{',jsvUnLock3(classFunction,classInternalName,classPrototype),0);

while ((lex->tk==LEX_ID || lex->tk==LEX_R_STATIC) && !jspIsInterrupted()) {
bool isStatic = lex->tk==LEX_R_STATIC;
if (isStatic) JSP_ASSERT_MATCH(LEX_R_STATIC);

JsVar *funcName = jslGetTokenValueAsVar(lex);
JSP_MATCH_WITH_CLEANUP_AND_RETURN(LEX_ID,jsvUnLock3(classFunction,classInternalName,classPrototype),0);
JsVar *method = jspeFunctionDefinition(false);
if (classFunction && classPrototype) {
if (jsvIsStringEqual(funcName, "get") || jsvIsStringEqual(funcName, "set")) {
jsExceptionHere(JSET_SYNTAXERROR, "'get' and 'set' and not supported in Espruino");
} else if (jsvIsStringEqual(funcName, "constructor")) {
jswrap_function_replaceWith(classFunction, method);
} else {
funcName = jsvMakeIntoVariableName(funcName, 0);
jsvSetValueOfName(funcName, method);
jsvAddName(isStatic ? classFunction : classPrototype, funcName);
}
}
jsvUnLock2(method,funcName);
}
jsvUnLock(classPrototype);
// If we had a name, add it to the end (or it gets confused with the constructor arguments)
if (classInternalName)
jsvObjectSetChildAndUnLock(classFunction, JSPARSE_FUNCTION_NAME_NAME, classInternalName);

JSP_MATCH_WITH_CLEANUP_AND_RETURN('}',jsvUnLock(classFunction),0);
return classFunction;
}

#endif

NO_INLINE JsVar *jspeFactor() {
if (lex->tk==LEX_ID) {
JsVar *a = jspGetNamedVariable(jslGetTokenValueAsString(lex));
JSP_ASSERT_MATCH(LEX_ID);
#ifndef SAVE_ON_FLASH
if (lex->tk==LEX_TEMPLATE_LITERAL)
jsExceptionHere(JSET_SYNTAXERROR, "Tagged template literals not supported");
else if (lex->tk==LEX_ARROW_FUNCTION && jsvIsName(a)) {
JsVar *funcVar = jspeArrowFunction(0,a);
jsvUnLock(a);
a=funcVar;
}
#endif
return a;
} else if (lex->tk==LEX_INT) {
JsVar *v = 0;
if (JSP_SHOULD_EXECUTE) {
v = jsvNewFromLongInteger(stringToInt(jslGetTokenValueAsString(lex)));
}
JSP_ASSERT_MATCH(LEX_INT);
return v;
} else if (lex->tk==LEX_FLOAT) {
JsVar *v = 0;
if (JSP_SHOULD_EXECUTE) {
v = jsvNewFromFloat(stringToFloat(jslGetTokenValueAsString(lex)));
}
JSP_ASSERT_MATCH(LEX_FLOAT);
return v;
} else if (lex->tk=='(') {
JSP_ASSERT_MATCH('(');
if (!jspCheckStackPosition()) return 0;
#ifdef SAVE_ON_FLASH
// Just parse a normal expression (which can include commas)
JsVar *a = jspeExpression();
if (!JSP_SHOULDNT_PARSE) JSP_MATCH_WITH_RETURN(')',a);
return a;
#else
return jspeExpressionOrArrowFunction();
#endif

} else if (lex->tk==LEX_R_TRUE) {
JSP_ASSERT_MATCH(LEX_R_TRUE);
return JSP_SHOULD_EXECUTE ? jsvNewFromBool(true) : 0;
} else if (lex->tk==LEX_R_FALSE) {
JSP_ASSERT_MATCH(LEX_R_FALSE);
return JSP_SHOULD_EXECUTE ? jsvNewFromBool(false) : 0;
} else if (lex->tk==LEX_R_NULL) {
JSP_ASSERT_MATCH(LEX_R_NULL);
return JSP_SHOULD_EXECUTE ? jsvNewWithFlags(JSV_NULL) : 0;
} else if (lex->tk==LEX_R_UNDEFINED) {
JSP_ASSERT_MATCH(LEX_R_UNDEFINED);
return 0;
} else if (lex->tk==LEX_STR) {
JsVar *a = 0;
if (JSP_SHOULD_EXECUTE)
a = jslGetTokenValueAsVar(lex);
JSP_ASSERT_MATCH(LEX_STR);
return a;
#ifndef SAVE_ON_FLASH
} else if (lex->tk==LEX_TEMPLATE_LITERAL) {
return jspeTemplateLiteral();
#endif
} else if (lex->tk==LEX_REGEX) {
JsVar *a = 0;
#ifdef SAVE_ON_FLASH
jsExceptionHere(JSET_SYNTAXERROR, "RegEx are not supported in this version of Espruino\n");
#else
JsVar *regex = jslGetTokenValueAsVar(lex);
size_t regexEnd = 0, regexLen = 0;
JsvStringIterator it;
jsvStringIteratorNew(&it, regex, 0);
while (jsvStringIteratorHasChar(&it)) {
regexLen++;
if (jsvStringIteratorGetChar(&it)=='/')
regexEnd = regexLen;
jsvStringIteratorNext(&it);
}
jsvStringIteratorFree(&it);
JsVar *flags = 0;
if (regexEnd < regexLen)
flags = jsvNewFromStringVar(regex, regexEnd, JSVAPPENDSTRINGVAR_MAXLENGTH);
JsVar *regexSource = jsvNewFromStringVar(regex, 1, regexEnd-2);
a = jswrap_regexp_constructor(regexSource, flags);
jsvUnLock3(regex, flags, regexSource);
#endif
JSP_ASSERT_MATCH(LEX_REGEX);
return a;
} else if (lex->tk=='{') {
if (!jspCheckStackPosition()) return 0;
return jspeFactorObject();
} else if (lex->tk=='[') {
if (!jspCheckStackPosition()) return 0;
return jspeFactorArray();
} else if (lex->tk==LEX_R_FUNCTION) {
if (!jspCheckStackPosition()) return 0;
JSP_ASSERT_MATCH(LEX_R_FUNCTION);
return jspeFunctionDefinition(true);
#ifndef SAVE_ON_FLASH
} else if (lex->tk==LEX_R_CLASS) {
if (!jspCheckStackPosition()) return 0;
JSP_ASSERT_MATCH(LEX_R_CLASS);
return jspeClassDefinition(true);
} else if (lex->tk==LEX_R_SUPER) {
JSP_ASSERT_MATCH(LEX_R_SUPER);
/* This is kind of nasty, since super appears to do
three different things.

* In the constructor it references the extended class's constructor
* in a method it references the constructor's prototype.
* in a static method it references the extended class's constructor (but this is different)
*/

if (jsvIsObject(execInfo.thisVar)) {
// 'this' is an object - must be calling a normal method
JsVar *proto1 = jsvObjectGetChild(execInfo.thisVar, JSPARSE_INHERITS_VAR, 0); // if we're in a method, get __proto__ first
JsVar *proto2 = jsvIsObject(proto1) ? jsvObjectGetChild(proto1, JSPARSE_INHERITS_VAR, 0) : 0; // still in method, get __proto__.__proto__
jsvUnLock(proto1);
if (!proto2) {
jsExceptionHere(JSET_SYNTAXERROR, "Calling 'super' outside of class");
return 0;
}
if (lex->tk=='(') return proto2; // eg. used in a constructor
// But if we're doing something else - eg '.' or '[' then it needs to reference the prototype
JsVar *proto3 = jsvIsFunction(proto2) ? jsvObjectGetChild(proto2, JSPARSE_PROTOTYPE_VAR, 0) : 0;
jsvUnLock(proto2);
return proto3;
} else if (jsvIsFunction(execInfo.thisVar)) {
// 'this' is a function - must be calling a static method
JsVar *proto1 = jsvObjectGetChild(execInfo.thisVar, JSPARSE_PROTOTYPE_VAR, 0);
JsVar *proto2 = jsvIsObject(proto1) ? jsvObjectGetChild(proto1, JSPARSE_INHERITS_VAR, 0) : 0;
jsvUnLock(proto1);
if (!proto2) {
jsExceptionHere(JSET_SYNTAXERROR, "Calling 'super' outside of class");
return 0;
}
return proto2;
}
jsExceptionHere(JSET_SYNTAXERROR, "Calling 'super' outside of class");
return 0;
#endif
} else if (lex->tk==LEX_R_THIS) {
JSP_ASSERT_MATCH(LEX_R_THIS);
return jsvLockAgain( execInfo.thisVar ? execInfo.thisVar : execInfo.root );
} else if (lex->tk==LEX_R_DELETE) {
if (!jspCheckStackPosition()) return 0;
return jspeFactorDelete();
} else if (lex->tk==LEX_R_TYPEOF) {
if (!jspCheckStackPosition()) return 0;
return jspeFactorTypeOf();
} else if (lex->tk==LEX_R_VOID) {
if (!jspCheckStackPosition()) return 0;
JSP_ASSERT_MATCH(LEX_R_VOID);
jsvUnLock(jspeUnaryExpression());
return 0;
}
JSP_MATCH(LEX_EOF);
jsExceptionHere(JSET_SYNTAXERROR, "Unexpected end of Input\n");
return 0;
}

NO_INLINE JsVar *__jspePostfixExpression(JsVar *a) {
while (lex->tk==LEX_PLUSPLUS || lex->tk==LEX_MINUSMINUS) {
int op = lex->tk;
JSP_ASSERT_MATCH(op);
if (JSP_SHOULD_EXECUTE) {
JsVar *one = jsvNewFromInteger(1);
JsVar *oldValue = jsvAsNumberAndUnLock(jsvSkipName(a)); // keep the old value (but convert to number)
JsVar *res = jsvMathsOpSkipNames(oldValue, one, op==LEX_PLUSPLUS ? '+' : '-');
jsvUnLock(one);

// in-place add/subtract
jspReplaceWith(a, res);
jsvUnLock(res);
// but then use the old value
jsvUnLock(a);
a = oldValue;
}
}
return a;
}

NO_INLINE JsVar *jspePostfixExpression() {
JsVar *a;
// TODO: should be in jspeUnaryExpression
if (lex->tk==LEX_PLUSPLUS || lex->tk==LEX_MINUSMINUS) {
int op = lex->tk;
JSP_ASSERT_MATCH(op);
a = jspePostfixExpression();
if (JSP_SHOULD_EXECUTE) {
JsVar *one = jsvNewFromInteger(1);
JsVar *res = jsvMathsOpSkipNames(a, one, op==LEX_PLUSPLUS ? '+' : '-');
jsvUnLock(one);
// in-place add/subtract
jspReplaceWith(a, res);
jsvUnLock(res);
}
} else
a = jspeFactorFunctionCall();
return __jspePostfixExpression(a);
}

NO_INLINE JsVar *jspeUnaryExpression() {
if (lex->tk=='!' || lex->tk=='~' || lex->tk=='-' || lex->tk=='+') {
short tk = lex->tk;
JSP_ASSERT_MATCH(tk);
if (!JSP_SHOULD_EXECUTE) {
return jspeUnaryExpression();
}
if (tk=='!') { // logical not
return jsvNewFromBool(!jsvGetBoolAndUnLock(jsvSkipNameAndUnLock(jspeUnaryExpression())));
} else if (tk=='~') { // bitwise not
return jsvNewFromInteger(~jsvGetIntegerAndUnLock(jsvSkipNameAndUnLock(jspeUnaryExpression())));
} else if (tk=='-') { // unary minus
return jsvNegateAndUnLock(jspeUnaryExpression()); // names already skipped
}  else if (tk=='+') { // unary plus (convert to number)
JsVar *v = jsvSkipNameAndUnLock(jspeUnaryExpression());
JsVar *r = jsvAsNumber(v); // names already skipped
jsvUnLock(v);
return r;
}
assert(0);
return 0;
} else
return jspePostfixExpression();
}


// Get the precedence of a BinaryExpression - or return 0 if not one
unsigned int jspeGetBinaryExpressionPrecedence(int op) {
switch (op) {
case LEX_OROR: return 1; break;
case LEX_ANDAND: return 2; break;
case '|' : return 3; break;
case '^' : return 4; break;
case '&' : return 5; break;
case LEX_EQUAL:
case LEX_NEQUAL:
case LEX_TYPEEQUAL:
case LEX_NTYPEEQUAL: return 6;
case LEX_LEQUAL:
case LEX_GEQUAL:
case '<':
case '>':
case LEX_R_INSTANCEOF: return 7;
case LEX_R_IN: return (execInfo.execute&EXEC_FOR_INIT)?0:7;
case LEX_LSHIFT:
case LEX_RSHIFT:
case LEX_RSHIFTUNSIGNED: return 8;
case '+':
case '-': return 9;
case '*':
case '/':
case '%': return 10;
default: return 0;
}
}

NO_INLINE JsVar *__jspeBinaryExpression(JsVar *a, unsigned int lastPrecedence) {
/* This one's a bit strange. Basically all the ops have their own precedence, it's not
* like & and | share the same precedence. We don't want to recurse for each one,
* so instead we do this.
*
* We deal with an expression in recursion ONLY if it's of higher precedence
* than the current one, otherwise we stick in the while loop.
*/
unsigned int precedence = jspeGetBinaryExpressionPrecedence(lex->tk);
while (precedence && precedence>lastPrecedence) {
int op = lex->tk;
JSP_ASSERT_MATCH(op);

// if we have short-circuit ops, then if we know the outcome
// we don't bother to execute the other op. Even if not
// we need to tell mathsOp it's an & or |
if (op==LEX_ANDAND || op==LEX_OROR) {
bool aValue = jsvGetBoolAndUnLock(jsvSkipName(a));
if ((!aValue && op==LEX_ANDAND) ||
(aValue && op==LEX_OROR)) {
// use first argument (A)
JSP_SAVE_EXECUTE();
jspSetNoExecute();
jsvUnLock(__jspeBinaryExpression(jspeUnaryExpression(),precedence));
JSP_RESTORE_EXECUTE();
} else {
// use second argument (B)
jsvUnLock(a);
a = __jspeBinaryExpression(jspeUnaryExpression(),precedence);
}
} else { // else it's a more 'normal' logical expression - just use Maths
JsVar *b = __jspeBinaryExpression(jspeUnaryExpression(),precedence);
if (JSP_SHOULD_EXECUTE) {
if (op==LEX_R_IN) {
JsVar *av = jsvSkipName(a); // needle
JsVar *bv = jsvSkipName(b); // haystack
if (jsvIsArray(bv) || jsvIsObject(bv)) { // search keys, NOT values
av = jsvAsArrayIndexAndUnLock(av);
JsVar *varFound = jspGetVarNamedField( bv, av, true);
jsvUnLock(a);
a = jsvNewFromBool(varFound!=0);
jsvUnLock(varFound);
} else {// else it will be undefined
jsExceptionHere(JSET_ERROR, "Cannot use 'in' operator to search a %t", bv);
jsvUnLock(a);
a = 0;
}
jsvUnLock2(av, bv);
} else if (op==LEX_R_INSTANCEOF) {
bool inst = false;
JsVar *av = jsvSkipName(a);
JsVar *bv = jsvSkipName(b);
if (!jsvIsFunction(bv)) {
jsExceptionHere(JSET_ERROR, "Expecting a function on RHS in instanceof check, got %t", bv);
} else {
if (jsvIsObject(av) || jsvIsFunction(av)) {
JsVar *bproto = jspGetNamedField(bv, JSPARSE_PROTOTYPE_VAR, false);
JsVar *proto = jsvObjectGetChild(av, JSPARSE_INHERITS_VAR, 0);
while (proto) {
if (proto == bproto) inst=true;
// search prototype chain
JsVar *childProto = jsvObjectGetChild(proto, JSPARSE_INHERITS_VAR, 0);
jsvUnLock(proto);
proto = childProto;
}
if (jspIsConstructor(bv, "Object")) inst = true;
jsvUnLock(bproto);
}
if (!inst) {
const char *name = jswGetBasicObjectName(av);
if (name) {
inst = jspIsConstructor(bv, name);
}
// Hack for built-ins that should also be instances of Object
if (!inst && (jsvIsArray(av) || jsvIsArrayBuffer(av)) &&
jspIsConstructor(bv, "Object"))
inst = true;
}
}
jsvUnLock3(av, bv, a);
a = jsvNewFromBool(inst);
} else {  // --------------------------------------------- NORMAL
JsVar *res = jsvMathsOpSkipNames(a, b, op);
jsvUnLock(a); a = res;
}
}
jsvUnLock(b);
}
precedence = jspeGetBinaryExpressionPrecedence(lex->tk);
}
return a;
}

JsVar *jspeBinaryExpression() {
return __jspeBinaryExpression(jspeUnaryExpression(),0);
}

NO_INLINE JsVar *__jspeConditionalExpression(JsVar *lhs) {
if (lex->tk=='?') {
JSP_ASSERT_MATCH('?');
if (!JSP_SHOULD_EXECUTE) {
// just let lhs pass through
jsvUnLock(jspeAssignmentExpression());
JSP_MATCH(':');
jsvUnLock(jspeAssignmentExpression());
} else {
bool first = jsvGetBoolAndUnLock(jsvSkipName(lhs));
jsvUnLock(lhs);
if (first) {
lhs = jspeAssignmentExpression();
JSP_MATCH(':');
JSP_SAVE_EXECUTE();
jspSetNoExecute();
jsvUnLock(jspeAssignmentExpression());
JSP_RESTORE_EXECUTE();
} else {
JSP_SAVE_EXECUTE();
jspSetNoExecute();
jsvUnLock(jspeAssignmentExpression());
JSP_RESTORE_EXECUTE();
JSP_MATCH(':');
lhs = jspeAssignmentExpression();
}
}
}

return lhs;
}

JsVar *jspeConditionalExpression() {
return __jspeConditionalExpression(jspeBinaryExpression());
}

NO_INLINE JsVar *__jspeAssignmentExpression(JsVar *lhs) {
if (lex->tk=='=' || lex->tk==LEX_PLUSEQUAL || lex->tk==LEX_MINUSEQUAL ||
lex->tk==LEX_MULEQUAL || lex->tk==LEX_DIVEQUAL || lex->tk==LEX_MODEQUAL ||
lex->tk==LEX_ANDEQUAL || lex->tk==LEX_OREQUAL ||
lex->tk==LEX_XOREQUAL || lex->tk==LEX_RSHIFTEQUAL ||
lex->tk==LEX_LSHIFTEQUAL || lex->tk==LEX_RSHIFTUNSIGNEDEQUAL) {
JsVar *rhs;

int op = lex->tk;
JSP_ASSERT_MATCH(op);
rhs = jspeAssignmentExpression();
rhs = jsvSkipNameAndUnLock(rhs); // ensure we get rid of any references on the RHS

if (JSP_SHOULD_EXECUTE && lhs) {
if (op=='=') {
        /* If we're assigning to this and we don't have a parent,
         * add it to the symbol table root *
        if (!jsvGetRefs(lhs) && jsvIsName(lhs)) {
          if (!jsvIsArrayBufferName(lhs) && !jsvIsNewChild(lhs))
            jsvAddName(execInfo.root, lhs);
        }
        jspReplaceWith(lhs, rhs);
} else {
if (op==LEX_PLUSEQUAL) op='+';
else if (op==LEX_MINUSEQUAL) op='-';
else if (op==LEX_MULEQUAL) op='*';
else if (op==LEX_DIVEQUAL) op='/';
else if (op==LEX_MODEQUAL) op='%';
else if (op==LEX_ANDEQUAL) op='&';
else if (op==LEX_OREQUAL) op='|';
else if (op==LEX_XOREQUAL) op='^';
else if (op==LEX_RSHIFTEQUAL) op=LEX_RSHIFT;
else if (op==LEX_LSHIFTEQUAL) op=LEX_LSHIFT;
else if (op==LEX_RSHIFTUNSIGNEDEQUAL) op=LEX_RSHIFTUNSIGNED;
if (op=='+' && jsvIsName(lhs)) {
JsVar *currentValue = jsvSkipName(lhs);
if (jsvIsString(currentValue) && !jsvIsFlatString(currentValue) && jsvGetRefs(currentValue)==1 && rhs!=currentValue) {
/* A special case for string += where this is the only use of the string
* and we're not appending to ourselves. In this case we can do a
* simple append (rather than clone + append)*/
JsVar *str = jsvAsString(rhs, false);
jsvAppendStringVarComplete(currentValue, str);
jsvUnLock(str);
op = 0;
}
jsvUnLock(currentValue);
}
if (op) {
/* Fallback which does a proper add */
JsVar *res = jsvMathsOpSkipNames(lhs,rhs,op);
jspReplaceWith(lhs, res);
jsvUnLock(res);
}
}
}
jsvUnLock(rhs);
}
return lhs;
}


JsVar *jspeAssignmentExpression() {
return __jspeAssignmentExpression(jspeConditionalExpression());
}

// ',' is allowed to add multiple expressions, this is not allowed in jspeAssignmentExpression
NO_INLINE JsVar *jspeExpression() {
while (!JSP_SHOULDNT_PARSE) {
JsVar *a = jspeAssignmentExpression();
if (lex->tk!=',') return a;
// if we get a comma, we just forget this data and parse the next bit...
jsvCheckReferenceError(a);
jsvUnLock(a);
JSP_ASSERT_MATCH(',');
}
return 0;
}

/** Parse a block `{ ... }` but assume brackets are already parsed */
NO_INLINE void jspeBlockNoBrackets() {
if (JSP_SHOULD_EXECUTE) {
while (lex->tk && lex->tk!='}') {
JsVar *a = jspeStatement();
jsvCheckReferenceError(a);
jsvUnLock(a);
if (JSP_HAS_ERROR) {
if (lex && !(execInfo.execute&EXEC_ERROR_LINE_REPORTED)) {
execInfo.execute = (JsExecFlags)(execInfo.execute | EXEC_ERROR_LINE_REPORTED);
JsVar *stackTrace = jsvObjectGetChild(execInfo.hiddenRoot, JSPARSE_STACKTRACE_VAR, JSV_STRING_0);
if (stackTrace) {
jsvAppendPrintf(stackTrace, "at ");
jspAppendStackTrace(stackTrace);
jsvUnLock(stackTrace);
}
}
}
if (JSP_SHOULDNT_PARSE)
return;
}
} else {
// fast skip of blocks
int brackets = 0;
while (lex->tk && (brackets || lex->tk != '}')) {
if (lex->tk == '{') brackets++;
if (lex->tk == '}') brackets--;
JSP_ASSERT_MATCH(lex->tk);
}
}
return;
}

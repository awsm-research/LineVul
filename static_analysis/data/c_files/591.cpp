decompileAction(int n, SWF_ACTION *actions, int maxn)
{
	if( n > maxn ) SWF_error("Action overflow!!");

#ifdef DEBUG
fprintf(stderr,"%d:\tACTION[%3.3d]: %s\n",
actions[n].SWF_ACTIONRECORD.Offset, n,
actionName(actions[n].SWF_ACTIONRECORD.ActionCode));
#endif

	switch(actions[n].SWF_ACTIONRECORD.ActionCode)
{
case SWFACTION_END:
return 0;

case SWFACTION_CONSTANTPOOL:
decompileCONSTANTPOOL(&actions[n]);
return 0;

case SWFACTION_GOTOLABEL:
return decompileGOTOFRAME(n, actions, maxn,1);

case SWFACTION_GOTOFRAME:
return decompileGOTOFRAME(n, actions, maxn,0);

case SWFACTION_GOTOFRAME2:
return decompileGOTOFRAME2(n, actions, maxn);

case SWFACTION_WAITFORFRAME:
decompileWAITFORFRAME(&actions[n]);
return 0;

case SWFACTION_GETURL2:
decompileGETURL2(&actions[n]);
return 0;

case SWFACTION_GETURL:
decompileGETURL(&actions[n]);
return 0;

case SWFACTION_PUSH:
decompilePUSH(&actions[n]);
return 0;

case SWFACTION_PUSHDUP:
decompilePUSHDUP(&actions[n]);
return 0;

case SWFACTION_STACKSWAP:
decompileSTACKSWAP(&actions[n]);
return 0;

case SWFACTION_SETPROPERTY:
decompileSETPROPERTY(n, actions, maxn);
return 0;

case SWFACTION_GETPROPERTY:
decompileGETPROPERTY(n, actions, maxn);
return 0;

case SWFACTION_GETTIME:
return decompileGETTIME(n, actions, maxn);

case SWFACTION_TRACE:
decompileTRACE(n, actions, maxn);
return 0;

case SWFACTION_CALLFRAME:
decompileCALLFRAME(n, actions, maxn);
return 0;

case SWFACTION_EXTENDS:
decompileEXTENDS(n, actions, maxn);
return 0;

case SWFACTION_INITOBJECT:
decompileINITOBJECT(n, actions, maxn);
return 0;

case SWFACTION_NEWOBJECT:
decompileNEWOBJECT(n, actions, maxn);
return 0;

case SWFACTION_NEWMETHOD:
decompileNEWMETHOD(n, actions, maxn);
return 0;

case SWFACTION_GETMEMBER:
decompileGETMEMBER(n, actions, maxn);
return 0;

case SWFACTION_SETMEMBER:
decompileSETMEMBER(n, actions, maxn);
return 0;

case SWFACTION_GETVARIABLE:
decompileGETVARIABLE(n, actions, maxn);
return 0;

case SWFACTION_SETVARIABLE:
decompileSETVARIABLE(n, actions, maxn, 0);
return 0;

case SWFACTION_DEFINELOCAL:
decompileSETVARIABLE(n, actions, maxn, 1);
return 0;

case SWFACTION_DEFINELOCAL2:
decompileDEFINELOCAL2(n, actions, maxn);
return 0;

case SWFACTION_DECREMENT:
return decompileINCR_DECR(n, actions, maxn, 0);

case SWFACTION_INCREMENT:
return decompileINCR_DECR(n, actions, maxn,1);

case SWFACTION_STOREREGISTER:
decompileSTOREREGISTER(n, actions, maxn);
return 0;

case SWFACTION_JUMP:
return decompileJUMP(n, actions, maxn);

case SWFACTION_RETURN:
decompileRETURN(n, actions, maxn);
return 0;

case SWFACTION_LOGICALNOT:
return decompileLogicalNot(n, actions, maxn);

case SWFACTION_IF:
return decompileIF(n, actions, maxn);

case SWFACTION_WITH:
decompileWITH(n, actions, maxn);
return 0;

case SWFACTION_ENUMERATE:
return decompileENUMERATE(n, actions, maxn, 0);

case SWFACTION_ENUMERATE2 :
return decompileENUMERATE(n, actions, maxn,1);

case SWFACTION_INITARRAY:
return decompileINITARRAY(n, actions, maxn);

case SWFACTION_DEFINEFUNCTION:
return decompileDEFINEFUNCTION(n, actions, maxn,0);

case SWFACTION_DEFINEFUNCTION2:
return decompileDEFINEFUNCTION(n, actions, maxn,1);

case SWFACTION_CALLFUNCTION:
return decompileCALLFUNCTION(n, actions, maxn);

case SWFACTION_CALLMETHOD:
return decompileCALLMETHOD(n, actions, maxn);

case SWFACTION_INSTANCEOF:
case SWFACTION_SHIFTLEFT:
case SWFACTION_SHIFTRIGHT:
case SWFACTION_SHIFTRIGHT2:
case SWFACTION_ADD:
case SWFACTION_ADD2:
case SWFACTION_SUBTRACT:
case SWFACTION_MULTIPLY:
case SWFACTION_DIVIDE:
case SWFACTION_MODULO:
case SWFACTION_BITWISEAND:
case SWFACTION_BITWISEOR:
case SWFACTION_BITWISEXOR:
case SWFACTION_EQUAL:
case SWFACTION_EQUALS2:
case SWFACTION_LESS2:
case SWFACTION_LOGICALAND:
case SWFACTION_LOGICALOR:
case SWFACTION_GREATER:
case SWFACTION_LESSTHAN:
case SWFACTION_STRINGEQ:
case SWFACTION_STRINGCOMPARE:
case SWFACTION_STRICTEQUALS:
return decompileArithmeticOp(n, actions, maxn);

case SWFACTION_POP:
pop();
return 0;

case SWFACTION_STARTDRAG:
return decompileSTARTDRAG(n, actions, maxn);

case SWFACTION_DELETE:
return decompileDELETE(n, actions, maxn,0);

case SWFACTION_DELETE2:
return decompileDELETE(n, actions, maxn,1);

case SWFACTION_TARGETPATH:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"targetPath");

case SWFACTION_TYPEOF:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"typeof");

case SWFACTION_ORD:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"ord");

case SWFACTION_CHR:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"chr");

case SWFACTION_INT:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"int");

case SWFACTION_TOSTRING:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"String");

case SWFACTION_TONUMBER:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"Number");

case SWFACTION_RANDOMNUMBER:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"random");

case SWFACTION_STRINGLENGTH:
return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"length");

case SWFACTION_PLAY:
return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"play");

case SWFACTION_STOP:
return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"stop");

case SWFACTION_NEXTFRAME:
return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"nextFrame");

case SWFACTION_PREVFRAME:
return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"prevFrame");

case SWFACTION_ENDDRAG:
return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"stopDrag");

case SWFACTION_STOPSOUNDS:
return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"stopAllSounds");

case SWFACTION_TOGGLEQUALITY:
return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"toggleHighQuality");

case SWFACTION_MBSUBSTRING:
case SWFACTION_SUBSTRING:
return decompileSUBSTRING(n, actions, maxn);

case SWFACTION_STRINGCONCAT:
return decompileSTRINGCONCAT(n, actions, maxn);

case SWFACTION_REMOVECLIP:
return decompileREMOVECLIP(n, actions, maxn);

case SWFACTION_DUPLICATECLIP:
return decompileDUPLICATECLIP(n, actions, maxn);

case SWFACTION_SETTARGET:
return decompileSETTARGET(n, actions, maxn,0);

case SWFACTION_SETTARGET2:
return decompileSETTARGET(n, actions, maxn,1);

case SWFACTION_IMPLEMENTSOP:
return decompileIMPLEMENTS(n, actions, maxn);

case SWFACTION_CASTOP:
return decompileCAST(n, actions, maxn);

case SWFACTION_THROW:
return decompileTHROW(n, actions, maxn);

case SWFACTION_TRY:
return decompileTRY(n, actions, maxn);

default:
outputSWF_ACTION(n,&actions[n]);
return 0;
}
}

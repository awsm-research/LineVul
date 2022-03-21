IPV6DefragReverseSimpleTest(void)
{
DefragContext *dc = NULL;
Packet *p1 = NULL, *p2 = NULL, *p3 = NULL;
Packet *reassembled = NULL;
int id = 12;
int i;
int ret = 0;

DefragInit();

dc = DefragContextNew();
if (dc == NULL)
goto end;

    p1 = IPV6BuildTestPacket(id, 0, 1, 'A', 8);
if (p1 == NULL)
goto end;
    p2 = IPV6BuildTestPacket(id, 1, 1, 'B', 8);
if (p2 == NULL)
goto end;
    p3 = IPV6BuildTestPacket(id, 2, 0, 'C', 3);
if (p3 == NULL)
goto end;

if (Defrag(NULL, NULL, p3, NULL) != NULL)
goto end;
if (Defrag(NULL, NULL, p2, NULL) != NULL)
goto end;
reassembled = Defrag(NULL, NULL, p1, NULL);
if (reassembled == NULL)
goto end;

/* 40 bytes in we should find 8 bytes of A. */
for (i = 40; i < 40 + 8; i++) {
if (GET_PKT_DATA(reassembled)[i] != 'A')
goto end;
}

/* 28 bytes in we should find 8 bytes of B. */
for (i = 48; i < 48 + 8; i++) {
if (GET_PKT_DATA(reassembled)[i] != 'B')
goto end;
}

/* And 36 bytes in we should find 3 bytes of C. */
for (i = 56; i < 56 + 3; i++) {
if (GET_PKT_DATA(reassembled)[i] != 'C')
goto end;
}

ret = 1;
end:
if (dc != NULL)
DefragContextDestroy(dc);
if (p1 != NULL)
SCFree(p1);
if (p2 != NULL)
SCFree(p2);
if (p3 != NULL)
SCFree(p3);
if (reassembled != NULL)
SCFree(reassembled);

DefragDestroy();
return ret;
}

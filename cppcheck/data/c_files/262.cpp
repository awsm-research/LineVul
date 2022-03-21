void Browser::FindInPage(bool find_next, bool forward_direction) {
ShowFindBar();
if (find_next) {
string16 find_text;
#if defined(OS_MACOSX)
// We always want to search for the contents of the find pasteboard on OS X.
find_text = GetFindPboardText();
#endif
GetSelectedTabContentsWrapper()->
        GetFindManager()->StartFinding(find_text,
                                       forward_direction,
                                       false);  // Not case sensitive.
}
}

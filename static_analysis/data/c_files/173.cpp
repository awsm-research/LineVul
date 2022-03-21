void TabStripGtk::TabDetachedAt(TabContents* contents, int index) {
GenerateIdealBounds();
  StartRemoveTabAnimation(index, contents->web_contents());
// Have to do this _after_ calling StartRemoveTabAnimation, so that any
// previous remove is completed fully and index is valid in sync with the
// model index.
GetTabAt(index)->set_closing(true);
}

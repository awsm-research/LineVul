void RenderBlockFlow::styleDidChange(StyleDifference diff, const RenderStyle* oldStyle)
{
RenderBlock::styleDidChange(diff, oldStyle);

// After our style changed, if we lose our ability to propagate floats into next sibling
// blocks, then we need to find the top most parent containing that overhanging float and
// then mark its descendants with floats for layout and clear all floats from its next
// sibling blocks that exist in our floating objects list. See bug 56299 and 62875.
bool canPropagateFloatIntoSibling = !isFloatingOrOutOfFlowPositioned() && !avoidsFloats();
    if (diff == StyleDifferenceLayout && s_canPropagateFloatIntoSibling && !canPropagateFloatIntoSibling && hasOverhangingFloats()) {
RenderBlockFlow* parentBlockFlow = this;
const FloatingObjectSet& floatingObjectSet = m_floatingObjects->set();
FloatingObjectSetIterator end = floatingObjectSet.end();

for (RenderObject* curr = parent(); curr && !curr->isRenderView(); curr = curr->parent()) {
if (curr->isRenderBlockFlow()) {
RenderBlockFlow* currBlock = toRenderBlockFlow(curr);

if (currBlock->hasOverhangingFloats()) {
for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
RenderBox* renderer = (*it)->renderer();
if (currBlock->hasOverhangingFloat(renderer)) {
parentBlockFlow = currBlock;
break;
}
}
}
}
}

parentBlockFlow->markAllDescendantsWithFloatsForLayout();
parentBlockFlow->markSiblingsWithFloatsForLayout();
}

    if (diff == StyleDifferenceLayout || !oldStyle)
createOrDestroyMultiColumnFlowThreadIfNeeded();
}

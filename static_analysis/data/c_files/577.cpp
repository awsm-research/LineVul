static inline bool shouldSetStrutOnBlock(const LayoutBlockFlow& block, const RootInlineBox& lineBox, LayoutUnit lineLogicalOffset, int lineIndex, LayoutUnit remainingLogicalHeight)
{
bool wantsStrutOnBlock = false;
if (!block.style()->hasAutoOrphans() && block.style()->orphans() >= lineIndex) {
// Not enough orphans here. Push the entire block to the next column / page as an
// attempt to better satisfy the orphans requirement.
wantsStrutOnBlock = true;
} else if (lineBox == block.firstRootBox() && lineLogicalOffset == block.borderAndPaddingBefore()) {
// This is the first line in the block. We can take the whole block with us to the next page
// or column, rather than keeping a content-less portion of it in the previous one. Only do
// this if the line is flush with the content edge of the block, though. If it isn't, it
// means that the line was pushed downwards by preceding floats that didn't fit beside the
// line, and we don't want to move all that, since it has already been established that it
// fits nicely where it is.
LayoutUnit lineHeight = lineBox.lineBottomWithLeading() - lineBox.lineTopWithLeading();
LayoutUnit totalLogicalHeight = lineHeight + std::max<LayoutUnit>(0, lineLogicalOffset);
LayoutUnit pageLogicalHeightAtNewOffset = block.pageLogicalHeightForOffset(lineLogicalOffset + remainingLogicalHeight);
// It's rather pointless to break before the block if the current line isn't going to
// fit in the same column or page, so check that as well.
if (totalLogicalHeight < pageLogicalHeightAtNewOffset)
wantsStrutOnBlock = true;
}
    // The block needs to be contained by a LayoutBlockFlow (and not by e.g. a flexbox or a table
    // (which would be the case for table cell or table caption)). The reason for this limitation is
    // simply that LayoutBlockFlow child layout code is the only place where we pick up the struts
    // and handle them. We handle floats and regular in-flow children, and that's all. We could
    // handle this in other layout modes as well (and even for out-of-flow children), but currently
    // we don't.
    if (!wantsStrutOnBlock || block.isOutOfFlowPositioned())
        return false;
    LayoutBlock* containingBlock = block.containingBlock();
    return containingBlock && containingBlock->isLayoutBlockFlow();
}

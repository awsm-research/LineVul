void RenderBlock::styleWillChange(StyleDifference diff, const RenderStyle& newStyle)
{
RenderStyle* oldStyle = style();

setReplaced(newStyle.isDisplayInlineType());

    if (oldStyle && parent() && diff == StyleDifferenceLayout && oldStyle->position() != newStyle.position()) {
if (newStyle.position() == StaticPosition)
// Clear our positioned objects list. Our absolutely positioned descendants will be
// inserted into our containing block's positioned objects list during layout.
removePositionedObjects(0, NewContainingBlock);
else if (oldStyle->position() == StaticPosition) {
// Remove our absolutely positioned descendants from their current containing block.
// They will be inserted into our positioned objects list during layout.
RenderObject* cb = parent();
while (cb && (cb->style()->position() == StaticPosition || (cb->isInline() && !cb->isReplaced())) && !cb->isRenderView()) {
if (cb->style()->position() == RelativePosition && cb->isInline() && !cb->isReplaced()) {
cb = cb->containingBlock();
break;
}
cb = cb->parent();
}

if (cb->isRenderBlock())
toRenderBlock(cb)->removePositionedObjects(this, NewContainingBlock);
}
}

RenderBox::styleWillChange(diff, newStyle);
}

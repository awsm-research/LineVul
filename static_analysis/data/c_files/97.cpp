bool isUserInteractionEventForSlider(Event* event, LayoutObject* layoutObject) {
// It is unclear if this can be converted to isUserInteractionEvent(), since
// mouse* events seem to be eaten during a drag anyway.  crbug.com/516416 .
if (isUserInteractionEvent(event))
return true;

// Some events are only captured during a slider drag.
LayoutSliderItem slider = LayoutSliderItem(toLayoutSlider(layoutObject));
if (!slider.isNull() && !slider.inDragMode())
return false;

const AtomicString& type = event->type();
return type == EventTypeNames::mouseover ||
         type == EventTypeNames::mouseout || type == EventTypeNames::mousemove;
}

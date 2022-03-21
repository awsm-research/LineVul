bool LayoutSVGTransformableContainer::calculateLocalTransform()
{
SVGGraphicsElement* element = toSVGGraphicsElement(this->element());
ASSERT(element);

// If we're either the layoutObject for a <use> element, or for any <g> element inside the shadow
// tree, that was created during the use/symbol/svg expansion in SVGUseElement. These containers
// need to respect the translations induced by their corresponding use elements x/y attributes.
SVGUseElement* useElement = nullptr;
if (isSVGUseElement(*element)) {
useElement = toSVGUseElement(element);
} else if (isSVGGElement(*element) && toSVGGElement(element)->inUseShadowTree()) {
SVGElement* correspondingElement = element->correspondingElement();
if (isSVGUseElement(correspondingElement))
useElement = toSVGUseElement(correspondingElement);
}

if (useElement) {
SVGLengthContext lengthContext(useElement);
FloatSize translation(
useElement->x()->currentValue()->value(lengthContext),
useElement->y()->currentValue()->value(lengthContext));
if (translation != m_additionalTranslation)
            m_needsTransformUpdate = true;
m_additionalTranslation = translation;
}

if (!m_needsTransformUpdate)
        return false;

m_localTransform = element->calculateAnimatedLocalTransform();
m_localTransform.translate(m_additionalTranslation.width(), m_additionalTranslation.height());
m_needsTransformUpdate = false;
    return true;
}

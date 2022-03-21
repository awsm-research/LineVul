void HTMLFormControlElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
if (name == formAttr) {
formAttributeChanged();
UseCounter::count(document(), UseCounter::FormAttribute);
} else if (name == disabledAttr) {
bool oldDisabled = m_disabled;
m_disabled = !value.isNull();
if (oldDisabled != m_disabled)
disabledAttributeChanged();
} else if (name == readonlyAttr) {
bool wasReadOnly = m_isReadOnly;
m_isReadOnly = !value.isNull();
if (wasReadOnly != m_isReadOnly) {
setNeedsWillValidateCheck();
            setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(name));
if (layoutObject())
LayoutTheme::theme().controlStateChanged(*layoutObject(), ReadOnlyControlState);
}
} else if (name == requiredAttr) {
bool wasRequired = m_isRequired;
m_isRequired = !value.isNull();
if (wasRequired != m_isRequired)
requiredAttributeChanged();
UseCounter::count(document(), UseCounter::RequiredAttribute);
} else if (name == autofocusAttr) {
HTMLElement::parseAttribute(name, value);
UseCounter::count(document(), UseCounter::AutoFocusAttribute);
} else {
HTMLElement::parseAttribute(name, value);
}
}

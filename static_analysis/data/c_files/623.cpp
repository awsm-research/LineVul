string16 ExtensionGlobalError::GenerateMessageSection(
const ExtensionIdSet* extensions,
int template_message_id) {
CHECK(extensions);
CHECK(template_message_id);
string16 message;

for (ExtensionIdSet::const_iterator iter = extensions->begin();
iter != extensions->end(); ++iter) {
const Extension* e = extension_service_->GetExtensionById(*iter, true);
    message += l10n_util::GetStringFUTF16(
        template_message_id,
        string16(ASCIIToUTF16(e->name())),
        l10n_util::GetStringUTF16(IDS_SHORT_PRODUCT_NAME));
}
return message;
}

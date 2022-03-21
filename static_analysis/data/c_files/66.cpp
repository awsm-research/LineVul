string16 ExtensionInstallUI::Prompt::GetHeading(std::string extension_name)
    const {
if (type_ == INLINE_INSTALL_PROMPT) {
return UTF8ToUTF16(extension_name);
} else {
return l10n_util::GetStringFUTF16(
kHeadingIds[type_], UTF8ToUTF16(extension_name));
}
}

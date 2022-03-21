PageInfoUI::GetSecurityDescription(const IdentityInfo& identity_info) const {
std::unique_ptr<PageInfoUI::SecurityDescription> security_description(
new PageInfoUI::SecurityDescription());

  switch (identity_info.safe_browsing_status) {
    case PageInfo::SAFE_BROWSING_STATUS_NONE:
      break;
    case PageInfo::SAFE_BROWSING_STATUS_MALWARE:
      return CreateSecurityDescription(SecuritySummaryColor::RED,
                                       IDS_PAGE_INFO_MALWARE_SUMMARY,
                                       IDS_PAGE_INFO_MALWARE_DETAILS);
    case PageInfo::SAFE_BROWSING_STATUS_SOCIAL_ENGINEERING:
      return CreateSecurityDescription(
          SecuritySummaryColor::RED, IDS_PAGE_INFO_SOCIAL_ENGINEERING_SUMMARY,
          IDS_PAGE_INFO_SOCIAL_ENGINEERING_DETAILS);
    case PageInfo::SAFE_BROWSING_STATUS_UNWANTED_SOFTWARE:
      return CreateSecurityDescription(SecuritySummaryColor::RED,
                                       IDS_PAGE_INFO_UNWANTED_SOFTWARE_SUMMARY,
                                       IDS_PAGE_INFO_UNWANTED_SOFTWARE_DETAILS);
    case PageInfo::SAFE_BROWSING_STATUS_SIGN_IN_PASSWORD_REUSE:
#if defined(FULL_SAFE_BROWSING)
      return CreateSecurityDescriptionForPasswordReuse(
          /*is_enterprise_password=*/false);
#endif
      NOTREACHED();
      break;
    case PageInfo::SAFE_BROWSING_STATUS_ENTERPRISE_PASSWORD_REUSE:
#if defined(FULL_SAFE_BROWSING)
      return CreateSecurityDescriptionForPasswordReuse(
          /*is_enterprise_password=*/true);
#endif
      NOTREACHED();
      break;
    case PageInfo::SAFE_BROWSING_STATUS_BILLING:
      return CreateSecurityDescription(SecuritySummaryColor::RED,
                                       IDS_PAGE_INFO_BILLING_SUMMARY,
                                       IDS_PAGE_INFO_BILLING_DETAILS);
  }
switch (identity_info.identity_status) {
case PageInfo::SITE_IDENTITY_STATUS_INTERNAL_PAGE:
#if defined(OS_ANDROID)
// We provide identical summary and detail strings for Android, which
// deduplicates them in the UI code.
return CreateSecurityDescription(SecuritySummaryColor::GREEN,
IDS_PAGE_INFO_INTERNAL_PAGE,
IDS_PAGE_INFO_INTERNAL_PAGE);
#else
// Internal pages on desktop have their own UI implementations which
// should never call this function.
NOTREACHED();
FALLTHROUGH;
#endif
case PageInfo::SITE_IDENTITY_STATUS_EV_CERT:
FALLTHROUGH;
case PageInfo::SITE_IDENTITY_STATUS_CERT:
FALLTHROUGH;
case PageInfo::SITE_IDENTITY_STATUS_CERT_REVOCATION_UNKNOWN:
FALLTHROUGH;
case PageInfo::SITE_IDENTITY_STATUS_ADMIN_PROVIDED_CERT:
switch (identity_info.connection_status) {
case PageInfo::SITE_CONNECTION_STATUS_INSECURE_ACTIVE_SUBRESOURCE:
return CreateSecurityDescription(SecuritySummaryColor::RED,
IDS_PAGE_INFO_NOT_SECURE_SUMMARY,
IDS_PAGE_INFO_NOT_SECURE_DETAILS);
case PageInfo::SITE_CONNECTION_STATUS_INSECURE_FORM_ACTION:
return CreateSecurityDescription(SecuritySummaryColor::RED,
IDS_PAGE_INFO_MIXED_CONTENT_SUMMARY,
IDS_PAGE_INFO_NOT_SECURE_DETAILS);
case PageInfo::SITE_CONNECTION_STATUS_INSECURE_PASSIVE_SUBRESOURCE:
return CreateSecurityDescription(SecuritySummaryColor::RED,
IDS_PAGE_INFO_MIXED_CONTENT_SUMMARY,
IDS_PAGE_INFO_MIXED_CONTENT_DETAILS);
default:
return CreateSecurityDescription(SecuritySummaryColor::GREEN,
IDS_PAGE_INFO_SECURE_SUMMARY,
IDS_PAGE_INFO_SECURE_DETAILS);
}
case PageInfo::SITE_IDENTITY_STATUS_DEPRECATED_SIGNATURE_ALGORITHM:
case PageInfo::SITE_IDENTITY_STATUS_UNKNOWN:
case PageInfo::SITE_IDENTITY_STATUS_NO_CERT:
default:
return CreateSecurityDescription(SecuritySummaryColor::RED,
IDS_PAGE_INFO_NOT_SECURE_SUMMARY,
IDS_PAGE_INFO_NOT_SECURE_DETAILS);
}
}

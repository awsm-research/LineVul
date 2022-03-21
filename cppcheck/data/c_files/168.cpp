void CreateOAuth2ServiceDelegate(
signin::AccountConsistencyMethod account_consistency) {
    oauth2_service_delegate_.reset(new MutableProfileOAuth2TokenServiceDelegate(
client_.get(), &signin_error_controller_, &account_tracker_service_,
token_web_data_, account_consistency, revoke_all_tokens_on_load_,
        true /* can_revoke_credantials */));
    // Make sure PO2TS has a chance to load itself before continuing.
base::RunLoop().RunUntilIdle();
oauth2_service_delegate_->AddObserver(this);
}

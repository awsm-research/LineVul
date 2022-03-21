void ChromeContentRendererClient::RenderViewCreated(RenderView* render_view) {
ContentSettingsObserver* content_settings =
new ContentSettingsObserver(render_view);
  new DevToolsAgent(render_view);
new ExtensionHelper(render_view, extension_dispatcher_.get());
new PageLoadHistograms(render_view, histogram_snapshots_.get());
new PrintWebViewHelper(render_view);
new SearchBox(render_view);
new SpellCheckProvider(render_view, spellcheck_.get());
#if defined(ENABLE_SAFE_BROWSING)
safe_browsing::MalwareDOMDetails::Create(render_view);
#endif

#if defined(OS_MACOSX)
new TextInputClientObserver(render_view);
#endif  // defined(OS_MACOSX)

PasswordAutofillManager* password_autofill_manager =
new PasswordAutofillManager(render_view);
AutofillAgent* autofill_agent = new AutofillAgent(render_view,
password_autofill_manager);
PageClickTracker* page_click_tracker = new PageClickTracker(render_view);
// Note that the order of insertion of the listeners is important.
// The password_autocomplete_manager takes the first shot at processing the
// notification and can stop the propagation.
page_click_tracker->AddListener(password_autofill_manager);
page_click_tracker->AddListener(autofill_agent);

TranslateHelper* translate = new TranslateHelper(render_view, autofill_agent);
new ChromeRenderViewObserver(
render_view, content_settings, extension_dispatcher_.get(), translate);

// Used only for testing/automation.
if (CommandLine::ForCurrentProcess()->HasSwitch(
switches::kDomAutomationController)) {
new AutomationRendererHelper(render_view);
}
}

static void webkit_web_view_update_settings(WebKitWebView* webView)
{
WebKitWebViewPrivate* priv = webView->priv;
WebKitWebSettings* webSettings = priv->webSettings.get();
Settings* settings = core(webView)->settings();

gchar* defaultEncoding, *cursiveFontFamily, *defaultFontFamily, *fantasyFontFamily, *monospaceFontFamily, *sansSerifFontFamily, *serifFontFamily, *userStylesheetUri, *defaultSpellCheckingLanguages;
gboolean autoLoadImages, autoShrinkImages, printBackgrounds,
enableScripts, enablePlugins, enableDeveloperExtras, resizableTextAreas,
enablePrivateBrowsing, enableCaretBrowsing, enableHTML5Database, enableHTML5LocalStorage,
enableXSSAuditor, enableSpatialNavigation, enableFrameFlattening, javascriptCanOpenWindows,
javaScriptCanAccessClipboard, enableOfflineWebAppCache,
enableUniversalAccessFromFileURI, enableFileAccessFromFileURI,
enableDOMPaste, tabKeyCyclesThroughElements, enableWebGL,
enableSiteSpecificQuirks, usePageCache, enableJavaApplet,
        enableHyperlinkAuditing, enableFullscreen, enableDNSPrefetching;

WebKitEditingBehavior editingBehavior;

g_object_get(webSettings,
"default-encoding", &defaultEncoding,
"cursive-font-family", &cursiveFontFamily,
"default-font-family", &defaultFontFamily,
"fantasy-font-family", &fantasyFontFamily,
"monospace-font-family", &monospaceFontFamily,
"sans-serif-font-family", &sansSerifFontFamily,
"serif-font-family", &serifFontFamily,
"auto-load-images", &autoLoadImages,
"auto-shrink-images", &autoShrinkImages,
"print-backgrounds", &printBackgrounds,
"enable-scripts", &enableScripts,
"enable-plugins", &enablePlugins,
"resizable-text-areas", &resizableTextAreas,
"user-stylesheet-uri", &userStylesheetUri,
"enable-developer-extras", &enableDeveloperExtras,
"enable-private-browsing", &enablePrivateBrowsing,
"enable-caret-browsing", &enableCaretBrowsing,
"enable-html5-database", &enableHTML5Database,
"enable-html5-local-storage", &enableHTML5LocalStorage,
"enable-xss-auditor", &enableXSSAuditor,
"enable-spatial-navigation", &enableSpatialNavigation,
"enable-frame-flattening", &enableFrameFlattening,
"javascript-can-open-windows-automatically", &javascriptCanOpenWindows,
"javascript-can-access-clipboard", &javaScriptCanAccessClipboard,
"enable-offline-web-application-cache", &enableOfflineWebAppCache,
"editing-behavior", &editingBehavior,
"enable-universal-access-from-file-uris", &enableUniversalAccessFromFileURI,
"enable-file-access-from-file-uris", &enableFileAccessFromFileURI,
"enable-dom-paste", &enableDOMPaste,
"tab-key-cycles-through-elements", &tabKeyCyclesThroughElements,
"enable-site-specific-quirks", &enableSiteSpecificQuirks,
"enable-page-cache", &usePageCache,
"enable-java-applet", &enableJavaApplet,
"enable-hyperlink-auditing", &enableHyperlinkAuditing,
"spell-checking-languages", &defaultSpellCheckingLanguages,
"enable-fullscreen", &enableFullscreen,
"enable-dns-prefetching", &enableDNSPrefetching,
"enable-webgl", &enableWebGL,
NULL);

settings->setDefaultTextEncodingName(defaultEncoding);
settings->setCursiveFontFamily(cursiveFontFamily);
settings->setStandardFontFamily(defaultFontFamily);
settings->setFantasyFontFamily(fantasyFontFamily);
settings->setFixedFontFamily(monospaceFontFamily);
settings->setSansSerifFontFamily(sansSerifFontFamily);
settings->setSerifFontFamily(serifFontFamily);
settings->setLoadsImagesAutomatically(autoLoadImages);
settings->setShrinksStandaloneImagesToFit(autoShrinkImages);
settings->setShouldPrintBackgrounds(printBackgrounds);
settings->setJavaScriptEnabled(enableScripts);
settings->setPluginsEnabled(enablePlugins);
settings->setTextAreasAreResizable(resizableTextAreas);
settings->setUserStyleSheetLocation(KURL(KURL(), userStylesheetUri));
settings->setDeveloperExtrasEnabled(enableDeveloperExtras);
settings->setPrivateBrowsingEnabled(enablePrivateBrowsing);
settings->setCaretBrowsingEnabled(enableCaretBrowsing);
#if ENABLE(DATABASE)
AbstractDatabase::setIsAvailable(enableHTML5Database);
#endif
settings->setLocalStorageEnabled(enableHTML5LocalStorage);
settings->setXSSAuditorEnabled(enableXSSAuditor);
settings->setSpatialNavigationEnabled(enableSpatialNavigation);
settings->setFrameFlatteningEnabled(enableFrameFlattening);
settings->setJavaScriptCanOpenWindowsAutomatically(javascriptCanOpenWindows);
settings->setJavaScriptCanAccessClipboard(javaScriptCanAccessClipboard);
settings->setOfflineWebApplicationCacheEnabled(enableOfflineWebAppCache);
settings->setEditingBehaviorType(static_cast<WebCore::EditingBehaviorType>(editingBehavior));
settings->setAllowUniversalAccessFromFileURLs(enableUniversalAccessFromFileURI);
settings->setAllowFileAccessFromFileURLs(enableFileAccessFromFileURI);
settings->setDOMPasteAllowed(enableDOMPaste);
settings->setNeedsSiteSpecificQuirks(enableSiteSpecificQuirks);
settings->setUsesPageCache(usePageCache);
settings->setJavaEnabled(enableJavaApplet);
settings->setHyperlinkAuditingEnabled(enableHyperlinkAuditing);
settings->setDNSPrefetchingEnabled(enableDNSPrefetching);

#if ENABLE(FULLSCREEN_API)
settings->setFullScreenEnabled(enableFullscreen);
#endif

#if ENABLE(SPELLCHECK)
    WebKit::EditorClient* client = static_cast<WebKit::EditorClient*>(core(webView)->editorClient());
    static_cast<WebKit::TextCheckerClientEnchant*>(client->textChecker())->updateSpellCheckingLanguage(defaultSpellCheckingLanguages);
#endif

#if ENABLE(WEBGL)
settings->setWebGLEnabled(enableWebGL);
#endif

Page* page = core(webView);
if (page)
page->setTabKeyCyclesThroughElements(tabKeyCyclesThroughElements);

g_free(defaultEncoding);
g_free(cursiveFontFamily);
g_free(defaultFontFamily);
g_free(fantasyFontFamily);
g_free(monospaceFontFamily);
g_free(sansSerifFontFamily);
g_free(serifFontFamily);
g_free(userStylesheetUri);

webkit_web_view_screen_changed(GTK_WIDGET(webView), NULL);
}

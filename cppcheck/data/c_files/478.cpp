static void webkitWebViewBaseContainerAdd(GtkContainer* container, GtkWidget* widget)
{
WebKitWebViewBase* webView = WEBKIT_WEB_VIEW_BASE(container);
WebKitWebViewBasePrivate* priv = webView->priv;

if (WEBKIT_IS_WEB_VIEW_BASE(widget)
&& WebInspectorProxy::isInspectorPage(WEBKIT_WEB_VIEW_BASE(widget)->priv->pageProxy.get())) {
ASSERT(!priv->inspectorView);
priv->inspectorView = widget;
        priv->inspectorViewHeight = gMinimumAttachedInspectorHeight;
} else {
GtkAllocation childAllocation;
gtk_widget_get_allocation(widget, &childAllocation);
priv->children.set(widget, childAllocation);
}

gtk_widget_set_parent(widget, GTK_WIDGET(container));
}

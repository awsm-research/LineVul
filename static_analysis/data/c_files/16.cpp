void GoBackCrossSite() {
NavigationEntry* entry = contents()->controller().GetEntryAtOffset(-1);
ASSERT_TRUE(entry);
contents()->controller().GoBack();

    // The navigation should commit in the pending RVH.
    contents()->TestDidNavigate(
        contents()->pending_rvh(), entry->page_id(), GURL(entry->url()),
        content::PAGE_TRANSITION_TYPED);
}

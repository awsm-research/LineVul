InputImeEventRouter* GetInputImeEventRouter(Profile* profile) {
if (!profile)
return nullptr;
return extensions::InputImeEventRouterFactory::GetInstance()->GetRouter(
      profile->GetOriginalProfile());
}

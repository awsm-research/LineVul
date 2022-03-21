PasswordGenerationUIData GetTestGenerationUIData2() {
  PasswordForm form;
  form.form_data = autofill::FormData();
  form.form_data.action = GURL("http://www.example2.com/accounts/Login");
  form.form_data.origin = GURL("http://www.example2.com/accounts/LoginAuth");
  PasswordGenerationUIData data;
  data.password_form = form;
  data.generation_element = ASCIIToUTF16("testelement2");
  data.max_length = 11;
  return data;
}

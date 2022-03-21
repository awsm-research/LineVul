_ksba_name_new_from_der (ksba_name_t *r_name,
const unsigned char *image, size_t imagelen)
{
gpg_error_t err;
ksba_name_t name;
struct tag_info ti;
const unsigned char *der;
size_t derlen;
int n;
char *p;

if (!r_name || !image)
return gpg_error (GPG_ERR_INV_VALUE);

*r_name = NULL;

  /* count and check for encoding errors - we won;t do this again
during the second pass */
der = image;
derlen = imagelen;
n = 0;
while (derlen)
{
err = _ksba_ber_parse_tl (&der, &derlen, &ti);
if (err)
return err;
if (ti.class != CLASS_CONTEXT)
return gpg_error (GPG_ERR_INV_CERT_OBJ); /* we expected a tag */
if (ti.ndef)
return gpg_error (GPG_ERR_NOT_DER_ENCODED);
if (derlen < ti.length)
return gpg_error (GPG_ERR_BAD_BER);
switch (ti.tag)
{
case 1: /* rfc822Name - this is an imlicit IA5_STRING */
case 4: /* Name */
case 6: /* URI */
n++;
break;
default:
break;
}

/* advance pointer */
der += ti.length;
derlen -= ti.length;
}

/* allocate array and set all slots to NULL for easier error recovery */
err = ksba_name_new (&name);
if (err)
return err;
if (!n)
return 0; /* empty GeneralNames */
name->names = xtrycalloc (n, sizeof *name->names);
if (!name->names)
{
ksba_name_release (name);
return gpg_error (GPG_ERR_ENOMEM);
}
name->n_names = n;

/* start the second pass */
der = image;
derlen = imagelen;
n = 0;
while (derlen)
{
char numbuf[21];

err = _ksba_ber_parse_tl (&der, &derlen, &ti);
assert (!err);
switch (ti.tag)
{
case 1: /* rfc822Name - this is an imlicit IA5_STRING */
p = name->names[n] = xtrymalloc (ti.length+3);
if (!p)
{
ksba_name_release (name);
return gpg_error (GPG_ERR_ENOMEM);
}
*p++ = '<';
memcpy (p, der, ti.length);
p += ti.length;
*p++ = '>';
*p = 0;
n++;
break;
case 4: /* Name */
err = _ksba_derdn_to_str (der, ti.length, &p);
if (err)
return err; /* FIXME: we need to release some of the memory */
name->names[n++] = p;
break;
case 6: /* URI */
sprintf (numbuf, "%u:", (unsigned int)ti.length);
p = name->names[n] = xtrymalloc (1+5+strlen (numbuf)
+ ti.length +1+1);
if (!p)
{
ksba_name_release (name);
return gpg_error (GPG_ERR_ENOMEM);
}
p = stpcpy (p, "(3:uri");
p = stpcpy (p, numbuf);
memcpy (p, der, ti.length);
p += ti.length;
*p++ = ')';
*p = 0; /* extra safeguard null */
n++;
break;
default:
break;
}

/* advance pointer */
der += ti.length;
derlen -= ti.length;
}
*r_name = name;
return 0;
}

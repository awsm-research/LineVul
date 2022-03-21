vips_malloc( VipsObject *object, size_t size )
{
void *buf;

	buf = g_malloc( size );

if( object ) {
g_signal_connect( object, "postclose",
G_CALLBACK( vips_malloc_cb ), buf );
object->local_memory += size;
}

return( buf );
}

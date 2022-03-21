static void array_cleanup( char* arr[] , int arr_size)
{
int i=0;
	for( i=0; i< arr_size; i++ ){ 
if( arr[i*2] ){
efree( arr[i*2]);
}
}
efree(arr);
}

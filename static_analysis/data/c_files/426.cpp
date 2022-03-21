void WebGL2RenderingContextBase::deleteVertexArray(
WebGLVertexArrayObject* vertex_array) {
  if (isContextLost() || !vertex_array)
return;

if (!vertex_array->IsDefaultObject() &&
vertex_array == bound_vertex_array_object_)
SetBoundVertexArrayObject(nullptr);

vertex_array->DeleteObject(ContextGL());
}

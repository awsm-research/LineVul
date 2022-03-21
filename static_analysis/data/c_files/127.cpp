write_message( RenderState  state )
{
ADisplay  adisplay = (ADisplay)state->display.disp;


if ( state->message == NULL )
{
FontFace  face = &state->faces[state->face_index];
int       idx, total;


idx   = face->index;
total = 1;
while ( total + state->face_index < state->num_faces &&
face[total].filepath == face[0].filepath     )
total++;

total += idx;

state->message = state->message0;
if ( total > 1 )
        sprintf( state->message0, "%s %d/%d @ %5.1fpt",
state->filename, idx + 1, total,
state->char_size );
else
        sprintf( state->message0, "%s @ %5.1fpt",
state->filename,
state->char_size );
}

grWriteCellString( adisplay->bitmap, 0, DIM_Y - 10, state->message,
adisplay->fore_color );

state->message = NULL;
}

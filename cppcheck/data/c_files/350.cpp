void qemu_spice_create_host_primary(SimpleSpiceDisplay *ssd)
{
QXLDevSurfaceCreate surface;

memset(&surface, 0, sizeof(surface));

    dprint(1, "%s/%d: %dx%d\n", __func__, ssd->qxl.id,
           surface_width(ssd->ds), surface_height(ssd->ds));

surface.format     = SPICE_SURFACE_FMT_32_xRGB;
surface.width      = surface_width(ssd->ds);
{
dprint(1, "%s/%d:\n", __func__, ssd->qxl.id);

qemu_spice_destroy_primary_surface(ssd, 0, QXL_SYNC);
}

void qemu_spice_display_init_common(SimpleSpiceDisplay *ssd)
{
qemu_mutex_init(&ssd->lock);
QTAILQ_INIT(&ssd->updates);
ssd->mouse_x = -1;
ssd->mouse_y = -1;
if (ssd->num_surfaces == 0) {
ssd->num_surfaces = 1024;
}
ssd->bufsize = (16 * 1024 * 1024);
ssd->buf = g_malloc(ssd->bufsize);
}

/* display listener callbacks */

void qemu_spice_display_update(SimpleSpiceDisplay *ssd,
int x, int y, int w, int h)
{
if (ssd->num_surfaces == 0) {
ssd->num_surfaces = 1024;
}
    ssd->bufsize = (16 * 1024 * 1024);
    ssd->buf = g_malloc(ssd->bufsize);
}

/* display listener callbacks */
update_area.top = y;
update_area.bottom = y + h;

if (qemu_spice_rect_is_empty(&ssd->dirty)) {
ssd->notify++;
}
qemu_spice_rect_union(&ssd->dirty, &update_area);
}

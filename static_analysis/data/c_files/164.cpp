Compositor::Compositor(const viz::FrameSinkId& frame_sink_id,
ui::ContextFactory* context_factory,
ui::ContextFactoryPrivate* context_factory_private,
scoped_refptr<base::SingleThreadTaskRunner> task_runner,
bool enable_surface_synchronization,
bool enable_pixel_canvas,
bool external_begin_frames_enabled,
bool force_software_compositor,
const char* trace_environment_name)
: context_factory_(context_factory),
context_factory_private_(context_factory_private),
frame_sink_id_(frame_sink_id),
task_runner_(task_runner),
vsync_manager_(new CompositorVSyncManager()),
external_begin_frames_enabled_(external_begin_frames_enabled),
force_software_compositor_(force_software_compositor),
layer_animator_collection_(this),
is_pixel_canvas_(enable_pixel_canvas),
lock_manager_(task_runner),
trace_environment_name_(trace_environment_name
? trace_environment_name
: kDefaultTraceEnvironmentName),
context_creation_weak_ptr_factory_(this) {
if (context_factory_private) {
auto* host_frame_sink_manager =
context_factory_private_->GetHostFrameSinkManager();
host_frame_sink_manager->RegisterFrameSinkId(
        frame_sink_id_, this, viz::ReportFirstSurfaceActivation::kYes);
host_frame_sink_manager->SetFrameSinkDebugLabel(frame_sink_id_,
"Compositor");
}
root_web_layer_ = cc::Layer::Create();

base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

cc::LayerTreeSettings settings;

// This will ensure PictureLayers always can have LCD text, to match the
// previous behaviour with ContentLayers, where LCD-not-allowed notifications
// were ignored.
settings.layers_always_allowed_lcd_text = true;
// Use occlusion to allow more overlapping windows to take less memory.
settings.use_occlusion_for_tile_prioritization = true;
refresh_rate_ = context_factory_->GetRefreshRate();
settings.main_frame_before_activation_enabled = false;
settings.delegated_sync_points_required =
context_factory_->SyncTokensRequiredForDisplayCompositor();

// Disable edge anti-aliasing in order to increase support for HW overlays.
settings.enable_edge_anti_aliasing = false;

if (command_line->HasSwitch(switches::kLimitFps)) {
std::string fps_str =
command_line->GetSwitchValueASCII(switches::kLimitFps);
double fps;
if (base::StringToDouble(fps_str, &fps) && fps > 0) {
forced_refresh_rate_ = fps;
}
}

if (command_line->HasSwitch(cc::switches::kUIShowCompositedLayerBorders)) {
std::string layer_borders_string = command_line->GetSwitchValueASCII(
cc::switches::kUIShowCompositedLayerBorders);
std::vector<base::StringPiece> entries = base::SplitStringPiece(
layer_borders_string, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
if (entries.empty()) {
settings.initial_debug_state.show_debug_borders.set();
} else {
for (const auto& entry : entries) {
const struct {
const char* name;
cc::DebugBorderType type;
} kBorders[] = {{cc::switches::kCompositedRenderPassBorders,
cc::DebugBorderType::RENDERPASS},
{cc::switches::kCompositedSurfaceBorders,
cc::DebugBorderType::SURFACE},
{cc::switches::kCompositedLayerBorders,
cc::DebugBorderType::LAYER}};
for (const auto& border : kBorders) {
if (border.name == entry) {
settings.initial_debug_state.show_debug_borders.set(border.type);
break;
}
}
}
}
}
settings.initial_debug_state.show_fps_counter =
command_line->HasSwitch(cc::switches::kUIShowFPSCounter);
settings.initial_debug_state.show_layer_animation_bounds_rects =
command_line->HasSwitch(cc::switches::kUIShowLayerAnimationBounds);
settings.initial_debug_state.show_paint_rects =
command_line->HasSwitch(switches::kUIShowPaintRects);
settings.initial_debug_state.show_property_changed_rects =
command_line->HasSwitch(cc::switches::kUIShowPropertyChangedRects);
settings.initial_debug_state.show_surface_damage_rects =
command_line->HasSwitch(cc::switches::kUIShowSurfaceDamageRects);
settings.initial_debug_state.show_screen_space_rects =
command_line->HasSwitch(cc::switches::kUIShowScreenSpaceRects);

settings.initial_debug_state.SetRecordRenderingStats(
command_line->HasSwitch(cc::switches::kEnableGpuBenchmarking));
settings.enable_surface_synchronization = enable_surface_synchronization;
settings.build_hit_test_data = features::IsVizHitTestingSurfaceLayerEnabled();

settings.use_zero_copy = IsUIZeroCopyEnabled();

settings.use_layer_lists =
command_line->HasSwitch(cc::switches::kUIEnableLayerLists);

// UI compositor always uses partial raster if not using zero-copy. Zero copy
// doesn't currently support partial raster.
settings.use_partial_raster = !settings.use_zero_copy;

settings.use_rgba_4444 =
command_line->HasSwitch(switches::kUIEnableRGBA4444Textures);

#if defined(OS_MACOSX)
// Using CoreAnimation to composite requires using GpuMemoryBuffers, which
// require zero copy.
settings.resource_settings.use_gpu_memory_buffer_resources =
settings.use_zero_copy;
settings.enable_elastic_overscroll = true;
#endif

settings.memory_policy.bytes_limit_when_visible = 512 * 1024 * 1024;
settings.memory_policy.priority_cutoff_when_visible =
gpu::MemoryAllocation::CUTOFF_ALLOW_NICE_TO_HAVE;

settings.disallow_non_exact_resource_reuse =
command_line->HasSwitch(switches::kDisallowNonExactResourceReuse);

if (command_line->HasSwitch(switches::kRunAllCompositorStagesBeforeDraw)) {
settings.wait_for_all_pipeline_stages_before_draw = true;
settings.enable_latency_recovery = false;
}

settings.always_request_presentation_time =
command_line->HasSwitch(cc::switches::kAlwaysRequestPresentationTime);

animation_host_ = cc::AnimationHost::CreateMainInstance();

cc::LayerTreeHost::InitParams params;
params.client = this;
params.task_graph_runner = context_factory_->GetTaskGraphRunner();
params.settings = &settings;
params.main_task_runner = task_runner_;
params.mutator_host = animation_host_.get();
host_ = cc::LayerTreeHost::CreateSingleThreaded(this, std::move(params));

if (base::FeatureList::IsEnabled(features::kUiCompositorScrollWithLayers) &&
host_->GetInputHandler()) {
scroll_input_handler_.reset(
new ScrollInputHandler(host_->GetInputHandler()));
}

animation_timeline_ =
cc::AnimationTimeline::Create(cc::AnimationIdProvider::NextTimelineId());
animation_host_->AddAnimationTimeline(animation_timeline_.get());

host_->SetHasGpuRasterizationTrigger(features::IsUiGpuRasterizationEnabled());
host_->SetRootLayer(root_web_layer_);
host_->SetVisible(true);

if (command_line->HasSwitch(switches::kUISlowAnimations)) {
slow_animations_ = std::make_unique<ScopedAnimationDurationScaleMode>(
ScopedAnimationDurationScaleMode::SLOW_DURATION);
}
}

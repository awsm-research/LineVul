virtual void SetUpCommandLine(CommandLine* command_line) {
    GpuFeatureTest::SetUpCommandLine(command_line);
    command_line->AppendSwitch(switches::kEnableThreadedCompositing);
  }

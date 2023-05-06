// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <QWidget>
#include "yuzu/configuration/configuration_shared.h"

namespace Core {
class System;
}

namespace Ui {
class ConfigureGraphicsAdvanced;
}

class ConfigureGraphicsAdvanced : public ConfigurationShared::Tab {
public:
    explicit ConfigureGraphicsAdvanced(
        const Core::System& system_,
        std::shared_ptr<std::forward_list<ConfigurationShared::Tab*>> group,
        QWidget* parent = nullptr);
    ~ConfigureGraphicsAdvanced() override;

    void ApplyConfiguration() override;
    void SetConfiguration() override;

    void ExposeComputeOption();

private:
    void changeEvent(QEvent* event) override;
    void RetranslateUI();

    void SetupPerGameUI();

    std::unique_ptr<Ui::ConfigureGraphicsAdvanced> ui;

    ConfigurationShared::CheckState async_present;
    ConfigurationShared::CheckState renderer_force_max_clock;
    ConfigurationShared::CheckState use_vsync;
    ConfigurationShared::CheckState async_astc;
    ConfigurationShared::CheckState use_reactive_flushing;
    ConfigurationShared::CheckState use_asynchronous_shaders;
    ConfigurationShared::CheckState use_fast_gpu_time;
    ConfigurationShared::CheckState use_vulkan_driver_pipeline_cache;
    ConfigurationShared::CheckState enable_compute_pipelines;
    ConfigurationShared::CheckState use_video_framerate;
    ConfigurationShared::CheckState barrier_feedback_loops;

    const Core::System& system;
};

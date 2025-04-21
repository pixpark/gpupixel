/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <vector>
#include "gpupixel/gpupixel_define.h"
#include "gpupixel/filter/filter.h"
#include "gpupixel/sink/sink.h"
#include "gpupixel/source/source.h"

namespace gpupixel {
class GPUPIXEL_API FilterGroup : public Filter {
 public:
  virtual ~FilterGroup();

  static std::shared_ptr<FilterGroup> Create();
  static std::shared_ptr<FilterGroup> Create(
      std::vector<std::shared_ptr<Filter>> filters);

  bool Init();
  bool Init(std::vector<std::shared_ptr<Filter>> filters);
  bool HasFilter(const std::shared_ptr<Filter> filter) const;
  void AddFilter(std::shared_ptr<Filter> filter);
  void RemoveFilter(std::shared_ptr<Filter> filter);
  void RemoveAllFilters();

  // Manually specify the terminal filter, which is the final output filter of
  // sequence Most often, it's not necessary to specify the terminal filter
  // manually, as the terminal filter will be specified automatically.
  void SetTerminalFilter(std::shared_ptr<Filter> filter) {
    terminal_filter_ = filter;
  }

  virtual std::shared_ptr<Source> AddSink(std::shared_ptr<Sink> sink) override;
  virtual std::shared_ptr<Source> AddSink(std::shared_ptr<Sink> sink,
                                          int inputNumber) override;
  virtual void RemoveSink(std::shared_ptr<Sink> sink) override;
  virtual void RemoveAllSinks() override;
  virtual bool HasSink(const std::shared_ptr<Sink> sink) const override;
  virtual std::map<std::shared_ptr<Sink>, int>& GetSinks() override;
  virtual bool DoRender(bool updateSinks = true) override;
  virtual void Render() override;
  virtual void DoUpdateSinks() override;
  virtual void SetFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> fb,
      RotationMode outputRotation = RotationMode::NoRotation) override;

  virtual std::shared_ptr<GPUPixelFramebuffer> GetFramebuffer() const override;
  virtual void SetInputFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> framebuffer,
      RotationMode rotation_mode = NoRotation,
      int texIdx = 0) override;

  virtual bool IsReady() const override;
  virtual void ResetAndClean() override;

 protected:
  std::vector<std::shared_ptr<Filter>> filters_;
  std::shared_ptr<Filter> terminal_filter_;

  FilterGroup();
  static std::shared_ptr<Filter> PredictTerminalFilter(
      std::shared_ptr<Filter> filter);
};

}  // namespace gpupixel

/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <vector>
#include "filter.h"
#include "gpupixel_macros.h"
#include "source.h"
#include "target.h"

NS_GPUPIXEL_BEGIN
class GPUPIXEL_API FilterGroup : public Filter {
 public:
  virtual ~FilterGroup();

  static std::shared_ptr<FilterGroup> create();
  static std::shared_ptr<FilterGroup> create(
      std::vector<std::shared_ptr<Filter>> filters);

  bool init();
  bool init(std::vector<std::shared_ptr<Filter>> filters);
  bool hasFilter(const std::shared_ptr<Filter> filter) const;
  void addFilter(std::shared_ptr<Filter> filter);
  void removeFilter(std::shared_ptr<Filter> filter);
  void removeAllFilters();

  // Manually specify the terminal filter, which is the final output filter of
  // sequence Most often, it's not necessary to specify the terminal filter
  // manually, as the terminal filter will be specified automatically.
  void setTerminalFilter(std::shared_ptr<Filter> filter) {
    _terminalFilter = filter;
  }

  virtual std::shared_ptr<Source> addTarget(
      std::shared_ptr<Target> target) override;
  virtual std::shared_ptr<Source> addTarget(std::shared_ptr<Target> target,
                                            int inputNumber) override;
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  virtual std::shared_ptr<Source> addTarget(id<GPUPixelTarget> target) override;
#endif
  virtual void removeTarget(std::shared_ptr<Target> target) override;
  virtual void removeAllTargets() override;
  virtual bool hasTarget(const std::shared_ptr<Target> target) const override;
  virtual std::map<std::shared_ptr<Target>, int>& getTargets() override;
  virtual bool proceed(bool bUpdateTargets = true,
                       int64_t frameTime = 0) override;
  virtual void update(int64_t frameTime) override;
  virtual void updateTargets(int64_t frameTime) override;
  virtual void setFramebuffer(
      std::shared_ptr<Framebuffer> fb,
      RotationMode outputRotation = RotationMode::NoRotation) override;

  virtual std::shared_ptr<Framebuffer> getFramebuffer() const override;
  virtual void setInputFramebuffer(std::shared_ptr<Framebuffer> framebuffer,
                                   RotationMode rotationMode = NoRotation,
                                   int texIdx = 0) override;

  virtual bool isPrepared() const override;
  virtual void unPrepear() override;

  FilterGroup();
 protected:
  std::vector<std::shared_ptr<Filter>> _filters;
  std::shared_ptr<Filter> _terminalFilter;

  static std::shared_ptr<Filter> _predictTerminalFilter(
      std::shared_ptr<Filter> filter);
};

NS_GPUPIXEL_END

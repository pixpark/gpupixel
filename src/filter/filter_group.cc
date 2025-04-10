/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/filter_group.h"
#include <assert.h>
#include <algorithm>
#include "core/gpupixel_context.h"

namespace gpupixel {

FilterGroup::FilterGroup() : terminal_filter_(0) {}

FilterGroup::~FilterGroup() {
  RemoveAllFilters();
  terminal_filter_ = 0;
}

std::shared_ptr<FilterGroup> FilterGroup::Create() {
  auto ret = std::shared_ptr<FilterGroup>(new FilterGroup());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

std::shared_ptr<FilterGroup> FilterGroup::Create(
    std::vector<std::shared_ptr<Filter>> filters) {
  auto ret = std::shared_ptr<FilterGroup>(new FilterGroup());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init(filters)) {
      ret.reset();
    }
  });
  return ret;
}

bool FilterGroup::Init() {
  return true;
}

bool FilterGroup::Init(std::vector<std::shared_ptr<Filter>> filters) {
  if (filters.size() == 0) {
    return true;
  }
  filters_ = filters;
  SetTerminalFilter(PredictTerminalFilter(filters[filters.size() - 1]));
  return true;
}

bool FilterGroup::HasFilter(const std::shared_ptr<Filter> filter) const {
  auto it = std::find(filters_.begin(), filters_.end(), filter);
  if (it != filters_.end()) {
    return true;
  } else {
    return false;
  }
}

void FilterGroup::AddFilter(std::shared_ptr<Filter> filter) {
  if (HasFilter(filter)) {
    return;
  }

  filters_.push_back(filter);
  SetTerminalFilter(PredictTerminalFilter(filter));
}

void FilterGroup::RemoveFilter(std::shared_ptr<Filter> filter) {
  auto itr = std::find(filters_.begin(), filters_.end(), filter);
  if (itr != filters_.end()) {
    filters_.erase(itr);
  }
}

void FilterGroup::RemoveAllFilters() {
  filters_.clear();
}

std::shared_ptr<Filter> FilterGroup::PredictTerminalFilter(
    std::shared_ptr<Filter> filter) {
  if (filter->GetSinks().size() == 0) {
    return filter;
  } else {
    return PredictTerminalFilter(
        std::dynamic_pointer_cast<Filter>(filter->GetSinks().begin()->first));
  }
}

std::shared_ptr<Source> FilterGroup::AddSink(std::shared_ptr<Sink> sink) {
  if (terminal_filter_) {
    return terminal_filter_->AddSink(sink);
  } else {
    return 0;
  }
}

std::shared_ptr<Source> FilterGroup::AddSink(std::shared_ptr<Sink> sink,
                                             int inputNumber) {
  if (terminal_filter_) {
    return terminal_filter_->AddSink(sink, inputNumber);
  } else {
    return 0;
  }
}

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
std::shared_ptr<Source> FilterGroup::AddSink(id<GPUPixelSink> sink) {
  if (terminal_filter_) {
    return terminal_filter_->AddSink(sink);
  } else {
    return 0;
  }
}
#endif

void FilterGroup::RemoveSink(std::shared_ptr<Sink> sink) {
  if (terminal_filter_) {
    terminal_filter_->RemoveSink(sink);
  }
}

void FilterGroup::RemoveAllSinks() {
  if (terminal_filter_) {
    terminal_filter_->RemoveAllSinks();
  }
}

bool FilterGroup::HasSink(const std::shared_ptr<Sink> sink) const {
  if (terminal_filter_) {
    return terminal_filter_->HasSink(sink);
  } else {
    return false;
  }
}

std::map<std::shared_ptr<Sink>, int>& FilterGroup::GetSinks() {
  assert(terminal_filter_);
  return terminal_filter_->GetSinks();
}

bool FilterGroup::DoRender(bool updateSinks) {
  return true;
}

void FilterGroup::Render() {
  DoRender();

  for (auto& filter : filters_) {
    if (filter->IsReady()) {
      filter->Render();
    }
  }
}

void FilterGroup::DoUpdateSinks() {
  if (terminal_filter_) {
    terminal_filter_->DoUpdateSinks();
  }
}

void FilterGroup::SetFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> fb,
    RotationMode outputRotation /* = RotationMode::NoRotation*/) {
  // if (terminal_filter_)
  //     terminal_filter_->SetFramebuffer(fb);
}

std::shared_ptr<GPUPixelFramebuffer> FilterGroup::GetFramebuffer() const {
  // if (terminal_filter_)
  //     return terminal_filter_->GetFramebuffer();
  return 0;
}

void FilterGroup::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotation_mode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : filters_) {
    filter->SetInputFramebuffer(framebuffer, rotation_mode, texIdx);
  }
}

bool FilterGroup::IsReady() const {
  //    for (auto& filter : filters_) {
  //        if (!filter->IsReady())
  //            return false;
  //    }
  return true;
}

void FilterGroup::ResetAndClean() {
  // for (auto& filter : filters_) {
  //    filter->unPrepeared();
  //}
}

}  // namespace gpupixel

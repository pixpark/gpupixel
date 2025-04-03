/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "filter_group.h"
#include <assert.h>
#include <algorithm>
#include "gpupixel_context.h"

namespace gpupixel {

FilterGroup::FilterGroup() : _terminalFilter(0) {}

FilterGroup::~FilterGroup() {
  removeAllFilters();
  _terminalFilter = 0;
}

std::shared_ptr<FilterGroup> FilterGroup::Create() {
  auto ret = std::shared_ptr<FilterGroup>(new FilterGroup());
  if (ret && !ret->Init()) {
    ret.reset();
  }
  return ret;
}

std::shared_ptr<FilterGroup> FilterGroup::Create(
    std::vector<std::shared_ptr<Filter>> filters) {
  auto ret = std::shared_ptr<FilterGroup>(new FilterGroup());
  if (ret && !ret->Init(filters)) {
    ret.reset();
  }
  return ret;
}

bool FilterGroup::Init() {
  return true;
}

bool FilterGroup::Init(std::vector<std::shared_ptr<Filter>> filters) {
  if (filters.size() == 0) {
    return true;
  }
  _filters = filters;
  setTerminalFilter(_predictTerminalFilter(filters[filters.size() - 1]));
  return true;
}

bool FilterGroup::hasFilter(const std::shared_ptr<Filter> filter) const {
  auto it = std::find(_filters.begin(), _filters.end(), filter);
  if (it != _filters.end()) {
    return true;
  } else {
    return false;
  }
}

void FilterGroup::addFilter(std::shared_ptr<Filter> filter) {
  if (hasFilter(filter)) {
    return;
  }

  _filters.push_back(filter);
  setTerminalFilter(_predictTerminalFilter(filter));
}

void FilterGroup::removeFilter(std::shared_ptr<Filter> filter) {
  auto itr = std::find(_filters.begin(), _filters.end(), filter);
  if (itr != _filters.end()) {
    _filters.erase(itr);
  }
}

void FilterGroup::removeAllFilters() {
  _filters.clear();
}

std::shared_ptr<Filter> FilterGroup::_predictTerminalFilter(
    std::shared_ptr<Filter> filter) {
  if (filter->GetSinks().size() == 0) {
    return filter;
  } else {
    return _predictTerminalFilter(
        std::dynamic_pointer_cast<Filter>(filter->GetSinks().begin()->first));
  }
}

std::shared_ptr<Source> FilterGroup::AddSink(std::shared_ptr<Sink> sink) {
  if (_terminalFilter) {
    return _terminalFilter->AddSink(sink);
  } else {
    return 0;
  }
}

std::shared_ptr<Source> FilterGroup::AddSink(std::shared_ptr<Sink> sink,
                                               int inputNumber) {
  if (_terminalFilter) {
    return _terminalFilter->AddSink(sink, inputNumber);
  } else {
    return 0;
  }
}

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
std::shared_ptr<Source> FilterGroup::AddSink(id<GPUPixelSink> sink) {
  if (_terminalFilter) {
    return _terminalFilter->AddSink(sink);
  } else {
    return 0;
  }
}
#endif

void FilterGroup::RemoveSink(std::shared_ptr<Sink> sink) {
  if (_terminalFilter) {
    _terminalFilter->RemoveSink(sink);
  }
}

void FilterGroup::RemoveAllSinks() {
  if (_terminalFilter) {
    _terminalFilter->RemoveAllSinks();
  }
}

bool FilterGroup::HasSink(const std::shared_ptr<Sink> sink) const {
  if (_terminalFilter) {
    return _terminalFilter->HasSink(sink);
  } else {
    return false;
  }
}

std::map<std::shared_ptr<Sink>, int>& FilterGroup::GetSinks() {
  assert(_terminalFilter);
  return _terminalFilter->GetSinks();
}

bool FilterGroup::DoRender(bool updateSinks) {
  return true;
}

void FilterGroup::Render() {
  DoRender();
  if (GPUPixelContext::GetInstance()->isCapturingFrame &&
      this == GPUPixelContext::GetInstance()->captureUpToFilter.get()) {
    GPUPixelContext::GetInstance()->captureUpToFilter = _terminalFilter;
  }

  for (auto& filter : _filters) {
    if (filter->IsReady()) {
      filter->Render();
    }
  }
}

void FilterGroup::DoUpdateSinks() {
  if (_terminalFilter) {
    _terminalFilter->DoUpdateSinks();
  }
}

void FilterGroup::SetFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> fb,
    RotationMode outputRotation /* = RotationMode::NoRotation*/) {
  // if (_terminalFilter)
  //     _terminalFilter->SetFramebuffer(fb);
}

std::shared_ptr<GPUPixelFramebuffer> FilterGroup::GetFramebuffer() const {
  // if (_terminalFilter)
  //     return _terminalFilter->GetFramebuffer();
  return 0;
}

void FilterGroup::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotationMode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : _filters) {
    filter->SetInputFramebuffer(framebuffer, rotationMode, texIdx);
  }
}

bool FilterGroup::IsReady() const {
  // todo(Jeayo)
  //    for (auto& filter : _filters) {
  //        if (!filter->IsReady())
  //            return false;
  //    }
  return true;
}

void FilterGroup::ResetAndClean() {
  // todo(Jeayo)
  // for (auto& filter : _filters) {
  //    filter->unPrepeared();
  //}
}

}

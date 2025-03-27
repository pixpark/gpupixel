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

std::shared_ptr<FilterGroup> FilterGroup::create() {
  auto ret = std::shared_ptr<FilterGroup>(new FilterGroup());
  if (ret && !ret->init()) {
    ret.reset();
  }
  return ret;
}

std::shared_ptr<FilterGroup> FilterGroup::create(
    std::vector<std::shared_ptr<Filter>> filters) {
  auto ret = std::shared_ptr<FilterGroup>(new FilterGroup());
  if (ret && !ret->init(filters)) {
    ret.reset();
  }
  return ret;
}

bool FilterGroup::init() {
  return true;
}

bool FilterGroup::init(std::vector<std::shared_ptr<Filter>> filters) {
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
  if (filter->getSinks().size() == 0) {
    return filter;
  } else {
    return _predictTerminalFilter(
        std::dynamic_pointer_cast<Filter>(filter->getSinks().begin()->first));
  }
}

std::shared_ptr<Source> FilterGroup::addSink(std::shared_ptr<Sink> sink) {
  if (_terminalFilter) {
    return _terminalFilter->addSink(sink);
  } else {
    return 0;
  }
}

std::shared_ptr<Source> FilterGroup::addSink(std::shared_ptr<Sink> sink,
                                               int inputNumber) {
  if (_terminalFilter) {
    return _terminalFilter->addSink(sink, inputNumber);
  } else {
    return 0;
  }
}

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
std::shared_ptr<Source> FilterGroup::addSink(id<GPUPixelSink> sink) {
  if (_terminalFilter) {
    return _terminalFilter->addSink(sink);
  } else {
    return 0;
  }
}
#endif

void FilterGroup::removeSink(std::shared_ptr<Sink> sink) {
  if (_terminalFilter) {
    _terminalFilter->removeSink(sink);
  }
}

void FilterGroup::removeAllSinks() {
  if (_terminalFilter) {
    _terminalFilter->removeAllSinks();
  }
}

bool FilterGroup::hasSink(const std::shared_ptr<Sink> sink) const {
  if (_terminalFilter) {
    return _terminalFilter->hasSink(sink);
  } else {
    return false;
  }
}

std::map<std::shared_ptr<Sink>, int>& FilterGroup::getSinks() {
  assert(_terminalFilter);
  return _terminalFilter->getSinks();
}

bool FilterGroup::doRender(bool updateSinks) {
  return true;
}

void FilterGroup::render() {
  doRender();
  if (GPUPixelContext::getInstance()->isCapturingFrame &&
      this == GPUPixelContext::getInstance()->captureUpToFilter.get()) {
    GPUPixelContext::getInstance()->captureUpToFilter = _terminalFilter;
  }

  for (auto& filter : _filters) {
    if (filter->isPrepared()) {
      filter->render();
    }
  }
}

void FilterGroup::doUpdateSinks() {
  if (_terminalFilter) {
    _terminalFilter->doUpdateSinks();
  }
}

void FilterGroup::setFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> fb,
    RotationMode outputRotation /* = RotationMode::NoRotation*/) {
  // if (_terminalFilter)
  //     _terminalFilter->setFramebuffer(fb);
}

std::shared_ptr<GPUPixelFramebuffer> FilterGroup::getFramebuffer() const {
  // if (_terminalFilter)
  //     return _terminalFilter->getFramebuffer();
  return 0;
}

void FilterGroup::setInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotationMode /* = NoRotation*/,
    int texIdx /* = 0*/) {
  for (auto& filter : _filters) {
    filter->setInputFramebuffer(framebuffer, rotationMode, texIdx);
  }
}

bool FilterGroup::isPrepared() const {
  // todo(Jeayo)
  //    for (auto& filter : _filters) {
  //        if (!filter->isPrepared())
  //            return false;
  //    }
  return true;
}

void FilterGroup::unPrepear() {
  // todo(Jeayo)
  // for (auto& filter : _filters) {
  //    filter->unPrepeared();
  //}
}

}

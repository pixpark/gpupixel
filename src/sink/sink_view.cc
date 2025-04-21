/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/sink/sink_view.h"
#include "sink/objc_view.h"

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)

#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

namespace gpupixel {

std::shared_ptr<SinkView> SinkView::Create(void* parent_view) {
  return std::shared_ptr<SinkView>(new SinkView(parent_view));
}

SinkView::SinkView(void* parent_view) {
#if defined(GPUPIXEL_IOS)
  UIView* parentUIView = (__bridge UIView*)parent_view;

  // 创建 ObjcView 实例
  ObjcView* gpuPixelView = [[ObjcView alloc] initWithFrame:parentUIView.bounds];
  gpuPixelView.autoresizingMask =
      UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

  [gpuPixelView setFillMode:(gpupixel::SinkRender::PreserveAspectRatio)];
  // 添加到父视图
  [parentUIView addSubview:gpuPixelView];

  // 保存 ObjcView 实例的引用
  render_view_ = (__bridge_retained void*)gpuPixelView;
#else
  NSView* parentNSView = (__bridge NSView*)parent_view;

  ObjcView* gpuPixelView = [[ObjcView alloc] initWithFrame:parentNSView.bounds];
  [gpuPixelView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

  [gpuPixelView setFillMode:(gpupixel::SinkRender::PreserveAspectRatio)];

  [parentNSView addSubview:gpuPixelView
                positioned:NSWindowBelow
                relativeTo:nil];

  // 保存 ObjcView 实例的引用
  render_view_ = (__bridge_retained void*)gpuPixelView;
#endif
}

SinkView::~SinkView() {
  if (render_view_) {
#if defined(GPUPIXEL_IOS)
    ObjcView* gpuPixelView = (__bridge_transfer ObjcView*)render_view_;
    [gpuPixelView removeFromSuperview];
#else
    ObjcView* gpuPixelView = (__bridge_transfer ObjcView*)render_view_;
    [gpuPixelView removeFromSuperview];
#endif
    render_view_ = nullptr;
  }
}

void SinkView::Render() {
#if defined(GPUPIXEL_IOS)
  ObjcView* gpuPixelView = (__bridge ObjcView*)render_view_;
  [gpuPixelView DoRender];
#else
  ObjcView* gpuPixelView = (__bridge ObjcView*)render_view_;
  [gpuPixelView DoRender];
#endif
}

void SinkView::SetInputFramebuffer(
    std::shared_ptr<GPUPixelFramebuffer> framebuffer,
    RotationMode rotation_mode,
    int texIdx) {
#if defined(GPUPIXEL_IOS)
  ObjcView* gpuPixelView = (__bridge ObjcView*)render_view_;
  [gpuPixelView SetInputFramebuffer:framebuffer
                       withRotation:rotation_mode
                            atIndex:texIdx];
#else
  ObjcView* gpuPixelView = (__bridge ObjcView*)render_view_;
  [gpuPixelView SetInputFramebuffer:framebuffer
                       withRotation:rotation_mode
                            atIndex:texIdx];
#endif
}

bool SinkView::IsReady() const {
#if defined(GPUPIXEL_IOS)
  ObjcView* gpuPixelView = (__bridge ObjcView*)render_view_;
  if ([gpuPixelView respondsToSelector:@selector(IsReady)]) {
    return [gpuPixelView IsReady];
  }
#else
  ObjcView* gpuPixelView = (__bridge ObjcView*)render_view_;
  if ([gpuPixelView respondsToSelector:@selector(IsReady)]) {
    return [gpuPixelView IsReady];
  }
#endif
  return true;
}

void SinkView::ResetAndClean() {
#if defined(GPUPIXEL_IOS)
  ObjcView* gpuPixelView = (__bridge ObjcView*)render_view_;
  if ([gpuPixelView respondsToSelector:@selector(unPrepared)]) {
    [gpuPixelView unPrepared];
  }
#else
  ObjcView* gpuPixelView = (__bridge ObjcView*)render_view_;
  if ([gpuPixelView respondsToSelector:@selector(unPrepared)]) {
    [gpuPixelView unPrepared];
  }
#endif
}

}  // namespace gpupixel

#endif

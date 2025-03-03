---
title: Basic Filters
editLink: true
description: This article introduces how to use the basic filters in GPUPixel library.
---

# Basic Filters

## Basic Concepts

GPUPixel is designed with a filter chain architecture, where video or image data flows through a series of filters to achieve various effects. The main components include:

- **Source**: Input for video/image data (e.g., `SourceRawDataInput`)
- **Filters**: Processing units that apply effects (e.g., `BeautyFaceFilter`, `FaceReshapeFilter`)
- **Target**: Output destination (e.g., `TargetRawDataOutput`)

## Initialization

Before using GPUPixel, you need to initialize the context and create the filter chain:

```cpp
// Initialize GPUPixel context and run operations in its thread
gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // Create your filters and set up the chain here
});
```

## Creating a Filter Chain

A typical filter chain setup includes creating sources, filters, and targets, then linking them together:

```cpp
// Declare components
std::shared_ptr<SourceRawDataInput> source_raw_input_;
std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
std::shared_ptr<TargetRawDataOutput> target_raw_output_;

// Initialize and link components
gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // Create source
    source_raw_input_ = SourceRawDataInput::create();
    
    // Create filters
    beauty_face_filter_ = BeautyFaceFilter::create();
    
    // Create target
    target_raw_output_ = TargetRawDataOutput::create();
    
    // Link the chain
    source_raw_input_->addTarget(beauty_face_filter_)
                     ->addTarget(target_raw_output_);
});
```

## Cleanup

When you're done with GPUPixel, you should clean up resources:

```cpp
// Clean up GPUPixel resources
gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    // Release your filter references
    source_raw_input_.reset();
    beauty_face_filter_.reset();
    target_raw_output_.reset();
    // Other filters...
});
```
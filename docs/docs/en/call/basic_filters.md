---
title: Basic Filters
editLink: true
description: This article introduces how to use the basic filters in GPUPixel library.
---

# Basic Filters

## Basic Concepts

GPUPixel is designed with a filter chain architecture, where video or image data flows through a series of filters to achieve various effects. The main components include:

- **Source**: Input for video/image data (e.g., `SourceRawData`)
- **Filters**: Processing units that apply effects (e.g., `BeautyFaceFilter`, `FaceReshapeFilter`)
- **Sink**: Output destination (e.g., `SinkRawData`)

## Initialization

Before using GPUPixel, you need to initialize the context and create the filter chain:

## Creating a Filter Chain

A typical filter chain setup includes creating sources, filters, and targets, then linking them together:

```cpp
// Declare components
std::shared_ptr<SourceRawData> source_raw_input_;
std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
std::shared_ptr<SinkRawData> target_raw_output_;

// Create source
source_raw_input_ = SourceRawData::Create();

// Create filters
beauty_face_filter_ = BeautyFaceFilter::Create();

// Create sink
target_raw_output_ = SinkRawData::Create();

// Link the chain
source_raw_input_->AddSink(beauty_face_filter_)
                    ->AddSink(target_raw_output_);
```

## Cleanup

When you're done with GPUPixel, you should clean up resources:

```cpp
// Clean up GPUPixel resources
// Release your filter references
source_raw_input_.reset();
beauty_face_filter_.reset();
target_raw_output_.reset();
// Other filters...
```